#include "nrf2401.h"

void nRF_init(void) {
  DDRB |= (1 << CSN) | (1 << CE);
  //nRF_CSN_HIGH();
  //nRF_CE_LOW();
  SPI_init();
}

uint8_t nRF_readReg(uint8_t addr) {
  nRF_CSN_LOW();
  SPI_writeReadByte((addr & 31) | 0x00);
  uint8_t response = SPI_writeReadByte(0xFF);
  nRF_CSN_HIGH();
  return response;
}

uint8_t nRF_writeBuf(uint8_t addr, uint8_t *buf, uint8_t count) {
nRF_CSN_LOW();
uint8_t response = SPI_writeReadByte(addr);
  while (count--) SPI_writeReadByte(*(buf++));
  nRF_CSN_HIGH();
  return response;
}

uint8_t nRF_writeReg(uint8_t addr, uint8_t val) {
  nRF_CSN_LOW();
  uint8_t response = SPI_writeReadByte(addr | 0x20);
  SPI_writeReadByte(val);
  nRF_CSN_HIGH();
  return response;
}

uint8_t nRF_writeRegBuf(uint8_t addr, uint8_t *buf, uint8_t count) {
  return nRF_writeBuf(addr | 0x20, buf, count);
}

void nRF_writeAddr(char *addr) {
  nRF_CSN_LOW();
  SPI_writeReadByte(0x20 | 0x10);
  for (uint8_t i = 0; i < 5; i++) {
    SPI_writeReadByte(*addr);
    addr++;
  }
  nRF_CSN_HIGH();
}
/*
uint8_t nRF_start(void) {
  uint8_t response = 0;
  uint8_t self_addr[] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
  uint8_t remote_addr[] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
  uint8_t chan = 5;
  nRF_CE_LOW();
  for (uint8_t cnt = 100; cnt;) {
    nRF_writeReg(CONFIG, (1 << EN_CRC) | (1 << CRCO) | (1 << PRIM_RX));
    if (nRF_readReg(CONFIG) == ((1 << EN_CRC) | (1 << CRCO) | (1 << PRIM_RX))) break;
    if (!cnt--) return 0;
    _delay_ms(1);
  }
  nRF_writeReg(EN_AA, (1 << ENAA_P1));
  nRF_writeReg(EN_RXADDR, (1 << ERX_P0) | (1 << ERX_P1));
  nRF_writeReg(SETUP_AW, R5B_AW_ADDR);
  nRF_writeReg(SETUP_RETR, RETR_DELAY_250MKS | RETR_UP_TO_2_RETR);
  nRF_writeReg(RF_CH, chan);
  nRF_writeReg(RF_SETUP, RF_1MBPS | RF_0DBM);
  nRF_writeRegBuf(RX_ADDR_P0, &remote_addr[0], 5);
  nRF_writeRegBuf(TX_ADDR, &remote_addr[0], 5);
  nRF_writeRegBuf(RX_ADDR_P1, &self_addr[0], 5);
  nRF_writeReg(RX_PW_P0, 0);
  nRF_writeReg(RX_PW_P1, 32);
  nRF_writeReg(DYNPD, (1 << DPL_P0) | (1 << DPL_P1));
  nRF_writeReg(FEATURE, 0x04);
  nRF_writeReg(CONFIG, (1 << EN_CRC) | (1 << CRCO) | (1 << PWR_UP) | (1 << PRIM_RX));
  response = (nRF_readReg(CONFIG) == (((1 << EN_CRC)) | (1 << CRCO) | (1 << PWR_UP) | (1 << PRIM_RX))) ? 1 : 0;
  return response;
}
*/

uint8_t nRF_start(void) {
  //uint8_t response = 0;
  //uint8_t self_addr[] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
  //uint8_t remote_addr[] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
  //uint8_t chan = 5;
  //for (uint8_t cnt = 100; cnt;) {
  //  nRF_writeReg(CONFIG, (1 << EN_CRC) | (1 << CRCO) | (1 << PRIM_RX));
  //  if (nRF_readReg(CONFIG) == ((1 << EN_CRC) | (1 << CRCO) | (1 << PRIM_RX))) break;
  //  if (!cnt--) return 0;
  //  _delay_ms(1);
  //}
  nRF_writeReg(0x00, 0b00001010);
  nRF_writeReg(0x01, 0b00111111);
  nRF_writeReg(0x02, 0b00000000);
  nRF_writeReg(0x03, 0b00000011);
  nRF_writeReg(0x04, 0b11111111);
  nRF_writeReg(0x05, 0x05);
  nRF_writeReg(0x06, 0b00000110);
  nRF_writeReg(0x07, 0b01110000);
  nRF_writeReg(0x11, 0b00010100);
  nRF_writeReg(0x12, 0b00010100);
  nRF_writeReg(0x13, 0b00010100);
  nRF_writeReg(0x14, 0b00010100);
  nRF_writeReg(0x15, 0b00010100);
  nRF_writeReg(0x16, 0b00010100);
  nRF_writeReg(0x17, 0b00000000);
  nRF_writeReg(0x1C, 0b00000000);
  nRF_writeReg(0x1D, 0b00000011);
  nRF_writeAddr("asdfg");
  return 1;
}

uint8_t nRF_sendMessage(uint8_t *buf, uint8_t size) {
  nRF_CSN_LOW();
  SPI_writeReadByte(0xE1);
  nRF_CSN_HIGH();
  nRF_writeReg(0x07, 0b00110000);
  nRF_CSN_LOW();
  SPI_writeReadByte(0xA0);
  for (uint8_t i = 0; i < size; i++) SPI_writeReadByte(buf[i]);
  nRF_CSN_HIGH();
  nRF_CE_HIGH();
  _delay_ms(80);
  nRF_CE_LOW();
  return 1;
}
