#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"

UART_self::UART_self(uint8_t ubrr) {
  UBRR0H = (unsigned char)(ubrr >> 8);
  UBRR0L = (unsigned char)ubrr;
  //UCSR0A = (1 << U2X0);
  UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);
  UCSR0C = (1 << USBS0) | (1 << UCSZ00) | (1 << UCSZ01);
  m_capacity1 = true;
}

void UART_self::writeChar(char data) {
  while (!(UCSR0A & (1 << UDRE0)));
  UDR0 = data;
  PORTG |= _BV(PING3);
  _delay_ms(100);
  PORTG &= ~_BV(PING3);
  _delay_ms(100);
}

void UART_self::writeString(char *data) {
	uint8_t i = 0;
  while (data[i++] != ';') writeChar(data[i]);
}

char UART_self::getChar() {
  while (!(UCSR1A & (1<<RXC1)));
  return UDR1;
}

void UART_self::getString(char *buff) {
  uint8_t i = 0;
  char c = '0';
  do {
    c = getChar();
    buff[i++] = c;
  } while (c != ';');
  buff[i] = 0;
}

bool UART_self::getCapacity(bool uart) {
	return (uart) ? m_capacity1 : m_capacity0;
}
