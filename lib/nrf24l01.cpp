#include "nrf24l01.hpp"

#define AUTO_ACK      false
#define DATARATE      RF_DR_1MBPS
#define POWER         POWER_MIN
#define CHANNEL       0x5C
#define DYN_PAYLOAD   true
#define CONTINUOUS    false

#define RX_INTERRUPT  false
#define TX_INTERRUPT  false
#define RT_INTERRUPT  false

#define PAYLOAD_SIZE  32

// CE
#define CE_DDR    DDRB
#define CE_PORT   PORTB
#define CE_PIN    PINB4
// CSN
#define CSN_DDR   DDRB
#define CSN_PORT  PORTB
#define CSN_PIN   PINB0
// IRQ
#define IRQ_DDR   DDRD
#define IRQ_PORT  PORTD
#define IRQ_PIN   DDD2

#define READ_PIPE 0
uint8_t rx_address[5] = { 0x78, 0x78, 0x78, 0x78, 0x78 };
uint8_t tx_address[5] = { 0x78, 0x78, 0x78, 0x78, 0x78 };

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

void nRF_radio::begin(void) {
  if (!(_spi.inited())) _spi.init();
  DDRB |= (1 << CSN);
  DDRB |= (1 << CE);
  CSN_up();
  CE_down();
  _delay_ms(100);
  _curData =
  (!(RX_INTERRUPT) << MASK_RX_DR)  |
  (!(TX_INTERRUPT) << MASK_TX_DS)  |
  (!(RT_INTERRUPT) << MASK_MAX_RT) |
  (1 << EN_CRC)                    |
  (1 << CRC0)                      |
  (1 << PWR_UP)                    |
  (1 << PRIM_RX);
  this->writeReg(CONFIG, &_curData, 0);
  _curData =
  (AUTO_ACK << ENAA_P5) |
  (AUTO_ACK << ENAA_P4) |
  (AUTO_ACK << ENAA_P3) |
  (AUTO_ACK << ENAA_P2) |
  (AUTO_ACK << ENAA_P1) |
  (AUTO_ACK << ENAA_P0);
  this->writeReg(EN_AA, &_curData, 0);
  _curData = 0xF0;
  this->writeReg(SETUP_RETR, &_curData, 0);
  _curData = 0;
  this->writeReg(EN_RXADDR, &_curData, 0);
  _curData = CHANNEL;
  this->writeReg(RF_CH, &_curData, 0);
  _curData =
  (CONTINUOUS << CONT_WAVE)                |
  ((DATARATE >> RF_DR_HIGH) << RF_DR_HIGH) |
  ((POWER >> RF_PWR) << RF_PWR);
  this->writeReg(RF_SETUP, &_curData, 0);
  _curData =
  (1 << RX_DR) |
  (1 << TX_DS) |
  (1 << MAX_RT);
  this->writeReg(STATUS, &_curData, 0);
  _curData =
  (DYN_PAYLOAD << DPL_P0) |
  (DYN_PAYLOAD << DPL_P1) |
  (DYN_PAYLOAD << DPL_P2) |
  (DYN_PAYLOAD << DPL_P3) |
  (DYN_PAYLOAD << DPL_P4) |
  (DYN_PAYLOAD << DPL_P5);
  this->writeReg(DYNPD, &_curData, 0);
  _dynPL = 1;
  _curData =
  (DYN_PAYLOAD << EN_DPL)  |
  (AUTO_ACK << EN_ACK_PAY) |
  (AUTO_ACK << EN_DYN_ACK);
  this->writeReg(FEATURE, &_curData, 0);
  _curData = 0;
  _curData = (1 << RX_DR) | (1 << TX_DS) | (1 << MAX_RT);
  this->writeReg(FLUSH_RX, NOP, 0);
  this->writeReg(FLUSH_TX, NOP, 0);
  this->writeReg(RX_ADDR_P0 + READ_PIPE, rx_address, 5);
  this->writeReg(TX_ADDR, tx_address, 5);
  this->writeReg(EN_RXADDR, &_curData, 0);
  _curData |= (1 << READ_PIPE);
  this->writeReg(EN_RXADDR, &_curData, 0);
}

uint8_t nRF_radio::readReg(uint8_t addr) {
  uint8_t status = 0;
  uint8_t result;
  CSN_down();
  status = _spi.writeReadByte(R_REGISTER | addr);
  result = _spi.writeReadByte(0xFF);
  CSN_up();
  if (status) return result;
  return 255;
}

uint8_t nRF_radio::readReg(uint8_t addr, uint8_t* buf, uint8_t len) {
  uint8_t status = 0;
  CSN_down();
  status = _spi.writeReadByte(R_REGISTER | addr);
  while (len--) *buf++ = _spi.writeReadByte(0xFF);
  CSN_up();
  return status;
}

uint8_t nRF_radio::writeReg(uint8_t addr, uint8_t value, uint8_t isCmd=0) {
  uint8_t status = 0;
  if (isCmd) {
    CSN_down();
    status = _spi.writeReadByte(W_REGISTER | addr);
    CSN_up();
  } else {
    CSN_down();
    status = _spi.writeReadByte(W_REGISTER | addr);
    _spi.writeReadByte(value);
    CSN_up();
  }
  if (!(status)) return 255;
  return status;
}

uint8_t nRF_radio::writeReg(uint8_t addr, const uint8_t* buf, uint8_t len) {
  uint8_t status = 0;
  CSN_down();
  status = _spi.writeReadByte(W_REGISTER | addr);
  while (len--) _spi.writeReadByte(*buf++);
  CSN_up();
  return status;
}

void nRF_radio::flushTx(void) {
  this->writeReg(FLUSH_TX, NOP, 1);
}

uint8_t nRF_radio::writePayload(const void* buf, uint8_t len, const uint8_t writeType) {
  uint8_t status;
  const uint8_t* current = reinterpret_cast<const uint8_t*>(buf);
  uint8_t blank = !(len) ? 1 : 0;
  uint8_t dataLen = len;
  dataLen = (dataLen < PAYLOAD_SIZE) ? len : 32;
  if (!_dynPL) blank = static_cast<uint8_t>(PAYLOAD_SIZE - dataLen);
  CSN_down();
  status = _spi.writeReadByte(writeType);
  while (dataLen--) _spi.writeReadByte(*current++);
  while (blank--) _spi.writeReadByte(0);
  CSN_up();
  return status;
}

uint8_t nRF_radio::getStatus(void) {
  return this->writeReg(NOP, NOP, 1);
}

uint8_t nRF_radio::send(const void* buf, uint8_t len, uint8_t ack) {
  this->writePayload(buf, len, ack ? W_TX_PAYLOAD_NOACK : W_TX_PAYLOAD);
  CE_up();
  _delay_ms(10);
  CE_down();
  return 1;
}

uint8_t nRF_radio::send(const void* buf, uint8_t len) {
  return this->send(buf, len, 0);
}

uint8_t nRF_radio::checkR(void) {
  return (this->readReg(CONFIG) != 255);
}

uint8_t nRF_radio::checkRW(void) {
  this->writeReg(CONFIG, 0x77, 1);
  uint8_t data = this->readReg(CONFIG);
  return (data == 0xFF);
}

uint8_t nRF_radio::checkW(void) {
  return (this->writeReg(CONFIG, 0x77, 0));
}