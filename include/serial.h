// Serial ports interface
// http://wiki.osdev.org/Serial_Ports

#pragma once

// XXX: Not 100% reliable ??
#define COM1 ((u16)0x3F8)
#define COM2 ((u16)0x2F8)
#define COM3 ((u16)0x3E8)
#define COM4 ((u16)0x2E8)

typedef struct {
    u16 port;
} Serial;

typedef enum {
    // With DLAB set to 1, this is the least significant byte of the 
    // divisor value for setting the baud rate.
    DataRegister = 0,
    // With DLAB set to 1, this is the most significant byte of the 
    // divisor value for setting the baud rate.
    InterruptEnableRegister = 1,
    InterruptIdentificationAndFIFOControlRegisters = 2,
    // The most significant bit is the DLAB .
    LineControlRegister = 3,
    ModemControlRegister = 4,
    LineStatusRegister = 5,
    ModemStatusRegister = 6,
    ScratchRegister = 7
} SerialPortOffset;


Serial Serial_new(u16 port);
bool Serial_can_recv(Serial s); // Can we receive without blocking ?
bool Serial_can_send(Serial s); // Can we send without blocking ?
u8 Serial_recv_u8(Serial s); // Blocking receive
void Serial_send_u8(Serial s, u8 a); // Blocking send
