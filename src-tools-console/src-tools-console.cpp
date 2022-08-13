// src-tools-console.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <windows.h>
#include <shlwapi.h>
#include "..\src-tools\src-tools.h"

void usage()
{
	std::cout << "Various source tree tools by Alasdair Craig.\n";
	std::cout << "Usage: program.exe tool-number [path|file] [optionsfile]\n";
	std::cout << "tool-number  : Tool number as follows:\n";
	std::cout << "           1 : search_commands_not_in_imageandcommands_h\n";
	std::cout << "           2 : search_duplicate_string_resources\n";
	std::cout << "           3 : search_unused_string_resources\n";
	std::cout << "           4 : search_cpp_files_missing_debug_new_macro\n";
	std::cout << "           5 : examine_rc_file_for_conformity\n";
	std::cout << "           6 : search_tooltips_exceeding_max_length\n";
	std::cout << "           7 : search_resizable_not_in_reg_reset\n";
	std::cout << "path         : Root folder path of source tree for multiple .rc files to be parsed.\n";
	std::cout << "file         : File path for a single .rc file to be parsed.\n";
	std::cout << "optionsfile  : .options file [Default looks in .exe folder]\n";
}

int main(int argc, char* argv[])
{
	if (argc == 1)
		{
		usage();
		return EXIT_FAILURE;
		}

	int tool = atoi(argv[1]);
	if (tool == 0)
		{
		usage();
		return EXIT_FAILURE;
		}

	std::filesystem::path rootPath;
	if (argc > 2)
		rootPath = argv[2];
	else
		rootPath = R"(D:\Src\Trunk_VS2013\CivilDesigner)";

	char dfltOptions[MAX_PATH];
	GetModuleFileNameA(0, dfltOptions, MAX_PATH);
	PathRemoveFileSpecA(dfltOptions);
	PathAppendA(dfltOptions, "src-tools.options");

	std::filesystem::path optionsFile;
	if (argc > 3)
		optionsFile = argv[3];
	else
		optionsFile = dfltOptions;

	Options options;
	ReadOptions(optionsFile.string(), options);

	switch (tool)
		{
		case 1:
			search_commands_not_in_imageandcommands(rootPath, std::cout, options.m_imagesAndCommandsOpts);
			break;
		case 2:
			search_duplicate_string_resources(rootPath, std::cout, options.m_duplicateStringsOpts);
			break;
		case 3:
			search_unused_string_resources(rootPath, std::cout, options.m_unusedStringsOpts);
			break;
		case 4:
			search_cpp_files_missing_debug_new_macro(rootPath, std::cout, options.m_missingMacroOpts);
			break;
		case 5:
			examine_rc_file_for_conformity(rootPath, std::cout, options.m_rcFileRulesOpts);
			break;
		case 6:
			search_tooltips_exceeding_max_length(rootPath, std::cout, options.m_tooltipLengthOpts);
			break;
		case 7:
			search_resizable_not_in_reg_reset(rootPath, std::cout, options.m_regResetOpts);
			break;
		default:
			usage();
			return EXIT_FAILURE;
		}

	return EXIT_SUCCESS;
}
