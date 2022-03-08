#ifndef HEAP_H_
#define HEAP_H_

extern char* heap_alloc(size_t);
extern void heap_dealloc(char* ptr);
extern void console_alloced();
extern void console_dealloced();

#endif
