/*
 * BP-EventWorker.cc
 *
 *  Created on: May 12, 2015
 *      Author: root
 */


// Blockparty main include file
#include "../include/BP-Main.h"


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Event Worker Threads %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// event worker thread
void * BP_EventMgrWorkerThread_thInit(void *data)
{


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Worker Entry Config %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// enter the worker thread, setup relevant structures, run basic sanity checks
	BP_EVENT_WORKER_ENTER_THREAD;

	// BP_JSCreateNewManager(evm->name, "")

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Internal Work Loop %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// get the current thread id
	pthread_t curr_thread_id = pthread_self();

	// number of events which have been processed
	size_t events_processed = 0;

	// display loop messages
	printf("\n [dbg]: Attempting to enter worker thread loop.");

	// enter the worker thread loop
	BP_EVENT_WORKER_THREAD_LOOP
	{


		// sleep a tiny amount between pickup to ensure that we don't just
		// free-wheel all the cpu (tightening the loop can make things more responsive
		// but it can result in cpu overhead).
		usleep(150000);
		// printf("\n Worker thread is polling wtf?!: %u", curr_thread_id);
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Gather New Event if Available %%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// attempt to lock the event queue
		// printf("\n [+] Waiting for event!");
		// lock the lock by default (threads wait on lock to determine
		// when events are available.)
		BP_SemaphoreWait(evm->event_loop_state.lock);


		// printf("\n [dbg:] Attempting to pickup event now after event loop state lock wait !");

		// enter ght critical section
		BP_GHT_ENTER_CRITICAL_SECTION;



		// printf("\n [dbg:] Attempting to pickup event now after critical section !");

		// pickup an highest priority event for processing
		P_BP_EVENT event_to_process = BP_EventMgrPickupHighestPriorityEventFromWorker
		(
			local_worker,
			BP_EVENT_MESSAGES_IN
		);

		// unlock the event loop state (must be done before leaving critical section)
		BP_SemaphorePost(evm->event_loop_state.lock);

		// exit ght critical section
		BP_GHT_LEAVE_CRITICAL_SECTION;



		// verbosity
		if(event_to_process)
		{
			// verbosity
			printf("\n [+] Picked up event!: %p - %s", event_to_process, event_to_process->name);
		}

		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Call Activation Processor %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// If an event to process exists, go ahead and dispatch it.  Do not
		// enter a critical section here.
		if(event_to_process)
		if(event_to_process->processor)
		{

			// create direct processor reference
			P_BP_EVENT_PROCESSOR processor = (P_BP_EVENT_PROCESSOR) event_to_process->processor;

			// ensure there is an entry point
			if(processor->entry_point)
			{

				// set the processed thread id
				event_to_process->processed_thread_id = curr_thread_id;

				//' call the entry point with the event as a parameter
				processor->entry_point((void *) event_to_process);

				// set the state to waiting for output transformation
				event_to_process->dispatch_state = BP_EVENT_DISPATCH_STATE_WAITING_FOR_OUTPUT_TRANSFORM;

			}

		}


		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Call Output Transformation Processor %%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// If an event to process exists, go ahead and dispatch it.  Do not
		// enter a critical section here.
		if(event_to_process)
		{

			if(event_to_process->ot_processor)
			{

				// create direct processor reference
				P_BP_EVENT_PROCESSOR ot_processor = (P_BP_EVENT_PROCESSOR) event_to_process->ot_processor;

				// ensure there is an entry point
				if(ot_processor->entry_point)
				{

					// set the output transformation processed thread id
					event_to_process->ot_processed_thread_id = curr_thread_id;

					// Set the state to completed by default.  The processor can change the state arbitrarily
					// from the entry point/callback directly below.
					event_to_process->dispatch_state = BP_EVENT_DISPATCH_STATE_COMPLETED;

					// call the entry point with the event as a parameter
					ot_processor->entry_point((void *) event_to_process);


				}

			}
			else
			{

				// set the state to completed automatically if there's no output transformation
				event_to_process->dispatch_state = BP_EVENT_DISPATCH_STATE_COMPLETED;

			}

		}


		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Merge Resultant Data %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// enter ght critical section
		BP_GHT_ENTER_CRITICAL_SECTION;


		// move the event to the processed out queue
		if(event_to_process)
		{

			// finish the event processing
			printf("\n [+] Done processing event %u on thread %u", events_processed, curr_thread_id);


			// attempt to checkin the event after it's been picked up
			BP_EventMgrCheckinProcessedEvent
			(
				local_worker,
				event_to_process,
				BP_EVENT_MESSAGES_OUT,
				BP_EVENT_PRIORITY_NORMAL
			);

			// display checkin message
			printf("\n [+] Event was checked in ok.");

			// BP_HashRegDisplay(NULL, BP_FALSE, BP_TRUE);

		}

		// increment the number of events processed
		events_processed++;

		// exit ght critical section
		BP_GHT_LEAVE_CRITICAL_SECTION;


	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%% Exit Thread / Decoupling %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// exit the worker thread
	BP_EVENT_WORKER_EXIT_THREAD;

}


