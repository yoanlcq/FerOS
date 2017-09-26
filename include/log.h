// Everything log-related - right now this defines the `logd()` and `logd_()`
// macros, which intent is to log debugging information.
// Under QEMU, this writes data to serial ports which are redirected to QEMU's
// stdio and a file.
//
// `logd()` takes any number of arguments and prints them (thanks to the magic
// of C11 generics, and the MAP() macro). The set of arguments is treated as
// a single logging unit (or entry). In practice right now, it just means that
// a `\n` is appended, but this might change in the future.
//
// For lack of a better name, `logd_()` does the same BUT the set of arguments
// is only treated as a _fraction_ of a logging unit. This is for convenience.
// This means that `logd_("foo"); log("bar")` do log "foobar" as a single
// entry.

#include <serial.h>

#ifdef IS_QEMU_GUEST
// COM1 => log file, COM2 => stdio
// See Makefile
#define LOGD_PORT1 COM1
#define LOGD_PORT2 COM2
#endif

void log_setup();
void log_shutdown();

#ifdef LOGD_PORT1

#define __logd_(x) logd_##x
#define __logd1(x) (def_generic_over_primitives_value(__logd_,x));
#define logd_(...) do { MAP(__logd1, __VA_ARGS__) } while(0)
#define logd(...) do { logd_(__VA_ARGS__); logd_("\n"); } while(0)

void logd_str  (const char* value);
void logd_cstr (const char* value);
void logd_bool (bool value);
void logd_char (char value);
void logd_u8   (u8   value);
void logd_i8   (i8   value);
void logd_u16  (u16  value);
void logd_i16  (i16  value);
void logd_u32  (u32  value);
void logd_i32  (i32  value);
void logd_u64  (u64  value);
void logd_i64  (i64  value);
TODO void logd_f32  (f32  value);
TODO void logd_f64  (f64  value);
TODO void logd_ptr  (const void* value);
TODO void logd_cptr (const void* value);

#else
// Disable debug-logging

#define _logd_ignore_arg(x) _allow_unused(x);
#define logd_(...) do { MAP(_logd_ignore_arg, __VA_ARGS__) } while(0)
#define logd(...)  do { MAP(_logd_ignore_arg, __VA_ARGS__) } while(0)

#endif
