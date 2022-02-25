#ifndef DS18B20_H
#define DS18B20_H

#include <avr/io.h>

#include "onewire.hpp"

class DS18_term : public OW_interface {
public:
  DS18_term(void);
  void readRawData(uint8_t *data);
  void readTemp(float *res);
};

#endif
