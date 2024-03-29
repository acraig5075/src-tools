#include "pch.h"
#include "filesystem-utils.h"
#include "reports.h"
#include "options.h"

namespace fs = std::filesystem;

/// Search all .cpp files for a search term
int search_cpp_files_missing_debug_new_macro(const fs::path &root, std::ostream &output, const MissingMacroOptions &options)
{
	std::string title = "Cpp missing DEBUG_NEW";
	std::string underline(title.length(), '=');
	output
		<< title
		<< "\n"
		<< underline
		<< "\n\n";

	bool topLevel = filesystem_utils::is_solution_folder(root);

	std::vector<fs::path> directories = filesystem_utils::get_directory_list(root, (topLevel ? false : true));

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
					if (options.m_onlySummary)
						{
						summary.back().m_count++;
						}
					else
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
			}

		if (summary.back().m_count > 0 && !options.m_onlySummary)
			output << "\n";
		}

	if (!summary.empty())
		output_report(summary, output);

	output << "Done\n\n";
	return 0;
}
