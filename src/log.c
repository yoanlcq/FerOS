#include <log.h>

static Serial logd_serial1 = {0};
static Serial logd_serial2 = {0};

void log_setup() {
#ifdef LOGD_PORT1
    logd_serial1 = Serial_new(LOGD_PORT1);
#ifdef LOGD_PORT2
    logd_serial2 = Serial_new(LOGD_PORT2);
#endif
#endif
}

// NOTE: Logs are NOT implemented with an `outsb`-`strlen` pair, because
// it's not cache-efficient, and there is some risks mentioned by
// the intel manual, that serial devices might not be able to handle the
// speed at which `outsb` executes.
// So sending one byte at a time in a blocking manner might be slower
// sometimes, but it's safer.

#ifdef LOGD_PORT1
void logd_noeol(const char *s) {
    for(int i=0 ; s[i] ; ++i) {
        Serial_send_u8(logd_serial1, s[i]);
    }
#ifdef LOGD_PORT2
    for(int i=0 ; s[i] ; ++i) {
        Serial_send_u8(logd_serial2, s[i]);
    }
#endif
}
void logd(const char *s) {
    logd_noeol(s);
    Serial_send_u8(logd_serial1, '\n');
#ifdef LOGD_PORT2
    Serial_send_u8(logd_serial2, '\n');
#endif
}
#endif
