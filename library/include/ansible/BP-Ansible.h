/*
 * BP-Ansible.h
 *
 *  Created on: Sep 21, 2016
 *      Author: root
 */

#ifndef INCLUDE_ANSIBLE_BP_ANSIBLE_H_
#define INCLUDE_ANSIBLE_BP_ANSIBLE_H_


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Create Ansible Subsystem Specific/Custom Structures %%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// this is the ansible playbook to use
typedef struct __BP_ANSIBLE_PLAYBOOK {

	// this is the playbook buffer to use
	char *playbook_buffer;

	// this is the playbook buffer length
	size_t playbook_buffer_len;

	// the number of times this playbook has been run (globally)
	size_t times_executed;

	// --- file path indicaansible -----

	// indicates whether or not this playbook has been loaded from a file
	BP_BOOL loaded_from_file;

	// path to file locally
	char *file_path;

} BP_ANSIBLE_PLAYBOOK, *P_BP_ANSIBLE_PLAYBOOK;




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Create Blockparty Generic Event Driven Subsystem %%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%






#endif /* INCLUDE_ANSIBLE_BP_ANSIBLE_H_ */
