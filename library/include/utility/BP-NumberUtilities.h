/*
 * BP-NumberUtilities.h
 *
 *  Created on: Dec 9, 2011
 *      Author: root
 */

#ifndef BPNUMBERUTILITIES_H_
#define BPNUMBERUTILITIES_H_


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Risk Calculation Formula Utilities %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// The following routines are useful for determining risk.  Risk values start at
// zero, for no risk, and scale up to 1.00.  There are different formulas
// for determining different types of risk, so you need to select the correct
// routine for your risk calculation purposes.

//
// This routine calculates a basic risk using the following formula.  Do not provide negative values
// to this function.
//
//	Formula:
//			(trust_required * ( exposure_required * illegality_required) * trust_established)
//
// return value:
//			0.00 - 1.00 based on inputs.
//
//  error: -1
//
float BP_CalculateBasicRiskOfAction(float trust_required, float exposure_required, float illegality_required, float trust_established);




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Numeric Randomization Utilities %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Number randomizers all use rand().  Rand is always seeded globally via
// system entropy sources.

// generates a random number between the low limit and high limit
// Uses rand() to generate a size_t.
size_t BP_GenRandomUnsignedLong(size_t low_limit, size_t high_limit);

// Generates a random uint16 value (primarily used for packing together
// uuid values)
uint16_t BP_GenerateRandomUInt16(uint16_t low_limit, uint16_t high_limit);

// Generates a random unsigned char value (primarily used for packing together
// uuid values)
unsigned char BP_GenerateRandomUChar(unsigned char low_limit, unsigned char high_limit);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Numeric Array Utilities %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Counts pointers in a null terminated pointer array and returns in size_t.
size_t BP_CountNullTerminatedPtrArray(char **count_array);

// generates a size_t * array of indexes the size of the array.  Results must be freed.
size_t * BP_GenIntegerIndexArray(size_t arr_elems, size_t randomize_order);

// Generates a size_t * array of values which exist between the specified min and max values.
size_t * BP_GenRandomUnsignedLongArray(size_t arr_elems, size_t min_val, size_t max_val);

// Pushes an array onto a heap allocated array of size_t. Automatically
// allocates memory via realloc.  Final allocated array must be freed.
//
// Example:
// 		size_t push_idx   = 0;
// 		size_t *num_stack = NULL;
// 		BP_ArrayPushSizeT(0xcafebabe, &num_stack, &push_idx);
// 		BP_ArrayPushSizeT(0xdeadbeef, &num_stack, &push_idx);
//
//		bpfree(num_stack);
BP_ERROR_T BP_ArrayPushSizeT(size_t value, size_t ** stack, size_t *stack_n, P_BP_LINKL tq);

// Pushes an array onto a heap allocated array of uint16_t. Automatically
// allocates memory via realloc.  Final allocated array must be freed.
BP_ERROR_T BP_ArrayPushUInt16(uint16_t value, uint16_t ** stack, uint16_t *stack_n, P_BP_LINKL tq);

// Pushes an array onto a heap allocated array of uint32_t. Automatically
// allocates memory via realloc.  Final allocated array must be freed.
BP_ERROR_T BP_ArrayPushUInt32(uint32_t value, uint32_t ** stack, uint32_t *stack_n, P_BP_LINKL tq);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Numeric Percentage Utilities %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// calculates a percentage based on two size_t values.
size_t BP_GetPercentage(_vin size_t fraction, _vin size_t whole);

// FLOAT: calculates a percentage based on two values.
float BP_GetPercentageFloat(_vin float fraction, _vin float whole);

// DOUBLE: calculates a percentage based on two values.
double BP_GetPercentageDouble(_vin double fraction, _vin double whole);

// LONG LONG INT: calculates a percentage based on two values.
long long int BP_GetPercentageLongLongInt(_vin long long int fraction, _vin long long int whole);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Numeric Type Conversion Utilities %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// converts a positive float to a size_t (safely via iteration, slow but safe for
// a perfect type conversion.  Drops any percision that may exist.  If the value is
// too large or too small, this will return 0.
size_t BP_FloatToSizeT(float val);

// converts a positive long long int to a size t (this is primarily
// used when dealing with the global os profiler (converting memory entries
// to size_t)
size_t BP_LongLongIntToSizeT(long long int val);

// Same as above except for double conversion.
size_t BP_DoubleToSizeT(double val);

// converts size_t to a float value.
float BP_SizeTToFloat(size_t val);

// converts size_t to a long long int value.
long long int BP_SizeTToLongLongInt(size_t val);

// converts size_t to a float value.
double BP_SizeTToDouble(size_t val);


#endif /* BPNUMBERUTILITIES_H_ */
