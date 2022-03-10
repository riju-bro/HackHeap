#ifndef HEAP_H_
#define HEAP_H_
#include <stdbool.h>
#include <stdint.h>

extern void* heap_alloc(size_t);
extern void heap_dealloc(void* ptr);
extern void heap_collect(uintptr_t* stack_end);

extern void console_alloced();
extern bool is_alloced(void* ptr);
extern void console_dealloced();
extern void console_reachable_chunks();

#endif
