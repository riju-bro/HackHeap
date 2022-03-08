#ifndef HEAP_H_
#define HEAP_H_
#include <stdbool.h>

extern void* heap_alloc(size_t);
extern void heap_dealloc(void* ptr);
extern void console_alloced();
extern void console_dealloced();
extern bool is_alloced(void* ptr);

#endif
