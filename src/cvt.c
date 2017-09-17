#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <cvt.h>

// NOTE: -9223372036854775808ll gives a warning in GCC
//     https://gcc.gnu.org/bugzilla/show_bug.cgi?id=52661
// So we have both INTXX_MIN and INTXX_MIN_STR as parameters since
// we can't just get the latter by stringifying the former.
#define __impl_str_from_xxx(uxx, ixx, UMAXCHARS, IMAXCHARS, INTXX_MIN, INTXX_MIN_STR, DIV_INIT) \
void str_from_##ixx(char s[static IMAXCHARS], ixx i) { \
    if(i == INTXX_MIN) { \
        strcpy(s, INTXX_MIN_STR); \
        return; \
    } \
    if(i < 0) { \
        *s = '-'; \
        ++s; \
        i = -i; \
    } \
    str_from_##uxx(s, i); \
} \
void str_from_##uxx(char s[static UMAXCHARS], uxx i) { \
    /* Special case not properly handled by our loop */ \
    if(i == 0) { \
        s[0] = '0'; \
        s[1] = '\0'; \
        return; \
    } \
    bool is_first_digit = true; \
    for(uxx div=DIV_INIT ; div > 0 ; i %= div, div /= 10) { \
        let digit_value = i/div; \
        if(!digit_value && is_first_digit) \
            continue; \
        is_first_digit = false; \
        *s = '0' + digit_value; \
        ++s; \
    } \
    *s = '\0'; \
}

__impl_str_from_xxx(u8 , i8 , 4, 5, INT8_MIN, "-128", 100);
__impl_str_from_xxx(u16, i16, 6, 7, INT16_MIN, "-32768", 10000);
__impl_str_from_xxx(u32, i32, 11, 12, INT32_MIN, "-2147483648", 1000000000);
__impl_str_from_xxx(u64, i64, 20, 21, INT64_MIN, "-9223372036854775808", 1000000000000000000ull);

#undef __impl_str_from_xxx

/*
#include <stdio.h>
void test_str_from_i32() {
    i32 ints[] = { 
        0, 42, 2000000000, 0xffffffff, 0x80000001, 0x80000000, 0x7fffffff 
    };
    const char *strs[] = { 
        "0", "42", "2000000000", "-1", "-2147483647", "-2147483648", "2147483647"
    };
    for(int i=0 ; i<7 ; ++i) {
        char buf[12];
        str_from_i32(buf, ints[i]);
        printf("%i = %s (expected %s)\n", ints[i], buf, strs[i]);
    }
    return 0;
}
*/
