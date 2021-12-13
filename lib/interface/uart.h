#ifndef UART_h
#define UART_h

#include <avr/io.h>

class UART_self {
public:
  UART_self(uint8_t ubrr); // init
  void writeChar(char data); // write char to uart
  void writeString(char *data); // write char array ro uart
  char getChar(void); // get char from uart
  void getString(char *buff); // get char array from uart
  bool getCapacity(bool uart); // get capacity of uart

private:
  bool m_capacity0; // capacity of UART0
  bool m_capacity1; // capacity of UART1
};

#endif
