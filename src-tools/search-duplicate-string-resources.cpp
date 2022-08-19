#include "pch.h"
#include "string-utils.h"
#include "filesystem-utils.h"
#include "options.h"

namespace fs = std::filesystem;

struct Resource
{
	std::string m_name;
	std::string m_description;
};

struct Duplicate
{
	std::string m_description;
	std::vector<std::string> m_names;
};

struct ReportEx
{
	fs::path m_filename;
	std::vector<Duplicate> m_duplicates;

	size_t count() const
		{
		return m_duplicates.size();
		}
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

std::vector<ReportEx> find_duplicates(const std::vector<fs::path> &files)
{
	std::vector<ReportEx> reports;

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

		ReportEx rep;
		rep.m_filename = files[i].string();
		rep.m_duplicates = duplicates;
		reports.push_back(rep);
		}

	return reports;
}

void output_report(const std::vector<ReportEx> &reports, std::ostream &output)
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

int search_duplicate_string_resources(const fs::path &input, std::ostream &output, const DuplicateStringsOptions &options)
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

	std::vector<ReportEx> reports = find_duplicates(files);

	std::sort(reports.begin(), reports.end(), [](const ReportEx& lhs, const ReportEx& rhs)
		{
		return lhs.count() > rhs.count();
		});

	if (!options.m_onlySummary)
		{
		for (const auto& r : reports)
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

	output_report(reports, output);

	output << "Done\n";

	return 0;
}
