#ifndef BMP280_H
#define BMP280_H

#include <avr/io.h>
#include "i2c.h"
#include "i2c.c"

#define BDEFADDR 0x77
#define BALTADDR 0x76

#define BDIGT1REG 0x88
#define BDIGT2REG 0x8A
#define BDIGT3REG 0x8C

#define BDIGP1REG 0x8E
#define BDIGP2REG 0x90
#define BDIGP3REG 0x92
#define BDIGP4REG 0x94
#define BDIGP5REG 0x96
#define BDIGP6REG 0x98
#define BDIGP7REG 0x9A
#define BDIGP8REG 0x9C
#define BDIGP9REG 0x9E

#define BCHIPIDREG 0xD0
#define BCHIPIDVAL 0x58
#define BREGVERS 0xD1
#define BREGSOFTSET 0xE0

#define BSTATUSREG 0xF3
#define BCONTREG 0xF4
#define BCONFREG 0xF5
#define BPRESDATAREG 0xF7
#define 

#endif
