#pragma once


struct IDSResource
	{
	std::string m_name;
	std::string m_description;
	};

std::ostream &operator<<(std::ostream &out, const IDSResource &r);
bool operator<(const IDSResource &lhs, const IDSResource &rhs);
bool operator==(const IDSResource &lhs, const IDSResource &rhs);


std::vector<IDSResource> parse_string_table(const std::filesystem::path &path);
