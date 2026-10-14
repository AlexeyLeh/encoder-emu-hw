/*
 * Created: 02.04.2024
 * Author: Lekhtin A.V.
 */

#include <stdint.h>
#include <string.h>
#include "uart.h"

//---------------------------------------------------------
void uart_init(enum BAUDRATE baudrate)
{
    disable_interrupt();
    uint16_t prescaller = 0;

    switch (baudrate)
    {
        case BAUD_2400:
        case BAUD_4800:
        case BAUD_9600:
        case BAUD_19200:
        case BAUD_38400:
        case BAUD_57600:
        case BAUD_115200:
            prescaller = (((F_CPU / (baudrate * 16UL))) - 1);
            break;
        default:
            break;
    }

    if (prescaller == 0)
    {
        enable_interrupt();
        return;
    }

    UBRR0H = (uint8_t)(prescaller >> 8);
    UBRR0L = (uint8_t)(prescaller & 0x00FF);


    UCSR0A_UDRE0 = 1;

    // Enable receiver and transmitter
    UCSR0B_RXEN0 = 1;
    UCSR0B_TXEN0 = 1;

    // 8-bit data format
    UCSR0C_UCSZ01 = 1;
    UCSR0C_UCSZ00 = 1;

    delay_100ms();
    enable_interrupt();
}

//---------------------------------------------------------
void uart_wait_rx_end()
{
    while (UCSR0A_RXC0);        // Wait for data to be received
}

//---------------------------------------------------------
char uart_receive()
{
    while (!(UCSR0A_RXC0));     // Wait for data to be received
    return UDR0;                // Get and return received data from buffer
}

//---------------------------------------------------------
void uart_transmit(const uint8_t data)
{
    while (!(UCSR0A_UDRE0));    // Wait for empty transmit buffer
    UDR0 = data;                // Put data into the buffer, sends the data
}

//---------------------------------------------------------
void uart_print(const char *str)
{
    for (uint8_t i = 0; str[i] != '\0'; ++i)
    {
        uart_transmit(str[i]);
    }
}

//---------------------------------------------------------
void uart_println(const char *str)
{
    for (uint8_t i = 0; str[i] != '\0'; ++i)
    {
        uart_transmit(str[i]);
    }
    uart_transmit('\n');
}

//---------------------------------------------------------
void uart_printch(char ch)
{
    uart_transmit(ch);
}

//---------------------------------------------------------
static char __istr[64];
static int32_t rem = 0;
static int32_t value;

//---------------------------------------------------------
void uart_print_decimal(int32_t val)
{
    uint8_t idx = 0;
    char *p = NULL;
    char minus = '\0';

    value = val;
    if (value < 0)
    {
        minus = '-';
        value = -value;
    }

    memset(__istr, 0, sizeof(__istr));
    idx = sizeof(__istr)-2;

    while(1)
    {
        rem = value % 10;
        value = value / 10;
        __istr[idx--] = rem + '0';
        if (value == 0) break;
    }
    if (minus) __istr[idx] = minus;
    p = __istr;
    while(*p == 0)
    {
      p++;
    }
    uart_print(p);
}