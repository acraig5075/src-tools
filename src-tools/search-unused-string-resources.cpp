// search-unused-string-resources.cpp : Defines the entry point for the console application.
//

#include "pch.h"

namespace fs = std::filesystem;

using PathResourcePair = std::pair<fs::path, std::vector<std::string>>;



static std::vector<PathResourcePair> get_file_list(const fs::path &root, const std::string &extension)
{
	std::vector<PathResourcePair> files;

	// intentionally not including root

	for (const auto &itr : fs::recursive_directory_iterator(root))
		{
		if (itr.path().extension() == extension)
			{
			files.push_back(std::make_pair(itr.path(), std::vector<std::string>()));
			}
		}

	return files;
}



void find_resource_strings(const fs::path &filename, std::vector<std::string> &resources)
{
	std::ifstream fs(filename);
	std::string line;

	while (std::getline(fs, line))
		{
		if (!line.empty())
			{
			size_t off = 0;
			size_t start = line.find("IDS_", off);
			while (start != std::string::npos)
				{
				size_t end = line.find_first_of(" ,;)]\t}", start);
				std::string token = line.substr(start, end - start);
				resources.push_back(token);
				off = start + 1;
				start = line.find("IDS_", off);
				}
			}
		}
}



std::vector<std::string> get_resource_list(const fs::path &root, const std::string &extension)
{
	auto files = get_file_list(root, extension);

	std::for_each(begin(files), end(files), [](PathResourcePair & p)
		{
		find_resource_strings(p.first, p.second);
		});

	std::vector<std::string> resources;
	std::for_each(begin(files), end(files), [&](const PathResourcePair & p)
		{
		resources.insert(resources.end(), begin(p.second), end(p.second));
		});

	std::sort(begin(resources), end(resources));
	resources.erase(std::unique(begin(resources), end(resources)), end(resources));

	return resources;
}


static std::vector<fs::path> get_directory_list(const fs::path &root)
{
	std::vector<fs::path> paths;

	for (const auto &itr : fs::directory_iterator(root))
		{
		if (fs::is_directory(itr))
			{
			paths.push_back(itr.path());
			}
		}

	return paths;
}


void exclusions(std::vector<fs::path> &directories, const std::vector<std::string> &exclusions)
{
	for (auto name : exclusions)
		{
		auto itr = std::find_if(directories.begin(), directories.end(), [name](const fs::path & dir)
			{
			return dir.string().find(name, 0) != std::string::npos;
			});

		if (itr != directories.end())
			directories.erase(itr);
		}
}


/// Lookup resource ID names defined in .rc files but not referenced in any .cpp files
int my_main1(const fs::path &root, std::ostream &output)
{
	std::vector<fs::path> directories = get_directory_list(root);

	exclusions(directories, 
		{
		"CommonDialogs",
		"Sewer",
		"Storm",
		}
	);

	for (const auto &dir : directories)
		{
		fs::path root(dir);

		std::vector<std::string> used = get_resource_list(root, ".cpp");
		std::vector<std::string> avail = get_resource_list(root, ".rc");

		std::vector<std::string> difference;
		std::set_difference(begin(avail), end(avail), begin(used), end(used), std::back_inserter(difference));

		if (difference.empty())
			continue;

		std::string heading(dir.filename().string());
		std::string underline(heading.length(), '-');
		output
				<< heading << "\n"
				<< underline << "\n";

		std::copy(begin(difference), end(difference), std::ostream_iterator<std::string>(output, "\n"));
		output << "\n";
		}

	return 0;
}


struct Query
{
	std::string number;
	std::string name;
	std::string literal;
};

void UpdateQuery(Query &query, std::string &line, std::string Query::* field)
{
	std::string name = query.name;

	size_t found = line.find(name, 0);

	if (found != std::string::npos)
		{
		if (line[found + name.length()] == ' ')
			{
			size_t off = found + name.length();
			size_t start = line.find_first_not_of(' ', off);

			if (start != std::string::npos)
				{
				std::string value = line.substr(start);
				query.*field = value;
				}
			}
		}
}

void UpdateAllQueries(std::vector<Query> &queries, const std::string &filename, std::string Query::* field)
{
	std::ifstream fin(filename);
	std::string line;
	while (std::getline(fin, line))
		{
		for (auto &q : queries)
			{
			UpdateQuery(q, line, field);
			}
		}
}

/// Lookup string resource number and literal for a list of resource ID names
int my_main2(std::ostream &output)
{
	std::vector<std::string> problems =
		{
		"IDS_3D_POLYFACE_MESH",
		"IDS_3D_POLYGON_MESH",
		"IDS_ALLAYERS",
		"IDS_ALLUSEDLAYERS",
		"IDS_AMP_SET_CURRENT",
		"IDS_ARROW",
		"IDS_BELOW",
		"IDS_BR",
		"IDS_CIVILENGINEERING",
		"IDS_CLICK_SELECT_THIS",
		"IDS_DWG_PROXY",
		"IDS_ENTERVALUE",
		"IDS_GENERICLISTDLG_TT1",
		"IDS_GENERICLISTDLG_TT2",
		"IDS_GENERICLISTDLG_TT3",
		"IDS_GENERICLISTDLG_TT4",
		"IDS_GETDETAILS_GROUP_NAME_F",
		"IDS_GETDETAILS_GROUP_SIZE_F",
		"IDS_GFX_ALLOCATE_FAILED",
		"IDS_HAVE_PURCHASE_SALES",
		"IDS_INTENSITY_SELECTED_LIGHT",
		"IDS_LENGTH1",
		"IDS_LIGHT",
		"IDS_MAPPING_POLYGON",
		"IDS_MECHENGINEERING",
		"IDS_METRICCUSTOM",
		"IDS_MULTILINE_TEXT",
		"IDS_MULTIPLE_SURVEY_XREF_EXIT",
		"IDS_OLE_CONTAINER",
		"IDS_PENMAPDLG_NOCHANGE",
		"IDS_POLYLINE",
		"IDS_PROPDESC_DECIMALS",
		"IDS_REACTIVE_TEXT",
		"IDS_RENDERGUI_DIFFUSE",
		"IDS_RENDER_ALTITUDE_LABEL",
		"IDS_SURVEY",
		"IDS_SURVEY_DIMENSION",
		"IDS_TABLEGRID_CM_ALIGNLEFT",
		"IDS_TRACK_NO_PATH_FOUND",
		"IDS_USEDRIVERLIMITS",
		"IDS_USERDEFINED",
		"IDS_USERDEFINEDSIZE",
		"IDS_VALUE",
		};

	std::string h_file = R"(D:\Src\Trunk_VS2013\CivilDesigner\CAD\Resource.h)";
	std::string rc_file = R"(D:\Src\Trunk_VS2013\CivilDesigner\CAD\CAD.rc)";

	std::vector<Query> queries;

	for (const auto &p : problems)
		{
		Query q;
		q.name = p;
		queries.push_back(q);
		}

	UpdateAllQueries(queries, h_file, &Query::number);
	UpdateAllQueries(queries, rc_file, &Query::literal);

	size_t max1 = std::numeric_limits<size_t>::lowest();
	size_t max2 = std::numeric_limits<size_t>::lowest();
	size_t max3 = std::numeric_limits<size_t>::lowest();

	for (const auto &q : queries)
		{
		if (q.name.length() > max1)
			max1 = q.name.length();
		if (q.number.length() > max2)
			max2 = q.number.length();
		if (q.literal.length() > max3)
			max3 = q.literal.length();
		}

	max1++;
	max2++;
	max3++;

	for (const auto &q : queries)
		{
		output
				<< std::left
				<< std::setw(max1) << q.name
				<< std::setw(max2) << q.number
				<< std::setw(max3) << q.literal
				<< "\n";
		}

	return 0;
}

/// Search all .cpp files for a search term
int my_main3(std::ostream &output)
{
	fs::path root = R"(D:\Src\Trunk_VS2013\CivilDesigner)";

	size_t files = 0;

	for (auto &itr : fs::recursive_directory_iterator(root))
		{
		fs::path path = itr.path();

		if (path.extension() == ".cpp")
			{
			++files;

			std::ifstream file(path);
			std::string line;

			while (std::getline(file, line))
				{
				if (line.find("\"IDS_") != std::string::npos)
					{
					output << path.string() << "\n";
					}
				}
			}
		}

	output << std::to_string(files) << " .cpp files\n";
	return 0;
}

int search_unused_string_resources(const fs::path &root, int mode, std::ostream &output)
{
	switch (mode)
		{
		case 1:
			my_main1(root, output);
			break;
		case 2:
			my_main2(output);
			break;
		case 3:
			my_main3(output);
			break;
		default:
			break;
		}

	output << "Done\n";

	return 0;
}