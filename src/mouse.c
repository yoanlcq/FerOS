#include <mouse.h>

// Experimental !!
// Right now this is a shameful copy-paste from
//     https://houbysoft.com/download/mouse.html
// . I've grabbed the code to make it easier for me later, but I'll want
// to fully understand what's going on and properly test it.

// TODO: Support hot-plugging and unplugging. Right now, we don't receive
// any more IRQs after unplugging and re-plugging the mouse

// TODO: Don't block indefinitely and support hot-pluggin instead.
void mouse_wait_in() {
    for(;;) {
        if(inb(0x64) & 1) {
            break;
        }
        _mm_pause();
    }
}
void mouse_wait_out() {
    for(;;) {
        if(!(inb(0x64) & 2)) {
            break;
        }
        _mm_pause();
    }
}

void mouse_handler(IsrContext *r) {
    (void)r;
    static i32 x = 0, y = 0;
    static u8 cycle = 0;
    static i8 mouse_bytes[3];
    mouse_bytes[cycle++] = inb(0x60);
    
    if (cycle == 3) { // if we have all the 3 bytes...
        cycle = 0; // reset the counter
        // do what you wish with the bytes, this is just a sample
        if ((mouse_bytes[0] & 0x80) || (mouse_bytes[0] & 0x40))
            return; // the mouse only sends information about overflowing, do not care about it and return
        if (!(mouse_bytes[0] & 0x20))
            y |= 0xFFFFFF00; //delta-y is a negative value
        if (!(mouse_bytes[0] & 0x10))
            x |= 0xFFFFFF00; //delta-x is a negative value
        if (mouse_bytes[0] & 0x4)
            logd("Middle button is pressed!");
        if (mouse_bytes[0] & 0x2)
            logd("Right button is pressed!");
        if (mouse_bytes[0] & 0x1)
            logd("Left button is pressed!");
        // do what you want here, just replace the puts's to execute an action for each button
        // to use the coordinate data, use mouse_bytes[1] for delta-x, and mouse_bytes[2] for delta-y
    }
}

void mouse_write(u8 a_write) {
    mouse_wait_out();
    outb(0x64, 0xD4);
    mouse_wait_out();
    outb(0x60, a_write);
}

u8 mouse_read() {
    mouse_wait_in();
    return inb(0x60);
}

void mouse_setup() {
    logd("Waiting for the mouse...");

    mouse_wait_out();
    outb(0x64,0xA8);
    mouse_wait_out();
    outb(0x64,0x20);
    u8 status_byte;
    mouse_wait_in();
    status_byte = (inb(0x60) | 2);
    mouse_wait_out();
    outb(0x64, 0x60);
    mouse_wait_out();
    outb(0x60, status_byte);
    mouse_write(0xF6);
    mouse_read();
    mouse_write(0xF4);
    mouse_read();
}
