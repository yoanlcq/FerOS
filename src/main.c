#include <stdint.h>
#include <multiboot.h>
#include <vga.h>
#include <log.h>
// TODO: Learn how to move the Text VGA cursor
// TODO: Implement useful printf-like primitives
// TODO: Perform some checks from the multiboot header
// TODO: Use _rdtsc() from <x86intrin.h>
// TODO: Get a cool video mode by setting the flags in the multiboot header

void main(uint32_t mb_magic, MultibootInfo *mb_info) {
    // TODO: We might want to put this kind of code elsewhere ?
    // For instance, in some kind of "early main" ??
    log_setup();

    (void)mb_info; // We'll stop ignoring you at some point, promise!

    if (mb_magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        vga_puts_logd("Invalid multiboot bootloader magic value!", VgaLightRed, VgaBlack);
        return;
    }

    vga_puts_logd("Hello, kernel world!", VgaLightGreen, VgaBlack);
    logd("Hello host!");
}
