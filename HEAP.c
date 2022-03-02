#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "HEAP.h"
#include <stdbool.h>

#define HEAP_CAP 10000
#define ALLOCED_CAP 1024
#define DE_ALLOCED_CAP 1024
#define ll long long int

char heap[HEAP_CAP] = {0};
size_t heap_size = 0;

// components needed to keep track of allocated memmory
typedef struct{
	char* ptr;
	size_t size;
    bool deleted;
} ALLOCED;

typedef struct
{
	char* ptr;
	size_t size;
    bool deleted;
}DE_ALLOCED;


ALLOCED ALLOCED_PTR[ALLOCED_CAP] = {0};
DE_ALLOCED DE_ALLOCED_PTR[DE_ALLOCED_CAP] = {0};

char* heap_alloc(size_t);
void heap_dealloc(char* ptr);
void insert_alloced(char* ptr, size_t size);
void insert_dealloced(char* ptr, size_t size);
int contains_alloced(char* ptr);
int contains_dealloced(char* ptr);

// console the heap_alloced_ptr array
void console_heapalloced()
{
    for (size_t i = 0; i < ALLOCED_CAP; i++)
        if(ALLOCED_PTR[i].ptr != 0)
            printf("ptr: %p \t size: %lu\n", ALLOCED_PTR[i].ptr,
                                            ALLOCED_PTR[i].size);
	
}

// console the de_alloced_ptr array
void console_de_alloced()
{
    for (size_t i = 0; i < DE_ALLOCED_CAP; i++)
        if(DE_ALLOCED_PTR[i].ptr != 0)
            printf("ptr: %p \t size: %lu\n", DE_ALLOCED_PTR[i].ptr,
                                        DE_ALLOCED_PTR[i].size);
}

char* heap_alloc(size_t size){

	// return null if size is invalid
	if(size <= 0)
		return NULL;

	char *allocated_ptr;

	// check the DE_ALLOCED_PTR can afford the requirement
	int i;
    // first fit searching alogorithm
    // FIXME this searching may cause traversing the whole DE_ALLOCED_PTR array.
    for(i = 0; i < DE_ALLOCED_CAP; i++)
		if(size <= DE_ALLOCED_PTR[i].size)
			break;
	
    if(i != DE_ALLOCED_CAP){

		// allocating the memmory
		allocated_ptr = DE_ALLOCED_PTR[i].ptr;
		if(DE_ALLOCED_PTR[i].size == size)
        {
            // deleting the elemnt from the DE_ALLOCED_PTR
            memset(DE_ALLOCED_PTR+i, 0, sizeof(DE_ALLOCED));
            DE_ALLOCED_PTR[i].deleted = true;
        }
		// DE_ALLOCED_PTR[i].size is greater than sufficient size
        else
        {
            DE_ALLOCED_PTR[i].ptr += size;
			DE_ALLOCED_PTR[i].size -= size;
        }
	}
	else{	
		// check heap can afford the requirement
		assert(heap_size + size <= HEAP_CAP);

		// allocating ptr and incrementing heap_size
		allocated_ptr = heap + heap_size;
		heap_size += size;
	}
	// keep tracking the allocated memmory
    insert_alloced(allocated_ptr, size);
    return allocated_ptr;
}

void heap_dealloc(char* ptr)
{
	int i = contains_alloced(ptr);

	// invalid ptr
    if(i == ALLOCED_CAP)
		return;
	
	// clean the memmory
    size_t size = ALLOCED_PTR[i].size;
	memset(ptr, 0, size);
	
	// removing informations of deallocated memmory from HEAP_ALLOCED_PTR
    memset(ALLOCED_PTR + i, 0, sizeof (ALLOCED));
    ALLOCED_PTR[i].deleted = true;

	// check deallocated ptr is contigous part of any DE_ALLOCED_PTR
	int ind = contains_dealloced(ptr);

    // ptr is contigous of DE_ALLOCED_PTR[ind]
    if(ind != DE_ALLOCED_CAP){
        size += DE_ALLOCED_PTR[ind].size;
        ptr = DE_ALLOCED_PTR[ind].ptr;
        // ptr is contiguous of DE_ALLOCED_PTR[ind] so deletion and new insertion is needed
        memset(DE_ALLOCED_PTR + ind, 0, sizeof (DE_ALLOCED));
        DE_ALLOCED_PTR[ind].deleted = true;
        insert_dealloced(ptr, size);
    }
    else
        insert_dealloced(ptr, size);
}

// functions for insert and search using hash table
// search and insert O(1)

void insert_alloced(char* ptr, size_t size)
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

void insert_dealloced(char* ptr, size_t size)
{
    int i = (ll) (ptr+size) % DE_ALLOCED_CAP;
    // FIXME: EXCEPTION if HEAP_ALLOCED_PTR is filled it will cause to infinite loop
    // search for empty space in ALLOCED_PTR
    while(DE_ALLOCED_PTR[i].ptr != 0)
        // linear probing in open addressing
        i = (i + 1) % ALLOCED_CAP;
    DE_ALLOCED_PTR[i].ptr = ptr;
    DE_ALLOCED_PTR[i].size = size;

}

// returns index if ptr is valid else heap_alloced_size
int contains_alloced(char* ptr)
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

int contains_dealloced(char* ptr)
{
    int hashed_ind = (ll) ptr % DE_ALLOCED_CAP;
    int i = hashed_ind;

    char *temp_ptr;
    size_t temp_size;
    do{
        // ptr found
        temp_ptr = DE_ALLOCED_PTR[i].ptr;
        temp_size = DE_ALLOCED_PTR[i].size;
        if(ptr == temp_ptr + temp_size)
            return i;
        else if(ptr == 0 && !DE_ALLOCED_PTR[i].deleted)
            break;
        i = (i + 1) % DE_ALLOCED_CAP;
    }while(i != hashed_ind);
    return DE_ALLOCED_CAP;
}
























