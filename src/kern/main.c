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

typedef struct {
    char signature[4];  // must be "VESA" to indicate valid VBE support
    uint16_t version;     // VBE version; high byte is major version, low byte is minor version
    uint32_t oem;         // segment:offset pointer to OEM
    uint32_t capabilities;// bitfield that describes card capabilities
    uint32_t video_modes;      // segment:offset pointer to list of supported video modes
    uint16_t video_memory;      // amount of video memory in 64KB blocks
    uint16_t software_rev;      // software revision
    uint32_t vendor;         // segment:offset to card vendor string
    uint32_t product_name;      // segment:offset to card model name
    uint32_t product_rev;      // segment:offset pointer to product revision
    char reserved[222];      // reserved for future expansion
    char oem_data[256];      // OEM BIOSes store their strings in this area
} vbe_info;
     
extern void Entry16(void);

static void int10h_4f00(void) {
    vbe_info vi;
    //Entry16(); // Welp
    uint16_t ax;
    asm(
        "mov ax, 0x4f00\n\t"
        "mov di, %1\n\t"
        //"int 0x10\n\t"
        "mov %0, ax\n\t"
        : "=r"(ax) : "D"((uint16_t)(uintptr_t)&vi)
    );
    /*
    asm(
        "cli\n\t"
        "mov     eax,cr0\n\t"
        "and     al,not 1\n\t"
        "mov     cr0,eax\n\t"
        "sti\n\t"
        "hlt\n\t"
    );
    */
    hang();
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
    char hello[] = "Hello world!";
    char msg[] = "sizeof(void*) is ";
    for(x=0 ; x<sizeof(hello)-1 ; ++x)
        vga[x] = vga_entry(hello[x], VGA_WHITE, VGA_BLACK);
    for(x=0 ; x<sizeof(msg)-1 ; ++x)
        vga[VGA_WIDTH+x] = vga_entry(msg[x], VGA_WHITE, VGA_BLACK);
    vga[VGA_WIDTH+x] = vga_entry('0'+sizeof(void*), VGA_WHITE, VGA_BLACK);
    int10h_4f00();
    hang();
}
