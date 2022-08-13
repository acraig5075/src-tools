#include "pch.h"
#include "options.h"
#include "json11.hpp"
#include "string-utils.h"

void WriteOptions(const std::string& filename, const Options& options)
{
	using namespace json11;

	Json json = Json::object
		{
			{ "tool_duplicate_strings_output_format", options.m_duplicateStringsOpts.m_outputFormat },
			{ "tool_duplicate_strings_only_summary",  options.m_duplicateStringsOpts.m_onlySummary },
			{ "tool_unused_strings_only_summary",     options.m_unusedStringsOpts.m_onlySummary },
			{ "tool_missing_macro_only_summary",      options.m_missingMacroOpts.m_onlySummary },
			{ "tool_rc_file_rules_output_format",     options.m_rcFileRulesOpts.m_outputFormat },
			{ "tool_tooltip_length_maximum",          static_cast<int>(options.m_tooltipLengthOpts.m_maximum) },
			{ "tool_tooltip_length_only_summary",     options.m_tooltipLengthOpts.m_onlySummary },
		};

	std::string text = json.dump();
	text = pretty_print_json11(text);
	std::ofstream fout(filename);
	fout << text;
}

void ReadOptions(const std::string& filename, Options& options)
{
	std::ifstream fin(filename);
	std::stringstream sstr;
	fin >> sstr.rdbuf();
	std::string text = sstr.str();

	using namespace json11;

	std::string err;
	Json json = Json::parse(text, err);
	if (!err.empty())
		return;

	options.m_duplicateStringsOpts.m_outputFormat = json["tool_duplicate_strings_output_format"].int_value();
	options.m_duplicateStringsOpts.m_onlySummary  = json["tool_duplicate_strings_only_summary"].bool_value();
	options.m_unusedStringsOpts.m_onlySummary     = json["tool_unused_strings_only_summary"].bool_value();
	options.m_missingMacroOpts.m_onlySummary      = json["tool_missing_macro_only_summary"].bool_value();
	options.m_rcFileRulesOpts.m_outputFormat      = json["tool_rc_file_rules_output_format"].int_value();
	options.m_tooltipLengthOpts.m_maximum         = json["tool_tooltip_length_maximum"].int_value();
	options.m_tooltipLengthOpts.m_onlySummary     = json["tool_tooltip_length_only_summary"].bool_value();
}
