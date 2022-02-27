#include "nrf24l01.hpp"

inline void CSN_down(void) {
  PORTB &= ~(1 << CSN);
}

inline void CSN_up(void) {
  PORTB |= (1 << CSN);
}

inline void CE_down(void) {
  PORTB &= ~(1 << CE);
}

inline void CE_up(void) {
  PORTB |= (1 << CE);
}

void nRF_radio::begin(uint8_t SPI_inited) {
  if (!(SPI_inited)) _spi.init();
  DDRB |= (1 << CSN) | (1 << CE);
  this->start();
}

uint8_t nRF_radio::readReg(uint8_t addr) {
  CSN_down();
  _spi.writeReadByte((addr & 31) | 0x00);
  uint8_t response = _spi.writeReadByte(0xFF);
  CSN_up();
  return response;
}

uint8_t nRF_radio::writeBuf(uint8_t addr, uint8_t *buf, uint8_t size) {
  CSN_down();
  uint8_t response = _spi.writeReadByte(addr);
  while (size--) _spi.writeReadByte(*(buf++));
  CSN_up();
  return response;
}

uint8_t nRF_radio::writeReg(uint8_t addr, uint8_t val) {
  CSN_down();
  uint8_t response = _spi.writeReadByte(addr | 0x20);
  _spi.writeReadByte(val);
  CSN_up();
  return response;
}

uint8_t nRF_radio::writeRegBuf(uint8_t addr, uint8_t *buf, uint8_t size) {
  return this->writeBuf(addr | 0x20, buf, size);
}

void nRF_radio::writeAddr(char *addr) {
  CSN_down();
  _spi.writeReadByte(0x20 | 0x10);
  for (uint8_t i = 0; i < 5; i++) {
    _spi.writeReadByte(*addr);
    addr++;
  }
  CSN_up();
}

uint8_t nRF_radio::start(void) {
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
  this->writeAddr((char*)RADIO_ADDRESS);
  return 1;
}

uint8_t nRF_radio::sendMessage(char *buf, uint8_t size) {
  // чистим буффер
  CSN_down();
  _spi.writeReadByte(0xE1);
  CSN_up();
  // пошло поехало
  this->writeReg(0x07, 0b00110000);
  CSN_down();
  _spi.writeReadByte(0xA0);
  for (uint8_t i = 0; i < size; i++) _spi.writeReadByte(buf[i]);
  CSN_up();
  CE_up();
  _delay_ms(80);
  CE_down();
  return 1;
}
