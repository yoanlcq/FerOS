#include <serial.h>

// WISH: We'll probably want this to be way more flexible
Serial Serial_new(u16 port) {
    outb(port + 1, 0x00); // Disable all interrupts
    outb(port + 3, 0x80); // Enable DLAB (set baud rate divisor)
    outb(port + 0, 0x01); // Set divisor to 1 (lo byte) 115200 baud
    outb(port + 1, 0x00); //                  (hi byte)
    outb(port + 3, 0x03); // 8 bits, no parity, one stop bit
    outb(port + 2, 0xC7); // Enable FIFO, clear them, with 14-byte threshold
    outb(port + 4, 0x0B); // IRQs enabled, RTS/DSR set
    return (Serial) { .port = port };
}

bool Serial_can_recv(Serial s) {
    return !!(inb(s.port + 5) & 1);
}
 
u8 Serial_recv_u8(Serial s) {
    while (!Serial_can_recv(s))
        ;
    return inb(s.port);
}

bool Serial_can_send(Serial s) {
    return !!(inb(s.port + 5) & 0x20);
}
 
void Serial_send_u8(Serial s, u8 a) {
    while (!Serial_can_send(s))
        ;    
    outb(s.port, a);
}
