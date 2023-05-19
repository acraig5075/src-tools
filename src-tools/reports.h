#pragma once

struct UnusedResources
{
	std::filesystem::path m_path;
	std::filesystem::path m_header;
	std::filesystem::path m_rc;
	std::vector<std::string> m_names;
};

struct UnusedStringsOutput
{
	std::vector<UnusedResources> m_folders;
};

struct Duplicate
{
	std::string m_description;
	std::vector<std::string> m_names;
};

struct DuplicateResources
{
	std::filesystem::path m_filename;
	std::vector<Duplicate> m_duplicates;

	size_t count() const
		{
		return m_duplicates.size();
		}
};

struct DuplicateStringsOutput
{
	std::vector<DuplicateResources> m_folders;
};

struct IDSResource
{
	std::string m_name;
	std::string m_description;
};

struct TooltipLength
{
	std::filesystem::path m_rcFilename;
	std::vector<IDSResource> m_stringResources;
};

struct TooltipLengthOutput
{
	std::vector<TooltipLength> m_projectResources;
};

struct TooltipReplacement
{
	std::filesystem::path m_filename;
	std::string m_oldString;
	std::string m_newString;
};

struct Report
{
	std::filesystem::path m_dir;
	size_t m_count = 0;
};

void output_report(std::vector<Report>& reports, std::ostream& output);
