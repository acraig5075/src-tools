// src-tools-console.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "..\src-tools\src-tools.h"

void usage()
{
	std::cout << "Various source tree tools by Alasdair Craig.\n";
	std::cout << "Usage: program.exe tool-number [path|file] [output-format]\n";
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
	std::cout << "output-format: 0= Group by rule convention (default); 1= Group by dialog\n";
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

	int format;
	if (argc > 3)
		format = atoi(argv[3]);
	else
		format = 0;

	ImagesAndCommandsOptions imagesAndCommandsOpts;
	DuplicateStringsOptions duplicateStringsOpts;
	UnusedStringsOptions unusedStringsOpts;
	MissingMacroOptions missingMacroOpts;
	RcFileRulesOptions rcFileRulesOpts;
	TooltipLengthOptions tooltipLengthOpts;
	RegResetOptions regResetOpts;

	duplicateStringsOpts.m_outputFormat = format;
	rcFileRulesOpts.m_outputFormat = format;

	switch (tool)
		{
		case 1:
			search_commands_not_in_imageandcommands(rootPath, std::cout, imagesAndCommandsOpts);
			break;
		case 2:
			search_duplicate_string_resources(rootPath, std::cout, duplicateStringsOpts);
			break;
		case 3:
			search_unused_string_resources(rootPath, std::cout, unusedStringsOpts);
			break;
		case 4:
			search_cpp_files_missing_debug_new_macro(rootPath, std::cout, missingMacroOpts);
			break;
		case 5:
			examine_rc_file_for_conformity(rootPath, std::cout, rcFileRulesOpts);
			break;
		case 6:
			search_tooltips_exceeding_max_length(rootPath, std::cout, tooltipLengthOpts);
			break;
		case 7:
			search_resizable_not_in_reg_reset(rootPath, std::cout, regResetOpts);
			break;
		default:
			usage();
			return EXIT_FAILURE;
		}

	return EXIT_SUCCESS;
}
