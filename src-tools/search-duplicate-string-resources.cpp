#include "pch.h"
#include "string-utils.h"
#include "filesystem-utils.h"
#include "options.h"
#include "reports.h"

namespace fs = std::filesystem;

struct Resource
{
	std::string m_name;
	std::string m_description;
};


std::ostream &operator<<(std::ostream &out, const Resource &r)
{
	out << r.m_name << " - " << r.m_description;
	return out;
}


std::vector<Resource> ParseStringTable(size_t fileID, const fs::path &path)
{
	std::vector<Resource> ids;

	std::ifstream fin(path.string());
	std::string line;
	bool table = false;
	bool begin = false;

	while (std::getline(fin, line))
		{
		if ("STRINGTABLE" == line)
			{
			table = true;
			begin = false;
			}
		else if (table && "BEGIN" == line)
			{
			begin = true;
			}
		else if (table && "END" == line)
			{
			begin = false;
			table = false;
			}
		else if (table && begin && starts_with(trim(line, " "), "IDS_"))
			{
			size_t fnd = line.find('\"', 0);
			if (fnd != line.npos)
				{
				std::string name = line.substr(0, fnd);
				std::string desc = line.substr(fnd);
				name = trim(name, " ");
				desc = trim(desc, " \"");

				if (!name.empty() && !desc.empty())
					{
					Resource s;
					s.m_name = name;
					s.m_description = desc;
					ids.push_back(s);
					}
				}
			}
		}

	return ids;
}


template<class ForwardIt, class Comparison>
ForwardIt next_unique(ForwardIt first, ForwardIt last, Comparison compare)
{
	if (first == last)
		return last;

	auto itr1 = first;
	for (auto itr2 = first + 1; itr2 != last; ++itr1, ++itr2)
		{
		if (compare(*itr1, *itr2))
			return itr2;
		}

	return last;
}


enum output_format
{
	BY_DESCRIPTION,
	BY_NAME,
};

std::string make_report_text(const std::vector<Duplicate> &duplicates, output_format order)
{
	if (BY_DESCRIPTION == order)
		{
		std::stringstream ss;
		for (const auto &d : duplicates)
			{
			for (const auto &n : d.m_names)
				{
				ss << n << ": " << d.m_description << "\n";
				}
			}
		return ss.str();
		}

	else if (BY_NAME == order)
		{
		std::stringstream ss;
		for (const auto &d : duplicates)
			{
			for (size_t i = 0; i < d.m_names.size(); ++i)
				{
				if (0 == i)
					ss << d.m_names[i];
				else
					ss << "," << d.m_names[i];
				}
			ss << "\n";
			}
		return ss.str();
		}

	return { "" };
}

void sort_in_replacement_order(std::vector<std::string>& names)
{
	auto part = std::partition(names.begin(), names.end(), [](const auto& name)
		{
			return !starts_with(name, "IDS_STRING");
		});

	std::sort(names.begin(), part, [](const auto& lhs, const auto& rhs)
		{
			if (lhs.length() == rhs.length())
				return lhs < rhs;
			else
				return lhs.length() < rhs.length();
		});

	std::sort(part, names.end());
}

DuplicateStringsOutput find_duplicates(const std::vector<fs::path> &files)
{
	DuplicateStringsOutput reports;

	for (size_t i = 0; i < files.size(); ++i)
		{
		size_t count = 0;

		std::vector<Resource> ids = ParseStringTable(i, files[i]);

		std::sort(begin(ids), end(ids), [](const Resource & lhs, const Resource & rhs)
			{
			return std::tie(lhs.m_description, lhs.m_name) < std::tie(rhs.m_description, rhs.m_name);
			});

		std::vector<Duplicate> duplicates;

		for (auto itr = ids.begin(); itr != ids.end();)
			{
			auto next = next_unique(itr, ids.end(), [](const Resource & lhs, const Resource & rhs)
				{
				return lhs.m_description != rhs.m_description;
				});

			size_t size = std::distance(itr, next);
			if (size > 1)
				{
				count++;

				std::vector<std::string> names(size);

				std::transform(itr, next, names.begin(), [](const auto & resource)
					{
					return resource.m_name;
					});

				sort_in_replacement_order(names);

				Duplicate dup;
				dup.m_description = itr->m_description;
				dup.m_names = names;
				duplicates.push_back(dup);
				}

			itr = next;
			}

		DuplicateResources rep;
		rep.m_filename = files[i].string();
		rep.m_duplicates = duplicates;
		reports.m_folders.push_back(rep);
		}

	return reports;
}

void output_report(const std::vector<DuplicateResources> &reports, std::ostream &output)
{
	size_t col_width = 0;
	for (const auto& r : reports)
		{
		if (r.m_filename.string().length() > col_width)
			col_width = r.m_filename.string().length();
		}

	std::string heading = "Report";
	std::string underline(heading.length(), '-');
	output
		<< heading << "\n"
		<< underline << "\n";

	for (const auto& r : reports)
		{
		if (r.count() > 0)
			{
			output
				<< std::left
				<< std::setw(col_width)
				<< r.m_filename.string()
				<< " "
				<< r.count()
				<< "\n";
			}
		}

	std::cout << "\n";
}

int search_duplicate_string_resources(const fs::path &input, std::ostream &output, const DuplicateStringsOptions &options, DuplicateStringsOutput &out)
{
	output_format order;

	if (0 == options.m_outputFormat)
		order = BY_DESCRIPTION;
	else if (1 == options.m_outputFormat)
		order = BY_NAME;
	else
		return 1;

	std::vector<fs::path> files;

	if (fs::exists(input) && fs::is_directory(input))
		{
		files = get_file_list(input, ".rc");
		}
	else if (fs::exists(input) && fs::is_regular_file(input) && ends_with(input.string(), ".rc"))
		{
		files.push_back(input);
		}
	else
		{
		std::cerr << "Error: Invalid input\n";
		return 1;
		}

	const std::vector<std::string>& exclusions = options.m_excludeFolders;
	filter_directory_list(files, exclusions);

	output << "Excluding directories: ";
	std::copy(begin(exclusions), end(exclusions), std::ostream_iterator<std::string>(output, ", "));
	output << "\n\n";

	out = find_duplicates(files);

	std::sort(out.m_folders.begin(), out.m_folders.end(), [](const DuplicateResources& lhs, const DuplicateResources& rhs)
		{
		return lhs.count() > rhs.count();
		});

	if (!options.m_onlySummary)
		{
		for (const auto& r : out.m_folders)
			{
			std::string text = make_report_text(r.m_duplicates, order);

			if (!text.empty())
				{
				std::string heading = r.m_filename.stem().string();
				std::string underline(heading.length(), '-');
				output
					<< heading << "\n"
					<< underline << "\n"
					<< text << "\n"
					<< "\n";
				}
			}
		output << "\n";
		}

	output_report(out.m_folders, output);

	output << "Done\n";

	return 0;
}

struct FileEdit
{
	fs::path m_filename;
	int m_lineNumber = 0;
	std::string m_replacee;
	std::string m_replacer;
};

// Get the list of files needing editing
std::vector<FileEdit> required_edits(const fs::path& filename, const std::string &replacer, const std::vector<std::string> &replacees)
{
	if (replacees.empty())
		return {};

	std::vector<FileEdit> edits;
	std::string lcs = longest_common_substring(replacees);
	std::ifstream fin(filename.string());
	std::string line;
	
	for (int number = 0; std::getline(fin, line); ++number)
		{
		if (line.find(lcs, 0) == std::string::npos)
			continue;

		size_t off = 0;
		for (const std::string& replacee : replacees)
			{
			size_t fnd = find_substr_exact(line, replacee, off);
			while (fnd != std::string::npos)
				{
				FileEdit fe;
				fe.m_filename = filename;
				fe.m_lineNumber = number;
				fe.m_replacee = replacee;
				fe.m_replacer = replacer;
				edits.push_back(fe);

				off = fnd + replacee.length();
				fnd = find_substr_exact(line, replacee, off);
				}
			}
		}

	return edits;
}


void replace_duplicate_string_resources(const DuplicateResources& report, size_t index, std::function<void(const std::string&)> RemoveReadOnlyFunc)
{
	fs::path root = report.m_filename.parent_path();

	if (index >= report.m_duplicates.size())
		return;

	const Duplicate& out = report.m_duplicates.at(index);
	if (out.m_names.size() <= 1)
		return;

	std::vector<std::string> replacees{ out.m_names.begin() + 1, out.m_names.end() };

	std::vector<FileEdit> allEdits;

	// Find targets for replacements
	for (const auto& itr : fs::recursive_directory_iterator(root))
		{
		fs::path ext = itr.path().extension();

		if (fs::is_regular_file(itr) && (".h" == ext.string() || ".cpp" == ext.string()))
			{
			if (itr.path().filename() == "resource.h")
				continue;

			std::vector<FileEdit> edits = required_edits(itr, out.m_names[0], replacees);
			if (!edits.empty())
				{
				allEdits.insert(allEdits.end(), edits.begin(), edits.end());
				}
			}
		}

	// Do replacements
	for (const FileEdit& edit : allEdits)
		{
		fs::path original = edit.m_filename;
		fs::path temporary = original.string() + ".bak";

		std::string line;
		std::ifstream fin(original.string());
		std::ofstream fout(temporary.string());

		bool modified = false;

		for (int number = 0; std::getline(fin, line); ++number)
			{
			if (number == edit.m_lineNumber)
				{
				std::string before = line;
				replace_substr(line, edit.m_replacee, edit.m_replacer);

				if (line != before)
					modified = true;
				}

			fout << line << "\n";
			}

		fin.close();
		fout.close();

		if (modified)
			{
			RemoveReadOnlyFunc(original.string());
			fs::copy(temporary, original, fs::copy_options::overwrite_existing);
			fs::remove(temporary);
			}
		}
}
