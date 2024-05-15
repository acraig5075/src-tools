#include "pch.h"
#include "filesystem-utils.h"
#include "string-utils.h"

namespace fs = std::filesystem;
using namespace string_utils;

namespace filesystem_utils
	{

	// This will return a list of files ending with the specified suffix
	std::vector<fs::path> get_file_list(const fs::path &root, const std::string &suffix)
		{
		std::vector<fs::path> files;

		for (const auto &itr : fs::recursive_directory_iterator(root))
			{
			if (ends_with(itr.path().string(), suffix))
				{
				files.push_back(itr.path());
				}
			}

		return files;
		}


	// This will return a list of files ending with the specified suffix, case sensitive
	std::vector<fs::path> get_file_list_case_sensitive(const fs::path &root, std::string suffix)
		{
		std::vector<fs::path> files;

		suffix = lowercase(suffix);

		for (const auto &itr : fs::recursive_directory_iterator(root))
			{
			if (ends_with(lowercase(itr.path().string()), suffix))
				{
				files.push_back(itr.path());
				}
			}

		return files;
		}


	// This will return a list of directories, optionally with or without root included (see is_solution_folder)
	std::vector<fs::path> get_directory_list(const fs::path &root, bool inclusive)
		{
		std::vector<fs::path> paths;

		if (inclusive && fs::is_directory(root))
			{
			paths.push_back(root);
			}

		for (const auto &itr : fs::directory_iterator(root))
			{
			if (fs::is_directory(itr))
				{
				paths.push_back(itr.path());
				}
			}

		return paths;
		}


	// Removed named directories from a directory list
	void filter_directory_list(std::vector<fs::path> &directory_list, const std::vector<std::string> &filter_list)
		{
		for (auto name : filter_list)
			{
			auto itr = std::find_if(directory_list.begin(), directory_list.end(), [name](const fs::path &dir)
				{
				return dir.string().find(name, 0) != std::string::npos;
				});

			if (itr != directory_list.end())
				{
				directory_list.erase(itr);
				}
			}
		}


	// Search for a filename
	fs::path search_for_filename(const fs::path &root, const std::string &name)
		{
		for (const auto &itr : fs::recursive_directory_iterator(root))
			{
			if (itr.is_regular_file() && itr.path().filename().string() == name)
				{
				return itr.path();
				}
			}

		return {};
		}


	// extract the .rc and resource.h filenames for a folder
	bool get_resource_filenames(const std::filesystem::path &folder, std::filesystem::path &rc, std::filesystem::path &header)
		{
		bool ok1 = false;
		bool ok2 = false;

		for (const auto &itr : fs::recursive_directory_iterator(folder))
			{
			if (itr.is_regular_file())
				{
				if (!ok1 && ends_with(itr.path().filename().string(), ".rc"))
					{
					rc = itr.path();
					ok1 = true;
					}
				else if (!ok2 && lowercase(itr.path().filename().string()) == "resource.h")
					{
					header = itr.path();
					ok2 = true;
					}

				if (ok1 && ok2)
					break;
				}
			}

		return ok1 && ok2;
		}

	// Determine if the folder is the .sln folder, and therefore can be considered the root of the entire solution
	bool is_solution_folder(const std::filesystem::path &folder)
		{
		bool sln = false;
		bool vcxproj = false;

		for (const auto &itr : fs::directory_iterator(folder))
			{
			if (itr.is_regular_file())
				{
				if (itr.path().extension() == ".sln")
					{
					sln = true;
					}
				else if (itr.path().extension() == ".vcxproj")
					{
					vcxproj = true;
					}
				}
			}

		return sln && !vcxproj;
		}

	// Search higher in the folder structure for a path containing the given name
	fs::path search_up_for_parent(const fs::path &root, const std::string &name)
		{
		fs::path parent = root;
		fs::path search;

		while (true)
			{
			search = parent;
			if (search.empty())
				break;

			search.append(name);
			if (fs::exists(search))
				break;

			parent = parent.parent_path();
			}

		return parent;
		}

	}