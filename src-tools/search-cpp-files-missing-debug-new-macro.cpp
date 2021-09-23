#include "pch.h"

namespace fs = std::filesystem;


/// Search all .cpp files for a search term
int search_cpp_files_missing_debug_new_macro(const fs::path &root, std::ostream &output)
{
	size_t files = 0;

	for (auto &itr : fs::recursive_directory_iterator(root))
		{
		fs::path path = itr.path();

		if (path.extension() == ".cpp" && path.filename() != "stdafx.cpp")
			{
			std::ifstream file(path);
			std::string line;
			bool found = false;

			while (std::getline(file, line))
				{
				if (line.find("#define new DEBUG_NEW") != std::string::npos)
					{
					found = true;
					break;
					}
				}

			if (!found)
				{
				output << path.string() << "\n";
				}
			}
		}

	output << "Done\n";
	return 0;
}
