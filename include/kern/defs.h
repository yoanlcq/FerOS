#ifndef KERN_DEFS_H
#define KERN_DEFS_H

#if defined(__linux__)
#error "You should rather use a cross-compiler."
#endif
 
#if !defined(__i386__)
#error "The kernel can only be compiled with a x86-elf compiler."
#else
#define KERN_ARCH_I686
#endif

#define asm __asm__


#endif/* KERN_DEFS_H*/
