/*
 * BP-MMapAllocator.cc
 *
 *  Created on: Dec 9, 2014
 *      Author: root
 */

// blockparty main include file
#include "../include/BP-Main.h"


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% MMap Malloc %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// creates a memory page
void * mmap_malloc(size_t len)
{
	void *buf;

	size_t pages = (len & PAGE_MASK) + 2;
	size_t offset = PAGE_SIZE - (len & ~PAGE_MASK);

    if(offset < sizeof(size_t))
    {
        pages++;
        offset += PAGE_SIZE;
    }

    if
    (
    	(
    		buf = mmap
    		(
    			NULL,
    			pages << PAGE_SHIFT,
    			PROT_READ | PROT_WRITE,
    			MAP_PRIVATE | MAP_ANONYMOUS,
    			0,
    			0
    		)
    	) == (void *)-1)
    {
        perror("mymalloc/mmap");
        exit(1);
    }

    /* We store the length twice, because we need it in the beginning of the
     * segment, and we can keep it in the protected region as well to double
     * check for heap corruption (ie: the user wrote to addresses BEFORE his
     * memory)
     */
    *(size_t *)buf = len;
    *(size_t *)(buf+offset+len) = len;

    if(mprotect(buf+offset+len, PAGE_SIZE, PROT_NONE) == -1)
    {
        perror("mymalloc/mprotect");
        exit(1);
    }

    return buf+offset;
}

// malloc and clear
void * mmap_calloc(size_t size, size_t elem_n)
{

	// allocate the new pages
	void * allocation = mmap_malloc((elem_n * size));
	if(!allocation)
		return NULL;

	// zero out the allocation
	memset(allocation, 0x00, (elem_n * size));

	// return the allocation
	return allocation;

}

// reallocate memory (requires an old size pointer because this isn't
// managed heap memory).
void * mmap_realloc(void * ptr, size_t new_size, size_t old_size)
{

	// create new allocation
	void * allocation = mmap_calloc(new_size, 1);
	if(!allocation)
		return NULL;

	// copy in new data
	memcpy(allocation, ptr, old_size);

	// return indicating null
	return NULL;

}

// frees a memory page
void mmap_free(void * buf)
{

    void *start = (void *)
    (
    		(long) buf & PAGE_MASK
    );

    size_t offset = (long) buf & ~PAGE_MASK;

    /* Will segfault here on double-free */
    size_t len = * (size_t *) start;
    static int zfd = 0;


    /* Check to see if we had to tack on another page in mymalloc */
    if((long) buf - (long) start < sizeof(size_t))
    {
        start -= PAGE_SIZE;
        len = *(size_t *)start;
    }

    /* Unprotect our memory */
    if(mprotect(buf+len, PAGE_SIZE, PROT_READ) == -1)
    {

        perror("myfree/mprotect");
        raise(SIGSEGV);

    }

    /* Check to make sure lengths are consistant. If not, advise user to try
       using the (not yet written) underflow version of this library */
    if(*(size_t *)(buf + len) != *(size_t *)start)
    {
        fprintf(stderr, "Error: heap corruption. Inconsistant allocation "
                "sizes. Try using the underflow option to pinpoint source "
                "of error\n");
        raise(SIGSEGV);
    }

    if(munmap(start, len+offset+PAGE_SIZE) == -1)
    {
        perror("myfree/munmap");
        raise(SIGSEGV);
    }

    /* Get a file descriptor for /dev/zero */
   if(!zfd)
   {
	   if((zfd = open("/dev/zero", O_RDWR)) == -1)
	   {
		   perror("myfree/devzero");
		   return;
	   }
   }


    if
    (
    	mmap
    	(
    		start,
    		len+offset+PAGE_SIZE,
    		PROT_NONE,
    		MAP_FIXED | MAP_PRIVATE,
    		zfd,
    		0
    	) == (void *) -1
    )
    {

        perror("myfree/mremap");
        return;

    }

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Slab Allocation %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


