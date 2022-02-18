#ifndef HEAP_H_
#define HEAp_H_

char* heap_alloc(size_t);
void heap_dealloc(char* ptr);
void console_heapalloced();
void console_de_alloced();

#endif