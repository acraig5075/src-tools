#include "pch.h"
#include "string-utils.h"

// Front and back trimming
std::string trim(const std::string &value, const std::string &chars)
{
	auto right_trim = [chars](const std::string & str) -> std::string
		{
		size_t endpos = str.find_last_not_of(chars);
		if (std::string::npos != endpos)
			{
			return str.substr(0, endpos + 1);
			}
		return{};
		};

	auto left_trim = [chars](const std::string & str) -> std::string
		{
		size_t startpos = str.find_first_not_of(chars);
		if (std::string::npos != startpos)
			{
			return str.substr(startpos);
			}
		return{};
		};

	return left_trim(right_trim(value));
}


// Test for a prefix
bool starts_with(const std::string &value, const std::string &test)
{
	return value.compare(0, test.length(), test) == 0;
}


// Test for a suffix
bool ends_with(const std::string &value, const std::string &ending)
{
	if (value.length() >= ending.length())
		return (0 == value.compare(value.length() - ending.length(), ending.length(), ending));
	return false;
}


// Tokenise a string
std::vector<std::string> split(const std::string &value, char delimiter)
{
	std::vector<std::string> ret;

	size_t start = 0;
	while (true)
		{
		size_t pos = value.find(delimiter, start);
		if (pos == -1)
			{
			ret.push_back(value.substr(start));
			break;
			}
		else
			{
			ret.push_back(value.substr(start, pos - start));
			start = pos + 1;
			}
		}

	return ret;
}

// Tokenise a string that may have quoted fields
std::vector<std::string> quote_aware_split(const std::string &line, char delimiter)
{
	std::vector<std::string> values;
	std::string value;
	bool in_quoted = false;

	for (const char *p = line.c_str(); *p; ++p)
		{
		if (*p == delimiter && !in_quoted)
			{
			values.push_back(value);
			value.clear();
			}
		else if (*p == '"')
			{
			if (in_quoted)
				{
				if (p[1] == '"')
					value += *++p;
				else
					in_quoted = false;
				}
			else
				in_quoted = true;
			}
		else
			value += *p;
		}

	values.push_back(value);
	return values;
};

// Convert to lowercase
std::string lowercase(const std::string &str)
{
	std::string ret = str;
	std::transform(begin(ret), end(ret), begin(ret), [](unsigned char c)
		{
		return std::tolower(c);
		});
	return ret;
}


// Title-case exceptions
static std::vector<std::string> conjunctions =
{
	"and",
	"from",
	"to",
	"of",
	"for",
	"the",
	"by",
	"on",
	"a",
	"an",
	"at",
	"with",
};

// Test for title case
bool is_title_case(const std::string &str)
{
	auto words = split(trim(str, "."), ' ');

	auto newEnd = std::remove_if(begin(words), end(words), [](const std::string & str)
		{
		return std::find(begin(conjunctions), end(conjunctions), str) != end(conjunctions);
		});

	for (auto itr = begin(words); itr != newEnd; ++itr)
		{
		if ((*itr)[0] >= 97 && (*itr)[0] <= 122)
			return false;
		}

	return true;
}


// Test for sentence case
bool is_sentence_case(const std::string &str)
{
	auto words = split(str, ' ');

	bool first = true;

	for (auto itr = begin(words); itr != end(words); ++itr)
		{
		if (first)
			{
			if ((*itr)[0] >= 97 && (*itr)[0] <= 122)
				return false;
			}
		else
			{
			if ((*itr)[0] >= 65 && (*itr)[0] <= 90)
				return false;
			}

		first = false;
		}

	return true;
}


// Erase a substring
std::string erase_substr(const std::string &str, const std::string &substr)
{
	std::string ret = str;

	size_t fnd = ret.find(substr, 0);
	if (fnd != std::string::npos)
		ret.erase(fnd, substr.size());

	return ret;
}


// Replace every occurence of a substring with another
bool replace_substr(std::string& str, const std::string& substr, const std::string& replacement)
{
	bool modified = false;
	size_t off = 0;
	size_t fnd = 0;;
	while ((fnd = str.find(substr, off)) != std::string::npos)
		{
		str = str.replace(fnd, substr.size(), replacement);
		off = fnd + replacement.size();
		modified = true;
		}

	return modified;
}


// json11 doesn't newline or indent it's output, so this is a naive effort at doing so
std::string pretty_print_json11(const std::string& text)
{
	std::string ret = text;

	replace_substr(ret, "{", "{\n");
	replace_substr(ret, "}", "\n}");
	replace_substr(ret, ", ", ",\n");
	replace_substr(ret, "[", "[\n");
	replace_substr(ret, "]", "\n]");

	return ret;
}


// Longest common prefix substring of a list of strings
std::string longest_common_substring(const std::vector<std::string>& strings)
{
	if (strings.empty())
		return {};

	size_t length = strings[0].length();
	for (size_t i = 1; i < strings.size(); ++i)
		{
		if (strings[i].length() < length)
			length = strings[i].length();
		}

	std::string lcs;

	for (size_t i = 0; i < length; ++i)
		{
		char ch = strings[0][i];

		for (size_t j = 1; j < strings.size(); ++j)
			{
			if (strings[j][i] != ch)
				return lcs;
			}

		lcs += ch;
		}

	return lcs;
}


// Search for a whole-word substring 
size_t find_substr_exact(const std::string& str, const std::string& substr, size_t off)
{
	size_t fnd = str.find(substr, off);
	while (fnd != std::string::npos)
		{
		char next = str[fnd + substr.length()];
		if (!std::isalnum(next))
			return fnd;

		off = fnd + substr.length();
		fnd = str.find(substr, off);
		}

	return std::string::npos;
}