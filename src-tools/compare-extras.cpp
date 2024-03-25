#include "pch.h"
#include <dtl/dtl.hpp>
#include <fstream>
#include <vector>
#include <filesystem>
#include <algorithm>
#include "string-utils.h"
#include "options.h"
#include "reports.h"
#include "filesystem-utils.h"

//#include <time.h>
//#include <sys/stat.h>

namespace fs = std::filesystem;

using dtl::Diff;
using dtl::elemInfo;
using dtl::uniHunk;

//static void showStats(std::string fp1, std::string fp2, std::ostream &out);
//static void unifiedDiff(std::string fp1, std::string fp2, std::string out);
//
//static void showStats(std::string fp1, std::string fp2, std::ostream &out)
//{
//	const int    MAX_LENGTH    = 255;
//	char         time_format[] = "%Y-%m-%d %H:%M:%S %z";
//	time_t       rawtime[2];
//	struct tm   *timeinfo[2];
//	struct stat  st[2];
//
//	if (stat(fp1.c_str(), &st[0]) == -1)
//		{
//		std::cerr << "argv1 is invalid." << std::endl;
//		exit(-1);
//		}
//	if (stat(fp2.c_str(), &st[1]) == -1)
//		{
//		std::cerr << "argv2 is invalid" << std::endl;
//		exit(-1);
//		}
//
//	char buf[2][MAX_LENGTH + 1];
//	rawtime[0] = st[0].st_mtime;
//	timeinfo[0] = localtime(&rawtime[0]);
//	strftime(buf[0], MAX_LENGTH, time_format, timeinfo[0]);
//	out << "--- " << fp1 << '\t' << buf[0] << std::endl;
//
//	rawtime[1] = st[1].st_mtime;
//	timeinfo[1] = localtime(&rawtime[1]);
//	strftime(buf[1], MAX_LENGTH, time_format, timeinfo[1]);
//	out << "+++ " << fp2 << '\t' << buf[1] << std::endl;
//}
//
//static void unifiedDiff(std::string fp1, std::string fp2, std::string out)
//{
//	typedef std::string                 elem;
//	typedef std::vector< elem >         sequence;
//	typedef std::pair< elem, elemInfo > sesElem;
//
//	std::ifstream      Aifs(fp1.c_str());
//	std::ifstream      Bifs(fp2.c_str());
//	elem          buf;
//	sequence      ALines, BLines;
//
//	while (std::getline(Aifs, buf))
//		{
//		ALines.push_back(buf);
//		}
//	while (std::getline(Bifs, buf))
//		{
//		BLines.push_back(buf);
//		}
//
//	Diff< elem > diff(ALines, BLines);
//	diff.onHuge();
//	//diff.onUnserious();
//	diff.compose();
//
//	// type unihunk definition test
//	uniHunk< sesElem > hunk;
//
//	if (diff.getEditDistance() > 0)
//		{
//		std::ofstream fout(out);
//
//		showStats(fp1, fp2, fout);             // show file info
//
//		diff.composeUnifiedHunks();
//
//		diff.printUnifiedFormat(fout);
//		}
//}


long long GetEditDistance(const fs::path &path1, const fs::path &path2)
{
	typedef std::string                 elem;
	typedef std::vector< elem >         sequence;

	std::ifstream      Aifs(path1.c_str());
	std::ifstream      Bifs(path2.c_str());
	elem          buf;
	sequence      ALines, BLines;

	while (std::getline(Aifs, buf))
		{
		ALines.push_back(buf);
		}
	while (std::getline(Bifs, buf))
		{
		BLines.push_back(buf);
		}

	Diff< elem > diff(ALines, BLines);
	diff.onHuge();
	diff.onUnserious();
	diff.compose();

	return diff.getEditDistance();
}

auto PrintList(std::vector<ComparePaths>::const_iterator beginItr, std::vector<ComparePaths>::const_iterator endItr, std::ostream &out)
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

int GetProgramVersionNumber(fs::path root)
{
	fs::path versionsh = root;
	versionsh.append("Common\\versions.h");

	if (!fs::exists(versionsh))
		return 0;

	std::string search = "#define CD_VER";

	std::ifstream ifs(versionsh.string().c_str());
	std::string line;
	while (std::getline(ifs, line))
		{
		if (string_utils::starts_with(line, search))
			{
			std::string versionstr = line.substr(search.length() + 1);
			int version = std::atoi(versionstr.c_str());
			if (version > 0)
				return version;
			}
		}

	return 0;
}

std::vector<fs::path> GetProgramInstallPath(int version)
{
	auto InstallPath = [](const std::string & program)
		{
		fs::path p = R"(\\KBServer2\Dev\InstallSource)"; // Scheduled tasks need UNC paths instead of mapped drive letters
		p.append(program);
		p.append("Extras");
		return p;
		};

	std::vector<fs::path> paths =
		{
		InstallPath("AllyCAD" + std::to_string(version)),
		InstallPath("AllyCAD" + std::to_string(version) + "Student"),
		InstallPath("AllyCAD" + std::to_string(version) + "Innovate"),
		InstallPath("CivilDesigner" + std::to_string(version)),
		InstallPath("CivilDesigner" + std::to_string(version) + "Student"),
		};

	// erase any that don't exist, e.g. Innovate from 90+
	paths.erase(std::remove_if(paths.begin(), paths.end(), [](const fs::path & p)
		{
		return !fs::exists(p);
		}),
	paths.end());

	return paths;
}

int compare_extras(const fs::path &root, std::ostream &output, const CompareExtrasOptions &, ComparisonOutput &out)
{
	std::string title = "Comparison of Extras";
	std::string underline(title.length(), '=');
	output
		<< title 
		<< "\n"
		<< underline
		<< "\n\n";

	fs::path extras = filesystem_utils::search_up_for_parent(root, "Extras");
	extras.append("Extras");
	if (!fs::exists(extras))
		return 0;

	fs::path locales = filesystem_utils::search_up_for_parent(root, "CD_Resource");
	locales.append("CD_Resource").append("Extras");
	if (!fs::exists(locales))
		return 0;

	int version = GetProgramVersionNumber(root);
	std::vector<fs::path> installPaths = GetProgramInstallPath(version);

	std::vector<ComparePaths> installComp;
	std::vector<ComparePaths> localeComp;

	std::vector<fs::path> types = { ".mnu", ".tbr", ".acc", };

	for (const auto &extrasFile : fs::directory_iterator{ extras })
		{
		fs::path name = extrasFile.path().filename();
		fs::path ext = extrasFile.path().extension();

		for (const auto &install : installPaths)
			{
			fs::path searchFile = install;
			searchFile.append(name.string());

			if (fs::exists(searchFile))
				{
				ComparePaths comp;
				comp.m_path1 = extrasFile;
				comp.m_path2 = searchFile;
				installComp.push_back(comp);
				break;
				}
			}

		if (std::find(types.begin(), types.end(), ext) != types.end())
			{
			fs::path searchFile = locales;
			searchFile.append(name.string());
			searchFile.replace_extension();
			searchFile.concat("UK");
			searchFile.replace_extension(ext);

			if (fs::exists(searchFile))
				{
				ComparePaths comp;
				comp.m_path1 = extrasFile;
				comp.m_path2 = searchFile;
				localeComp.push_back(comp);
				}
			}
		}

	auto SetEditDistance = [](ComparePaths & comp)
		{
		comp.m_editDistance = GetEditDistance(comp.m_path1, comp.m_path2);
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

	std::vector<ComparePaths> cadComp;

	std::vector<std::string> variations = { "CadStudent", "CadInnovate", "CadCD", "CadStudentCD" };

	for (const auto &variation : variations)
		{
		for (const auto &ext : types)
			{
			fs::path cad1 = extras;
			fs::path cad2 = extras;
			cad1.append("Cad");
			cad2.append(variation);
			cad1.replace_extension(ext);
			cad2.replace_extension(ext);

			if (fs::exists(cad1) && fs::exists(cad2))
				{
				ComparePaths comp;
				comp.m_path1 = cad1;
				comp.m_path2 = cad2;
				cadComp.push_back(comp);
				}
			}
		}

	std::for_each(installComp.begin(), installComp.end(), SetEditDistance);
	std::sort(installComp.begin(), installComp.end(), ByEditDistanceDesc);
	auto installCompEndItr = std::remove_if(installComp.begin(), installComp.end(), IsZeroEditDistance);

	std::for_each(localeComp.begin(), localeComp.end(), SetEditDistance);
	std::sort(localeComp.begin(), localeComp.end(), ByEditDistanceDesc);
	auto localeCompEndItr = std::remove_if(localeComp.begin(), localeComp.end(), IsZeroEditDistance);

	std::for_each(cadComp.begin(), cadComp.end(), SetEditDistance);
	std::sort(cadComp.begin(), cadComp.end(), ByEditDistanceDesc);
	auto cadCompEndItr = std::remove_if(cadComp.begin(), cadComp.end(), IsZeroEditDistance);


	auto PrintHeading = [](const std::string &heading, std::ostream &output)
		{
		std::string underline(heading.length(), '-');
		output
			<< heading << "\n"
			<< underline << "\n";
		};

	PrintHeading("Comparisons with J:\\InstallSource", output);
	PrintList(installComp.begin(), installCompEndItr, output);
	output << "\n";

	PrintHeading("Comparisons with CD_Resource", output);
	PrintList(localeComp.begin(), localeCompEndItr, output);
	output << "\n";

	PrintHeading("Comparisons of Cad variations", output);
	PrintList(cadComp.begin(), cadCompEndItr, output);
	output << "\n";

	// Out parameter
	out.m_comparisons.insert(out.m_comparisons.end(), installComp.begin(), installComp.end());
	out.m_comparisons.insert(out.m_comparisons.end(), localeComp.begin(), localeComp.end());
	out.m_comparisons.insert(out.m_comparisons.end(), cadComp.begin(), cadComp.end());

	output << "Done\n\n";
	return 0;
}
