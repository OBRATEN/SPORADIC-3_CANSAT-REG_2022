#include "nrf24l01.hpp"

NRF::NRF() {
  _spi = SPI_interface();
}

void NRF::writeReg(uint8_t reg, uint8_t data ) {
  SS_high();
  _spi.transfer(reg | NRF24L01__WRITE_MASK);
  _spi.transfer(data);
  SS_low();
}

void NRF::writeReg(uint8_t reg, const uint8_t* buf, uint8_t len) {
  SS_high();
  _spi.transfer(reg | NRF24L01__WRITE_MASK);
  while (len--) _spi.transfer(*buf++);
  SS_low();
}

uint8_t NRF::readReg(uint8_t reg) {    
  SS_high();
  _spi.transfer(reg&NRF24L01__READ_MASK);
  uint8_t result= _spi.transfer(0xFF);
  SS_low();
  return result;
}

void NRF::flush_tx() {
  SS_high();
  _spi.transfer(0xE1);
  SS_low();
}

void NRF::send(const void* buf) {
  PORTB &= ~(1<<CE);
  const uint8_t* current = reinterpret_cast<const uint8_t*>(buf);
  flush_tx(); // clear previous transmission
  writeReg(0x07, 0b00110000); // clear previous transmission
  PORTB &= ~(1<<CSN);
  _spi.transfer(0xA0);
  for(uint8_t i = 0; i < 32; i++) _spi.transfer(*current++);
  PORTB |= (1<<CSN);
  PORTB |= (1<<CE);
}

void NRF::set_address(uint64_t value) {
  writeReg(0x0A, reinterpret_cast<uint8_t*>(&value), 5);
  writeReg(0x10, reinterpret_cast<uint8_t*>(&value), 5);
}

void NRF::begin(void) {
  writeReg(0x00, 0b00001110);
  writeReg(0x01, 0b00111111);
  writeReg(0x02, 0b00000000);
  writeReg(0x03, 0b00000011); // 5 byte adres wight
  writeReg(0x04, 0b11111111); // wait 4000us and repeat 15 times, if data will be not given
  writeReg(0x05, 0x5C);       // frequency set
  writeReg(0x06, 0b00000010); // speed and power rate set
  writeReg(0x07, 0b01110000); // Clear status register
  writeReg(0x11, 0b00010100); // PACKET_SIZE pipe 0
  writeReg(0x12, 0b00010100); // PACKET_SIZE pipe 1
  writeReg(0x13, 0b00010100); // PACKET_SIZE pipe 2
  writeReg(0x14, 0b00010100); // PACKET_SIZE pipe 3
  writeReg(0x15, 0b00010100); // PACKET_SIZE pipe 4
  writeReg(0x16, 0b00010100); // PACKET_SIZE pipe 5
  writeReg(0x17, 0b00000000); // Clear status FIFO register
  writeReg(0x1C, 0b00000000);
  writeReg(0x1D, 0b00000011);
  set_address(0x7878787878LL);
}