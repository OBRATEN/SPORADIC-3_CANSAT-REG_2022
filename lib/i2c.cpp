#include "i2c.hpp"

void I2C_interface::init(uint32_t f_cpu, uint32_t f_scl) {
  _f_cpu = f_cpu;
  _f_scl = f_scl;
  //TWBR = (((f_cpu) / (f_scl) - 16) / 2);
  TWBR = 0x20;
  TWSR = 0;
  TWCR |= (1 << TWEN);
  _inited = 1;
}

uint8_t I2C_interface::inited(void) {
  return _inited;
}

uint8_t I2C_interface::start(void) {
  TWCR = ((1 << TWINT) | (1 << TWEN) | (1 << TWSTA));
  while(!(TWCR & (1 << TWINT)));
  _started = 1;
  return 1;
}

uint8_t I2C_interface::stop(void) {
  TWCR = ((1 << TWINT) | (1 << TWEN) | (1 << TWSTO));
  this->_started = 0;
  return 1;
}

uint8_t I2C_interface::close(void) {
  TWCR &= ~(1 << TWEN);
  _inited = 0;
  return 1;
}

uint8_t I2C_interface::writeByte(uint8_t data) {
  TWDR = data;
  TWCR = (1 << TWINT) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT)));
  if ((TWSR & 0xF8) == 0x18 || (TWSR & 0xF8) == 0x28 || (TWSR & 0xF8) == 0x40) return 1;
  return 0;
}

uint8_t I2C_interface::readByte(uint8_t *data, uint8_t opt) {
  if (opt) TWCR |= (1 << TWEA);
  else TWCR &= ~(1 << TWEA);
  TWCR = ((1<< TWINT) | (1<<TWEN));
  while (!(TWCR & (1 << TWINT)));
  if (((TWSR & 0xF8) == 0x58) || ((TWSR & 0xF8) == 0x50)) {
    *data = TWDR;
    return 1;
  } return 0;
}

uint8_t I2C_interface::read(uint8_t ack) {
  TWCR = (1<<TWINT)|(1<<TWEN)|(ack<<TWEA);
  while(!(TWCR & (1<<TWINT)));
  return TWDR;
}

uint64_t I2C_interface::readReg(uint8_t devAddr, uint8_t addr, uint8_t byte_count) {
  uint64_t data = 0;
  start();
  writeByte((devAddr << 1) | 0);
  writeByte(addr);
  stop();
  start();
  writeByte((devAddr << 1) | 1);
  for (uint8_t i = 0; i < byte_count - 1; i++) data = data | (read(1) << 8 * i);
  data = data | (read(0) << 8 * (byte_count - 1));
  stop();
  return data;
}

void I2C_interface::readReg(uint8_t daddr, uint8_t raddr, uint8_t num, uint8_t *buffer) {
  start();
  writeByte((daddr << 1));
  writeByte(raddr);
  start();
  writeByte((daddr << 1) + 1);
  for (uint8_t i = 0; i < num; i++) {
    readByte(buffer, 1);
    buffer++;
  }
}

void I2C_interface::writeReg(uint8_t address, uint8_t reg, uint8_t val) {
  start();
  writeByte((address << 1));
  writeByte(reg);
  writeByte(val);
  stop();
}
