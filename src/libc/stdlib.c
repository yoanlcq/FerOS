#include <stdint.h>
#include <utils.h>

int streq(const char *s1, const char *s2) {
    for( ; *s1==*s2 && *s1 && *s2 ; ++s1, ++s2);
    return *s1==*s2;
}
int streqn(const char *s1, const char *s2, uint32_t n) {
    for( ; *s1==*s2 && *s1 && *s2 && n ; ++s1, ++s2, --n);
    return *s1==*s2;
}
