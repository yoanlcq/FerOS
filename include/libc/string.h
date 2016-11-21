#ifndef LIBC_STRING_H
#define LIBC_STRING_H

#include <kern/stdc.h>

bool streq(const char *s1, const char *s2);
bool streqn(const char *s1, const char *s2, size_t n);
/*
strcat();
strncat();
strchr();
strrchr();
strchrnul();
strcmp();
strncmp();
strcpy();
strncpy();
strdup();
strfry();
strlen();
strstr();
strtok();
*/
#endif /* LIBC_STRING_H */
