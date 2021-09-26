#pragma once

#include <filesystem>

int search_commands_not_in_imageandcommands(const std::filesystem::path& rootPath, std::ostream& output);
int search_duplicate_string_resources(const std::filesystem::path& input, std::ostream& output, int format = 0);
int search_unused_string_resources(const std::filesystem::path& root, int mode, std::ostream& output);
int search_cpp_files_missing_debug_new_macro(const std::filesystem::path& root, std::ostream& output);
int examine_rc_file_for_conformity(const std::filesystem::path& input, std::ostream& output, int format = 0);
int search_tooltips_exceeding_max_length(const std::filesystem::path& root, std::ostream& output, size_t maximum);
int search_resizable_not_in_reg_reset(const std::filesystem::path& root, std::ostream& output);
