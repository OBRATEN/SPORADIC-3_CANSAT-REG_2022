#ifndef UART_H
#define UART_H

#include "config.h"
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

void UART_init(uint16_t ubrr, uint8_t idx);
int UART_writeChar1(char data, FILE *stream);
int UART_getChar1(FILE *stream);
int UART_writeChar0(char data);
int UART_getChar0(FILE *stream);

#endif // UART_H
