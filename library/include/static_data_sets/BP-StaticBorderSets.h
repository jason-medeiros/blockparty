#ifndef __BP_STATIC_BORDER_SETS_H
#define __BP_STATIC_BORDER_SETS_H

// Bordersets are used to wrap portions of columnized text with a variety of
// borders.  This file contains the delcaration of the border set type as well
// as the externalizations of a variety of built in bordersets which can be used
// for development purposes.


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Border Set Declaration %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// border set used for creating borders for item
typedef struct __BP_STRING_BORDER_SET {

	// if this structure was allocated via a tailqueue, this will be set to
	// BP_TRUE
	BP_BOOL allocated_via_tailqueue;

	// the tail queue allocator used, if set
	P_BP_LINKL tq;

	// this is set statically in the case we're using a border set from BP-StaticBorderSets.cc, and
	// removed if the border set is cloned through some other mechanism (or generated dynamically through other means)
	BP_BOOL border_set_is_hardcoded_static;

	// left border (and colorized len if necessary)
	char * border_left;
	size_t border_left_display_len_if_colorized;

	// right border (and colorized len if necessary)
	char * border_right;
	size_t border_right_display_len_if_colorized;

	// border top data
	BP_BOOL border_top_enabled;
	char *  border_top_char;
	size_t  border_top_char_len_if_colorized;
	char *  border_top_left_corner;
	size_t  border_top_left_corner_len_if_colorized;
	char *  border_top_right_corner;
	size_t  border_top_right_corner_len_if_colorized;

	// border bottom data
	BP_BOOL border_bottom_enabled;
	char *  border_bottom_char;
	size_t  border_bottom_char_len_if_colorized;
	char *  border_bottom_left_corner;
	size_t  border_bottom_left_corner_len_if_colorized;
	char *  border_bottom_right_corner;
	size_t  border_bottom_right_corner_len_if_colorized;

} BP_STRING_ROW_BORDER_SET, *P_BP_STRING_ROW_BORDER_SET;


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Statically Defined Border Sets %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

extern BP_STRING_ROW_BORDER_SET BP_STSBS_border_set1;

extern BP_STRING_ROW_BORDER_SET BP_STSBS_border_set2;

extern BP_STRING_ROW_BORDER_SET BP_STSBS_border_set3;


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Padding Border Sets %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

extern BP_STRING_ROW_BORDER_SET BP_STSBS_border_set_onlypadding_1;
extern BP_STRING_ROW_BORDER_SET BP_STSBS_border_set_onlypadding_2;
extern BP_STRING_ROW_BORDER_SET BP_STSBS_border_set_onlypadding_3;
extern BP_STRING_ROW_BORDER_SET BP_STSBS_border_set_onlypadding_4;
extern BP_STRING_ROW_BORDER_SET BP_STSBS_border_set_onlypadding_5;
extern BP_STRING_ROW_BORDER_SET BP_STSBS_border_set_onlypadding_6;

extern BP_STRING_ROW_BORDER_SET BP_STSBS_border_set_onlypadding_no_bottom_border_1;
extern BP_STRING_ROW_BORDER_SET BP_STSBS_border_set_onlypadding_no_bottom_border_2;
extern BP_STRING_ROW_BORDER_SET BP_STSBS_border_set_onlypadding_no_bottom_border_3;
extern BP_STRING_ROW_BORDER_SET BP_STSBS_border_set_onlypadding_no_bottom_border_4;
extern BP_STRING_ROW_BORDER_SET BP_STSBS_border_set_onlypadding_no_bottom_border_5;
extern BP_STRING_ROW_BORDER_SET BP_STSBS_border_set_onlypadding_no_bottom_border_6;

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Generic Underline Sets %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


extern BP_STRING_ROW_BORDER_SET BP_STSBS_border_set_underline_1;


#endif // __BP_STATIC_BORDER_SETS_H
