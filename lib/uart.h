#ifndef UART_h
#define UART_h

#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

void UART_init(uint8_t ubrr);
int UART_writeChar(char data, FILE *stream);
int UART_getChar(FILE *stream);

#endif
