#pragma once

// String utilities
std::string trim(const std::string& value, const std::string& chars);
bool starts_with(const std::string& value, const std::string& test);
bool ends_with(const std::string& value, const std::string& ending);
std::vector<std::string> split(const std::string& value, char delimiter);
std::string lowercase(const std::string& str);
bool is_title_case(const std::string& str);
bool is_sentence_case(const std::string& str);
std::string erase_substr(const std::string& str, const std::string& substr);