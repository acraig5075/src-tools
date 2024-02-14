#include "pch.h"
#include "resource-utils.h"
#include "string-utils.h"

namespace fs = std::filesystem;
using namespace string_utils;


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



bool control_rect::Intersects(const control_rect &r2) const
{
	return !(r2.m_left > m_right
					 || r2.m_right  < m_left
					 || r2.m_top    > m_bottom
					 || r2.m_bottom < m_top
					);
}

bool control_rect::Contains(const control_rect &r2) const
{
	return (r2.m_left > m_left
					&& r2.m_right < m_right
					&& r2.m_top > m_top
					&& r2.m_bottom < m_bottom
				 );
}

bool control_rect::Identical(const control_rect &r2) const
{
	return (m_top == r2.m_top
					&& m_left == r2.m_left
					&& m_right == r2.m_right
					&& m_bottom == r2.m_bottom
				 );
}


control_defn::control_defn(const std::string &type, const std::string &uid, const std::string &label, const std::string &flags, int x, int y, int width, int height)
	: m_type(type)
	, m_uid(trim(uid, " "))
	, m_label(trim(label, "\""))
	, m_flags(flags)
	, m_x(x)
	, m_y(y)
	, m_width(width)
	, m_height(height)
{}

int control_defn::Top() const
{
	return m_y;
}
int control_defn::Left() const
{
	return m_x;
}
int control_defn::Right() const
{
	return m_x + m_width;
}
int control_defn::Bottom() const
{
	return m_y + m_height;
}

bool control_defn::HasFlag(const std::string &flag) const
{
	return m_flags.find(flag) != std::string::npos;
}

bool control_defn::IsAutoAligned() const
{
	return m_type == "CONTROL" && HasFlag("UDS_AUTOBUDDY");
}

control_rect control_defn::GetRect() const
{
	control_rect rect;
	rect.m_top = m_y;
	rect.m_left = m_x;
	rect.m_right = m_x + m_width;
	rect.m_bottom = m_y + m_height;
	return rect;
}


namespace resource_utils
{
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

	// Build up dialog definitions from an .rc file
	std::vector<dialog_defn> get_dialog_definitions(const fs::path &path)
	{
		std::vector<dialog_defn> dialogs;

		std::string line;
		dialog_defn dlg;

		std::ifstream fin(path.string());

		while (std::getline(fin, line))
			{
			line = trim(line, " \t");

			if (starts_with(line, "IDD_"))
				{
				auto fields = split(line, ' ');
				if (fields.size() == 6)
					{
					dlg.m_uid = fields[0];
					dlg.m_width = std::stoi(trim(fields[4], ","));
					dlg.m_height = std::stoi(trim(fields[5], ","));
					}
				}
			else if (starts_with(line, "STYLE "))
				{
				dlg.m_style = true;
				}
			else if (starts_with(line, "CAPTION "))
				{
				auto fields = split(line, '\"');
				if (fields.size() == 3)
					dlg.m_caption = trim(fields[1], "\"");
				}
			else if (starts_with(line, "EDITTEXT"))
				{
				std::string control = trim(erase_substr(line, "EDITTEXT"), " ");
				auto fields = split(control, ',');
				if (fields.size() == 6)
					dlg.m_controls.push_back(control_defn{ "EDITTEXT", fields[0], "", fields[5], std::stoi(fields[1]), std::stoi(fields[2]), std::stoi(fields[3]), std::stoi(fields[4]) });
				}
			else if (starts_with(line, "COMBOBOX"))
				{
				std::string control = trim(erase_substr(line, "COMBOBOX"), " ");
				auto fields = split(control, ',');
				if (fields.size() == 6)
					dlg.m_controls.push_back(control_defn{ "COMBOBOX", fields[0], "", fields[5], std::stoi(fields[1]), std::stoi(fields[2]), std::stoi(fields[3]), 12 });
				}
			else if (starts_with(line, "GROUPBOX"))
				{
				std::string control = trim(erase_substr(line, "GROUPBOX"), " ");
				auto fields = split(control, ',');
				if (fields.size() == 6)
					dlg.m_controls.push_back(control_defn{ "GROUPBOX", fields[1], fields[0], "", std::stoi(fields[2]), std::stoi(fields[3]), std::stoi(fields[4]), std::stoi(fields[5]) });
				}
			else if (starts_with(line, "DEFPUSHBUTTON") || starts_with(line, "PUSHBUTTON"))
				{
				std::string control = trim(erase_substr(line, "PUSHBUTTON"), " ");
				auto fields = split(control, ',');
				if (fields.size() == 6)
					dlg.m_controls.push_back(control_defn{ "PUSHBUTTON", fields[1], fields[0], "", std::stoi(fields[2]), std::stoi(fields[3]), std::stoi(fields[4]), std::stoi(fields[5]) });
				}
			else if (starts_with(line, "LTEXT"))
				{
				std::string control = trim(erase_substr(line, "LTEXT"), " ");
				auto fields = quote_aware_split(control, ',');
				if (fields.size() >= 6)
					dlg.m_controls.push_back(control_defn{ "LTEXT", fields[1], fields[0], (fields.size() > 6 ? fields[6] : ""), std::stoi(fields[2]), std::stoi(fields[3]), std::stoi(fields[4]), std::stoi(fields[5]) });
				}
			else if (starts_with(line, "RTEXT"))
				{
				std::string control = trim(erase_substr(line, "RTEXT"), " ");
				auto fields = quote_aware_split(control, ',');
				if (fields.size() >= 6)
					dlg.m_controls.push_back(control_defn{ "RTEXT", fields[1], fields[0], (fields.size() > 6 ? fields[6] : ""), std::stoi(fields[2]), std::stoi(fields[3]), std::stoi(fields[4]), std::stoi(fields[5]) });
				}
			else if (starts_with(line, "CTEXT"))
				{
				std::string control = trim(erase_substr(line, "CTEXT"), " ");
				auto fields = quote_aware_split(control, ',');
				if (fields.size() >= 6)
					dlg.m_controls.push_back(control_defn{ "CTEXT", fields[1], fields[0], (fields.size() > 6 ? fields[6] : ""), std::stoi(fields[2]), std::stoi(fields[3]), std::stoi(fields[4]), std::stoi(fields[5]) });
				}
			else if (starts_with(line, "CONTROL"))
				{
				std::string control = trim(erase_substr(line, "CONTROL"), " ");
				auto fields = split(control, ',');
				if (fields.size() == 8)
					dlg.m_controls.push_back(control_defn{ "CONTROL", fields[1], fields[0], fields[3], std::stoi(fields[4]), std::stoi(fields[5]), std::stoi(fields[6]), std::stoi(fields[7]) });
				}
			else if (line == "END")
				{
				if (dlg.m_style)
					dialogs.push_back(dlg);

				dlg = dialog_defn{};
				}
			}

		return dialogs;
	}
}
