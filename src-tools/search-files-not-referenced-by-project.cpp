#include "pch.h"
#include "filesystem-utils.h"
#include "string-utils.h"
#include "reports.h"

namespace fs = std::filesystem;

std::vector<fs::path> get_referenced_files(const fs::path &project)
{
	std::vector<fs::path> filenames;

	std::ifstream ifs(project.string());
	std::string line;
	while (std::getline(ifs, line))
		{
		line = trim(line, " ");
		if (starts_with(line, "<ClCompile ") || starts_with(line, "<ClInclude "))
			{
			std::string name = get_xml_text_attribute(line, "Include");

			if (name.empty() || starts_with(name, "..\\"))
				continue;

			filenames.push_back(name);
			}
		}

	std::sort(filenames.begin(), filenames.end());
	return filenames;
}


int search_files_not_referenced_by_project(const fs::path &rootPath, std::ostream &output)
{
	std::vector<Report> reports;

	std::vector<fs::path> directories = get_directory_list(rootPath, false);
	for (const auto &dir : directories)
		{
		if (dir.filename() == "RxRasterServices")
			continue;

		std::vector<fs::path> filenames;
		std::vector<fs::path> projects;

		for (const auto &file : fs::recursive_directory_iterator(dir))
			{
			fs::path path = file.path();
			fs::path name = fs::relative(file, dir);
			fs::path ext = path.extension();

			if (name == "resource.h" || ends_with(name.string(), "_h.h"))
				continue;

			if (ext == ".cpp" || ext == ".h")
				{
				filenames.push_back(name);
				}
			else if (ext == ".vcxproj")
				{
				projects.push_back(path);
				}
			}

		projects.erase(std::remove_if(projects.begin(), projects.end(), [](const fs::path & path)
			{
			return path.filename().string().find('_') != std::string::npos;
			}), projects.end());

		if (projects.empty() || filenames.empty())
			continue;

		std::sort(filenames.begin(), filenames.end());
		std::sort(projects.begin(), projects.end());

		for (const auto &vcxproj : projects)
			{
			std::vector<fs::path> references = get_referenced_files(vcxproj);

			std::vector<fs::path> orphans;
			std::set_difference(filenames.begin(), filenames.end(), references.begin(), references.end(), std::back_inserter(orphans));

			Report report;
			report.m_dir = fs::relative(vcxproj, dir);
			report.m_count = orphans.size();
			reports.push_back(report);

			if (orphans.empty())
				continue;

			std::string heading = vcxproj.filename().string();
			std::string underline(heading.length(), '-');
			output
					<< heading << "\n"
					<< underline << "\n";

			std::copy(orphans.begin(), orphans.end(), std::ostream_iterator<fs::path>(output, "\n"));
			output << "\n";
			}
		}

	output_report(reports, output);

	return 0;
}

