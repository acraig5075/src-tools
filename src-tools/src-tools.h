#pragma once

#include <filesystem>
#include "options.h"

struct UnusedStringsOptions;
struct UnusedStringsOutput;
struct UnusedResources;

int search_commands_not_in_imageandcommands(const std::filesystem::path& rootPath, std::ostream& output, const ImagesAndCommandsOptions &options);
int search_duplicate_string_resources(const std::filesystem::path& input, std::ostream& output, const DuplicateStringsOptions &options);
int search_unused_string_resources(const std::filesystem::path& root, std::ostream& output, const UnusedStringsOptions &options, UnusedStringsOutput &out);
int search_cpp_files_missing_debug_new_macro(const std::filesystem::path& root, std::ostream& output, const MissingMacroOptions &options);
int examine_rc_file_for_conformity(const std::filesystem::path& input, std::ostream& output, const RcFileRulesOptions &options);
int search_tooltips_exceeding_max_length(const std::filesystem::path& root, std::ostream& output, const TooltipLengthOptions &options);
int search_resizable_not_in_reg_reset(const std::filesystem::path& root, std::ostream& output, const RegResetOptions &options);

void delete_unused_string_resources(UnusedResources& report);
