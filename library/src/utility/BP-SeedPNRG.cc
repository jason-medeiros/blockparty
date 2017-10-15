#include "../../include/BP-Main.h"

// set the global seed configuration
BP_SEED_PNRG_CONFIG global_pnrg_seed_configuration =
{
		0,0,BP_SEED_PNRG_CONFIG_STATE_UNINITIALIZED
};


// seeds the localized PNRG with the best possible source per system (/dev/urandom, etc)
BP_ERROR_T BP_SeedPNRG()
{

	// check if the srand is uninitialized
	if(global_pnrg_seed_configuration.config_state == BP_SEED_PNRG_CONFIG_STATE_UNINITIALIZED)
	{

			// set the random item
			size_t random_item = 0;

		#ifdef __GNUC__

			// open urandom
			FILE * urandom = fopen("/dev/urandom", "r");
			if(!urandom)
				return ERR_FAILURE;

			// read four bytes worth
			fread((void *) &random_item, sizeof(size_t), 1, urandom);

			// close /dev/urandom
			fclose(urandom);


		#endif
		#ifndef __GNUC__
			printf("\n TODO: IMPLEMENT SYSTEM ENTROPY RANDOM NUM GEN ON WIN32.");
		#endif

		global_pnrg_seed_configuration.srand_current_seed    = random_item;
		global_pnrg_seed_configuration.sraondom_current_seed = random_item;

		// actually call srand
		srand(random_item);

		// actually call srandom
		srandom(random_item);

		// set singleton configuration
		global_pnrg_seed_configuration.config_state = BP_SEED_PNRG_CONFIG_STATE_INITIALIZED;

		// return indicating success
		return ERR_SUCCESS;

	}
	else if (global_pnrg_seed_configuration.config_state == BP_SEED_PNRG_CONFIG_STATE_INITIALIZED)
		return ERR_SUCCESS;

	// return with failure if we have a bad config state
	return ERR_FAILURE;

}
