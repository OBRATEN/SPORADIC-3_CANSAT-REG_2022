#ifndef DS18B20_H
#define DS18B20_H

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include "onewire.h"
#include "onewire.c"

void DS_init(void);
void DS_readRaw(uint8_t *data);
void DS_readTemp(float *temp);

#endif
