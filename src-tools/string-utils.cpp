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
void replace_substr(std::string& str, const std::string& substr, const std::string& replacement)
{
	size_t off = 0;
	size_t fnd = 0;;
	while ((fnd = str.find(substr, off)) != std::string::npos)
		{
		str = str.replace(fnd, substr.size(), replacement);
		off = fnd + replacement.size();
		}
}


// json11 doesn't newline or indent it's output, so this is a naive effort at doing so
std::string pretty_print_json11(const std::string& text)
{
	std::string ret = text;

	replace_substr(ret, "{", "{\n");
	replace_substr(ret, "}", "\n}");
	replace_substr(ret, ", ", ",\n");

	return ret;
}