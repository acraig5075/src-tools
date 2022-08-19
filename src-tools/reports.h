#pragma once

struct UnusedResources
{
	std::filesystem::path m_path;
	std::vector<std::string> m_names;
};

struct UnusedStringsOutput
{
	std::vector<UnusedResources> m_folders;
};

struct Report
{
	std::filesystem::path m_dir;
	size_t m_count = 0;
};

void output_report(std::vector<Report>& reports, std::ostream& output);
