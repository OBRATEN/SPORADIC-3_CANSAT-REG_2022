#include "i2c.h"

void I2C_init(uint32_t f_cpu, uint32_t f_scl) {
  TWBR = (((f_cpu) / (f_scl) - 16) / 2);
  TWSR = 0;
  TWCR = (1 << TWEN);
}

void I2C_close(void) {
  TWCR &= ~(1 << TWEN);
}

void I2C_start(void) {
  TWCR = ((1 << TWINT) | (1 << TWEN) | (1 << TWSTA));
  while (!(TWCR & (1 << TWINT)));
}

void I2C_stop(void) {
  TWCR = ((1 << TWINT) | (1 << TWEN) | (1 << TWSTO));
}

uint8_t I2C_writeByte(uint8_t data) {
  TWDR = data;
  TWCR = ((1 << TWEN) | (1 << TWINT));
  while (!(TWCR & (1 << TWINT)));
  if ((TWSR & 0xF8) == 0x18 || (TWSR & 0xF8) == 0x28 || (TWSR & 0xF8) == 0x40) return 1;
  return 0;
}

uint8_t I2C_readByte(uint8_t *data, uint8_t opt) {
  if (opt) TWCR |= (1 << TWEA);
  else TWCR &= ~(1 << TWEA);
  TWCR |= (1 << TWINT);
  while (!(TWCR & (1 << TWINT)));
  if ((TWSR & 0xF8) == 0x58 || (TWSR & 0xF8) == 0x50) {
    *data = TWDR;
    return 1;
  } return 0;
}

void I2C_writeReg(uint8_t address, uint8_t reg, uint8_t val) {
  I2C_start();
  I2C_writeByte((address << 1));
  I2C_writeByte(reg);
  I2C_writeByte(val);
  _delay_ms(10);
  I2C_stop();
  _delay_ms(6);
}

void I2C_readBytes(uint8_t daddr, uint8_t raddr, uint8_t num, uint8_t *buffer) {
  I2C_start();
  I2C_writeByte((daddr << 1));
  I2C_writeByte(raddr);
  I2C_start();
  I2C_writeByte((daddr << 1) + 1);
  for (uint8_t i = 0; i < num; i++) {
    I2C_readByte(buffer, 1);
    buffer++;
  }
}
