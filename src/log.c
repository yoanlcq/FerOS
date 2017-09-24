#include <log.h>
#include <cvt.h>

#ifdef LOGD_PORT1
static Serial ser1 = {0};
#ifdef LOGD_PORT2
static Serial ser2 = {0};
#endif
#endif


void log_setup() {
#ifdef LOGD_PORT1
    ser1 = Serial_new(LOGD_PORT1);
#ifdef LOGD_PORT2
    ser2 = Serial_new(LOGD_PORT2);
#endif
#endif
}

void log_shutdown() {
    // Nothing for now
}

// NOTE: logd_cstr is NOT implemented with an `outsb`-`strlen` pair, because
// it's not cache-efficient, and there is some risk mentioned by
// the intel manual, that serial devices might not be able to handle the
// speed at which `outsb` executes.
// So sending one byte at a time in a blocking manner might be slower
// sometimes, but it's safer.
// We should let a profiler speak, but meh - we don't have one right now.

#ifdef LOGD_PORT1

void logd_str(const char *s) { logd_cstr(s); }

void logd_cstr(const char *s) {
    for(usize i=0 ; s[i] ; ++i) {
        Serial_send_u8(ser1, s[i]);
    }
#ifdef LOGD_PORT2
    for(usize i=0 ; s[i] ; ++i) {
        Serial_send_u8(ser2, s[i]);
    }
#endif
}

void logd_u32(u32 value) {
    char s[11];
    str_from_u32(s, value);
    logd_cstr(s);
}
void logd_i32(i32 value) {
    char s[12];
    str_from_i32(s, value);
    logd_cstr(s);
}
void logd_u64(u64 value) {
    char s[20];
    str_from_u64(s, value);
    logd_cstr(s);
}
void logd_i64(i64 value) {
    char s[21];
    str_from_i64(s, value);
    logd_cstr(s);
}

void logd_bool (bool value) { logd_cstr(value ? "true" : "false"); }
void logd_char (char value) { char s[2] = { value, '\0' }; logd_cstr(s); }
void logd_u8   (u8   value) { logd_u32(value); }
void logd_u16  (u16  value) { logd_u32(value); }
void logd_i8   (i8   value) { logd_i32(value); }
void logd_i16  (i16  value) { logd_i32(value); }
void logd_f32  (f32  value) { (void)value; }
void logd_f64  (f64  value) { (void)value; }
void logd_ptr  (const void* value) { (void)value; }
void logd_cptr (const void* value) { (void)value; }

#endif // LOGD_PORT1
