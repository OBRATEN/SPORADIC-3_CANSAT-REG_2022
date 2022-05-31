#ifndef ONEWIRE_H
#define ONEWIRE_H

/* Библиотека управления интерфейсом OneWire
 * Автор: Гарагуля Артур, "SPORADIC", г. Курск
 * Зависимости: avr
 */

#include "config.h"
#include "pinout.h"
#include <util/delay.h>

class OW_interface {
public:
  void init(void);
  uint8_t inited(void) { return _inited; }
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
private:
  uint8_t _inited = 0;
};

#endif
