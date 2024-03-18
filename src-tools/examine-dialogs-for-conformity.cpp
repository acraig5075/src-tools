#include "pch.h"
#include "string-utils.h"
#include "filesystem-utils.h"
#include "resource-utils.h"
#include "options.h"

namespace fs = std::filesystem;
using namespace string_utils;

enum rule_code
{
	NONE = 0,
	REDUNDANT_CAPTION_SUFFIX,
	CAPTIONS_NOT_TITLE_CASE,
	INPUTBOXES_WRONG_HEIGHT,
	GROUPBOX_CAPTIONS_NOT_TITLE_CASE,
	OK_CANCEL_NOT_LEFT_TO_RIGHT,
	OK_CANCEL_WRONG_GAP,
	LABEL_ENDS_WITH_COLON,
	LABEL_NOT_SENTENCE_CASE,
	EXCEEDS_DIALOG_MARGIN,
	RIGHT_ALIGNED_CHECKBOX,
	VERTICAL_ALIGNMENT_LEFT,
	VERTICAL_ALIGNMENT_RIGHT,
	HORIZONTAL_ALIGNMENT_TOP,
	HORIZONTAL_ALIGNMENT_BOTTOM,
	CONTROLS_OVERLAP,
};

enum output_format
{
	FAULT_ORDER,
	DIALOG_ORDER,
};


std::unordered_map<rule_code, std::string> rule_descriptions =
{
	{ REDUNDANT_CAPTION_SUFFIX,         "Caption ends with the word Dialog or Window" },        // captions and labels
	{ CAPTIONS_NOT_TITLE_CASE,          "Dialog caption is not title case" },                   // captions and labels
	{ GROUPBOX_CAPTIONS_NOT_TITLE_CASE, "Group box caption is not title case" },                // captions and labels
	{ INPUTBOXES_WRONG_HEIGHT,          "Input box has the wrong height" },                     // control size
	{ OK_CANCEL_NOT_LEFT_TO_RIGHT,      "OK and Cancel buttons are not horizontally aligned" }, // dialog buttons
	{ OK_CANCEL_WRONG_GAP,              "OK and Cancel buttons have the wrong gap" },           // dialog buttons
	{ LABEL_ENDS_WITH_COLON,            "Label ends with a colon" },                            // captions and label
	{ EXCEEDS_DIALOG_MARGIN,            "Control overlaps margin" },                            // alignment
	{ RIGHT_ALIGNED_CHECKBOX,           "Checkbox is right-aligned" },                          // alignment
	{ VERTICAL_ALIGNMENT_LEFT,          "Controls don't align vertically on the left" },        // alignment
	{ VERTICAL_ALIGNMENT_RIGHT,         "Controls don't align vertically on the right" },       // alignment
	{ HORIZONTAL_ALIGNMENT_TOP,         "Controls don't align horizontally at the top" },       // alignment
	{ HORIZONTAL_ALIGNMENT_BOTTOM,      "Controls don't align horizontally at the bottom" },    // alignment
	{ CONTROLS_OVERLAP,                 "Controls overlap" },                                   // control size
};


struct broken_rule
{
	rule_code m_code = NONE;
	std::string m_dialog;
	std::string m_comment;

	broken_rule(rule_code code, std::string uid, std::string comment = "")
		: m_code(code)
		, m_dialog(uid)
		, m_comment(comment)
	{}
};


struct control_coord
{
	std::string m_uid;
	int m_value = 0;

	control_coord(std::string uid, int value)
		: m_uid(uid)
		, m_value(value)
	{}
};


// Count the occurences of different settings
std::unordered_map<int, unsigned int> inputbox_heights;
std::unordered_map<int, unsigned int> okCancel_gaps;
std::unordered_map<int, unsigned int> label_colons;


bool top_exclusions(const control_defn &ctrl)
{
	if (ctrl.IsAutoAligned() || ctrl.m_type == "LTEXT" || ctrl.m_type == "RTEXT" || ctrl.m_type == "CTEXT")
		return true;
	return false;
}

bool left_exclusions(const control_defn &ctrl)
{
	if (ctrl.IsAutoAligned() || ctrl.m_type == "RTEXT")
		return true;
	return false;
}

bool right_exclusions(const control_defn &ctrl)
{
	if (ctrl.IsAutoAligned() || ctrl.m_type == "LTEXT" || (ctrl.m_type == "CONTROL" && (ctrl.HasFlag("BS_AUTORADIOBUTTON") || ctrl.HasFlag("BS_AUTOCHECKBOX"))))
		return true;
	return false;
}

bool bottom_exclusions(const control_defn &ctrl)
{
	if (ctrl.IsAutoAligned() || ctrl.m_type == "LTEXT" || ctrl.m_type == "RTEXT" || ctrl.m_type == "CTEXT")
		return true;
	return false;
}

bool word_ellipsis(const control_defn &ctrl1, const control_defn &ctrl2)
{
	if (ctrl1.Left() == ctrl2.Left())
		return false;

	const control_defn &lhs = ctrl1.Left() < ctrl2.Left() ? ctrl1 : ctrl2;

	return ("LTEXT" == lhs.m_type || "RTEXT" == lhs.m_type || "CTEXT" == lhs.m_type) && lhs.HasFlag("SS_WORDELLIPSIS");
}

std::vector<std::string> check_alignment(const std::vector<control_defn> &controls_, int(control_defn::* coordFunc)() const, bool(*excludeFunc)(const control_defn &))
{
	std::vector<std::string> comments;

	std::vector<control_coord> coordinates;

	std::vector<control_defn> controls(controls_);

	auto end_itr = std::remove_if(begin(controls), end(controls), [&](const control_defn & ctrl)
		{
		if (excludeFunc)
			return (*excludeFunc)(ctrl);
		else
			return false;
		});
	std::transform(begin(controls), end_itr, std::back_inserter(coordinates), [&](const control_defn & ctrl)
		{
		return control_coord{ ctrl.m_uid, (ctrl.*coordFunc)() };
		});
	std::sort(coordinates.begin(), coordinates.end(), [](const auto & lhs, const auto & rhs)
		{
		return lhs.m_value < rhs.m_value;
		});

	auto CheckGroupAlignment = [&](std::vector<control_coord>::iterator begin, std::vector<control_coord>::iterator end)
		{
		if (std::distance(begin, end) <= 1)
			return std::string{};

		int frontvalue = begin->m_value;
		int backvalue = (end - 1)->m_value;

		if (frontvalue == backvalue)
			return std::string{};

		size_t num_front = std::count_if(begin, end, [&](const control_coord & coord)
			{
			return coord.m_value == frontvalue;
			});
		size_t num_back = std::count_if(begin, end, [&](const control_coord & coord)
			{
			return coord.m_value == backvalue;
			});

		std::vector<control_coord>::iterator front_begin, front_end, back_begin, back_end;

		if (num_front <= num_back)
			{
			front_begin = begin;
			front_end = begin + num_front;
			back_begin = begin + num_front;
			back_end = end;
			}
		else
			{
			front_begin = begin + num_front;
			front_end = end;
			back_begin = begin;
			back_end = begin + num_front;
			}

		std::string comment;

		for (auto itr = front_begin; itr != front_end; ++itr)
			comment += itr->m_uid + ",";
		comment = trim(comment, ",");

		comment += " misaligned with ";

		for (auto itr = back_begin; itr != back_end; ++itr)
			comment += itr->m_uid + ",";
		comment = trim(comment, ",");

		return comment;
		};

	auto groupbegin = coordinates.begin();

	if (coordinates.empty())
		return {};

	for (auto itr = coordinates.begin() + 1; itr != coordinates.end(); ++itr)
		{
		if (abs(itr->m_value - groupbegin->m_value) > 1)
			{
			std::string misalignment = CheckGroupAlignment(groupbegin, itr);

			if (!misalignment.empty())
				comments.push_back(misalignment);

			groupbegin = itr;
			}
		}

	return comments;
}

std::vector<std::string> check_overlaps(const std::vector<control_defn> &controls)
{
	if (controls.size() < 2)
		return {};

	std::vector<std::string> comments;

	for (auto i = controls.begin(); i < controls.end() - 1; ++i)
		{
		if (i->IsAutoAligned())
			continue;

		control_rect rect1 = i->GetRect();

		for (auto j = i + 1; j < controls.end(); ++j)
			{
			if (j->IsAutoAligned())
				continue;

			control_rect rect2 = j->GetRect();

			if (rect1.Intersects(rect2) && !rect1.Contains(rect2) && !rect2.Contains(rect1) && !rect1.Identical(rect2) && !word_ellipsis(*i, *j))
				{
				std::stringstream ss;
				ss
					<< "Overlapping controls: "
					<< i->m_uid
					<< " and "
					<< j->m_uid;

				comments.push_back(ss.str());
				}
			}
		}

	return comments;
}

// Inspect dialog definitions for breaking the rules
std::vector<broken_rule> inspect(const std::vector<dialog_defn> &dialogs, const RcFileRulesOptions& options)
{
	const int MARGIN              = options.m_dialogMargin;
	const int MIN_INPUTBOX_HEIGHT = options.m_minInputBoxHeight;
	const int MAX_INPUTBOX_HEIGHT = options.m_maxInputBoxHeight;
	const int OK_CANCEL_GAP       = options.m_okCancelGap;

	std::vector<broken_rule> faults;

	for (auto dlg : dialogs)
		{
		if (ends_with(lowercase(dlg.m_caption), "dialog") && lowercase(dlg.m_caption) != "dialog")
			{
			faults.push_back(broken_rule{ REDUNDANT_CAPTION_SUFFIX, dlg.m_uid, dlg.m_caption });
			}
		if (ends_with(lowercase(dlg.m_caption), "window") && lowercase(dlg.m_caption) != "window")
			{
			faults.push_back(broken_rule{ REDUNDANT_CAPTION_SUFFIX, dlg.m_uid, dlg.m_caption });
			}
		if (!is_title_case(dlg.m_caption))
			{
			faults.push_back(broken_rule{ CAPTIONS_NOT_TITLE_CASE, dlg.m_uid, dlg.m_caption });
			}

		for (auto ctrl : dlg.m_controls)
			{
			if (!ends_with(dlg.m_uid, "_BR") && (ctrl.m_x < MARGIN || ctrl.m_y < MARGIN || (ctrl.m_x + ctrl.m_width > dlg.m_width - MARGIN + 1) || (ctrl.m_y + ctrl.m_height > dlg.m_height - MARGIN + 1)))
				{
				faults.push_back(broken_rule{ EXCEEDS_DIALOG_MARGIN, dlg.m_uid, ctrl.m_uid });
				}
			if (ctrl.m_type == "EDITTEXT" && (ctrl.m_height < MIN_INPUTBOX_HEIGHT || ctrl.m_height > MAX_INPUTBOX_HEIGHT) && ctrl.m_flags.find("ES_MULTILINE") == std::string::npos)
				{
				std::string comment = ctrl.m_uid + " height is " + std::to_string(ctrl.m_height) + " should be " + std::to_string(MIN_INPUTBOX_HEIGHT) + " to " + std::to_string(MAX_INPUTBOX_HEIGHT);
				faults.push_back(broken_rule{ INPUTBOXES_WRONG_HEIGHT, dlg.m_uid, comment });
				}
			if (ctrl.m_type == "GROUPBOX" && !is_title_case(ctrl.m_label))
				{
				faults.push_back(broken_rule{ GROUPBOX_CAPTIONS_NOT_TITLE_CASE, dlg.m_uid, ctrl.m_label });
				}
			if (ctrl.m_type == "EDITTEXT" && ctrl.m_flags.find("ES_MULTILINE") == std::string::npos)
				{
				inputbox_heights[ctrl.m_height]++;
				}
			if ((ctrl.m_type == "LTEXT" || ctrl.m_type == "RTEXT" || ctrl.m_type == "CTEXT") && ends_with(trim(ctrl.m_label, " "), ":"))
				{
				faults.push_back(broken_rule{ LABEL_ENDS_WITH_COLON, dlg.m_uid, ctrl.m_label });
				}
			if ((ctrl.m_type == "LTEXT" || ctrl.m_type == "RTEXT" || ctrl.m_type == "CTEXT"))
				{
				if (ends_with(trim(ctrl.m_label, " "), ":"))
					label_colons[1]++;
				else
					label_colons[0]++;
				}
			if (ctrl.m_type == "CONTROL" && ctrl.m_flags.find("BS_AUTOCHECKBOX") != std::string::npos && ctrl.m_flags.find("BS_LEFTTEXT") != std::string::npos)
				{
				faults.push_back(broken_rule{ RIGHT_ALIGNED_CHECKBOX, dlg.m_uid, ctrl.m_label });
				}
			//if ((ctrl.m_type == "LTEXT" || ctrl.m_type == "RTEXT" || ctrl.m_type == "CTEXT") && !is_sentence_case(ctrl.m_label))
			//{
			//	faults.push_back(broken_rule{ LABEL_NOT_SENTENCE_CASE, dlg.m_uid, ctrl.m_label });
			//}
			}

		auto itrOk = std::find_if(begin(dlg.m_controls), end(dlg.m_controls), [](const control_defn & ctrl)
			{
			return ctrl.m_uid == "IDOK";
			});
		auto itrCancel = std::find_if(begin(dlg.m_controls), end(dlg.m_controls), [](const control_defn & ctrl)
			{
			return ctrl.m_uid == "IDCANCEL";
			});

		if (itrOk != dlg.m_controls.end() && itrCancel != dlg.m_controls.end())
			{
			int gap = itrCancel->m_x - itrOk->m_x - itrOk->m_width;

			if (itrOk->m_x < itrCancel->m_x)
				okCancel_gaps[gap]++;

			if (itrOk->m_y != itrCancel->m_y)
				{
				faults.push_back(broken_rule{ OK_CANCEL_NOT_LEFT_TO_RIGHT, dlg.m_uid, "Different y-coordinates" });
				}
			else if (itrOk->m_x > itrCancel->m_x)
				{
				faults.push_back(broken_rule{ OK_CANCEL_NOT_LEFT_TO_RIGHT, dlg.m_uid, "OK to the right of Cancel" });
				}
			else if (gap != OK_CANCEL_GAP)
				{
				std::string comment;
				if (gap > OK_CANCEL_GAP)
					comment = "Gap is " + std::to_string(gap) + ", should be " + std::to_string(OK_CANCEL_GAP) + ", so decrease by " + std::to_string(gap - OK_CANCEL_GAP);
				else
					comment = "Gap is " + std::to_string(gap) + ", should be " + std::to_string(OK_CANCEL_GAP) + ", so increase by " + std::to_string(OK_CANCEL_GAP - gap);;
				faults.push_back(broken_rule{ OK_CANCEL_WRONG_GAP, dlg.m_uid, comment });
				}
			}

		std::vector<std::string> top_misalignments = check_alignment(dlg.m_controls, &control_defn::Top, &top_exclusions);
		std::vector<std::string> left_misalignments = check_alignment(dlg.m_controls, &control_defn::Left, &left_exclusions);
		std::vector<std::string> right_misalignments = check_alignment(dlg.m_controls, &control_defn::Right, &right_exclusions);
		std::vector<std::string> bottom_misalignments = check_alignment(dlg.m_controls, &control_defn::Bottom, &bottom_exclusions);

		std::for_each(begin(top_misalignments), end(top_misalignments), [&](const auto & comment)
			{
			faults.push_back(broken_rule{ HORIZONTAL_ALIGNMENT_TOP, dlg.m_uid, comment });
			});
		std::for_each(begin(left_misalignments), end(left_misalignments), [&](const auto & comment)
			{
			faults.push_back(broken_rule{ VERTICAL_ALIGNMENT_LEFT, dlg.m_uid, comment });
			});
		std::for_each(begin(right_misalignments), end(right_misalignments), [&](const auto & comment)
			{
			faults.push_back(broken_rule{ VERTICAL_ALIGNMENT_RIGHT, dlg.m_uid, comment });
			});
		std::for_each(begin(bottom_misalignments), end(bottom_misalignments), [&](const auto & comment)
			{
			faults.push_back(broken_rule{ HORIZONTAL_ALIGNMENT_BOTTOM, dlg.m_uid, comment });
			});

		std::vector<std::string> overlaps = check_overlaps(dlg.m_controls);

		std::for_each(begin(overlaps), end(overlaps), [&](const auto &comment)
			{
			faults.push_back(broken_rule{ CONTROLS_OVERLAP, dlg.m_uid, comment });
			});
		}

	return faults;
}


void SortByDialog(std::vector<broken_rule>::iterator begin, std::vector<broken_rule>::iterator end)
{
	std::sort(begin, end, [](const auto & lhs, const auto & rhs)
		{
		return std::tie(lhs.m_dialog, lhs.m_code) < std::tie(rhs.m_dialog, rhs.m_code);
		});
}

// List the faults grouped by dialogs
void OutputOrderedByDialog(std::vector<broken_rule> &faults, std::ostream &output)
{
	auto begin = faults.begin();
	while (begin != faults.end())
		{
		std::string current = begin->m_dialog;
		std::string heading = current;
		std::string underline(heading.size(), '-');
		output
				<< heading << "\n"
				<< underline << "\n";

		auto end = std::find_if(begin, faults.end(), [current](const broken_rule & f)
			{
			return f.m_dialog != current;
			});

		size_t col_width = 0;
		for (auto itr = begin; itr != end; ++itr)
			{
			std::string desc = rule_descriptions[itr->m_code];
			if (desc.size() > col_width)
				col_width = desc.size();
			}

		for (auto itr = begin; itr != end; ++itr)
			{
			output
					<< std::left << std::setw(col_width)
					<< rule_descriptions[itr->m_code]
					<< " (" << itr->m_comment << ")\n";
			}

		output << "\n";
		if (end == faults.end())
			break;
		begin = end;
		}
}


void PartitionBySummary(std::vector<broken_rule> &faults, const std::vector<std::pair<rule_code, unsigned int>> &summary)
{
	auto begin = faults.begin();

	for (auto itr = rbegin(summary); itr != rend(summary); ++itr)
		{
		rule_code code = itr->first;
		auto next = std::partition(begin, faults.end(), [code](const auto & f)
			{
			return f.m_code == code;
			});

		SortByDialog(begin, next);

		begin = next;
		}
}

// List the faults grouped by rules
void OutputOrderedByFault(std::vector<broken_rule> &faults, std::ostream &output)
{
	auto begin = faults.begin();
	while (begin != faults.end())
		{
		rule_code current = begin->m_code;
		std::string heading = rule_descriptions[current];
		std::string underline(heading.size(), '-');
		output
				<< heading << "\n"
				<< underline << "\n";

		auto end = std::find_if(begin, faults.end(), [current](const broken_rule & f)
			{
			return f.m_code != current;
			});

		size_t col_width = 0;
		for (auto itr = begin; itr != end; ++itr)
			{
			if (itr->m_dialog.size() > col_width)
				col_width = itr->m_dialog.size();
			}

		for (auto itr = begin; itr != end; ++itr)
			{
			output
					<< std::left
					<< std::setw(col_width)
					<< itr->m_dialog
					<< " (" << itr->m_comment << ")\n";
			}

		output << "\n";
		if (end == faults.end())
			break;
		begin = end;
		}
}


// Output stats accumulated during parsing
void OutputOccurences(const std::string &heading, const std::string &col1, const std::unordered_map<int, unsigned int> &occ, bool boolean, std::ostream &output)
{
	std::vector<std::pair<int, unsigned int>> stats;
	std::transform(begin(occ), end(occ), std::back_inserter(stats), [](const auto & p)
		{
		return p;
		});
	std::sort(begin(stats), end(stats), [](const auto & lhs, const auto & rhs)
		{
		return lhs.second > rhs.second;
		});
	stats.erase(std::remove_if(begin(stats), end(stats), [](const auto & p)
		{
		return p.second == 1;
		}), stats.end());

	std::string col2 = "Occurences";
	size_t col_width = col1.size();

	output
			<< heading << ":\n"
			<< std::left << std::setw(col_width)
			<< col1 << "  " << col2 << "\n"
			<< std::string(col1.size(), '-') << "  "
			<< std::string(col2.size(), '-') << "\n";

	for (auto itr = begin(stats); itr != end(stats); ++itr)
		{
		if (boolean)
			{
			output
					<< std::boolalpha
					<< std::left << std::setw(col_width)
					<< static_cast<bool>(itr->first) << "  "
					<< itr->second << "\n";
			}
		else
			{
			output
					<< std::left << std::setw(col_width)
					<< itr->first << "  "
					<< itr->second << "\n";
			}
		}
	output << "\n";
}


// Get total counts
std::vector<std::pair<rule_code, unsigned int>> Summarise(std::vector<broken_rule> &faults)
{
	std::unordered_map<rule_code, unsigned int> stats;
	std::vector<std::pair<rule_code, unsigned int>> summary;

	for (auto itr = rule_descriptions.begin(); itr != rule_descriptions.end(); ++itr)
		stats[itr->first] = 0;
	for (auto itr = faults.begin(); itr != faults.end(); ++itr)
		stats[itr->m_code]++;

	std::transform(begin(stats), end(stats), std::back_inserter(summary), [](const auto & p)
		{
		return p;
		});
	std::sort(begin(summary), end(summary), [](const auto & lhs, const auto & rhs)
		{
		return lhs.second > rhs.second;
		});

	return summary;
}


// Output total counts
void OutputSummary(const std::vector<std::pair<rule_code, unsigned int>> &summary, std::ostream &output)
{
	size_t col_width = 0;

	for (auto itr = summary.begin(); itr != summary.end(); ++itr)
		{
		size_t length = rule_descriptions[itr->first].size();
		if (length > col_width)
			col_width = length;
		}

	std::string col1 = "Summary of faults";
	std::string col2 = "Count";
	output
			<< std::left << std::setw(col_width)
			<< col1 << "  " << col2 << "\n"
			<< std::left << std::setw(col_width)
			<< std::string(col1.size(), '-') << "  "
			<< std::string(col2.size(), '-') << "\n";

	for (auto itr = begin(summary); itr != end(summary); ++itr)
		{
		output
				<< std::left << std::setw(col_width)
				<< rule_descriptions[itr->first] << "  "
				<< itr->second << "\n";
		}

	output << "\n";
	output << "\n";
}


//void usage()
//{
//	std::cout << "Examine .rc file for conformity with Knowledge Base dialog conventions\n";
//	std::cout << "Usage: program.exe path|file [output-format]\n";
//	std::cout << "path         : Root folder path of source tree for multiple .rc files to be parsed.\n";
//	std::cout << "file         : File path for a single .rc file to be parsed.\n";
//	std::cout << "output-format: 0= Group by rule convention (default); 1= Group by dialog\n";
//}
//

void filter_by_options(std::vector<broken_rule> &faults, const RcFileRulesOptions& options)
{
	auto FilterOut = [&faults](rule_code code)
		{
		faults.erase(std::remove_if(faults.begin(), faults.end(), [&](const broken_rule& fault)
			{
			return fault.m_code == code;
			}), faults.end());
		};

	if (!options.m_redundantCaptionSuffix)
		FilterOut(REDUNDANT_CAPTION_SUFFIX);
	if (!options.m_captionsNotTitleCase)
		FilterOut(CAPTIONS_NOT_TITLE_CASE);
	if (!options.m_groupboxCaptionsNotTitleCase)
		FilterOut(GROUPBOX_CAPTIONS_NOT_TITLE_CASE);
	if (!options.m_inputboxesWrongHeight)
		FilterOut(INPUTBOXES_WRONG_HEIGHT);
	if (!options.m_okCancelNotLeftToRight)
		FilterOut(OK_CANCEL_NOT_LEFT_TO_RIGHT);
	if (!options.m_okCancelWrongGap)
		FilterOut(OK_CANCEL_WRONG_GAP);
	if (!options.m_labelEndsWithColon)
		FilterOut(LABEL_ENDS_WITH_COLON);
	if (!options.m_exceedsDialogMargin)
		FilterOut(EXCEEDS_DIALOG_MARGIN);
	if (!options.m_rightAlignedCheckbox)
		FilterOut(RIGHT_ALIGNED_CHECKBOX);
	if (!options.m_verticalAlignmentLeft)
		FilterOut(VERTICAL_ALIGNMENT_LEFT);
	if (!options.m_verticalAlignmentRight)
		FilterOut(VERTICAL_ALIGNMENT_RIGHT);
	if (!options.m_horizontalAlignmentTop)
		FilterOut(HORIZONTAL_ALIGNMENT_TOP);
	if (!options.m_horizontalAlignmentBottom)
		FilterOut(HORIZONTAL_ALIGNMENT_BOTTOM);
	if (!options.m_controlsOverlap)
		FilterOut(CONTROLS_OVERLAP);
}

int examine_dialogs_for_conformity(const fs::path &input, std::ostream &output, const RcFileRulesOptions &options)
{
	std::string title = "Dialog rules conformance";
	std::string underline(title.length(), '=');
	output
		<< title
		<< "\n"
		<< underline
		<< "\n\n";

	inputbox_heights.clear();
	okCancel_gaps.clear();
	label_colons.clear();

	std::vector<fs::path> files;

	if (fs::exists(input) && fs::is_directory(input))
		{
		files = filesystem_utils::get_file_list(input, ".rc");
		}
	else if (fs::exists(input) && fs::is_regular_file(input) && ends_with(input.string(), ".rc"))
		{
		files.push_back(input);
		}
	else
		{
		std::cerr << "Error: Invalid input\n";
		//usage();
		return EXIT_FAILURE;
		}

	std::vector<dialog_defn> dialogs;

	for (const auto &path : files)
		{
		std::vector<dialog_defn> single = resource_utils::get_dialog_definitions(path);

		dialogs.insert(dialogs.end(), single.begin(), single.end());
		}

	std::vector<broken_rule> faults = inspect(dialogs, options);

	filter_by_options(faults, options);

	output_format order = FAULT_ORDER;

	if (options.m_outputFormat == 1)
		{
		order = DIALOG_ORDER;
		}

	output << "Number of .rc files parsed  : " << files.size() << "\n";
	output << "Number of dialogs inspected : " << dialogs.size() << "\n";
	output << "\n";

	auto summary = Summarise(faults);
	OutputSummary(summary, output);

	switch (order)
		{
		case DIALOG_ORDER:
			SortByDialog(begin(faults), end(faults));
			OutputOrderedByDialog(faults, output);
			break;
		case FAULT_ORDER:
			PartitionBySummary(faults, summary);
			OutputOrderedByFault(faults, output);
			break;
		}

	OutputOccurences("EDITTEXT heights", "Pixels", inputbox_heights, false, output);
	OutputOccurences("IDOK/IDCANCEL gaps", "Pixels", okCancel_gaps, false, output);
	OutputOccurences("Label suffix", "Has a colon",  label_colons, true, output);


	output << "Done\n";
	return EXIT_SUCCESS;
}