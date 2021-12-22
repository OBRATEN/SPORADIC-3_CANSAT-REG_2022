#ifndef BMP_H
#define BMP_H

#include <avr/io.h>
#include <util/delay.h>

#define BMP_T1REG 0x88
#define BMP_T2REG 0x8A
#define BMP_T3REG 0x8C
#define BMP_P1REG 0x8E
#define BMP_P2REG 0x90
#define BMP_P3REG 0x92
#define BMP_P4REG 0x94
#define BMP_P5REG 0x96
#define BMP_P6REG 0x98
#define BMP_P7REG 0x9A
#define BMP_P8REG 0x9C
#define BMP_P9REG 0x9E

#define BMP_CHIPIDREG 0xD0
#define BMP_CHIPIDVAL 0x58

#define BMP_TEMPDATAREG 0xFA
#define BMP_PRESDATAREG 0xF7
#define BMP_CFGREG 0xF5
#define BMP_CTRLREG 0xF4

#define BMP_OVSAMP_T2 0b10
#define BMP_OVSAMP_T16 0b101
#define BMP_FORCE 0b01

#define BMP_TSB250 0b11
#define BMP_FILTC4 0b10
#define BMP_NOSPI 0b00

#define BMP_CFG  (BMP_TSB250 << 5)   | (BMP_FILTC4 << 2)      | (BMP_NOSPI)
#define BMP_MEAS (BMP_OVSAMP_T2 << 5) | (BMP_OVSAMP_T16 << 2) | (BMP_FORCE)


uint8_t BMP_init(void);
void BMP_writeReg(uint8_t addr, uint8_t data);
uint8_t BMP_readReg1B(uint8_t addr);
uint16_t BMP_readReg2B(uint8_t addr);
uint16_t BMP_readReg2BLE(uint8_t reg);
int16_t BMP_readRegS2BLE(uint8_t reg);
uint32_t BMP_readReg3B(uint8_t addr);
void BMP_readCalData(void);
uint8_t BMP_begin(uint8_t addr);
int32_t BMP_readTemp(void);
void BMP_readFloatPress(int32_t *temp, float *press);
float BMP_readAlt(float *press);
uint8_t BMP_getData(int32_t *temp, float *press, float *alt);

#endif
