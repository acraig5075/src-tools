// search-unused-string-resources.cpp : Defines the entry point for the console application.
//

#include "pch.h"
#include "filesystem-utils.h"

namespace fs = std::filesystem;

using PathResourcePair = std::pair<fs::path, std::vector<std::string>>;



struct Unused
{
	fs::path m_filename;
	size_t m_count = 0;
};



static std::vector<PathResourcePair> get_file_list_custom(const fs::path &root, const std::string &extension)
{
	std::vector<PathResourcePair> files;

	// intentionally not including root

	for (const auto &itr : fs::recursive_directory_iterator(root))
		{
		if (itr.path().extension() == extension)
			{
			files.push_back(std::make_pair(itr.path(), std::vector<std::string>()));
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
				resources.push_back(token);
				off = start + 1;
				start = line.find("IDS_", off);
				}
			}
		}
}



std::vector<std::string> get_resource_list(const fs::path &root, const std::string &extension)
{
	auto files = get_file_list_custom(root, extension);

	std::for_each(begin(files), end(files), [](PathResourcePair & p)
		{
		find_resource_strings(p.first, p.second);
		});

	std::vector<std::string> resources;
	std::for_each(begin(files), end(files), [&](const PathResourcePair & p)
		{
		resources.insert(resources.end(), begin(p.second), end(p.second));
		});

	std::sort(begin(resources), end(resources));
	resources.erase(std::unique(begin(resources), end(resources)), end(resources));

	return resources;
}



void OutputReport(std::vector<Unused>& reports, std::ostream& output)
{
	std::sort(begin(reports), end(reports), [](const Unused& lhs, const Unused& rhs)
		{
			return lhs.m_count > rhs.m_count;
		});

	std::string heading("Report");
	std::string underline(heading.length(), '-');
	output
		<< heading << "\n"
		<< underline << "\n";

	size_t col_width = 0;
	for (const auto& r : reports)
	{
		if (r.m_filename.string().length() > col_width)
			col_width = r.m_filename.string().length();
	}

	for (const auto& r : reports)
	{
		output
			<< std::left
			<< std::setw(col_width)
			<< r.m_filename.string()
			<< " "
			<< r.m_count
			<< "\n";
	}
}

/// Lookup resource ID names defined in .rc files but not referenced in any .cpp files
int my_main(const fs::path &root, std::ostream &output)
{
	std::vector<Unused> reports;

	std::vector<fs::path> directories = get_directory_list(root);

	std::vector<std::string> exclusions = { "CommonDialogs", "Sewer", "Storm", };
	filter_directory_list(directories, exclusions);

	output << "Excluding directories: ";
	std::copy(begin(exclusions), end(exclusions), std::ostream_iterator<std::string>(output, ", "));
	output << "\n\n";

	for (const auto &dir : directories)
		{
		std::vector<std::string> used = get_resource_list(dir, ".cpp");
		std::vector<std::string> avail = get_resource_list(dir, ".rc");

		std::vector<std::string> difference;
		std::set_difference(begin(avail), end(avail), begin(used), end(used), std::back_inserter(difference));

		if (difference.empty())
			continue;

		Unused r;
		r.m_filename = dir;
		r.m_count = difference.size();
		reports.push_back(r);

		std::string heading(dir.filename().string());
		std::string underline(heading.length(), '-');
		output
				<< heading << "\n"
				<< underline << "\n";

		std::copy(begin(difference), end(difference), std::ostream_iterator<std::string>(output, "\n"));
		output << "\n";
		}

	OutputReport(reports, output);

	return 0;
}


int search_unused_string_resources(const fs::path &root, std::ostream &output)
{
	my_main(root, output);

	output << "Done\n";

	return 0;
}