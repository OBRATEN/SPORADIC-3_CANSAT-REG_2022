#include "nrf2401.h"

static inline void CSN_down(void) {
  PORTB &= ~(1 << CSN);
}

static inline void CSN_up(void) {
  PORTB |= (1 << CSN);
}

static inline void CE_down(void) {
  PORTB &= ~(1 << CE);
}

static inline void CE_up(void) {
  PORTB |= (1 << CE);
}

void nRF_self::begin(void) {
  DDRB |= (1 << CSN) | (1 << CE);
  this->start();
}

uint8_t nRF_self::readReg(uint8_t addr) {
  CSN_down();
  SPI_writeReadByte((addr & 31) | 0x00);
  uint8_t response = SPI_writeReadByte(0xFF);
  CSN_up();
  return response;
}

uint8_t nRF_self::writeBuf(uint8_t addr, uint8_t *buf, uint8_t size) {
  CSN_down();
  uint8_t response = SPI_writeReadByte(addr);
  while (size--) SPI_writeReadByte(*(buf++));
  CSN_up();
  return response;
}

uint8_t nRF_self::writeReg(uint8_t addr, uint8_t val) {
  CSN_down();
  uint8_t response = SPI_writeReadByte(addr | 0x20);
  SPI_writeReadByte(val);
  CSN_up();
  return response;
}

uint8_t nRF_self::writeRegBuf(uint8_t addr, uint8_t *buf, uint8_t size) {
  return this->writeBuf(addr | 0x20, buf, size);
}

void nRF_self::writeAddr(char *addr) {
  CSN_down();
  SPI_writeReadByte(0x20 | 0x10);
  for (uint8_t i = 0; i < 5; i++) {
    SPI_writeReadByte(*addr);
    addr++;
  }
  CSN_up();
}


uint8_t nRF_self::start(void) {
  this->writeReg(0x00, 0b00001010);
  this->writeReg(0x01, 0b00111111);
  this->writeReg(0x02, 0b00000000);
  this->writeReg(0x03, 0b00000011);
  this->writeReg(0x04, 0b11111111);
  this->writeReg(0x05, 0x05);
  this->writeReg(0x06, 0b00000110);
  this->writeReg(0x07, 0b01110000);
  this->writeReg(0x11, 0b00010100);
  this->writeReg(0x12, 0b00010100);
  this->writeReg(0x13, 0b00010100);
  this->writeReg(0x14, 0b00010100);
  this->writeReg(0x15, 0b00010100);
  this->writeReg(0x16, 0b00010100);
  this->writeReg(0x17, 0b00000000);
  this->writeReg(0x1C, 0b00000000);
  this->writeReg(0x1D, 0b00000011);
  this->writeAddr((char*)"asdfg");
  return 1;
}

uint8_t nRF_self::sendMessage(uint8_t *buf, uint8_t size) {
  CSN_down();
  SPI_writeReadByte(0xE1);
  CSN_up();
  this->writeReg(0x07, 0b00110000);
  CSN_down();
  SPI_writeReadByte(0xA0);
  for (uint8_t i = 0; i < size; i++) SPI_writeReadByte(buf[i]);
  CSN_up();
  CE_up();
  _delay_ms(80);
  CE_down();
  return 1;
}

