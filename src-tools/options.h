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
	bool m_onlySummary = false;
	};

struct MissingMacroOptions
	{
	bool m_onlySummary = false;
	};

struct RcFileRulesOptions
	{
	int m_outputFormat = 0;
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
