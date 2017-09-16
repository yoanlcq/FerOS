// Serial ports interface
// http://wiki.osdev.org/Serial_Ports

#pragma once

#include <stdint.h>
#include <stdbool.h>

// XXX: Not 100% reliable ??
static const uint16_t COM1 = 0x3F8;
static const uint16_t COM2 = 0x2F8;
static const uint16_t COM3 = 0x3E8;
static const uint16_t COM4 = 0x2E8;

typedef struct {
    uint16_t port;
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


Serial Serial_new(uint16_t port);
bool Serial_can_recv(Serial s); // Can we receive without blocking ?
bool Serial_can_send(Serial s); // Can we send without blocking ?
uint8_t Serial_recv_u8(Serial s); // Blocking receive
void Serial_send_u8(Serial s, uint8_t a); // Blocking send
