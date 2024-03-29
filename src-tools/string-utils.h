#pragma once

// String utilities

namespace string_utils
{
	std::string trim(const std::string &value, const std::string &chars);
	bool starts_with(const std::string &value, const std::string &test);
	bool ends_with(const std::string &value, const std::string &ending);
	std::vector<std::string> split(const std::string &value, char delimiter);
	std::vector<std::string> quote_aware_split(const std::string &line, char delimiter);
	std::string lowercase(const std::string &str);
	bool is_title_case(const std::string &str);
	bool is_sentence_case(const std::string &str);
	std::string erase_substr(const std::string &str, const std::string &substr);
	std::string pretty_print_json11(const std::string &text);
	bool replace_substr(std::string &str, const std::string &substr, const std::string &replacement);
	std::string longest_common_substring(const std::vector<std::string> &strings);
	size_t find_substr_exact(const std::string &str, const std::string &substr, size_t off);
	std::string get_xml_text_attribute(const std::string &line, const std::string &attr);
}