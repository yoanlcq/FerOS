#pragma once

#if defined(__linux__)
#error "Please use a genuine cross-compiler."
#endif
 
#if !defined(__i386__)
#error "The kernel can only be compiled with a x86-elf compiler."
#endif

#define asm __asm__
