#ifndef NRF_H
#define NRF_H

#include "spi.h"
#include "spi.c"

#define CSN PINB0
#define CE PINB4

#define CONFIG 0x00
#define EN_CRC 3
#define CRCO 2
#define PRIM_RX 0

#define EN_AA 0x01
#define ENAA_P1 1
#define EN_RXADDR 0x02
#define ERX_P0 0
#define ERX_P1 1
#define SETUP_AW 0x03
#define R5B_AW_ADDR (3 << SETUP_AW)
#define SETUP_RETR 0x04
#define ARD 4
#define ARC 3
#define RETR_DELAY_250MKS (0 << ARD)
#define RETR_UP_TO_2_RETR (2 << ARC)
#define RF_CH 0x05
#define RF_SETUP 0x06
#define RF_DR_HIGH 3
#define RF_PWR 1
#define RF_1MBPS (0 << RF_DR_HIGH)
#define RF_0DBM (3 << RF_PWR)
#define RX_ADDR_P0 0x0A
#define RX_ADDR_P1 0x0B
#define TX_ADDR 0x10
#define RX_PW_P0 0x11
#define RX_PW_P1 0x12
#define DYNPD 0x1C
#define DPL_P0 0
#define DPL_P1 1
#define FEATURE 0x1D
#define PWR_UP 1
#define TX_FULL 0
#define W_TX_PAYLOAD 0xA0

class nRF_self {
public:
  void begin(void);
  void writeAddr(char *addr);
  uint8_t writeBuf(uint8_t addr, uint8_t *buf, uint8_t count);
  uint8_t writeReg(uint8_t addr, uint8_t val);
  uint8_t writeRegBuf(uint8_t addr, uint8_t *data, uint8_t count);
  uint8_t readReg(uint8_t reg);
  uint8_t start(void);
  uint8_t sendMessage(uint8_t *buf, uint8_t size);
};

#endif
