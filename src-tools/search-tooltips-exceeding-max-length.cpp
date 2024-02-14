#include "pch.h"
#include "string-utils.h"
#include "filesystem-utils.h"
#include "resource-utils.h"
#include "reports.h"
#include "options.h"
#include <stack>

namespace fs = std::filesystem;

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


int search_tooltips_exceeding_max_length(const fs::path &root, std::ostream &output, const TooltipLengthOptions &options, TooltipLengthOutput &out)
{
	std::vector<fs::path> directories = get_directory_list(root);

	std::vector<Report> summary;

	for (const auto& dir : directories)
		{
		std::vector<IDSResource> rcStrings; // resource strings (name and descriptions) in the .rc file
		std::vector<IDSResource> cppStrings; // resource strings (name only) used by tooltips in the .cpp files
		std::vector<unsigned int> cppValues; // resource string numbers used by tooltips in the .cpp files
		std::unordered_map<unsigned int, std::string> valueMap; // map of resource.h number to resource name
		fs::path rcFilename;

		for (const auto& file : fs::directory_iterator(dir))
			{
			if (fs::is_regular_file(file))
				{
				if (file.path().extension() == ".cpp")
					{
					tooltips_in_use(file, cppStrings, cppValues);
					}
				else if (file.path().extension() == ".rc")
					{
					rcFilename = file;
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
		rcStrings.erase(std::remove_if(rcStrings.begin(), rcStrings.end(), [&](const IDSResource& r)
			{
			return r.m_description.length() <= options.m_maximum;
			}), rcStrings.end());

		cppStrings.erase(std::unique(cppStrings.begin(), cppStrings.end()), cppStrings.end());

		// Get the subset of too long resource strings that are used by tooltips in cpp
		std::vector<IDSResource> suspects;
		std::set_intersection(rcStrings.begin(), rcStrings.end(), cppStrings.begin(), cppStrings.end(), std::back_inserter(suspects));

		for (auto& r : suspects)
			{
			r.m_description.insert(options.m_maximum, "|");
			}

		TooltipLength tooltips;
		tooltips.m_rcFilename = rcFilename;
		tooltips.m_stringResources = suspects;
		out.m_projectResources.push_back(tooltips);

		Report report;
		report.m_dir = dir;
		report.m_count = suspects.size();
		summary.push_back(report);

		if (summary.back().m_count == 0)
			continue;

		if (!options.m_onlySummary)
			{
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
		}

	output_report(summary, output);

	output << "Done\n";

	return 0;
}

void replace_tooltip(const TooltipReplacement& tooltip, std::function<void(const std::string&)> RemoveReadOnlyFunc)
{
	fs::path original = tooltip.m_filename;
	fs::path temporary = original.string() + ".bak";

	std::ifstream fin(original.string());
	std::ofstream fout(temporary.string());

	bool modified = false;

	for (std::string line; std::getline(fin, line);)
		{
		if (replace_substr(line, tooltip.m_oldString, tooltip.m_newString))
			{
			modified = true;
			}

		fout << line << "\n";
		}

	fin.close();
	fout.close();

	if (modified)
		{
		// overwrite original file with new
		RemoveReadOnlyFunc(original.string());
		fs::copy(temporary, original, fs::copy_options::overwrite_existing);
		}

	fs::remove(temporary);
}
