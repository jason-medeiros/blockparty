/*
 * BP-MMapAllocator.h
 *
 *  Created on: Dec 9, 2014
 *      Author: root
 */

#ifndef BP_MMAPALLOCATOR_H_
#define BP_MMAPALLOCATOR_H_



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% MMap Malloc %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// creates a memory page
void * mmap_malloc(size_t size);

// malloc and clear
void * mmap_calloc(size_t size, size_t elem_n);

// reallocate memory
void * mmap_realloc(void * ptr, size_t new_size);

// frees a memory page
void mmap_free(void * ptr);



#endif /* BP_MMAPALLOCATOR_H_ */
