#pragma once

struct Report
{
	std::filesystem::path m_dir;
	size_t m_count = 0;
};

void output_report(std::vector<Report>& reports, std::ostream& output);
