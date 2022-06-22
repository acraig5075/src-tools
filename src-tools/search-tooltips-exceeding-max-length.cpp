#include "pch.h"
#include "string-utils.h"
#include "filesystem-utils.h"
#include "reports.h"
#include <stack>

namespace fs = std::filesystem;

struct IDSResource
{
	std::string m_name;
	std::string m_description;
};

std::ostream &operator<<(std::ostream &out, const IDSResource &r)
{
	out << r.m_name << " - " << r.m_description;
	return out;
}

bool operator<(const IDSResource &lhs, const IDSResource &rhs)
{
	return lhs.m_name.compare(rhs.m_name) < 0;
}

bool operator==(const IDSResource& lhs, const IDSResource& rhs)
{
	return lhs.m_name.compare(rhs.m_name) == 0;
}


std::vector<IDSResource> parse_string_table(const fs::path &path)
{
	std::vector<IDSResource> ids;

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
					IDSResource s;
					s.m_name = name;
					s.m_description = desc;
					ids.push_back(s);
					}
				}
			}
		}

	return ids;
}


void tooltips_in_use(const fs::path &path, std::vector<IDSResource> &names, std::vector<unsigned int>& values)
{
	std::ifstream fin(path.string());
	std::string line;

	bool tooltips = false;
	std::stack<char> braces;
	std::string search = ".LoadResString(";

	auto FindIDS = [search](const std::string & line)
		{
		size_t fnd = line.find(search, 0);
		if (fnd == std::string::npos)
			return std::string{};

		size_t start = fnd += search.length();
		size_t stop = line.find(')', start);
		return line.substr(fnd, stop - start);
		};

	while (std::getline(fin, line))
		{
		if (!tooltips && line.find("::GetTooltipText", 0) != std::string::npos)
			{
			tooltips = true;
			braces = {};
			}
		else if (tooltips)
			{
			std::string name = FindIDS(line);

			if (!braces.empty() && !name.empty())
				{
				if (std::atoi(name.c_str()) == 0)
					{
					IDSResource found;
					found.m_name = name;
					names.push_back(found);
					}
				else
					{
					values.push_back(std::atoi(name.c_str()));
					}
				}

			for (const auto c : line)
				{
				if (c == '{')
					braces.push('{');
				else if (c == '}')
					braces.pop();
				}

			if (braces.empty())
				tooltips = false;
			}
		}
}


void parse_resource_h(const fs::path& file, std::unordered_map<unsigned int, std::string>& valueMap)
{
	std::ifstream fin(file.string());
	std::string line;

	while (std::getline(fin, line))
		{
		if (starts_with(line, "#define IDS_"))
			{
			size_t end_name = line.find(' ', 8);
			size_t start_value = line.find_last_not_of("0123456789");

			if (end_name < start_value && start_value + 1 < line.length())
				{
				std::string name = line.substr(8, end_name - 8);
				std::string szvalue = line.substr(start_value + 1);
				unsigned int value = std::atoi(szvalue.c_str());
				if (value != 0)
					valueMap[value] = name;
				}
			}
		}
}


int search_tooltips_exceeding_max_length(const fs::path &root, std::ostream &output, size_t maximum)
{
	std::vector<fs::path> directories = get_directory_list(root);

	std::vector<Report> summary;

	for (const auto &dir : directories)
		{
		std::vector<IDSResource> rcStrings; // resource strings (name and descriptions) in the .rc file
		std::vector<IDSResource> cppStrings; // resource strings (name only) used by tooltips in the .cpp files
		std::vector<unsigned int> cppValues; // resource string numbers used by tooltips in the .cpp files
		std::unordered_map<unsigned int, std::string> valueMap; // map of resource.h number to resource name

		for (const auto &file : fs::directory_iterator(dir))
			{
			if (fs::is_regular_file(file))
				{
				if (file.path().extension() == ".cpp")
					{
					tooltips_in_use(file, cppStrings, cppValues);
					}
				else if (file.path().extension() == ".rc")
					{
					rcStrings = parse_string_table(file);
					}
				else if (file.path().filename() == "Resource.h")
					{
					parse_resource_h(file, valueMap);
					}
				}
			}

		// Include resource strings that are referenced by resource.h numbers rather than defines. (e.g. as with Sewer & Storm)
		for (auto v : cppValues) 
			{
			auto itr = valueMap.find(v);
			if (itr != valueMap.end())
				{
				IDSResource res;
				res.m_name = itr->second;
				cppStrings.push_back(res);
				}
			}

		if (rcStrings.empty() || cppStrings.empty())
			continue;

		std::sort(rcStrings.begin(), rcStrings.end());
		std::sort(cppStrings.begin(), cppStrings.end());

		// Remove resource strings that are of permissable length. We don't care about them.
		rcStrings.erase(std::remove_if(rcStrings.begin(), rcStrings.end(), [&](const IDSResource & r)
			{
			return r.m_description.length() <= maximum;
			}), rcStrings.end());

		cppStrings.erase(std::unique(cppStrings.begin(), cppStrings.end()), cppStrings.end());

		// Get the subset of too long resource strings that are used by tooltips in cpp
		std::vector<IDSResource> suspects;
		std::set_intersection(rcStrings.begin(), rcStrings.end(), cppStrings.begin(), cppStrings.end(), std::back_inserter(suspects));

		for (auto& r : suspects)
			{
			r.m_description.insert(maximum, "|");
			}

		Report report;
		report.m_dir = dir;
		report.m_count = suspects.size();
		summary.push_back(report);

		if (summary.back().m_count == 0)
			continue;

		std::string heading = dir.filename().string();
		std::string underline(heading.length(), '-');
		output 
			<< heading << "\n"
			<< underline << "\n";

		size_t col_width = 0;
		for (const auto& s : suspects)
			{
			if (s.m_name.length() > col_width)
				col_width = s.m_name.length();
			}

		for (const auto& s : suspects)
			{
			output
				<< std::left << std::setw(col_width)
				<< s.m_name << " - "
				<< s.m_description << "\n";
			}
		output << "\n";
		}

	output_report(summary, output);

	output << "Done\n";

	return 0;
}