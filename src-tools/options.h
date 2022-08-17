#pragma once

struct ImagesAndCommandsOptions
	{
	};

struct DuplicateStringsOptions
	{
	int m_outputFormat = 0;
	bool m_onlySummary = false;
	};

struct UnusedStringsOptions
	{
	std::vector<std::string> m_excludeFolders;
	bool m_onlySummary = false;
	};

struct MissingMacroOptions
	{
	bool m_onlySummary = false;
	};

struct RcFileRulesOptions
	{
	int m_outputFormat = 0;
	bool m_redundantCaptionSuffix = true;
	bool m_captionsNotTitleCase = true;
	bool m_groupboxCaptionsNotTitleCase = true;
	bool m_inputboxesWrongHeight = true;
	bool m_okCancelNotLeftToRight = true;
	bool m_okCancelWrongGap = true;
	bool m_labelEndsWithColon = true;
	bool m_exceedsDialogMargin = true;
	bool m_rightAlignedCheckbox = true;
	bool m_verticalAlignmentLeft = true;
	bool m_verticalAlignmentRight = true;
	bool m_horizontalAlignmentTop = true;
	bool m_horizontalAlignmentBottom = true;
	};

struct TooltipLengthOptions
	{
	size_t m_maximum = 80;
	bool m_onlySummary = false;
	};

struct RegResetOptions
	{
	};

struct Options
	{
	ImagesAndCommandsOptions m_imagesAndCommandsOpts;
	DuplicateStringsOptions m_duplicateStringsOpts;
	UnusedStringsOptions m_unusedStringsOpts;
	MissingMacroOptions m_missingMacroOpts;
	RcFileRulesOptions m_rcFileRulesOpts;
	TooltipLengthOptions m_tooltipLengthOpts;
	RegResetOptions m_regResetOpts;
	};

void WriteOptions(const std::string& filename, const Options &options);
void ReadOptions(const std::string& filename, Options &options);
