#pragma once

// Super cool when warnings are errors!
#define TODO c_attr(warning("This is not implemented yet!"))

#ifdef NDEBUG
#define assert_cmp(lhs, cmp, rhs, msg)
#else
#define assert_cmp(lhs, cmp, rhs, msg) do { \
    let _lhs = lhs; \
    let _rhs = rhs; \
    if(!(_lhs cmp _rhs)) { \
        logd_( \
            "\n=== ASSERTION FAILED ===\n" \
            "(", msg, ")\n" \
            "In ", __FILE__, ":", __LINE__, ", in function ", __func__, ":\n" \
            "Expected...\n" \
            "    `", #lhs, " ", #cmp, " ", #rhs, "`\n" \
            "... But :\n" \
            "lhs = ", _lhs, "; (", #lhs, ")\n" \
            "rhs = ", _rhs, "; (", #rhs, ")\n", \
            "\nAborting.\n" \
            "========================\n" \
        ); \
        abort(); \
    } \
} while(0)
#endif

