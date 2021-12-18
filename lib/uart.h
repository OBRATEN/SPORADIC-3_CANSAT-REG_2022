#ifndef UART_h
#define UART_h

#include <avr/io.h>
#include <util/delay.h>

void UART_init(uint8_t ubrr);
int UART_writeChar(char data, FILE *stream);
void UART_writeString(char *data);
char UART_getChar(void);
void UART_getString(char *buff, char del);

#endif
