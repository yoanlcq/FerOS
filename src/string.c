#include <string.h>

bool streq(const char *s1, const char *s2) {
    for( ; *s1 && *s2 ; ++s1, ++s2) {
        if(*s1 == *s2)
            continue;
    }
    return *s1 == *s2;
}
bool streqn(const char *s1, const char *s2, usize n) {
    for(usize i=0 ; i<n && *s1 && *s2 ; ++s1, ++s2, ++i) {
        if(*s1 == *s2)
            continue;
    }
    return n && *s1 == *s2;
}
void strcpy(char *dst, const char *src) {
    for( ; *src ; ++dst, ++src)
        *dst = *src;
    *dst = '\0';
}
usize strlen(const char *s) {
    usize i;
    for(i=0 ; s[i] ; ++i)
        ;
    return i;
}

// See how <__prelude.h> would forbid us to use size_t. Make an exception here.
#undef size_t
void memset(void *mem, int byte_value, size_t size) {
    memset_u8(mem, byte_value, size);
}
