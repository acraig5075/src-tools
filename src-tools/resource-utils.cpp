#include "pch.h"
#include "resource-utils.h"
#include "string-utils.h"

namespace fs = std::filesystem;


std::ostream &operator<<(std::ostream &out, const IDSResource &r)
{
	out << r.m_name << " - " << r.m_description;
	return out;
}

bool operator<(const IDSResource &lhs, const IDSResource &rhs)
{
	return lhs.m_name.compare(rhs.m_name) < 0;
}

bool operator==(const IDSResource &lhs, const IDSResource &rhs)
{
	return lhs.m_name.compare(rhs.m_name) == 0;
}

std::vector<IDSResource> parse_string_table(const fs::path &path)
{
	std::vector<IDSResource> ids;

	std::ifstream fin(path.string());
	std::string line;
	bool table = false;
	bool begin = false;

	while (std::getline(fin, line))
		{
		if ("STRINGTABLE" == line)
			{
			table = true;
			begin = false;
			}
		else if (table && "BEGIN" == line)
			{
			begin = true;
			}
		else if (table && "END" == line)
			{
			begin = false;
			table = false;
			}
		else if (table && begin && starts_with(trim(line, " "), "IDS_"))
			{
			size_t fnd = line.find('\"', 0);
			if (fnd != line.npos)
				{
				std::string name = line.substr(0, fnd);
				std::string desc = line.substr(fnd);
				name = trim(name, " ");
				desc = trim(desc, " \"");

				if (!name.empty() && !desc.empty())
					{
					IDSResource s;
					s.m_name = name;
					s.m_description = desc;
					ids.push_back(s);
					}
				}
			}
		}

	return ids;
}
