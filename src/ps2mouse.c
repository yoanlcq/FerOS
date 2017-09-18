// Experimental !!
// Right now this is a shameful copy-paste from
//     https://houbysoft.com/download/ps2mouse.html
// . I've grabbed the code to make it easier for me later, but I'll want
// to fully understand what's going on and properly test it.

typedef struct {} Regs;
TODO void irq_install_handler(u32 i, void (*handler)(Regs *r)) {
    (void)i;
    (void)handler;
}

void ps2mouse_wait(u8 type) {
    u32 timeout=100000;
    if(type==0) {
        while(timeout--) {
            if((inb(0x64) & 1)==1) {
                return;
            }
        }
    } else {
        while(timeout--) {
            if((inb(0x64) & 2)==0) {
                return;
            }
        }
    }
}

void ps2mouse_handler(Regs *r) {
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
            logd("Middle button is pressed!n");
        if (mouse_bytes[0] & 0x2)
            logd("Right button is pressed!n");
        if (mouse_bytes[0] & 0x1)
            logd("Left button is pressed!n");
        // do what you want here, just replace the puts's to execute an action for each button
        // to use the coordinate data, use mouse_bytes[1] for delta-x, and mouse_bytes[2] for delta-y
    }
}

void ps2mouse_write(u8 a_write) {
    ps2mouse_wait(1);
    outb(0x64, 0xD4);
    ps2mouse_wait(1);
    outb(0x60, a_write);
}

u8 ps2mouse_read() {
    ps2mouse_wait(0);
    return inb(0x60);
}

void ps2mouse_setup() {
    ps2mouse_wait(1);
    outb(0x64,0xA8);
    ps2mouse_wait(1);
    outb(0x64,0x20);
    u8 status_byte;
    ps2mouse_wait(0);
    status_byte = (inb(0x60) | 2);
    ps2mouse_wait(1);
    outb(0x64, 0x60);
    ps2mouse_wait(1);
    outb(0x60, status_byte);
    ps2mouse_write(0xF6);
    ps2mouse_read();
    ps2mouse_write(0xF4);
    ps2mouse_read();
    irq_install_handler(12, ps2mouse_handler);
}
