#include "i2c.hpp"

void I2C_interface::init(uint32_t f_cpu, uint32_t f_scl) {
	_f_cpu = f_cpu;
	_f_scl = f_scl;
  TWBR = (((f_cpu) / (f_scl) - 16) / 2);
  TWSR = 0;
  TWCR |= (1 << TWEN);
  this->_inited = 1;
}

uint8_t I2C_interface::inited(void) {
	return this->_inited;
}

uint8_t I2C_interface::start(void) {
  TWCR = ((1 << TWINT) | (1 << TWEN) | (1 << TWSTA));
  while(!(TWCR & (1 << TWINT)));
  this->_started = 1;
  return 1;
}

uint8_t I2C_interface::stop(void) {
  TWCR = ((1 << TWINT) | (1 << TWEN) | (1 << TWSTO));
  this->_started = 0;
  return 1;
}

uint8_t I2C_interface::close(void) {
  TWCR &= ~(1 << TWEN);
  this->_inited = 0;
  return 1;
}

uint8_t I2C_interface::writeByte(uint8_t data) {
	uint8_t response = 0;
  TWDR = data;
  //if (!this->start()) return response;
  TWCR = (1 << TWINT) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT)));
  if ((TWSR & 0xF8) == 0x18 || (TWSR & 0xF8) == 0x28 || (TWSR & 0xF8) == 0x40) {
    response = 1;
  }
  return response;
}

uint8_t I2C_interface::readByte(uint8_t *data, uint8_t opt) {
  if (opt) TWCR |= (1 << TWEA);
  else TWCR &= ~(1 << TWEA);
  uint8_t response = 0;
  //if (!this->start()) return response;
  TWCR = ((1<< TWINT) | (1<<TWEN));
  while (!(TWCR & (1 << TWINT)));
  if (((TWSR & 0xF8) == 0x58) || ((TWSR & 0xF8) == 0x50)) {
    *data = TWDR;
    response = 1;
  }
  return response;
}

void I2C_interface::writeReg(uint8_t address, uint8_t reg, uint8_t val) {
  this->start();
  this->writeByte((address << 1));
  this->writeByte(reg);
  this->writeByte(val);
  _delay_ms(10);
  this->stop();
  _delay_ms(6);
}

void I2C_interface::readReg(uint8_t daddr, uint8_t raddr, uint8_t num, uint8_t *buffer) {
  this->start();
  this->writeByte((daddr << 1));
  this->writeByte(raddr);
  this->stop();
  this->start();
  this->writeByte((daddr << 1) + 1);
  for (uint8_t i = 0; i < num; i++) {
    this->readByte(buffer, 1);
    buffer++;
  }
  this->stop();
}
