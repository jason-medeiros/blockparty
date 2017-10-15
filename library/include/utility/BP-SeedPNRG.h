#ifndef __BP_SEED_PNRG_H
#define __BP_SEED_PNRG_H

typedef enum BP_SEED_PNRG_CONFIG_STATE {
	BP_SEED_PNRG_CONFIG_STATE_UNSET,
	BP_SEED_PNRG_CONFIG_STATE_UNINITIALIZED,
	BP_SEED_PNRG_CONFIG_STATE_INITIALIZED
};

// configuration for gloobal seed randomization
typedef struct __BP_SEED_PNRG_CONFIG {

	// current seed for srand
	unsigned int srand_current_seed;

	// current seed for srandom
	unsigned int sraondom_current_seed;

	// flag indicating whether or not the seed has been initialized
	BP_SEED_PNRG_CONFIG_STATE config_state;

} BP_SEED_PNRG_CONFIG, *P_BP_SEED_PNRG_CONFIG;



// seeds the localized PNRG with the best possible source per system (/dev/urandom, etc)
BP_ERROR_T BP_SeedPNRG();

#endif /* __BP_SEED_PNRG_H */
