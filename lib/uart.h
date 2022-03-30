#ifndef UART_h
#define UART_h

#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

void UART_init(uint8_t ubrr, uint8_t idx);
int UART_writeChar1(char data, FILE *stream);
int UART_getChar1(FILE *stream);
int UART_writeChar0(char data, FILE *stream);
int UART_getChar0(FILE *stream);

#endif
