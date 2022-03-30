#include "uart.h"

inline void blink(void) {
  cli();
  PORTG |= _BV(PING3);
  _delay_ms(100);
  PORTG &= ~_BV(PING3);
  _delay_ms(100);
  sei();
}

void UART_init(uint8_t ubrr, uint8_t idx) {
	if (!idx) {
		UBRR0L = ubrr;
		UBRR0H = 0x00;
		UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);
		UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
	} else {
		UBRR1H = (unsigned char)(ubrr>>8);
		UBRR1L = (unsigned char)ubrr;
		UCSR1B = (1<<RXEN1)|(1<<TXEN1)|(1<<RXCIE1);
		UCSR1C = (1<<USBS1)|(1<<UCSZ10)|(1<<UCSZ11);
	}
	//UCSR0C = (1 << USBS0) | (3 << UCSZ00);
	//UCSR0C |= (1 << 1);
	//UCSR0C |= (1 << 2);
	//UCSR0B &= ~(1 << 2);
	//UCSR0C |= (1 << 3);
}

int UART_writeChar0(char data, FILE *stream) {
	while (!(UCSR0A & (1 << UDRE0)))
	UDR0 = data;
	return 0;
}

int UART_getChar0(FILE *stream) {
	while (!(UCSR0A & (1 << RXC0)));
	return UDR0;
}

int UART_writeChar1(char data, FILE *stream) {
	while (!(UCSR1A & (1 << UDRE1)))
	UDR1 = data;
	blink();
	return 0;
}

int UART_getChar1(FILE *stream) {
	while (!(UCSR1A & (1 << RXC1)));
	return UDR1;
}
