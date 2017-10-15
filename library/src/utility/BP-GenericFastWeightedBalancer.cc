// include main functions
#include "../../include/BP-Main.h"



// creates a new allocated balance structure
P_BP_WEIGHTED_BALANCER BP_CreateNewBalancer()
{

	// create new balancer structure
	P_BP_WEIGHTED_BALANCER balancer = (P_BP_WEIGHTED_BALANCER) bpcalloc(sizeof(BP_WEIGHTED_BALANCER), 1);
	if(!balancer)
		return NULL;

	// create a new garbage collector
	balancer->gc = BP_CreateNewGarbageCollector("balancer garbage collector", 0);

	// ensure that we have a new collector created for this balancer
	if(!balancer->gc)
	{
		bpfree(balancer);
		return NULL;
	}

	// return the balancer
	return balancer;

}

// destroy the balancer
BP_ERROR_T BP_DestroyBalancer(P_BP_WEIGHTED_BALANCER balancer)
{

	// check to ensure pointer is set
	if(!balancer)
		return ERR_FAILURE;

	// destroy all garbage collector chunks
	BP_GarbageCollectorFreeAllChunks(balancer->gc);

	// destroy the garbage collector
	BP_DestroyGarbageCollector(balancer->gc);

	// destroy the data
	bpfree(balancer);

	// return indicating success.  Pointer should never be used
	// after this (should be null)
	return ERR_SUCCESS;


}

// push an item onto the stack
BP_ERROR_T BP_BalancerPushItemOnStack(P_BP_WEIGHTED_BALANCER balancer, P_BP_WEIGHT_HIT_ENTRY item)
{

	// check pointers
	if(!balancer || !item)
		return ERR_FAILURE;

	// require a garbage collector
	if(!balancer->gc)
		return ERR_FAILURE;

	// increase the entry count
	balancer->entry_n++;

	// allocate space
	balancer->entries = (P_BP_WEIGHT_HIT_ENTRY) BP_realloc(balancer->gc, NULL,(void *) balancer->entries, sizeof(BP_WEIGHT_HIT_ENTRY)*balancer->entry_n);

	// copy in structure
	memcpy((void *) &balancer->entries[balancer->entry_n-1], item, sizeof(BP_WEIGHT_HIT_ENTRY));

	// mark that table requires analysis and exit
	balancer->requires_update = 1;
	return ERR_SUCCESS;

}


// increment an index in the entry table, sets the update flag to true
BP_ERROR_T BP_BalancerIncWeightEntryAtIndex(P_BP_WEIGHTED_BALANCER balancer , size_t index)
{

	// ensure we have a balancer
	if(!balancer)
		return ERR_FAILURE;

	// ensure our update index isn't greater than the index count
	if(index >= balancer->entry_n)
		return ERR_FAILURE;

	// never process overweight entries, but mark them as dropped
	if(balancer->entries[index].overweight)
	{
		balancer->total_hits_dropped_by_this_balancer++;
		return ERR_FAILURE;
	}

	// increment current hits
	balancer->entries[index].current_hits++;

	// increment the total hit count
	balancer->total_hits_processed_by_this_balancer++;

	// set the update flag
	balancer->requires_update=1;

	// return indicating success
	return ERR_SUCCESS;

}

// recalculates weight percentages and updates values.  Must be called
// before any of the data in the percentage fields can be considered up to
// date.
BP_ERROR_T BP_BalancerUpdate(P_BP_WEIGHTED_BALANCER balancer)
{

	//ensure we have a balancer
	if(!balancer)
		return ERR_FAILURE;

	// initialize
	size_t total_weight = 0;
	size_t total_hits   = 0;

	// loop and set values
	size_t n = 0;
	for (; n < balancer->entry_n; n++)
	{
		total_weight+=balancer->entries[n].current_weight;
		total_hits  +=balancer->entries[n].current_hits;
	}

	// check for empty totals
	if(total_weight==0)
		return ERR_SUCCESS;

	// now update percentages
	for(n = 0; n < balancer->entry_n; n++)
	{

		// calculate percentages
		balancer->entries[n].current_percentage     = BP_GetPercentage(balancer->entries[n].current_hits,   total_hits);
		balancer->entries[n].weight_goal_percentage = BP_GetPercentage(balancer->entries[n].current_weight, total_weight);

		// determine if item is overweight or not
		if(balancer->entries[n].current_percentage > balancer->entries[n].weight_goal_percentage)
			balancer->entries[n].overweight = 1;
		else
			balancer->entries[n].overweight = 0;

	}

	// return indicating sussess
	return ERR_SUCCESS;
}

// displays the balancer as configured
BP_ERROR_T BP_DisplayBalancer(P_BP_WEIGHTED_BALANCER balancer)
{

	// ensure we have a balancer
	if(!balancer)
		return ERR_FAILURE;

	printf("\n [+] Balancer %p", balancer);
	printf("\n\t balancer entries:      %p", balancer->entries);
	printf("\n\t balancer entry count:  %u", balancer->entry_n);
	printf("\n\t total processed:       %u", balancer->total_hits_processed_by_this_balancer);
	printf("\n\t total dropped:         %u", balancer->total_hits_dropped_by_this_balancer);
	printf("\n\t garbage collector:     %p", balancer->gc);
	printf("\n");
	printf("\n\t [+] Displaying Entries:");

	// now display entries
	size_t n = 0;
	for(; n < balancer->entry_n; n++)
	{
		printf("\n\t Index:  entry[%u]", n);
		printf("\n\t\t - hits:   %u", balancer->entries[n].current_hits);
		printf("\n\t\t - weight: %u", balancer->entries[n].current_weight);
		printf("\n\t\t - goal balance percentage: %u%%", balancer->entries[n].weight_goal_percentage);
		printf("\n\t\t - curr balance percentage: %u%%", balancer->entries[n].current_percentage);
		printf("\n\t\t - currently overweight?: %s", balancer->entries[n].overweight ? "yes" : "no");
		printf("\n");
	}

	// return indicating success
	return ERR_SUCCESS;

}


