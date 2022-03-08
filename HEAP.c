#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "HEAP.h"
#include <stdbool.h>

#define HEAP_CAP 10000
#define ALLOCED_CAP 1024
#define DEALLOCED_CAP 1024
#define ll long long int

char heap[HEAP_CAP] = {0};
size_t heap_size = 0;

// components needed to keep track of allocated memmory
typedef struct{
	void* ptr;
	size_t size;
    bool deleted;
}ALLOCED;

typedef struct
{
	void* ptr;
	size_t size;
}DEALLOCED;


ALLOCED ALLOCED_PTR[ALLOCED_CAP] = {0};
DEALLOCED DEALLOCED_PTR[DEALLOCED_CAP] = {0};
size_t dealloced_size = 0;

void* heap_alloc(size_t);
void heap_dealloc(void* ptr);
void insert_alloced(void* ptr, size_t size);
int contains_alloced(void* ptr);
void insert_or_merge_dealloced(void* ptr, size_t size);
void remove_dealloced(int i);

// console the ALLOCED_PTR array
void console_alloced()
{
    for (size_t i = 0; i < ALLOCED_CAP; i++)
        if(ALLOCED_PTR[i].ptr != 0)
            printf("ptr: %p \t size: %lu\n", ALLOCED_PTR[i].ptr,
                                            ALLOCED_PTR[i].size);
	
}

// console the DEALLOCED_PTR array
void console_dealloced()
{
    for (size_t i = 0; i < dealloced_size; i++)
        printf("ptr: %p \t size: %lu\n", DEALLOCED_PTR[i].ptr,
                                    DEALLOCED_PTR[i].size);

    // console the remaning chunks size in heap array
    printf("ptr: %p \t size: %zu\n", heap + heap_size,
                                HEAP_CAP - heap_size);

}

bool is_alloced(void* ptr)
{
    return contains_alloced(ptr) != ALLOCED_CAP;
}

void* heap_alloc(size_t size){

	// return null if size is invalid
	if(size <= 0)
		return NULL;


    // check the DEALLOCED_PTR can afford the requirement
	int i;
    // first fit searching alogorithm
    for(i = 0; i < dealloced_size; i++)
        if(size <= DEALLOCED_PTR[i].size)
			break;
	
    void* allocated_ptr;

    // DEALLOCED_PTR can afford the requirement
    if(i != dealloced_size)
    {
        allocated_ptr = DEALLOCED_PTR[i].ptr;

        if(DEALLOCED_PTR[i].size == size)
            remove_dealloced(i);

        // DEALLOCED_PTR[i].size is greater than sufficient size
        else
        {
            DEALLOCED_PTR[i].ptr += size;
            DEALLOCED_PTR[i].size -= size;
        }
    }else{
        // heap array can afford the requirement
        assert(heap_size + size <= HEAP_CAP);
        allocated_ptr = heap + heap_size;
        heap_size += size;
    }
    // keep tracking the allocated memmory
    insert_alloced(allocated_ptr, size);
    return allocated_ptr;
}

void heap_dealloc(void* ptr)
{
	int i = contains_alloced(ptr);

	// invalid ptr
    if(i == ALLOCED_CAP)
		return;

    size_t size = ALLOCED_PTR[i].size;
    // removing informations of deallocated memmory from ALLOCED_PTR
    memset(ALLOCED_PTR + i, 0, sizeof (ALLOCED));
    ALLOCED_PTR[i].deleted = true;

    insert_or_merge_dealloced(ptr, size);
}

// functions for insert using hash table
// insert O(1)
void insert_alloced(void* ptr, size_t size)
{
    int i = (ll) ptr % ALLOCED_CAP;
    // FIXME: EXCEPTION if HEAP_ALLOCED_PTR is filled it will cause to infinite loop
    // search for empty space in ALLOCED_PTR
    while(ALLOCED_PTR[i].ptr != 0)
        // linear probing in open addressing    
        i = (i + 1) % ALLOCED_CAP;
    ALLOCED_PTR[i].ptr = ptr;
    ALLOCED_PTR[i].size = size;
}

// functions for search using hash table
// search O(1)
// returns index if ptr is valid else ALLOCED_CAP
int contains_alloced(void* ptr)
{
    int hashed_ind = (ll) ptr % ALLOCED_CAP;
    int i = hashed_ind;
    // linear probing in open addressing
    do{
        // ptr found
        if(ALLOCED_PTR[i].ptr == ptr)
            return i;
        // empty slot found and not deleted
        // if a empty slot found and deleted it wont' stop the search
        else if(ALLOCED_PTR[i].ptr == 0 && !ALLOCED_PTR[i].deleted)
            break;
        i = (i+1) % ALLOCED_CAP;
    }while(i != hashed_ind);
    // ptr not found
    return ALLOCED_CAP;
}

void insert_or_merge_dealloced(void* ptr, size_t size)
{
    // Search for the adjacent memory block to the (heap + heap_size).
    if((heap + heap_size) - size == ptr){
        heap_size -= size;

        // Search for the contiguous memory block
        // FIXME: This search costs O(dealloced_size) time
        for(int i = 0; i < dealloced_size; i++)
        {
            if((heap + heap_size) - DEALLOCED_PTR[i].size == DEALLOCED_PTR[i].ptr){
                heap_size -= DEALLOCED_PTR[i].size;
                remove_dealloced(i);
            }
        }
        return;
    }

    // Search for the adjacent memory block.
    // FIXME: This search costs O(dealloced_size) time
    for(int i = 0; i < dealloced_size; i++)
    {
        if(DEALLOCED_PTR[i].ptr + DEALLOCED_PTR[i].size == ptr)
        {
            DEALLOCED_PTR[i].size += size;
            return;
        }
        if(DEALLOCED_PTR[i].ptr - size == ptr)
        {
            DEALLOCED_PTR[i].size += size;
            DEALLOCED_PTR[i].ptr = ptr;
            return;
        }
    }

    DEALLOCED dealloced_chunk = {
        .ptr = ptr,
        .size = size
    };

    DEALLOCED_PTR[dealloced_size++] = dealloced_chunk;
}

void remove_dealloced(int i)
{

    DEALLOCED_PTR[i] = DEALLOCED_PTR[--dealloced_size];
    memset(DEALLOCED_PTR + dealloced_size, 0, sizeof(DEALLOCED));
}



























