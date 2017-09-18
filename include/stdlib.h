// Our own, very restricted subset of <stdlib.h>.
//
// The primary reason for having it in the first place is for compatibility
// with <mm_malloc.h> included by <x86intrin.h>, because it requires the
// `malloc` function.

#pragma once

TODO void* malloc(size_t size);
TODO void free(void *p);
/*
alloc
alloc_zeroed
realloc
dealloc

virtual_alloc...
aligned_alloc

abs
labs
llabs
div
ldiv
lldiv
*/

static inline noreturn void hang(); // Defined in the prelude
static inline noreturn void abort() { hang(); }
static inline noreturn void exit(int code) {
    (void)code; // What to do ? Move to EAX ? Or call logd() ?
    hang();
}
