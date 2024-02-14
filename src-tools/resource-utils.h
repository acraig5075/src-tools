#pragma once


struct IDSResource
{
	std::string m_name;
	std::string m_description;
};

std::ostream &operator<<(std::ostream &out, const IDSResource &r);
bool operator<(const IDSResource &lhs, const IDSResource &rhs);
bool operator==(const IDSResource &lhs, const IDSResource &rhs);


struct control_rect
{
	int m_top = 0;
	int m_left = 0;
	int m_right = 0;
	int m_bottom = 0;

	bool Intersects(const control_rect &r2) const;
	bool Contains(const control_rect &r2) const;
	bool Identical(const control_rect &r2) const;
};


struct control_defn
{
	std::string m_type;
	std::string m_uid;
	std::string m_label;
	std::string m_flags;
	int m_x = 0;
	int m_y = 0;
	int m_width = 0;
	int m_height = 0;

	control_defn(const std::string &type, const std::string &uid, const std::string &label, const std::string &flags, int x, int y, int width, int height);

	int Top() const;
	int Left() const;
	int Right() const;
	int Bottom() const;
	bool HasFlag(const std::string &flag) const;
	bool IsAutoAligned() const;
	control_rect GetRect() const;
};


struct dialog_defn
{
	std::string m_uid;
	std::string m_caption;
	int m_width = 0;
	int m_height = 0;
	bool m_style = false;
	std::vector<control_defn> m_controls;
};


std::vector<IDSResource> parse_string_table(const std::filesystem::path &path);
std::vector<dialog_defn> get_dialog_definitions(const std::filesystem::path &path);
