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
