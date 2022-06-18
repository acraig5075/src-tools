#include "pch.h"
#include "string-utils.h"
#include "filesystem-utils.h"

namespace fs = std::filesystem;

// This checks for a valid command name format and convention
bool is_valid_command_name(const std::string &value)
{
	for (int c : value)
		{
		if (!(std::isupper(c) || std::isdigit(c) || c == '_'))
			return false;
		}

	return true;
}


void process_line(const std::string &line, std::vector<std::string> &commands)
{
	if (line.empty() || starts_with(line, R"(//)") || starts_with(line, "#"))
		return;

	std::vector<std::string> tokens;

	tokens = split(line, ',');
	if (tokens.size() != 3)
		return;

	tokens = split(tokens[0], '\"');
	if (tokens.size() != 3)
		return;

	if (!tokens[1].empty())
		commands.push_back(tokens[1]);
}

// This will get a list of commands found in ImagesAndCommands.h
std::vector<std::string> get_images_commands(const fs::path &filename)
{
	std::vector<std::string> commands;

	bool readDecl = false;
	bool readStart = false;
	std::string line;

	std::ifstream fin(filename);

	while (std::getline(fin, line))
		{
		line = trim(line, " \t,");

		if (line.empty())
			continue;

		else if (readStart)
			{
			if (line == "};")
				break;
			else
				process_line(line, commands);
			}

		else if (readDecl && line == "{")
			readStart = true;

		else if (!readDecl && line == "CString commandNames[][3] =")
			readDecl = true;
		}

	std::sort(begin(commands), end(commands));

	return commands;
}

// This will get a list of commands used in all the .mnu files
std::vector<std::string> get_menu_commands(const std::vector<fs::path> &mnuFiles)
{
	std::vector<std::string> commands;

	for (auto path : mnuFiles)
		{
		std::string line;
		std::ifstream fin(path.string());

		while (std::getline(fin, line))
			{
			std::vector<std::string> tokens = split(line, ',');
			if (tokens.size() == 3)
				{
				std::string cmd = trim(tokens[1], " ");
				if (is_valid_command_name(cmd))
					commands.push_back(cmd);
				}
			}
		}

	std::sort(commands.begin(), commands.end());

	commands.erase(std::unique(commands.begin(), commands.end()), commands.end());

	return commands;
}

// Thisi will get a list of commands found in one of the api files
std::vector<std::string> get_api_commands(const fs::path &path)
{
	std::vector<std::string> commands;

	std::string line;
	std::ifstream fin(path.string());

	while (std::getline(fin, line))
		{
		line = trim(line, " \t");

		if (line.find("AddFunctionToMap(", 0) == std::string::npos)
			continue;

		if (starts_with(line, "//"))
			continue;

		std::vector<std::string> tokens = split(line, '\"');
		if (tokens.size() != 3)
			continue;

		commands.push_back(tokens[1]);
		}

	std::sort(commands.begin(), commands.end());

	return commands;
}

int search_commands_not_in_imageandcommands(const fs::path &rootPath, std::ostream &output)
{
	fs::path imagesAndCommands = search_for_filename(rootPath, "ImagesAndCommands.h");
	if (imagesAndCommands.empty())
		return 1;

	std::vector<fs::path> apiFiles = get_file_list(rootPath, "API.cpp");
	std::vector<fs::path> mnuFiles = get_file_list(rootPath, ".mnu");

	std::vector<std::string> imagesCommands = get_images_commands(imagesAndCommands);
	std::vector<std::string> menuCommands   = get_menu_commands(mnuFiles);

	size_t count = 0;

	for (const auto &f : apiFiles)
		{
		std::vector<std::string> apiCommands = get_api_commands(f);
		std::vector<std::string> usedCommands;
		std::vector<std::string> missingCommands;

		// Get the subset of commands both in the api file and in the menu files. This will discard unimportant commands such as testing or debugging commands
		std::set_intersection(begin(apiCommands), end(apiCommands), begin(menuCommands), end(menuCommands), std::back_inserter(usedCommands));

		// Get the subset of commands that are used but not in imagesAndCommands.h
		std::set_difference(begin(usedCommands), end(usedCommands), begin(imagesCommands), end(imagesCommands), std::back_inserter(missingCommands));

		// Report what is missing
		if (!missingCommands.empty())
			{
			std::string filename = f.filename().string();
			std::string underline(filename.length(), '-');
			output
					<< filename << "\n"
					<< underline << "\n";

			std::copy(begin(missingCommands), end(missingCommands), std::ostream_iterator<std::string>(output, "\n"));
			output << "\n";
			}

		count += missingCommands.size();
		}

	output << count << " commands\n";

	output << "Done\n";

	return 0;
}
