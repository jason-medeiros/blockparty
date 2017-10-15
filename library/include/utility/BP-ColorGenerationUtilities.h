/*
 * BP-ColorGenerationUtilities.h
 *
 *  Created on: Aug 20, 2014
 *      Author: root
 */

#ifndef BP_COLORGENERATIONUTILITIES_H_
#define BP_COLORGENERATIONUTILITIES_H_

// Simply fills the passed in set with random colors that are not the same
// color.
BP_ERROR_T BP_FillColorSetRandomDissimilarColors
(
	P_BP_COLOR_CODE_SET_T set,
	BP_BOOL fg_not_white = BP_FALSE,
	BP_BOOL bg_not_white = BP_FALSE,
	BP_BOOL fg_not_black = BP_FALSE,
	BP_BOOL bg_not_black = BP_FALSE
);




#endif /* BP_COLORGENERATIONUTILITIES_H_ */
