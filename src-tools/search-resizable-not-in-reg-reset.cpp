#include "pch.h"
#include "string-utils.h"

namespace fs = std::filesystem;

struct ResizableDialog
{
	std::string filename;
	std::string resourceName;
	std::string resourceID;
};

bool SortByID (const ResizableDialog& lhs, const ResizableDialog& rhs)
{
	return lhs.resourceID < rhs.resourceID;
}

bool SortByName(const ResizableDialog& lhs, const ResizableDialog& rhs)
{
	return lhs.resourceID < rhs.resourceID;
}


std::string ExtractName(const std::string& line)
{
	std::string prefix = "IDD = ";
	size_t begin = line.find(prefix) + prefix.length();
	if (begin > line.length())
		return "";

	size_t end = line.find_first_of(" \t}", begin);
	if (end == std::string::npos)
		return line.substr(begin);

	return line.substr(begin, end - begin);
}

std::string ExtractBCGP(const std::string& line)
{
	std::string prefix = "\"BCGPDialog-";
	size_t begin = line.find(prefix) + prefix.length();
	if (begin > line.length())
		return "";

	size_t end = line.find("\"", begin);
	if (end == std::string::npos)
		return "";

	return line.substr(begin, end - begin);
}

// This will return a list of files ending with the specified suffix
static std::vector<fs::path> get_file_list(const fs::path& root, const std::string& suffix)
{
	std::vector<fs::path> files;

	auto suffix_ = lowercase(suffix);

	for (const auto& itr : fs::recursive_directory_iterator(root))
	{
		if (ends_with(lowercase(itr.path().string()), suffix_))
		{
			files.push_back(itr.path());
		}
	}

	return files;
}

fs::path get_reg_reset(const fs::path& root)
{
	for (const auto& itr : fs::recursive_directory_iterator(root))
	{
		if (itr.path().filename() == "RegResetDlg.cpp")
			return itr.path();
	}

	return {};
}


void iterate_resizable_dialogs(const fs::path& root, const std::unordered_map<std::string, std::string> &ids, std::vector<ResizableDialog> &dialogs)
{
	for (auto& itr : fs::recursive_directory_iterator(root))
	{
		fs::path path = itr.path();

		if (path.extension() == ".h")
		{
			std::ifstream file(path);
			std::string line;
			bool found = false;

			while (std::getline(file, line))
			{
				if (!found && (line.find("public CCADResizableDialog") != std::string::npos))
				{
					found = true;
				}
				if (found && line.find("IDD = ") != std::string::npos)
				{
					std::string name = ExtractName(line);
					auto itr = ids.find(name);

					if (!name.empty() && itr != ids.end())
					{
						ResizableDialog dlg;
						dlg.filename = path.string();
						dlg.resourceID = itr->second;
						dlg.resourceName = name;
						dialogs.push_back(dlg);
					}
				}
			}
		}
	}
}

void map_dialog_resources(const fs::path& root, std::unordered_map<std::string, std::string> &ids)
{
	auto resources = get_file_list(root, "resource.h");

	for (const auto filename : resources)
	{
		std::ifstream file(filename);
		std::string line;

		while (std::getline(file, line))
		{
			size_t pos = line.find("#define IDD_");
			if (pos == std::string::npos)
				continue;
			size_t field1 = pos + 8;

			pos = line.find_last_of(" ");
			if (pos == std::string::npos)
				continue;
			size_t field2 = pos + 1;

			std::string name = line.substr(field1, field2 - field1);
			std::string id = line.substr(field2);
			name = trim(name, " ");

			if (name.empty() || id.empty())
				continue;

			ids[name] = id;
		}
	}
}

void get_registered_dialogs(const fs::path& root, std::vector<std::string> &registered)
{
	auto path = get_reg_reset(root);
	if (!fs::is_regular_file(path))
		return;

	std::ifstream file(path.string());
	std::string line;

	while (std::getline(file, line))
	{
		if (line.find("WriteProfileString") != std::string::npos && line.find("\"BCGPDialog-") != std::string::npos)
		{
			std::string id = ExtractBCGP(line);
			if (!id.empty())
			{
				registered.push_back(id);
			}
		}
	}
}

/// Search all resizable headers not referenced in RegReset project
int search_resizable_not_in_reg_reset(const fs::path& root, std::ostream& output)
{
	std::unordered_map<std::string, std::string> ids;
	map_dialog_resources(root, ids);

	std::vector<ResizableDialog> dialogs;
	iterate_resizable_dialogs(root, ids, dialogs);

	std::vector<std::string> registered;
	get_registered_dialogs(root, registered);

	std::sort(dialogs.begin(), dialogs.end(), SortByID);
	std::sort(registered.begin(), registered.end());

	dialogs.erase(std::remove_if(dialogs.begin(), dialogs.end(), [&](const auto& dlg) 
		{
			return std::binary_search(registered.begin(), registered.end(), dlg.resourceID);
		}), dialogs.end());

	std::sort(dialogs.begin(), dialogs.end(), SortByName);
	
	for (const auto& dlg : dialogs)
	{
		output << dlg.resourceName << "\t" << dlg.filename << "\n";
	}

	output << "Done\n";
	return 0;
}
