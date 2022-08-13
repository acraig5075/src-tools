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

void WriteOptions(const std::string& filename, const ImagesAndCommandsOptions &opt1, const DuplicateStringsOptions &opt2, const UnusedStringsOptions &opt3, const MissingMacroOptions &opt4, const RcFileRulesOptions &opt5, const TooltipLengthOptions &opt6, const RegResetOptions &opt7);
void ReadOptions(const std::string& filename, ImagesAndCommandsOptions &opt1, DuplicateStringsOptions &opt2, UnusedStringsOptions &opt3, MissingMacroOptions &opt4, RcFileRulesOptions &opt5, TooltipLengthOptions &opt6, RegResetOptions &opt7);
