/*
 * BP-StaticBorderSetsNew.cc
 *
 *  Created on: Nov 26, 2014
 *      Author: root
 */




#include "../include/BP-Main.h"


// ====== First Border Set ================================================

// create border set to use for borderization
BP_STRING_ROW_BORDER_SET BP_STSBS_border_set1 =
{
		BP_FALSE,
		NULL,
		BP_TRUE,
		" ^ ",   // border left
		0,       // border_left_display_len_if_colorized
		" ^ ",   // border_right
		0,       // border_right_display_len_if_colorized
		BP_TRUE, // border_top_enabled
		"=",     // border_top_char
		0,       // border_top_char_len_if_colorized
		">x<",   // border_top_left_corner
		0,       // border_top_left_corner_len_if_colorized
		">x<",   // border_top_right_corner
		0,       // border_top_right_corner_len_if_colorized
		BP_TRUE, // border_bottom_enabled
		"=",     // border_bottom_char
		0,       // border_bottom_char_len_if_colorized
		">x<",   // border_bottom_left_corner
		0,       // border_bottom_left_corner_len_if_colorized
		">x<",   // border_bottom_right_corner
		0        // border_bottom_right_corner_len_if_colorized
};


// create border set to use for borderization
BP_STRING_ROW_BORDER_SET BP_STSBS_border_set2 =
{
		BP_FALSE,
		NULL,
		BP_TRUE,
		" | ",   // border left
		0,       // border_left_display_len_if_colorized
		" | ",   // border_right
		0,       // border_right_display_len_if_colorized
		BP_TRUE, // border_top_enabled
		"-",     // border_top_char
		0,       // border_top_char_len_if_colorized
		"[+]",   // border_top_left_corner
		0,       // border_top_left_corner_len_if_colorized
		"[+]",   // border_top_right_corner
		0,       // border_top_right_corner_len_if_colorized
		BP_TRUE, // border_bottom_enabled
		"-",     // border_bottom_char
		0,       // border_bottom_char_len_if_colorized
		"[+]",   // border_bottom_left_corner
		0,       // border_bottom_left_corner_len_if_colorized
		"[+]",   // border_bottom_right_corner
		0        // border_bottom_right_corner_len_if_colorized
};


// create border set to use for borderization
BP_STRING_ROW_BORDER_SET BP_STSBS_border_set3 =
{
		BP_FALSE,
		NULL,
		BP_TRUE,
		" % ",   // border left
		0,       // border_left_display_len_if_colorized
		" % ",   // border_right
		0,       // border_right_display_len_if_colorized
		BP_TRUE, // border_top_enabled
		"%",     // border_top_char
		0,       // border_top_char_len_if_colorized
		"{+}",   // border_top_left_corner
		0,       // border_top_left_corner_len_if_colorized
		"{+}",   // border_top_right_corner
		0,       // border_top_right_corner_len_if_colorized
		BP_TRUE, // border_bottom_enabled
		"%",     // border_bottom_char
		0,       // border_bottom_char_len_if_colorized
		"{-}",   // border_bottom_left_corner
		0,       // border_bottom_left_corner_len_if_colorized
		"{-}",   // border_bottom_right_corner
		0        // border_bottom_right_corner_len_if_colorized
};


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Padding Border Sets %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// create border set to use for borderization
BP_STRING_ROW_BORDER_SET BP_STSBS_border_set_onlypadding_1 =
{
		BP_FALSE,
		NULL,
		BP_TRUE,
		" ",   // border left
		0,       // border_left_display_len_if_colorized
		" ",   // border_right
		0,       // border_right_display_len_if_colorized
		BP_TRUE, // border_top_enabled
		" ",     // border_top_char
		0,       // border_top_char_len_if_colorized
		" ",   // border_top_left_corner
		0,       // border_top_left_corner_len_if_colorized
		" ",   // border_top_right_corner
		0,       // border_top_right_corner_len_if_colorized
		BP_TRUE, // border_bottom_enabled
		" ",     // border_bottom_char
		0,       // border_bottom_char_len_if_colorized
		" ",   // border_bottom_left_corner
		0,       // border_bottom_left_corner_len_if_colorized
		" ",   // border_bottom_right_corner
		0        // border_bottom_right_corner_len_if_colorized
};


// create border set to use for borderization
BP_STRING_ROW_BORDER_SET BP_STSBS_border_set_onlypadding_2 =
{
		BP_FALSE,
		NULL,
		BP_TRUE,
		"  ",   // border left
		0,       // border_left_display_len_if_colorized
		"  ",   // border_right
		0,       // border_right_display_len_if_colorized
		BP_TRUE, // border_top_enabled
		"  ",     // border_top_char
		0,       // border_top_char_len_if_colorized
		"  ",   // border_top_left_corner
		0,       // border_top_left_corner_len_if_colorized
		"  ",   // border_top_right_corner
		0,       // border_top_right_corner_len_if_colorized
		BP_TRUE, // border_bottom_enabled
		"  ",     // border_bottom_char
		0,       // border_bottom_char_len_if_colorized
		"  ",   // border_bottom_left_corner
		0,       // border_bottom_left_corner_len_if_colorized
		"  ",   // border_bottom_right_corner
		0        // border_bottom_right_corner_len_if_colorized
};

// create border set to use for borderization
BP_STRING_ROW_BORDER_SET BP_STSBS_border_set_onlypadding_3 =
{
		BP_FALSE,
		NULL,
		BP_TRUE,
		"  ",   // border left
		0,       // border_left_display_len_if_colorized
		"  ",   // border_right
		0,       // border_right_display_len_if_colorized
		BP_TRUE, // border_top_enabled
		"  ",     // border_top_char
		0,       // border_top_char_len_if_colorized
		"  ",   // border_top_left_corner
		0,       // border_top_left_corner_len_if_colorized
		"  ",   // border_top_right_corner
		0,       // border_top_right_corner_len_if_colorized
		BP_TRUE, // border_bottom_enabled
		"  ",     // border_bottom_char
		0,       // border_bottom_char_len_if_colorized
		"  ",   // border_bottom_left_corner
		0,       // border_bottom_left_corner_len_if_colorized
		"  ",   // border_bottom_right_corner
		0        // border_bottom_right_corner_len_if_colorized
};

// create border set to use for borderization
BP_STRING_ROW_BORDER_SET BP_STSBS_border_set_onlypadding_4 =
{
		BP_FALSE,
		NULL,
		BP_TRUE,
		"   ",   // border left
		0,       // border_left_display_len_if_colorized
		"   ",   // border_right
		0,       // border_right_display_len_if_colorized
		BP_TRUE, // border_top_enabled
		"   ",     // border_top_char
		0,       // border_top_char_len_if_colorized
		"   ",   // border_top_left_corner
		0,       // border_top_left_corner_len_if_colorized
		"   ",   // border_top_right_corner
		0,       // border_top_right_corner_len_if_colorized
		BP_TRUE, // border_bottom_enabled
		"   ",     // border_bottom_char
		0,       // border_bottom_char_len_if_colorized
		"   ",   // border_bottom_left_corner
		0,       // border_bottom_left_corner_len_if_colorized
		"   ",   // border_bottom_right_corner
		0        // border_bottom_right_corner_len_if_colorized
};

// create border set to use for borderization
BP_STRING_ROW_BORDER_SET BP_STSBS_border_set_onlypadding_5 =
{
		BP_FALSE,
		NULL,
		BP_TRUE,
		"    ",   // border left
		0,       // border_left_display_len_if_colorized
		"    ",   // border_right
		0,       // border_right_display_len_if_colorized
		BP_TRUE, // border_top_enabled
		"    ",     // border_top_char
		0,       // border_top_char_len_if_colorized
		"    ",   // border_top_left_corner
		0,       // border_top_left_corner_len_if_colorized
		"    ",   // border_top_right_corner
		0,       // border_top_right_corner_len_if_colorized
		BP_TRUE, // border_bottom_enabled
		"    ",     // border_bottom_char
		0,       // border_bottom_char_len_if_colorized
		"    ",   // border_bottom_left_corner
		0,       // border_bottom_left_corner_len_if_colorized
		"    ",   // border_bottom_right_corner
		0        // border_bottom_right_corner_len_if_colorized
};


// create border set to use for borderization
BP_STRING_ROW_BORDER_SET BP_STSBS_border_set_onlypadding_6 =
{
		BP_FALSE,
		NULL,
		BP_TRUE,
		"      ",   // border left
		0,       // border_left_display_len_if_colorized
		"      ",   // border_right
		0,       // border_right_display_len_if_colorized
		BP_TRUE, // border_top_enabled
		"      ",     // border_top_char
		0,       // border_top_char_len_if_colorized
		"      ",   // border_top_left_corner
		0,       // border_top_left_corner_len_if_colorized
		"      ",   // border_top_right_corner
		0,       // border_top_right_corner_len_if_colorized
		BP_TRUE, // border_bottom_enabled
		"      ",     // border_bottom_char
		0,       // border_bottom_char_len_if_colorized
		"      ",   // border_bottom_left_corner
		0,       // border_bottom_left_corner_len_if_colorized
		"      ",   // border_bottom_right_corner
		0        // border_bottom_right_corner_len_if_colorized
};


// create border set to use for borderization
BP_STRING_ROW_BORDER_SET BP_STSBS_border_set_onlypadding_no_bottom_border_1 =
{
		BP_FALSE,
		NULL,
		BP_TRUE,
		" ",   // border left
		0,       // border_left_display_len_if_colorized
		" ",   // border_right
		0,       // border_right_display_len_if_colorized
		BP_TRUE, // border_top_enabled
		" ",     // border_top_char
		0,       // border_top_char_len_if_colorized
		" ",   // border_top_left_corner
		0,       // border_top_left_corner_len_if_colorized
		" ",   // border_top_right_corner
		0,       // border_top_right_corner_len_if_colorized
		BP_FALSE, // border_bottom_enabled
		" ",     // border_bottom_char
		0,       // border_bottom_char_len_if_colorized
		" ",   // border_bottom_left_corner
		0,       // border_bottom_left_corner_len_if_colorized
		" ",   // border_bottom_right_corner
		0        // border_bottom_right_corner_len_if_colorized
};


// create border set to use for borderization
BP_STRING_ROW_BORDER_SET BP_STSBS_border_set_onlypadding_no_bottom_border_2 =
{
		BP_FALSE,
		NULL,
		BP_TRUE,
		"  ",   // border left
		0,       // border_left_display_len_if_colorized
		"  ",   // border_right
		0,       // border_right_display_len_if_colorized
		BP_TRUE, // border_top_enabled
		"  ",     // border_top_char
		0,       // border_top_char_len_if_colorized
		"  ",   // border_top_left_corner
		0,       // border_top_left_corner_len_if_colorized
		"  ",   // border_top_right_corner
		0,       // border_top_right_corner_len_if_colorized
		BP_FALSE, // border_bottom_enabled
		"  ",     // border_bottom_char
		0,       // border_bottom_char_len_if_colorized
		"  ",   // border_bottom_left_corner
		0,       // border_bottom_left_corner_len_if_colorized
		"  ",   // border_bottom_right_corner
		0        // border_bottom_right_corner_len_if_colorized
};

// create border set to use for borderization
BP_STRING_ROW_BORDER_SET BP_STSBS_border_set_onlypadding_no_bottom_border_3 =
{
		BP_FALSE,
		NULL,
		BP_TRUE,
		"  ",   // border left
		0,       // border_left_display_len_if_colorized
		"  ",   // border_right
		0,       // border_right_display_len_if_colorized
		BP_TRUE, // border_top_enabled
		"  ",     // border_top_char
		0,       // border_top_char_len_if_colorized
		"  ",   // border_top_left_corner
		0,       // border_top_left_corner_len_if_colorized
		"  ",   // border_top_right_corner
		0,       // border_top_right_corner_len_if_colorized
		BP_FALSE, // border_bottom_enabled
		"  ",     // border_bottom_char
		0,       // border_bottom_char_len_if_colorized
		"  ",   // border_bottom_left_corner
		0,       // border_bottom_left_corner_len_if_colorized
		"  ",   // border_bottom_right_corner
		0        // border_bottom_right_corner_len_if_colorized
};

// create border set to use for borderization
BP_STRING_ROW_BORDER_SET BP_STSBS_border_set_onlypadding_no_bottom_border_4 =
{
		BP_FALSE,
		NULL,
		BP_TRUE,
		"   ",   // border left
		0,       // border_left_display_len_if_colorized
		"   ",   // border_right
		0,       // border_right_display_len_if_colorized
		BP_TRUE, // border_top_enabled
		"   ",     // border_top_char
		0,       // border_top_char_len_if_colorized
		"   ",   // border_top_left_corner
		0,       // border_top_left_corner_len_if_colorized
		"   ",   // border_top_right_corner
		0,       // border_top_right_corner_len_if_colorized
		BP_FALSE, // border_bottom_enabled
		"   ",     // border_bottom_char
		0,       // border_bottom_char_len_if_colorized
		"   ",   // border_bottom_left_corner
		0,       // border_bottom_left_corner_len_if_colorized
		"   ",   // border_bottom_right_corner
		0        // border_bottom_right_corner_len_if_colorized
};

// create border set to use for borderization
BP_STRING_ROW_BORDER_SET BP_STSBS_border_set_onlypadding_no_bottom_border_5 =
{
		BP_FALSE,
		NULL,
		BP_TRUE,
		"    ",   // border left
		0,       // border_left_display_len_if_colorized
		"    ",   // border_right
		0,       // border_right_display_len_if_colorized
		BP_TRUE, // border_top_enabled
		"    ",     // border_top_char
		0,       // border_top_char_len_if_colorized
		"    ",   // border_top_left_corner
		0,       // border_top_left_corner_len_if_colorized
		"    ",   // border_top_right_corner
		0,       // border_top_right_corner_len_if_colorized
		BP_FALSE, // border_bottom_enabled
		"    ",     // border_bottom_char
		0,       // border_bottom_char_len_if_colorized
		"    ",   // border_bottom_left_corner
		0,       // border_bottom_left_corner_len_if_colorized
		"    ",   // border_bottom_right_corner
		0        // border_bottom_right_corner_len_if_colorized
};


// create border set to use for borderization
BP_STRING_ROW_BORDER_SET BP_STSBS_border_set_onlypadding_no_bottom_border_6 =
{
		BP_FALSE,
		NULL,
		BP_TRUE,
		"      ",   // border left
		0,       // border_left_display_len_if_colorized
		"      ",   // border_right
		0,       // border_right_display_len_if_colorized
		BP_TRUE, // border_top_enabled
		"      ",     // border_top_char
		0,       // border_top_char_len_if_colorized
		"      ",   // border_top_left_corner
		0,       // border_top_left_corner_len_if_colorized
		"      ",   // border_top_right_corner
		0,       // border_top_right_corner_len_if_colorized
		BP_FALSE, // border_bottom_enabled
		"      ",     // border_bottom_char
		0,       // border_bottom_char_len_if_colorized
		"      ",   // border_bottom_left_corner
		0,       // border_bottom_left_corner_len_if_colorized
		"      ",   // border_bottom_right_corner
		0        // border_bottom_right_corner_len_if_colorized
};


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Generic Underline Sets %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// create border set to use for borderization
BP_STRING_ROW_BORDER_SET BP_STSBS_border_set_underline_1 =
{
		BP_FALSE,
		NULL,
		BP_TRUE,
		" ",   // border left
		0,       // border_left_display_len_if_colorized
		" ",   // border_right
		0,       // border_right_display_len_if_colorized
		BP_FALSE, // border_top_enabled
		" ",     // border_top_char
		0,       // border_top_char_len_if_colorized
		" ",   // border_top_left_corner
		0,       // border_top_left_corner_len_if_colorized
		" ",   // border_top_right_corner
		0,       // border_top_right_corner_len_if_colorized
		BP_TRUE, // border_bottom_enabled
		"-",     // border_bottom_char
		0,       // border_bottom_char_len_if_colorized
		" ",   // border_bottom_left_corner
		0,       // border_bottom_left_corner_len_if_colorized
		" ",   // border_bottom_right_corner
		0        // border_bottom_right_corner_len_if_colorized
};


