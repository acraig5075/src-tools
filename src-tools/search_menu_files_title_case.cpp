#include "pch.h"
#include "string-utils.h"
#include "filesystem-utils.h"
#include "reports.h"
#include "options.h"
#include <stack>

namespace fs = std::filesystem;

struct MenuItem
{
	std::string m_title;
	std::string m_command;
	std::string m_description;
	std::string m_breadcrumbs;
};

std::string stringify(std::stack<std::string> hierarchy)
{
	std::vector<std::string> levels;
	while (!hierarchy.empty())
		{
		levels.push_back(hierarchy.top());
		hierarchy.pop();
		}

	std::string str;
	for (auto itr = levels.rbegin(); itr != levels.rend(); ++itr)
		{
		str.append(*itr);
		str.append(std::string{ " > " });
		}

	return str;
}

std::vector<MenuItem> get_menu_items(const std::filesystem::path &path)
{
	std::vector<MenuItem> menuItems;

	std::string line;
	std::ifstream file(path.string());
	std::stack<std::string> hierarchy;

	while (std::getline(file, line))
		{
		line = trim(line, " \t");
		if (line.empty() || starts_with(line, "#"))
			continue;

		if (starts_with(line, "POPUP"))
			{
			std::string flyout = line.substr(6);
			flyout = trim(flyout, "\"");
			flyout = erase_substr(flyout, "&");
			hierarchy.push(flyout);
			}
		else if (line == "END")
			{
			if (!hierarchy.empty())
				hierarchy.pop();
			}
		else
			{
			std::vector<std::string> tokens = quote_aware_split(line, ',');
			if (tokens.size() == 3)
				{
				MenuItem item;
				item.m_title = trim(tokens[0], "\"");
				item.m_command = trim(tokens[1], " ");
				item.m_description = trim(tokens[2], "\" ");
				item.m_breadcrumbs = stringify(hierarchy);

				menuItems.push_back(item);
				}
			}
		}

	return menuItems;
}


int search_menu_files_title_case(const std::filesystem::path &root, std::ostream &output, const MenuTitleCaseOptions &options)
{
	std::vector<fs::path> mnuFiles = filesystem_utils::get_file_list(root, ".mnu");

	std::vector<Report> reports;

	for (const auto &path : mnuFiles)
		{
		std::vector<MenuItem> menuItems = get_menu_items(path);

		auto newEnd = std::remove_if(menuItems.begin(), menuItems.end(), [](const MenuItem &item)
			{
			return is_title_case(erase_substr(item.m_title, "&"));
			});

		menuItems.erase(newEnd, menuItems.end());

		Report r;
		r.m_dir = path;
		r.m_count = menuItems.size();
		reports.push_back(r);

		if (!menuItems.empty())
			{
			std::string heading = path.string();
			std::string underline(heading.length(), '-');
			output
					<< heading << "\n"
					<< underline << "\n";
			}

		for (const MenuItem &item : menuItems)
			{
			output << item.m_breadcrumbs << item.m_title << "\n";
			}

		if (!menuItems.empty())
			{
			output << "\n";
			}
		}

	output_report(reports, output);

	output << "Done\n";
	return 0;
}
