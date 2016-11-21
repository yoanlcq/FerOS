#include <kern/stdc.h>

bool streq(const char *s1, const char *s2) {
    for( ; *s1==*s2 && *s1 && *s2 ; ++s1, ++s2);
    return *s1==*s2;
}
bool streqn(const char *s1, const char *s2, size_t n) {
    for( ; *s1==*s2 && *s1 && *s2 && n ; ++s1, ++s2, --n);
    return *s1==*s2;
}
