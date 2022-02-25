#include "uart.h"

void UART_init(uint8_t ubrr) {
	UBRR0L = ubrr;
	UBRR0H = 0x00;
	UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);
	UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
	//UCSR0C = (1 << USBS0) | (3 << UCSZ00);
	//UCSR0C |= (1 << 1);
	//UCSR0C |= (1 << 2);
	//UCSR0B &= ~(1 << 2);
	//UCSR0C |= (1 << 3);
}

int UART_writeChar(char data, FILE *stream) {
	while (!(UCSR0A & (1 << UDRE0)))
	UDR0 = data;
	return 0;
}

int UART_getChar(FILE *stream) {
	while (!(UCSR0A & (1 << RXC0)));
	return UDR0;
}
