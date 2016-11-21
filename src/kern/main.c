#include <kern/defs.h>
#include <kern/stdc.h>
#include <kern/vga.h>

/* For debugging. You can spot this one with 'info reg' on GDB ;)
 * Try 'make run', boot the system, and when it hangs, run 'make dbg'
 * in a separate shell. */
static void hang(void) {
    asm("mov eax, 0xdeadc0de");
    for(;;);
}

void kern_main(void) {
    size_t x, y, offset, step, entry;
    uint16_t *vga = VGA_BUFADDR;
    for(step=y=0 ; y<VGA_HEIGHT ; ++y, step=0) {
        for(offset=x=0 ; x<VGA_WIDTH ; ++x, ++step, step%=4) {
            if(!step) {
                ++offset;
                offset %= 6;
                entry = vga_entry('0'+y, 9+offset, 1+offset);
            }
            vga[y*VGA_WIDTH+x] = entry;
        }
    }
    hang();
}
