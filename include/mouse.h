#pragma once

#include <isr.h>

void mouse_setup();
u8 mouse_read();
void mouse_write(u8 a_write);
void mouse_handler(IsrContext *r);
void mouse_wait_in();
void mouse_wait_out();
