#ifndef ONEWIRE_H
#define ONEWIRE_H

#include <avr/io.h>
#include <util/delay.h>

#define OWPORT PORTC
#define OWDIR  DDRC
#define OWPIN  PINC
#define OWPINN PC1

void OW_init(void);
uint8_t OW_reset(void);
static inline uint8_t OW_level(void);
static inline void OW_setLow(void);
static inline void OW_setHigh(void);
static inline void OW_input(void);
static inline void OW_output(void);
void OW_writeBit(uint8_t bit);
void OW_writeByte(uint8_t data);
uint8_t OW_readBit(void);
uint8_t OW_readByte(void);

#endif
