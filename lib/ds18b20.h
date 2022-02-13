#ifndef DS18B20_H
#define DS18B20_H

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include "onewire.c"

void DS_init(void);
void DS_readRaw(uint8_t *data);
void DS_readTemp(float *res);

class DS18_term {
public:
  DS18_term(void);
  void readRawData(uint8_t *data);
  void readTemp(float *res);
};

#endif
