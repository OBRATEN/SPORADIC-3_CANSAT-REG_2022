#include "nrf24l01.hpp"

#define AUTO_ACK      false
#define DATARATE      RF_DR_1MBPS
#define POWER         POWER_MAX
#define CHANNEL       0x5C
#define DYN_PAYLOAD   true
#define CONTINUOUS    false

#define RX_INTERRUPT  false
#define TX_INTERRUPT  false
#define RT_INTERRUPT  false

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

uint8_t nRF_radio::sendSPI(uint8_t addr, uint8_t *data, uint8_t count) {
  uint8_t status;
  CSN_down();
  status = _spi.writeReadByte(addr);
  for (uint8_t i = 0; i < count; i++) _spi.writeReadByte(data[i]);
  CSN_up();
  return status;
}

uint8_t nRF_radio::readReg(uint8_t addr, uint8_t *data, uint8_t count) {
  return this->sendSPI(R_REGISTER | addr, data, count);
}

uint8_t nRF_radio::writeReg(uint8_t addr, uint8_t *data, uint8_t count) {
  return this->sendSPI(W_REGISTER | addr, data, count);
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
  this->writeReg(CONFIG, &_curData, 1);
  _curData =
  (AUTO_ACK << ENAA_P5) |
  (AUTO_ACK << ENAA_P4) |
  (AUTO_ACK << ENAA_P3) |
  (AUTO_ACK << ENAA_P2) |
  (AUTO_ACK << ENAA_P1) |
  (AUTO_ACK << ENAA_P0);
  this->writeReg(EN_AA, &_curData,1 );
  _curData = 0xF0;
  this->writeReg(SETUP_RETR, &_curData, 1);
  _curData = 0;
  this->writeReg(EN_RXADDR, &_curData, 1);
  _curData = CHANNEL;
  this->writeReg(RF_CH, &_curData, 1);
  _curData =
  (CONTINUOUS << CONT_WAVE)                |
  ((DATARATE >> RF_DR_HIGH) << RF_DR_HIGH) |
  ((POWER >> RF_PWR) << RF_PWR);
  this->writeReg(RF_SETUP, &_curData, 1);
  _curData =
  (1 << RX_DR) |
  (1 << TX_DS) |
  (1 << MAX_RT);
  this->writeReg(STATUS, &_curData, 1);
  _curData =
  (DYN_PAYLOAD << DPL_P0) |
  (DYN_PAYLOAD << DPL_P1) |
  (DYN_PAYLOAD << DPL_P2) |
  (DYN_PAYLOAD << DPL_P3) |
  (DYN_PAYLOAD << DPL_P4) |
  (DYN_PAYLOAD << DPL_P5);
  this->writeReg(DYNPD, &_curData,1);
  _curData =
  (DYN_PAYLOAD << EN_DPL)  |
  (AUTO_ACK << EN_ACK_PAY) |
  (AUTO_ACK << EN_DYN_ACK);
  this->writeReg(FEATURE, &_curData, 1);
  _curData = 0;
  _curData = (1 << RX_DR) | (1 << TX_DS) | (1 << MAX_RT);
  this->writeReg(FLUSH_RX, 0, 0);
  this->writeReg(FLUSH_TX, 0, 0);
  this->writeReg(RX_ADDR_P0 + READ_PIPE, rx_address, 5);
  this->writeReg(TX_ADDR, tx_address, 5);
  this->writeReg(EN_RXADDR, &_curData, 1);
  _curData |= (1 << READ_PIPE);
  this->writeReg(EN_RXADDR, &_curData,1);
}

uint8_t nRF_radio::sendMessage(const char *data, uint8_t size) {
  uint8_t curCfg;
  this->readReg(CONFIG, &curCfg, 1);
  _curData = curCfg & ~(1 << PRIM_RX);
  this->writeReg(CONFIG, &_curData, 1);
  this->writeReg(FLUSH_RX, 0, 0);
  this->writeReg(FLUSH_TX, 0, 0);
  _curData = (1 << TX_DS);
  this->writeReg(STATUS, &_curData, 1);
  this->writeReg(CONFIG, &_curData, 1);
  _curData |= (1 << MASK_RX_DR);
  this->writeReg(CONFIG, &_curData, 1);
  CSN_down();
  _spi.writeReadByte(W_TX_PAYLOAD_NOACK);
  while (size--) _spi.writeReadByte(*(uint8_t*)data++);
  _spi.writeReadByte(0);
  CSN_up();
  CE_up();
  _delay_ms(100);
  CE_down();
  this->readReg(STATUS, &_curData, 1);
  while (!(_curData & (1 << TX_DS))) this->readReg(STATUS, &_curData, 1);
  return 1;
}

void nRF_radio::poshumim_blyat(void) {
  CE_up();
}