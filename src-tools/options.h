#pragma once

struct ImagesAndCommandsOptions
	{
	};

struct DuplicateStringsOptions
	{
	std::vector<std::string> m_excludeFolders;
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
	bool m_controlsOverlap = true;
	int m_dialogMargin = 7;
	int m_minInputBoxHeight = 12;
	int m_maxInputBoxHeight = 14;
	int m_okCancelGap = 4;
	};

struct TooltipLengthOptions
	{
	size_t m_maximum = 80;
	bool m_onlySummary = false;
	};

struct RegResetOptions
	{
	};

struct MenuTitleCaseOptions
	{
	};

struct ProjectUnreferencedOptions
	{
	std::vector<std::string> m_excludeFolders;
	int m_outputFormat = 0;
	bool m_onlySummary = false;
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
	MenuTitleCaseOptions m_menuTitleCaseOpts;
	ProjectUnreferencedOptions m_projectUnreferencedOpts;
	};

void WriteOptions(const std::string& filename, const Options &options);
void ReadOptions(const std::string& filename, Options &options);
