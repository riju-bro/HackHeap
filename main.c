#include <stdio.h>
#include <assert.h>
#include <string.h>

#define HEAP_CAP 10000
#define HEAP_ALLOCED_CAP 1024
#define DE_ALLOCED_PTR_CAP 1024

char heap[HEAP_CAP] = {0};
size_t heap_size = 0;

// components needed to keep track of allocated memmory
typedef struct{
	char* ptr;
	size_t size;
} HEAP_ALLOCED;

typedef struct
{
	char* ptr;
	size_t size;
}DE_ALLOCED;


HEAP_ALLOCED HEAP_ALLOCED_PTR[HEAP_ALLOCED_CAP] = {0};
size_t heap_alloced_size = 0;

DE_ALLOCED DE_ALLOCED_PTR[DE_ALLOCED_PTR_CAP] = {0};
size_t de_alloced_size = 0;

char* heap_alloc(size_t);
void heap_dealloc(char* ptr);
int contains_alloced(char* ptr);
int contains_dealloced(char* ptr);

// console the heap_alloced_ptr array
void console_heapalloced()
{
	for (size_t i = 0; i < heap_alloced_size; i++)
		printf("ptr: %p \t size: %lu\n", HEAP_ALLOCED_PTR[i].ptr,
										HEAP_ALLOCED_PTR[i].size);
	
}

// console the de_alloced_ptr array
void console_de_alloced()
{
	for (size_t i = 0; i < de_alloced_size; i++)
		printf("ptr: %p \t size: %lu\n", DE_ALLOCED_PTR[i].ptr,
										DE_ALLOCED_PTR[i].size);
	
}
int main()
{
	int *ptr = (int*) heap_alloc(4);
	*ptr = 444;
	printf("*ptr: %d \t ptr: %p \n", *ptr, ptr);
}


char* heap_alloc(size_t size){

	// return null if size is invalid
	if(size <= 0)
		return NULL;

	HEAP_ALLOCED chunk;
	char *allocated_ptr;

	// check the DE_ALLOCED_PTR can afford the requirement
	int i;
	for(i = 0; i < heap_alloced_size; i++)
		if(size <= DE_ALLOCED_PTR[i].size)
			break;
	
	if(i != heap_alloced_size){

		// allocating the memmory
		allocated_ptr = DE_ALLOCED_PTR[i].ptr;
		if(DE_ALLOCED_PTR[i].size == size)
		{
			// removing information from DE_ALLOCED_PTR of alloced size
			DE_ALLOCED_PTR[i] = DE_ALLOCED_PTR[de_alloced_size-1];

			// popping last element and decrementing the de_alloced_size
			memset(&DE_ALLOCED_PTR[--de_alloced_size], 0, sizeof(DE_ALLOCED));
		}
		// DE_ALLOCED_PTR[i].size is greater than sufficient size
		else
			DE_ALLOCED_PTR[i].size -= size;
	}
	else{	
		// check heap can afford the requirement
		assert(heap_size + size <= HEAP_CAP);

		// allocating ptr and incrementing heap_size
		allocated_ptr = heap + heap_size;
		heap_size += size;
	}
	// keep tracking the allocated memmory
	chunk.ptr = allocated_ptr;
	chunk.size = size;

	// insert alloced chunks to HEAP_ALLOCED_PTR
	HEAP_ALLOCED_PTR[heap_alloced_size++] = chunk;

	return allocated_ptr;
}

void heap_dealloc(char* ptr)
{
	int i = contains_alloced(ptr);

	// invalid ptr
	if(i == heap_alloced_size)
		return;
	
	// clean the memmory
	size_t size = HEAP_ALLOCED_PTR[i].size;
	memset(ptr, 0, size);
	
	// removing informations of deallocated memmory from HEAP_ALLOCED_PTR
	HEAP_ALLOCED_PTR[i] = HEAP_ALLOCED_PTR[heap_alloced_size-1];

	// decrementing the heap_alloced_size and popping last element
	memset(&HEAP_ALLOCED_PTR[--heap_alloced_size], 0, sizeof(HEAP_ALLOCED));

	// check deallocated ptr is contigous part of any DE_ALLOCED_PTR
	int ind = contains_dealloced(ptr);

	// memmory are contigous 
	if(ind != de_alloced_size)
		DE_ALLOCED_PTR[ind].size += size;

	else{
		// pushing deallocated elements
		DE_ALLOCED chunk = {
			.ptr = ptr,
			size = size
		};
		DE_ALLOCED_PTR [de_alloced_size++] = chunk;
	}
}

// FIXME operation tiem is O(de_alloced_size) for a ptr
// returns index if ptr is valid else heap_alloced_size
int contains_alloced(char* ptr)
{
	int i = 0;
	for (i = 0; i < heap_alloced_size; i++)
		if(ptr == HEAP_ALLOCED_PTR[i].ptr)
			break;
	return i;
}

// FIXME operation tiem is O(de_alloced_size) for a ptr	
int contains_dealloced(char* ptr)
{
	// check the any memmory adjascent to ptr
	int i;
	for (i = 0; i < de_alloced_size; i++)
		if(DE_ALLOCED_PTR[i].ptr + DE_ALLOCED_PTR[i].size == ptr)
			break;
	return i;	
}

