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