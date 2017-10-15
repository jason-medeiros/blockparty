#ifndef __BP_GENERIC_FAST_WEIGHTED_BALANCER_H
#define __BP_GENERIC_FAST_WEIGHTED_BALANCER_H

/*
 *
 * Balanced Weight Oriented Selector:
 * =====================================
 * Provided a set of input data, this allows
 * a quick weight calculation / update capacity.
 *
 */

// hit entry structure, used for balancing
typedef struct __BP_WEIGHT_HIT_ENTRY {

	// iterated on incrementation request
	size_t current_hits;

	// set to the desired weighting (weights are auto
	// translated to percentages.
	size_t current_weight;

	// propaged via calculate update
	size_t weight_goal_percentage;
	size_t current_percentage;

	// flag indicating if this entry is overweight.  Set on
	// update calculation.
	size_t overweight;

} BP_WEIGHT_HIT_ENTRY, *P_BP_WEIGHT_HIT_ENTRY;

// weighted balancer session
typedef struct __BP_WEIGHTED_BALANCER {

	// stack of entries (reallocated)
	P_BP_WEIGHT_HIT_ENTRY entries;

	// number of entries in the entry stack
	size_t entry_n;

	// flag (0/1) field indicating whether or not
	// the weight data needs to be sorted.
	size_t requires_update;

	// incremented automatically whenever any increment
	// operation ocurs.
	size_t total_hits_processed_by_this_balancer;

	// total number of hits which were dropped
	size_t total_hits_dropped_by_this_balancer;

	// garbage collector used for the session.  Destroyed on
	// session destroy request.
	BP_GARBAGE_COLLECTOR_DATA * gc;

}BP_WEIGHTED_BALANCER, *P_BP_WEIGHTED_BALANCER;

// creates a new allocated balance structure
P_BP_WEIGHTED_BALANCER BP_CreateNewBalancer();

// destroy the balancer
BP_ERROR_T BP_DestroyBalancer(P_BP_WEIGHTED_BALANCER balancer);

// push an item onto the stack
BP_ERROR_T BP_BalancerPushItemOnStack(P_BP_WEIGHTED_BALANCER balancer, P_BP_WEIGHT_HIT_ENTRY item);

// increment an index in the entry table, sets the update flag to true
BP_ERROR_T BP_BalancerIncWeightEntryAtIndex(P_BP_WEIGHTED_BALANCER balancer , size_t index);

// displays the balancer as configured
BP_ERROR_T BP_DisplayBalancer(P_BP_WEIGHTED_BALANCER balancer);

// recalculates weight percentages and updates values.  Must be called
// before any of the data in the percentage fields can be considered up to
// date.
BP_ERROR_T BP_BalancerUpdate(P_BP_WEIGHTED_BALANCER balancer);

#endif // __BP_GENERIC_FAST_WEIGHTED_BALANCER_H
