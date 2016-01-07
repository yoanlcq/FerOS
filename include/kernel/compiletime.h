#if defined(__linux__)
#error "You should rather use a cross-compiler."
#endif
 
#if !defined(__i386__)
#error "The kernel can only be compiled with a x86-elf compiler."
#endif

