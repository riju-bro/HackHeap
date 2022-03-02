#include <stdio.h>
#include "HEAP.h"

int main()
{
	int *ptr = (int*) heap_alloc(4);
	*ptr = 444;
	heap_dealloc((char*)ptr);

	// functions for debug
	printf("ALLOCED_PTR\n-----------\n");
	console_heapalloced();
	printf("DE_ALLOCED_PTR\n-----------\n");
	console_de_alloced();
	
}
