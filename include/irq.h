#pragma once

u64 irq0_get_timer_ticks();
f32 irq0_get_timer_frequency();
void irq0_set_timer_frequency(f32 hz);
void irq0_timer_wait_ticks(u32 ticks);

