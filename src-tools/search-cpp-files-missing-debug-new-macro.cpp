#include "pch.h"
#include "filesystem-utils.h"
#include "reports.h"

namespace fs = std::filesystem;


/// Search all .cpp files for a search term
int search_cpp_files_missing_debug_new_macro(const fs::path &root, std::ostream &output)
{
	std::vector<fs::path> directories = get_directory_list(root);

	std::vector<Report> summary;

	size_t files = 0;

	for (const auto& dir : directories)
		{
		bool first = true;
		Report missing;
		missing.m_dir = dir;
		summary.push_back(missing);

		for (const auto& file : fs::recursive_directory_iterator(dir))
			{
			fs::path path = file.path();

			if (path.extension() == ".cpp" && path.filename() != "stdafx.cpp")
				{
				std::ifstream file(path);
				std::string line;
				bool found = false;

				while (std::getline(file, line))
					{
					if (line.find("#define new DEBUG_NEW") != std::string::npos)
						{
						found = true;
						break;
						}
					}

				if (!found)
					{
					if (first)
						{
						std::string heading = dir.filename().string();
						std::string underline(heading.length(), '-');
						output
							<< heading << "\n"
							<< underline << "\n";
						first = false;
						}

					output << path.string() << "\n";

					summary.back().m_count++;
					}
				}
			}

		if (summary.back().m_count > 0)
			output << "\n";
		}

	if (!summary.empty())
		output_report(summary, output);

	output << "Done\n";
	return 0;
}
