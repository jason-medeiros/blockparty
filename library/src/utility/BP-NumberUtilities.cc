/*
 * BP-NumberUtilities.cc
 *
 *  Created on: Dec 9, 2011
 *      Author: root
 */

// include main functions
#include "../../include/BP-Main.h"


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
float BP_CalculateBasicRiskOfAction(float trust_required, float exposure_required, float illegality_required, float trust_established)
{

		// ensure that parameters are correct (between 0.00 and 1.00
		if
		(
			((trust_required      > 1) || (trust_required      < 0)) ||
			((exposure_required   > 1) || (exposure_required   < 0)) ||
			((illegality_required > 1) || (illegality_required < 0)) ||
			((trust_established   > 1) || (trust_established   < 0))
		)
		{
			return -1;
		}

        // calculate exposure risk
        float risk = (float) (trust_required * ( exposure_required * illegality_required) * trust_established);

        // return the calculated risk
        return risk;

}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Numeric Randomization Utilities %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// generates a random number between the low limit and high limit
// This implementation uses the system entropy pool in order to generate
// any number versus using rand().  This improves randomness, but does
// add overhead.  This is linux only
size_t BP_GenRandomUnsignedLong(size_t low_limit, size_t high_limit)
{

	// try to set pnrg if not already set
	BP_SeedPNRG();

	// check to see that parameters are set
	if(!low_limit && !high_limit)
	{
		// printf("\n [!!] Attempted to generate a random number between 0 and 0 (only outcome being zero).");
		return 0;
	}

	// set the random item
	size_t random_item = 0;

#ifdef __GNUC__

	// set random item from rand
	random_item = (size_t) rand();

	// if we're doing a binary generation, we can just bitshift for our answer
	if((low_limit == 0) && (high_limit == 1))
	{
		// set the random item as a bitshift
		random_item = (random_item & (1 << 0));
		return random_item;
	}

	// set the random item
	random_item = (random_item % high_limit) + low_limit;

	// subtract lowlimit from top in case number is too high
	if(random_item > high_limit)
		random_item -= low_limit;

#endif
#ifndef __GNUC__
	printf("\n TODO: IMPLEMENT SYSTEM ENTROPY RANDOM NUM GEN ON WIN32.");
#endif

	// return the new 4 byte value
	return random_item;

}

// Generates a random uint16 value (primarily used for packing together
// uuid values)
uint16_t BP_GenerateRandomUInt16(uint16_t low_limit, uint16_t high_limit)
{

	// try to set pnrg if not already set
	BP_SeedPNRG();

	// check to see that parameters are set
	if(!low_limit && !high_limit)
	{
		// printf("\n [!!] Attempted to generate a random number between 0 and 0 (only outcome being zero).");
		return 0;
	}

	// set the random item
	uint16_t random_item = 0;

#ifdef __GNUC__

	// set random item from rand
	random_item = (uint16_t) rand();

	// if we're doing a binary generation, we can just bitshift for our answer
	if((low_limit == 0) && (high_limit == 1))
	{
		// set the random item as a bitshift
		random_item = (random_item & (1 << 0));
		return random_item;
	}

	// set the random item
	random_item = (random_item % high_limit) + low_limit;

	// subtract lowlimit from top in case number is too high
	if(random_item > high_limit)
		random_item -= low_limit;

#endif
#ifndef __GNUC__
	printf("\n TODO: IMPLEMENT SYSTEM ENTROPY RANDOM NUM GEN ON WIN32.");
#endif

	// return the random item
	return random_item;

}


// Generates a random unsigned char value (primarily used for packing together
// uuid values)
unsigned char BP_GenerateRandomUChar(unsigned char low_limit, unsigned char high_limit)
{

	// try to set pnrg if not already set
	BP_SeedPNRG();

	// check to see that parameters are set
	if(!low_limit && !high_limit)
	{
		printf("\n [!!] Attempted to generate a random number between 0 and 0 (only outcome being zero).");
		return 0;
	}

	// set the random item
	unsigned char random_item = 0;

#ifdef __GNUC__

	// set random item from rand
	random_item = (unsigned char) rand();

	// if we're doing a binary generation, we can just bitshift for our answer
	if((low_limit == 0) && (high_limit == 1))
	{
		// set the random item as a bitshift
		random_item = (random_item & (1 << 0));
		return random_item;
	}

	// set the random item
	random_item = (random_item % high_limit) + low_limit;

	// subtract lowlimit from top in case number is too high
	if(random_item > high_limit)
		random_item -= low_limit;

#endif
#ifndef __GNUC__
	printf("\n TODO: IMPLEMENT SYSTEM ENTROPY RANDOM NUM GEN ON WIN32.");
#endif

	// return the random item
	return random_item;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Numeric Array Utilities %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Counts pointers in a null terminated pointer array and returns in size_t.
size_t BP_CountNullTerminatedPtrArray(char **count_array)
{

	if(!count_array)
		return 0;
	if(!count_array[0])
		return 0;

	// count the items
	size_t i = 0;
	for(; count_array[i]; i++){}

	// return indicating success
	return i;

}

// generates a size_t * array of indexes the size of the array.  Results must be bpfreed.
size_t * BP_GenIntegerIndexArray(size_t arr_elems, size_t randomize_order)
{

	// check to make sure that the element size request is non-zero
	if(!arr_elems)
		return NULL;

	// generate elements
	size_t * elems = (size_t *) bpcalloc(sizeof(size_t) * arr_elems, 1);
	size_t n = 0;
	for(; n < arr_elems; n++)
		elems[n] = n;

	// return null on allocation failure
	if(!elems)
		return NULL;

	// randomize the elements if asked
	if(randomize_order)
	{

		// save old before randomizing
		size_t * elems_old = elems;

		// create new randomized array from the old array
		elems = (size_t *) BP_RandomizePointerArray((char **) elems, arr_elems);

		// destroy the non-randomized elements
		bpfree(elems_old);

	}

	// return the elements
	return elems;

}


// Generates a size_t * array of values which exist between the specified min and max values.
size_t * BP_GenRandomUnsignedLongArray(size_t arr_elems, size_t min_val, size_t max_val)
{

	// check to make sure that the element size request is non-zero
	if(!arr_elems)
		return NULL;

	// generate elements (automatically null terminates arrays last pointer)
	size_t * elems = (size_t *) bpcalloc(sizeof(size_t) * (arr_elems + 1), 1);

	// walk the element place holders and generate individual values
	size_t n = 0;
	for(; n < arr_elems; n++)
		elems[n] = BP_GenRandomUnsignedLong(min_val, max_val);

	// return null on allocation failure
	if(!elems)
		return NULL;

	// return the elements
	return elems;

}

// calculates a percentage based on two size_t values.  Returns a double
size_t BP_GetPercentage(_vin size_t fraction, _vin size_t whole)
{

	if(!fraction || !whole)
		return 0;

	size_t percentage = (fraction*100)/whole;
	return percentage;

}


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
BP_ERROR_T BP_ArrayPushSizeT(size_t value, size_t ** stack, size_t *stack_n, P_BP_LINKL tq)
{

	// ensure we have a pointer to the stack
	if(!stack_n)
		return ERR_FAILURE;

	// calculate the new stack size based on provided inputs
	size_t new_stack_size = (*stack_n+1) * sizeof(size_t);

	// create new stack
	if(!tq)
		stack[0] = (size_t *) bprealloc(*stack, new_stack_size);
	else
		stack[0] = (size_t *) tq_realloc(tq, *stack, new_stack_size, BPLN);

	// set the value in the gadget stack
	stack[0][*stack_n] = value;

	// increment the stack counter
	*stack_n = *stack_n + 1;

	// return indicating success
	return ERR_SUCCESS;

}



// Pushes an array onto a heap allocated array of uint16_t. Automatically
// allocates memory via realloc.  Final allocated array must be freed.
BP_ERROR_T BP_ArrayPushUInt16(uint16_t value, uint16_t ** stack, uint16_t *stack_n, P_BP_LINKL tq)
{

	// ensure we have a pointer to the stack
	if(!stack_n)
		return ERR_FAILURE;

	// calculate the new stack size based on provided inputs
	uint16_t new_stack_size = (*stack_n+1) * sizeof(uint16_t);

	// create new stack
	if(!tq)
		stack[0] = (uint16_t *) bprealloc(*stack, new_stack_size);
	else
		stack[0] = (uint16_t *) tq_realloc(tq, *stack, new_stack_size, BPLN);

	// set the value in the gadget stack
	stack[0][*stack_n] = value;

	// increment the stack counter
	*stack_n = *stack_n + 1;

	// return indicating success
	return ERR_SUCCESS;

}

// Pushes an array onto a heap allocated array of uint32_t. Automatically
// allocates memory via realloc.  Final allocated array must be freed.
BP_ERROR_T BP_ArrayPushUInt32(uint32_t value, uint32_t ** stack, uint32_t *stack_n, P_BP_LINKL tq)
{

	// ensure we have a pointer to the stack
	if(!stack_n)
		return ERR_FAILURE;

	// calculate the new stack size based on provided inputs
	uint32_t new_stack_size = (*stack_n+1) * sizeof(uint32_t);

	// create new stack
	stack[0] = (uint32_t *) bprealloc(*stack, new_stack_size);

	// create new stack
	if(!tq)
		stack[0] = (uint32_t *) bprealloc(*stack, new_stack_size);
	else
		stack[0] = (uint32_t *) tq_realloc(tq, *stack, new_stack_size, BPLN);


	// set the value in the gadget stack
	stack[0][*stack_n] = value;

	// increment the stack counter
	*stack_n = *stack_n + 1;

	// return indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Numeric Percentage Utilities %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// FLOAT: calculates a percentage based on two values.
float BP_GetPercentageFloat(_vin float fraction, _vin float whole)
{

	if(!fraction || !whole)
		return 0;

	size_t percentage = (fraction*100)/whole;
	return percentage;

}

// DOUBLE: calculates a percentage based on two values.
double BP_GetPercentageDouble(_vin double fraction, _vin double whole)
{

	if(!fraction || !whole)
		return 0;

	size_t percentage = (fraction*100)/whole;
	return percentage;

}

// LONG LONG INT: calculates a percentage based on two values.
long long int BP_GetPercentageLongLongInt(_vin long long int fraction, _vin long long int whole)
{

	if(!fraction || !whole)
		return 0;

	size_t percentage = (fraction*100)/whole;
	return percentage;

}


// =========== Type to size_t convertors =====================


// converts a positive float to a size_t (safely via iteration, slow but safe for
// a perfect type conversion.  Drops any percision that may exist.  If the value is
// too large or too small, this will return 0.
size_t BP_FloatToSizeT(float val)
{


	// set the iterator
	float iter = 0;
	size_t ret_iter = 0;

	// ensure the value is positive
	if(iter >= val)
		return 0;

	for(; iter <= val;iter++)
	{

		// only ever go as high as 4GB-1
		if(iter >= (BP_GIGABYTE * 4 - 1))
			return 0;

		// iterate the size_t
		ret_iter++;

	}

	// return the iter
	return ret_iter;

}

// converts a positive long long int to a size t (this is primarily
// used when dealing with the global os profiler (converting memory entries
// to size_t).  The method id kind of slow, but it's also absolutely safe
// to use so long as you account for the potential zero return values.
size_t BP_LongLongIntToSizeT(long long int val)
{

	// set the iterator
	long long int iter = 0;
	size_t ret_iter = 0;

	// ensure the value is positive
	if(iter >= val)
		return 0;

	for(; iter <= val;iter++)
	{

		// only ever go as high as 4GB-1
		if(iter >= (BP_GIGABYTE * 4 - 1))
			return 0;

		// iterate the size_t
		ret_iter++;

	}

	// return the iter
	return ret_iter;

}

// Same as above except for double conversion.
size_t BP_DoubleToSizeT(double val)
{

	// set the iterator
	double iter = 0;
	size_t ret_iter = 0;

	// ensure the value is positive
	if(iter >= val)
		return 0;

	for(; iter <= val;iter++)
	{

		// only ever go as high as 4GB-1
		if(iter >= (BP_GIGABYTE * 4 - 1))
			return 0;

		// iterate the size_t
		ret_iter++;

	}

	// return the iter
	return ret_iter;

}



// =========== size_t to type conversions =====================


// converts size_t to a float value.
float BP_SizeTToFloat(size_t val)
{

	// set the iterator
	size_t iter      = 0;
	float ret_iter = 0;

	// ensure the value is positive
	if(iter >= val)
		return 0;

	for(; iter <= val;iter++)
	{

		// only ever go as high as 4GB-1
		if(iter >= (BP_GIGABYTE * 4 - 1))
			return 0;

		// iterate the size_t
		ret_iter++;

	}

	// return the iter
	return ret_iter;

}


// converts size_t to a long long int value.
long long int BP_SizeTToLongLongInt(size_t val)
{

	// set the iterator
	size_t iter      = 0;
	long long int ret_iter = 0;

	// ensure the value is positive
	if(iter >= val)
		return 0;

	for(; iter <= val;iter++)
	{

		// only ever go as high as 4GB-1
		if(iter >= (BP_GIGABYTE * 4 - 1))
			return 0;

		// iterate the size_t
		ret_iter++;

	}

	// return the iter
	return ret_iter;

}


// converts size_t to a float value.
double BP_SizeTToDouble(size_t val)
{

	// set the iterator
	size_t iter      = 0;
	double ret_iter = 0;

	// ensure the value is positive
	if(iter >= val)
		return 0;

	for(; iter <= val;iter++)
	{

		// only ever go as high as 4GB-1
		if(iter >= (BP_GIGABYTE * 4 - 1))
			return 0;

		// iterate the size_t
		ret_iter++;

	}

	// return the iter
	return ret_iter;

}
