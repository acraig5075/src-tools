#include "pch.h"
#include "options.h"
#include "json11.hpp"
#include "string-utils.h"

void WriteOptions(const std::string& filename, const Options& options)
{
	using namespace json11;

	Json json = Json::object
		{
			{ "tool_duplicate_strings_output_format",                options.m_duplicateStringsOpts.m_outputFormat },
			{ "tool_duplicate_strings_only_summary",                 options.m_duplicateStringsOpts.m_onlySummary },
			{ "tool_duplicate_strings_exclude_folders",              Json{options.m_duplicateStringsOpts.m_excludeFolders} },
			{ "tool_unused_strings_only_summary",                    options.m_unusedStringsOpts.m_onlySummary },
			{ "tool_unused_strings_exclude_folders",                 Json{options.m_unusedStringsOpts.m_excludeFolders} },
			{ "tool_missing_macro_only_summary",                     options.m_missingMacroOpts.m_onlySummary },
			{ "tool_rc_file_rules_output_format",                    options.m_rcFileRulesOpts.m_outputFormat },
			{ "tool_rc_file_rules_redundant_caption_suffix",         options.m_rcFileRulesOpts.m_redundantCaptionSuffix },
			{ "tool_rc_file_rules_captions_not_title_case",          options.m_rcFileRulesOpts.m_captionsNotTitleCase },
			{ "tool_rc_file_rules_groupbox_captions_not_title_case", options.m_rcFileRulesOpts.m_groupboxCaptionsNotTitleCase },
			{ "tool_rc_file_rules_inputboxes_wrong_height",          options.m_rcFileRulesOpts.m_inputboxesWrongHeight },
			{ "tool_rc_file_rules_ok_cancel_not_left_to_right",      options.m_rcFileRulesOpts.m_okCancelNotLeftToRight },
			{ "tool_rc_file_rules_ok_cancel_wrong_gap",              options.m_rcFileRulesOpts.m_okCancelWrongGap },
			{ "tool_rc_file_rules_label_ends_with_colon",            options.m_rcFileRulesOpts.m_labelEndsWithColon },
			{ "tool_rc_file_rules_exceeds_dialog_margin",            options.m_rcFileRulesOpts.m_exceedsDialogMargin },
			{ "tool_rc_file_rules_right_aligned_checkbox",           options.m_rcFileRulesOpts.m_rightAlignedCheckbox },
			{ "tool_rc_file_rules_vertical_alignment_left",          options.m_rcFileRulesOpts.m_verticalAlignmentLeft },
			{ "tool_rc_file_rules_vertical_alignment_right",         options.m_rcFileRulesOpts.m_verticalAlignmentRight },
			{ "tool_rc_file_rules_horizontal_alignment_top",         options.m_rcFileRulesOpts.m_horizontalAlignmentTop },
			{ "tool_rc_file_rules_horizontal_alignment_bottom",      options.m_rcFileRulesOpts.m_horizontalAlignmentBottom },
			{ "tool_tooltip_length_maximum",                         static_cast<int>(options.m_tooltipLengthOpts.m_maximum) },
			{ "tool_tooltip_length_only_summary",                    options.m_tooltipLengthOpts.m_onlySummary },
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

	options.m_duplicateStringsOpts.m_outputFormat            = json["tool_duplicate_strings_output_format"].int_value();
	options.m_duplicateStringsOpts.m_onlySummary             = json["tool_duplicate_strings_only_summary"].bool_value();
	options.m_unusedStringsOpts.m_onlySummary                = json["tool_unused_strings_only_summary"].bool_value();
	options.m_missingMacroOpts.m_onlySummary                 = json["tool_missing_macro_only_summary"].bool_value();
	options.m_rcFileRulesOpts.m_outputFormat                 = json["tool_rc_file_rules_output_format"].int_value();
	options.m_rcFileRulesOpts.m_redundantCaptionSuffix       = json["tool_rc_file_rules_redundant_caption_suffix"].bool_value();
	options.m_rcFileRulesOpts.m_captionsNotTitleCase         = json["tool_rc_file_rules_captions_not_title_case"].bool_value();
	options.m_rcFileRulesOpts.m_groupboxCaptionsNotTitleCase = json["tool_rc_file_rules_groupbox_captions_not_title_case"].bool_value();
	options.m_rcFileRulesOpts.m_inputboxesWrongHeight        = json["tool_rc_file_rules_inputboxes_wrong_height"].bool_value();
	options.m_rcFileRulesOpts.m_okCancelNotLeftToRight       = json["tool_rc_file_rules_ok_cancel_not_left_to_right"].bool_value();
	options.m_rcFileRulesOpts.m_okCancelWrongGap             = json["tool_rc_file_rules_ok_cancel_wrong_gap"].bool_value();
	options.m_rcFileRulesOpts.m_labelEndsWithColon           = json["tool_rc_file_rules_label_ends_with_colon"].bool_value();
	options.m_rcFileRulesOpts.m_exceedsDialogMargin          = json["tool_rc_file_rules_exceeds_dialog_margin"].bool_value();
	options.m_rcFileRulesOpts.m_rightAlignedCheckbox         = json["tool_rc_file_rules_right_aligned_checkbox"].bool_value();
	options.m_rcFileRulesOpts.m_verticalAlignmentLeft        = json["tool_rc_file_rules_vertical_alignment_left"].bool_value();
	options.m_rcFileRulesOpts.m_verticalAlignmentRight       = json["tool_rc_file_rules_vertical_alignment_right"].bool_value();
	options.m_rcFileRulesOpts.m_horizontalAlignmentTop       = json["tool_rc_file_rules_horizontal_alignment_top"].bool_value();
	options.m_rcFileRulesOpts.m_horizontalAlignmentBottom    = json["tool_rc_file_rules_horizontal_alignment_bottom"].bool_value();
	options.m_tooltipLengthOpts.m_maximum                    = json["tool_tooltip_length_maximum"].int_value();
	options.m_tooltipLengthOpts.m_onlySummary                = json["tool_tooltip_length_only_summary"].bool_value();

	Json::array folders;
	
	folders = json["tool_unused_strings_exclude_folders"].array_items();
	options.m_unusedStringsOpts.m_excludeFolders.resize(folders.size());
	for (size_t i = 0; i < folders.size(); ++i)
		options.m_unusedStringsOpts.m_excludeFolders[i] = folders[i].string_value();

	folders = json["tool_duplicate_strings_exclude_folders"].array_items();
	options.m_duplicateStringsOpts.m_excludeFolders.resize(folders.size());
	for (size_t i = 0; i < folders.size(); ++i)
		options.m_duplicateStringsOpts.m_excludeFolders[i] = folders[i].string_value();
}
