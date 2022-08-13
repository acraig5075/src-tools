#include "pch.h"
#include "options.h"
#include "json11.hpp"
#include "string-utils.h"

void WriteOptions(const std::string& filename, const ImagesAndCommandsOptions& opt1, const DuplicateStringsOptions& opt2, const UnusedStringsOptions& opt3, const MissingMacroOptions& opt4, const RcFileRulesOptions& opt5, const TooltipLengthOptions& opt6, const RegResetOptions& opt7)
{
	using namespace json11;

	Json json = Json::object
		{
			{ "tool_duplicate_strings_output_format", opt2.m_outputFormat },
			{ "tool_duplicate_strings_only_summary", opt2.m_onlySummary },
			{ "tool_unused_strings_only_summary", opt3.m_onlySummary },
			{ "tool_missing_macro_only_summary", opt4.m_onlySummary },
			{ "tool_rc_file_rules_output_format", opt5.m_outputFormat },
			{ "tool_tooltip_length_maximum", static_cast<int>(opt6.m_maximum) },
			{ "tool_tooltip_length_only_summary", opt6.m_onlySummary },
		};

	std::string text = json.dump();
	text = pretty_print_json11(text);
	std::ofstream fout(filename);
	fout << text;
}

void ReadOptions(const std::string& filename, ImagesAndCommandsOptions& opt1, DuplicateStringsOptions& opt2, UnusedStringsOptions& opt3, MissingMacroOptions& opt4, RcFileRulesOptions& opt5, TooltipLengthOptions& opt6, RegResetOptions& opt7)
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

	opt2.m_outputFormat = json["tool_duplicate_strings_output_format"].int_value();
	opt2.m_onlySummary  = json["tool_duplicate_strings_only_summary"].bool_value();
	opt3.m_onlySummary  = json["tool_unused_strings_only_summary"].bool_value();
	opt4.m_onlySummary  = json["tool_missing_macro_only_summary"].bool_value();
	opt5.m_outputFormat = json["tool_rc_file_rules_output_format"].int_value();
	opt6.m_maximum      = json["tool_tooltip_length_maximum"].int_value();
	opt6.m_onlySummary  = json["tool_tooltip_length_only_summary"].bool_value();
}
