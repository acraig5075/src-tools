#include "pch.h"
#include "reports.h"


void output_report(std::vector<Report>& reports, std::ostream& output)
	{
	bool none = std::all_of(begin(reports), end(reports), [](const Report &r)
		{
		return r.m_count == 0;
		});

	if (none)
		return;

	std::sort(begin(reports), end(reports), [](const auto& lhs, const auto& rhs)
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
		if (r.m_count > 0)
			{
			if (r.m_dir.string().length() > col_width)
				col_width = r.m_dir.string().length();
			}
		}

	for (const auto& r : reports)
		{
		if (r.m_count > 0)
			{
			output
				<< std::left
				<< std::setw(col_width)
				<< r.m_dir.string()
				<< " "
				<< r.m_count
				<< "\n";
			}
		}

	output << "\n";
	}
