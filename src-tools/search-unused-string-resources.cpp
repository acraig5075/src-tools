// search-unused-string-resources.cpp : Defines the entry point for the console application.
//

#include "pch.h"
#include "string-utils.h"
#include "filesystem-utils.h"
#include "reports.h"
#include "options.h"

namespace fs = std::filesystem;
using namespace string_utils;

static std::vector<UnusedResources> get_file_list_custom(const fs::path &root, const std::string &extension)
{
	std::vector<UnusedResources> files;

	// intentionally not including root

	for (const auto &itr : fs::recursive_directory_iterator(root))
		{
		if (itr.path().extension() == extension)
			{
			if (lowercase(itr.path().filename().string()) == "resource.h")
				continue;

			UnusedResources p;
			p.m_path = itr.path();
			files.push_back(p);
			}
		}

	return files;
}



void find_resource_strings(const fs::path &filename, std::vector<std::string> &resources)
{
	std::ifstream fs(filename);
	std::string line;

	while (std::getline(fs, line))
		{
		if (!line.empty())
			{
			size_t off = 0;
			size_t start = line.find("IDS_", off);
			while (start != std::string::npos)
				{
				size_t end = line.find_first_of(" ,;)]\t}", start);
				std::string token = line.substr(start, end - start);
				if (token.length() > 4)
					resources.push_back(token);
				off = start + 1;
				start = line.find("IDS_", off);
				}
			}
		}
}



std::vector<std::string> get_resource_list(const fs::path &root, const std::vector<std::string> &extensions)
{
	std::vector<std::string> resources;

	for (const std::string &ext : extensions)
		{
		auto files = get_file_list_custom(root, ext);

		

		std::for_each(begin(files), end(files), [](UnusedResources& p)
			{
			find_resource_strings(p.m_path, p.m_names);
			});

		std::for_each(begin(files), end(files), [&](const UnusedResources& p)
			{
			resources.insert(resources.end(), begin(p.m_names), end(p.m_names));
			});
		}

	std::sort(begin(resources), end(resources));
	resources.erase(std::unique(begin(resources), end(resources)), end(resources));

	return resources;
}


/// Lookup resource ID names defined in .rc files but not referenced in any .cpp files
int search_unused_string_resources(const fs::path &root, std::ostream &output, const UnusedStringsOptions &options, UnusedStringsOutput &out)
{
	std::string title = "Unused string resources";
	std::string underline(title.length(), '=');
	output
		<< title
		<< "\n"
		<< underline
		<< "\n\n";

	std::vector<Report> reports;

	std::vector<fs::path> directories = filesystem_utils::get_directory_list(root);

	const std::vector<std::string> &exclusions = options.m_excludeFolders;
	filesystem_utils::filter_directory_list(directories, exclusions);

	output << "Excluding directories: ";
	std::copy(begin(exclusions), end(exclusions), std::ostream_iterator<std::string>(output, ", "));
	output << "\n\n";

	for (const auto &dir : directories)
		{
		fs::path rc, header;
		if (!filesystem_utils::get_resource_filenames(dir, rc, header))
			continue;

		std::vector<std::string> used = get_resource_list(dir, { ".cpp", ".h" });
		std::vector<std::string> avail = get_resource_list(dir, { ".rc" });

		std::vector<std::string> difference;
		std::set_difference(begin(avail), end(avail), begin(used), end(used), std::back_inserter(difference));

		if (difference.empty())
			continue;

		UnusedResources o;
		o.m_path = dir;
		o.m_rc = rc;
		o.m_header = header;
		o.m_names = difference;
		out.m_folders.push_back(o);

		Report r;
		r.m_dir = dir;
		r.m_count = difference.size();
		reports.push_back(r);

		if (!options.m_onlySummary)
			{
			std::string heading(dir.filename().string());
			std::string underline(heading.length(), '-');
			output
					<< heading << "\n"
					<< underline << "\n";

			std::copy(begin(difference), end(difference), std::ostream_iterator<std::string>(output, "\n"));
			output << "\n";
			}
		}

	output_report(reports, output);

	output << "Done\n";

	return 0;
}

struct DeletionCandidate
{
	std::string m_id;
	int m_rcLineNum = -1;
	int m_headerLineNum = -1;
};


// search a file for a given string returning the line number if found, else -1
int find_search_term_line_number(const fs::path &filename, const std::string &search)
{
	std::string line;
	std::ifstream fin(filename.string());

	for (int lineNum = 0; std::getline(fin, line); ++lineNum)
		{
		if (line.find(search) != std::string::npos)
			{
			return lineNum;
			}
		}

	return -1;
};


// Copy a file by making a line-by-line copy, but excluding given line numbers
void copy_file_line_by_line(const fs::path &source, const fs::path &destination, std::vector<int> &ignoreLines)
{
	std::string line;
	std::ifstream fin(source.string());
	std::ofstream fout(destination.string());

	std::sort(ignoreLines.begin(), ignoreLines.end());
	auto itr = ignoreLines.cbegin();

	for (int lineNum = 0; std::getline(fin, line); ++lineNum)
		{
		if (itr != ignoreLines.cend() && lineNum == *itr)
			{
			++itr;
			}
		else
			{
			fout << line << "\n";
			}
		}
}

void delete_unused_string_resources(UnusedResources &report, std::function<void(const std::string&)> RemoveReadOnlyFunc)
{
	std::vector<DeletionCandidate> candidates(report.m_names.size());
	for (size_t i = 0; i < report.m_names.size(); ++i)
		{
		candidates[i].m_id = report.m_names[i];
		}

	// find the line number of the string resource in both the .rc and the header file
	for (DeletionCandidate &c : candidates)
		{
		std::string search = std::string(" ") + c.m_id + " ";
		c.m_rcLineNum     = find_search_term_line_number(report.m_rc, search);
		c.m_headerLineNum = find_search_term_line_number(report.m_header, search);
		}

	// not found candidates still have a -1 line number - these are possible errors in `search_unused_string_resources`
	candidates.erase(std::remove_if(candidates.begin(), candidates.end(), [](const DeletionCandidate & c)
		{
		return c.m_rcLineNum == -1 || c.m_headerLineNum == -1;
		}), candidates.end());

	fs::path tempFileRc = report.m_rc.string() + ".bak";
	fs::path tempFileHeader = report.m_header.string() + ".bak";

	std::vector<int> ignoreLines(candidates.size());

	// copy the .rc file ignoring the candidate lines
	std::transform(candidates.begin(), candidates.end(), ignoreLines.begin(), [](const DeletionCandidate & c)
		{
		return c.m_rcLineNum;
		});
	copy_file_line_by_line(report.m_rc, tempFileRc, ignoreLines);

	// copy the header file ignoring the candidate lines
	std::transform(candidates.begin(), candidates.end(), ignoreLines.begin(), [](const DeletionCandidate & c)
		{
		return c.m_headerLineNum;
		});
	copy_file_line_by_line(report.m_header, tempFileHeader, ignoreLines);

	// leave the report with the resource strings not deleted
	for (const DeletionCandidate &c : candidates)
		{
		report.m_names.erase(std::find(report.m_names.cbegin(), report.m_names.cend(), c.m_id));
		}

	// replace originals
	RemoveReadOnlyFunc(report.m_rc.string());
	RemoveReadOnlyFunc(report.m_header.string());
	fs::copy(tempFileRc, report.m_rc, fs::copy_options::overwrite_existing);
	fs::copy(tempFileHeader, report.m_header, fs::copy_options::overwrite_existing);
	fs::remove(tempFileRc);
	fs::remove(tempFileHeader);
}

size_t count_unused_string_resources(const fs::path& path)
	{
	std::stringstream ss;
	UnusedStringsOptions options;
	UnusedStringsOutput out;

	search_unused_string_resources(path, ss, options, out);
	if (!out.m_folders.empty())
		{
		return out.m_folders[0].m_names.size();
		}

	return 0;
	}