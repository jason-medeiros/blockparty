#include "../include/BP-Main.h"


// Simply fills the passed in set with random colors that are not the same
// color.
BP_ERROR_T BP_FillColorSetRandomDissimilarColors
(
	P_BP_COLOR_CODE_SET_T set,
	BP_BOOL fg_not_white,
	BP_BOOL bg_not_white,
	BP_BOOL fg_not_black,
	BP_BOOL bg_not_black
)
{

	// ensure the set was passed in
	if(!set)
		return ERR_FAILURE;

	// foreground and background
	BP_FOREGROUND_COLOR_CODE_T fg = BP_COLOR_CODE_FG_NONE;
	BP_BACKGROUND_COLOR_CODE_T bg = BP_COLOR_CODE_BG_NONE;
	for(;;)
	{

		// set random fg and background using generation macros
		fg = BP_RANDOM_FG_COLOR;
		bg = BP_RANDOM_BG_COLOR;

		if((size_t) fg == (size_t) bg)
			continue;

		if(fg_not_white == BP_TRUE)
		if(fg == BP_COLOR_CODE_FG_WHITE)
			continue;

		if(bg_not_white == BP_TRUE)
		if(bg == BP_COLOR_CODE_BG_WHITE)
			continue;

		if(fg_not_black == BP_TRUE)
		if(fg == BP_COLOR_CODE_FG_BLACK)
			continue;

		if(fg_not_black == BP_TRUE)
		if(fg == BP_COLOR_CODE_FG_BLACK)
			continue;

		// exit loop if we've got selected colors
		break;

	}

	set->fg = fg;
	set->bg = bg;

	// return indicating success
	return ERR_SUCCESS;

}
