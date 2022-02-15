#include <stdio.h>
#include <assert.h>

#define HEAP_CAP 10000

char heap[HEAP_CAP] = {0};
void* heap_ptr = heap;

void* heap_alloc(size_t);

int main(void) {
	
	int *ptr = (int*) heap_alloc(4);
	*ptr = 9999;
	printf("ptr: %d \n", *ptr);
	return 0;
}

void* heap_alloc(size_t size){

	// return null is size is invalid
	if(size <= 0)
		return NULL;
	assert(heap_ptr + size <= heap+HEAP_CAP);
	void *allocated_ptr;

	// allocating ptr and incrementing heap_ptr
	allocated_ptr = heap_ptr;
	heap_ptr += size;
	return allocated_ptr;
}


















