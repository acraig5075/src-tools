#include "pch.h"
#include <dtl/dtl.hpp>
#include <fstream>
#include <vector>
#include <filesystem>
#include <algorithm>
#include "string-utils.h"
#include "filesystem-utils.h"
#include "options.h"
#include "reports.h"

//#include <time.h>
//#include <sys/stat.h>

namespace fs = std::filesystem;

using dtl::Diff;
using dtl::elemInfo;
using dtl::uniHunk;

static void PrintStats(std::string fp1, std::string fp2, std::ostream &out)
{
	const int    MAX_LENGTH    = 255;
	char         time_format[] = "%Y-%m-%d %H:%M:%S %z";
	time_t       rawtime[2];
	struct tm    timeinfo[2];
	struct stat  st[2];

	if (stat(fp1.c_str(), &st[0]) == -1)
		{
		std::cerr << "argv1 is invalid." << std::endl;
		exit(-1);
		}
	if (stat(fp2.c_str(), &st[1]) == -1)
		{
		std::cerr << "argv2 is invalid" << std::endl;
		exit(-1);
		}

	char buf[2][MAX_LENGTH + 1];
	rawtime[0] = st[0].st_mtime;
	localtime_s(&timeinfo[0], &rawtime[0]);
	strftime(buf[0], MAX_LENGTH, time_format, &timeinfo[0]);
	out << "--- " << fp1 << '\t' << buf[0] << std::endl;

	rawtime[1] = st[1].st_mtime;
	localtime_s(&timeinfo[1], &rawtime[1]);
	strftime(buf[1], MAX_LENGTH, time_format, &timeinfo[1]);
	out << "+++ " << fp2 << '\t' << buf[1] << std::endl;
}

void ExtractSection(const fs::path &rc, const std::string &section, std::vector<std::string> &extract)
{
	// An opening empty line for subsequent sections
	if (!extract.empty())
		extract.push_back(std::string{});

	std::string start = "// " + section;
	std::ifstream ifs(rc.string().c_str());
	std::string line;

	while (std::getline(ifs, line))
		{
		if (line == start)
			break;
		}

	extract.push_back(line);

	auto stop = [](const std::string & str)
		{
		return str.length() > 70 && std::string::npos == str.find_first_not_of('/', 0);
		};

	while (std::getline(ifs, line))
		{
		if (stop(line))
			break;
		else
			extract.push_back(line);
		}

	// Trailing empting lines or comment lines are problematic for the diff
	while (!extract.empty())
		{
		std::string back = extract.back();
		if (back.empty() || string_utils::starts_with(back, "#"))
			extract.pop_back();
		else
			break;
		}
}

static auto PrintList(std::vector<ComparePaths>::const_iterator beginItr, std::vector<ComparePaths>::const_iterator endItr, std::ostream &out)
{
	if (std::distance(beginItr, endItr) == 0)
		{
		out << "<Equal>\n";
		return;
		}

	size_t width = 0;
	for (auto itr = beginItr; itr != endItr; ++itr)
		{
		size_t width0 = itr->m_path2.string().length();
		if (width0 > width)
			width = width0;
		}

	for (auto itr = beginItr; itr != endItr; ++itr)
		{
		out
				<< std::left
				<< std::setw(width + 1)
				<< itr->m_path2.string()
				<< " "
				<< itr->m_editDistance
				<< "\n";
		}
}

int compare_resources(const fs::path &root, std::ostream &output, const CompareExtrasOptions &, ComparisonOutput &out)
{
	std::string title = "Comparison of Locale Resources";
	std::string underline(title.length(), '=');
	output
			<< title
			<< "\n"
			<< underline
			<< "\n\n";

	fs::path localePath = root.parent_path();
	localePath.append("CD_Resource");

	std::vector<ComparePaths> comparisons;

	std::vector<fs::path> localeRC = filesystem_utils::get_file_list(localePath, ".rc");
	std::vector<fs::path> regularRC = filesystem_utils::get_file_list(root, ".rc");

	for (const auto &path : regularRC)
		{
		fs::path filename = path.filename();
		filename.replace_extension();
		filename.concat("ResUK");
		filename.replace_extension(".rc");

		auto FindByFilename = [filename](const fs::path & locale)
			{
			return filename == locale.filename();
			};

		auto itr = std::find_if(localeRC.begin(), localeRC.end(), FindByFilename);
		if (itr != localeRC.end())
			{
			ComparePaths comp;
			comp.m_path1 = path;
			comp.m_path2 = *itr;
			comparisons.push_back(comp);
			}
		}

	auto Compare = [](ComparePaths & comp)
		{
		std::vector<std::string> lhs;
		ExtractSection(comp.m_path1, "Dialog", lhs);
		ExtractSection(comp.m_path1, "String Table", lhs);

		std::vector<std::string> rhs;
		ExtractSection(comp.m_path2, "Dialog", rhs);
		ExtractSection(comp.m_path2, "String Table", rhs);

		Diff<std::string> diff(lhs, rhs);
		diff.onHuge();
		diff.compose();

		if (diff.getEditDistance() > 0)
			{
			std::stringstream ss;

			PrintStats(comp.m_path1.string(), comp.m_path2.string(), ss);
			diff.composeUnifiedHunks();
			diff.printUnifiedFormat(ss);

			comp.m_editDistance = diff.getEditDistance();
			comp.m_unifiedDiff = ss.str();
			}
		};

	auto ByEditDistanceDesc = [](const ComparePaths & lhs, const ComparePaths & rhs)
		{
		if (lhs.m_editDistance > rhs.m_editDistance)
			return true;
		else if (lhs.m_editDistance == rhs.m_editDistance)
			return lhs.m_path2 < rhs.m_path2;
		else
			return false;
		};

	auto IsZeroEditDistance = [](const ComparePaths & comp)
		{
		return comp.m_editDistance == 0;
		};

	std::for_each(comparisons.begin(), comparisons.end(), Compare);
	std::sort(comparisons.begin(), comparisons.end(), ByEditDistanceDesc);
	auto endItr = std::remove_if(comparisons.begin(), comparisons.end(), IsZeroEditDistance);

	PrintList(comparisons.begin(), endItr, output);
	output << "\n";

	// Out parameter
	out.m_comparisons = comparisons;

	output << "Done\n\n";
	return 0;
}
