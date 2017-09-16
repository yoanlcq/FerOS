#include <serial.h>

#ifdef QEMU_GUEST
// COM1 => log file, COM2 => stdio
// See Makefile
#define LOGD_PORT1 COM1
#define LOGD_PORT2 COM2
#endif


void log_setup();

// logd() always appends a '\n'.
#ifdef LOGD_PORT1
void logd_noeol(const char *s);
void logd(const char *s);
#else
#define logd(s)
#define logd_noeol(s)
#endif
