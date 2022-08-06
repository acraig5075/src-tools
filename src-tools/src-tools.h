#pragma once

#include <filesystem>
#include "options.h"

int search_commands_not_in_imageandcommands(const std::filesystem::path& rootPath, std::ostream& output, ImagesAndCommandsOptions &options);
int search_duplicate_string_resources(const std::filesystem::path& input, std::ostream& output, DuplicateStringsOptions &options);
int search_unused_string_resources(const std::filesystem::path& root, std::ostream& output, UnusedStringsOptions &options);
int search_cpp_files_missing_debug_new_macro(const std::filesystem::path& root, std::ostream& output, MissingMacroOptions &options);
int examine_rc_file_for_conformity(const std::filesystem::path& input, std::ostream& output, RcFileRulesOptions &options);
int search_tooltips_exceeding_max_length(const std::filesystem::path& root, std::ostream& output, TooltipLengthOptions &options);
int search_resizable_not_in_reg_reset(const std::filesystem::path& root, std::ostream& output, RegResetOptions &options);
