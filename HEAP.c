#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "HEAP.h"
#include <stdbool.h>
#include <stdint.h>


#define HEAP_CAP_BYTES 80000
#define HEAP_CAP HEAP_CAP_BYTES / 8
#define ALLOCED_CAP 1024
#define DEALLOCED_CAP 1024
#define REACHABLE_CAP 1024
#define ll long long int


uintptr_t heap[HEAP_CAP] = {0};
size_t heap_size = 0;

// components needed to keep track of allocated memmory
typedef struct{
    uintptr_t* ptr;
	size_t size;
    bool deleted;
}ALLOCED;

typedef struct
{
    uintptr_t* ptr;
	size_t size;
}DEALLOCED;


ALLOCED ALLOCED_PTR[ALLOCED_CAP] = {0};
size_t alloced_size = 0;
DEALLOCED DEALLOCED_PTR[DEALLOCED_CAP] = {0};
size_t dealloced_size = 0;
uintptr_t* REACHABLE_PTR[REACHABLE_CAP];
size_t reachable_size = 0;

void* heap_alloc(size_t);
void insert_alloced(uintptr_t* ptr, size_t size);
int contains_alloced(uintptr_t* ptr);

void heap_dealloc(void* void_ptr);
void heap_dealloc_helper(uintptr_t* ptr, int i);
void insert_or_merge_dealloced(uintptr_t* ptr, size_t size);
void remove_dealloced(int i);

void heap_collect(uintptr_t* stack_end);
void heap_collect_helper(uintptr_t* ptr);
void insert_reachable(uintptr_t* ptr);
bool is_reachable(uintptr_t* ptr);

// console the ALLOCED_PTR array
void console_alloced()
{
    for (size_t i = 0; i < ALLOCED_CAP; i++)
        if(ALLOCED_PTR[i].ptr != 0)
            printf("ptr: %p \t byte_size: %lu\n", ALLOCED_PTR[i].ptr,
                                            ALLOCED_PTR[i].size * 8);
	
}

bool is_alloced(uintptr_t* ptr)
{
    return contains_alloced(ptr) != ALLOCED_CAP;
}

// console the DEALLOCED_PTR array
void console_dealloced()
{
    for (size_t i = 0; i < dealloced_size; i++)
        printf("ptr: %p \t byte_size: %lu\n", DEALLOCED_PTR[i].ptr,
                                    DEALLOCED_PTR[i].size * 8);

    // console the remaning chunks size in heap array
    printf("ptr: %p \t byte_size: %zu\n", heap + heap_size,
                                (HEAP_CAP - heap_size) * 8);

}

void console_reachable_chunks()
{
    for(int i = 0; i < REACHABLE_CAP; i++)
    {
        if(REACHABLE_PTR[i] == 0)
            continue;
        printf("%p \n", (uintptr_t*) REACHABLE_PTR[i]);
    }
}

void* heap_alloc(size_t byte_size){

	// return null if size is invalid
    if(byte_size <= 0)
		return NULL;

    size_t size;
    if(byte_size % 8 == 0)
        size = byte_size / 8;
    else
        size = byte_size / 8 + 1;

    // check the DEALLOCED_PTR can afford the requirement
	int i;
    // first fit searching alogorithm
    for(i = 0; i < dealloced_size; i++)
        if(size <= DEALLOCED_PTR[i].size)
			break;
	
    uintptr_t* allocated_ptr;

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
    return (void*) allocated_ptr;
}

// functions for insert using hash table
// insert O(1)
void insert_alloced(uintptr_t* ptr, size_t size)
{
    assert(alloced_size < ALLOCED_CAP);

    int i = (ll) ptr % ALLOCED_CAP;
    while(ALLOCED_PTR[i].ptr != 0)
        // linear probing in open addressing
        i = (i + 1) % ALLOCED_CAP;
    ALLOCED_PTR[i].ptr = ptr;
    ALLOCED_PTR[i].size = size;
    alloced_size++;
}

// Using hashing
// search O(1)
// returns index if ptr is valid else ALLOCED_CAP
int contains_alloced(uintptr_t* ptr)
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

void heap_dealloc(void* void_ptr)
{
    uintptr_t* ptr = (uintptr_t*) void_ptr;
    int i = contains_alloced(ptr);

    // invalid ptr
    if(i == ALLOCED_CAP)
        return;
    heap_dealloc_helper(ptr, i);
}

void heap_dealloc_helper(uintptr_t* ptr, int i)
{
    size_t size = ALLOCED_PTR[i].size;
    // removing informations of deallocated memmory from ALLOCED_PTR
    memset(ALLOCED_PTR + i, 0, sizeof (ALLOCED));
    ALLOCED_PTR[i].deleted = true;

    insert_or_merge_dealloced(ptr, size);
}

void insert_or_merge_dealloced(uintptr_t* ptr, size_t size)
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

// garbage collector
void heap_collect(uintptr_t* stack_end)
{
    uintptr_t* stack_start = __builtin_frame_address(0);

    // insert reachable heap pointers to REACHABLE_PTR array
    for(; stack_start <= stack_end; stack_start += 1)
    {
        uintptr_t* p = (uintptr_t*) *stack_start;

        int ind = contains_alloced((uintptr_t*) p);
        size_t size = ALLOCED_PTR[ind].size;
        // p is allocated
        if(ind != ALLOCED_CAP)
        {
            insert_reachable((uintptr_t*) p);
            for(int i = 0; i < size; i++)
                heap_collect_helper((uintptr_t*) *(p+i));
        }
    }
    // traverse the ALLOCED_PTR and find the unreachable chunks in heap and deallocated them
    // FIXME it traverse the whole ALLOCED_PTR
    for(int i = 0; i < ALLOCED_CAP; i++)
    {
        // empty slot
        if(ALLOCED_PTR[i].ptr == 0)
            continue;
        if(!is_reachable((uintptr_t*) ALLOCED_PTR[i].ptr))
            heap_dealloc_helper(ALLOCED_PTR[i].ptr, i);
    }
}

void heap_collect_helper(uintptr_t* ptr)
{
    int ind = contains_alloced(ptr);
    size_t size = ALLOCED_PTR[ind].size;
    if(ind != ALLOCED_CAP)
    {
        insert_reachable((uintptr_t*) ptr);
        for(int i = 0; i < size; i++)
            heap_collect_helper((uintptr_t*) *(ptr+i));
    }
}

// Using hashing
bool is_reachable(uintptr_t* ptr)
{
    int hashed_ind = (ll) ptr % REACHABLE_CAP;
    int i = hashed_ind;
    // linear probing in open addressing
    do{
        // ptr is reachable
        if(REACHABLE_PTR[i] == ptr)
            return true;

        else if(ALLOCED_PTR[i].ptr == 0)
            break;
        i = (i+1) % ALLOCED_CAP;
    }while(i != hashed_ind);
    // ptr isn't reachable
    return false;
}

// Using hashing
void insert_reachable(uintptr_t* ptr)
{
    assert(reachable_size < REACHABLE_CAP);

    int i = (ll) ptr % REACHABLE_CAP;
    while(REACHABLE_PTR[i] != 0)
        // linear probing in open addressing
        i = (i + 1) % REACHABLE_CAP;
    REACHABLE_PTR[i]= ptr;
    reachable_size++;
}



















