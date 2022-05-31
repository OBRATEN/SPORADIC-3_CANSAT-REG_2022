#ifndef DS18B20_H
#define DS18B20_H

/* Библиотека взаимодействия с термометром DS18B20
 * Автор: Гарагуля Артур, "SPORADIC", г. Курск
 * Интерфейс: OneWire
 * Зависимости: avr, onewire
 */

#include "config.h"
#include "onewire.hpp"

class DS18_term : public OW_interface {
public:
  DS18_term(void);
  void startMeasure(void);
  void readRawData(uint8_t *data);
  void readTemp(float *res);
};

#endif
