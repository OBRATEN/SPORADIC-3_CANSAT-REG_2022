#ifndef ONEWIRE_H
#define ONEWIRE_H

#define F_CPU 8000000UL

/* Библиотека управления интерфейсом OneWire
 * Автор: Гарагуля Артур, "SPORADIC", г. Курск
 * Зависимости: avr
 */

#include <avr/io.h>
#include <util/delay.h>

#define OWPORT PORTD
#define OWDIR  DDRD
#define OWPIN  PIND
#define OWPINN PD7

class OW_interface {
public:
  void init(void);
  uint8_t reset(void);
  void writeBit(uint8_t bit);
  void writeByte(uint8_t data);
  uint8_t readBit(void);
  uint8_t readByte(void);
private:
  inline uint8_t level(void);
  inline void setLow(void);
  inline void setHigh(void);
  inline void input(void);
  inline void output(void);
};

#endif
