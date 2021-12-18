#include "uart.h"

void UART_init(uint8_t ubrr) {
	UBRR0L = (unsigned char)ubrr;
	UBRR0H = (unsigned char)(ubrr >> 8);
	UCSR0A = 0x00;
	UCSR0B = (1<<RXEN0) | (1<<TXEN0) | (1 << RXCIE0);
	UCSR0C |= (1 << 1);
	UCSR0C |= (1 << 2);
	UCSR0B &= ~(1 << 2);
	UCSR0C |= (1 << 3);
}

int UART_writeChar(char data, FILE *stream) {
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = data;
	return 0;
}

/*
void UART_writeArray(char *array) {
	uint8_t idx = 0;
	while (array[idx] != 0x00) {
		UART_writeChar(array[idx]);
		idx++;
	}
}
*/

char UART_getChar(void) {
	while (!(UCSR0A & (1 << RXC0)));
	return UDR0;
}

void UART_getString(char *array, char del) {
  uint8_t idx = 0;
  char c;
  do {
    c = UART_getChar();
    array[idx++] = c;
	} while (c != del);
	array[idx + 1] = 0;
}

