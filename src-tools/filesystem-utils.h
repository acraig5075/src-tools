#pragma once

// Filesystem utilities
std::vector<std::filesystem::path> get_file_list(const std::filesystem::path &root, const std::string &suffix);
std::vector<std::filesystem::path> get_file_list_case_sensitive(const std::filesystem::path &root, std::string suffix);
std::vector<std::filesystem::path> get_directory_list(const std::filesystem::path &root, bool inclusive = true);
void filter_directory_list(std::vector<std::filesystem::path> &directory_list, const std::vector<std::string> &filter_list);
std::filesystem::path search_for_filename(const std::filesystem::path &root, const std::string &name);
bool get_resource_filenames(const std::filesystem::path &folder, std::filesystem::path &rc, std::filesystem::path &header);