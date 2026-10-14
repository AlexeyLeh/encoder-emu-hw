/*
* Created: 02.04.2024
* Author: Lekhtin A.V.
*/

#ifndef UART__H_
#define UART__H_

#include "hw.h"

#ifndef F_CPU
#error ERROR: F_CPU not defined
#endif

enum BAUDRATE {
    BAUD_2400   = (uint16_t)2400,
    BAUD_4800   = 4800,
    BAUD_9600   = 9600,
    BAUD_19200  = 19200,
    BAUD_38400  = 38400,
    BAUD_57600  = 57600,
    BAUD_115200 = 115200
};

void uart_init(enum BAUDRATE baudrate);
void uart_wait_rx_end(void);
void uart_transmit(const uint8_t data);
char uart_receive(void);

void uart_print(const char *str);
void uart_println(const char *str);
void uart_printch(char ch);

void uart_print_decimal(int32_t value);

#endif // UART__H_
