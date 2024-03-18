#include "pch.h"
#include "string-utils.h"
#include "filesystem-utils.h"
#include "options.h"

namespace fs = std::filesystem;
using namespace string_utils;

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

						found = false; // don't break because there may be another resizable dialog in this header
					}
				}
			}
		}
	}
}

void map_dialog_resources(const fs::path& root, std::unordered_map<std::string, std::string> &ids)
{
	auto resources = filesystem_utils::get_file_list_case_sensitive(root, "resource.h");

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
	auto path = filesystem_utils::search_for_filename(root, "RegResetDlg.cpp");
	if (path.empty())
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
int search_resizable_not_in_reg_reset(const fs::path& root, std::ostream& output, const RegResetOptions &)
{
	std::string title = "Resizable dialogs not in RegResetDlg.cpp";
	std::string underline(title.length(), '=');
	output
		<< title
		<< "\n"
		<< underline
		<< "\n\n";

	std::unordered_map<std::string, std::string> ids;
	map_dialog_resources(root, ids);

	std::vector<ResizableDialog> dialogs;
	iterate_resizable_dialogs(root, ids, dialogs);

	std::vector<std::string> registered;
	get_registered_dialogs(root, registered);

	std::sort(dialogs.begin(), dialogs.end(), SortByID);
	std::sort(registered.begin(), registered.end());

	// ignore the dialogs that do have the required registry entry
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
