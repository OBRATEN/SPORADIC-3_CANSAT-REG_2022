#ifndef NRF_H
#define NRF_H

#define F_CPU 8000000UL

#include "spi.hpp"
#include <util/delay.h>

/* Библиотека управления радиомодулем nRF24L01+
 * Автор: Гарагуля Артур, "SPORADIC", г. Курск
 * Интерфейс: SPI + 1 digital pin
 * Зависимости: avr, spi
 */

#define CSN PINB0
#define CE PINB4

// Команды SPI
#define REGISTER_MASK      0b00011111
#define R_REGISTER         0b00000000
#define W_REGISTER         0b00100000
#define R_RX_PAYLOAD       0b01100001
#define W_TX_PAYLOAD       0b10100000
#define FLUSH_TX           0b11100001
#define FLUSH_RX           0b11100010
#define REUSE_TX_PL        0b11100011
#define ACTIVATE           0b01010000 
#define R_RX_PL_WID        0b01100000
#define ACK_PAYLOAD_MASK   0b00000111
#define W_ACK_PAYLOAD      0b10101000
#define W_TX_PAYLOAD_NOACK 0b10110000
#define NOP                0b11111111

// Регистры конфигурации
#define CONFIG      0x00
#define MASK_RX_DR  6
#define MASK_TX_DS  5
#define MASK_MAX_RT 4
#define EN_CRC      3
#define CRC0        2
#define PWR_UP      1
#define PRIM_RX     0

// ACK
#define EN_AA   0x01
#define ENAA_P5 5
#define ENAA_P4 4
#define ENAA_P3 3
#define ENAA_P2 2
#define ENAA_P1 1
#define ENAA_P0 0

// Конфигурация адресов для принятия
#define EN_RXADDR 0x02
#define ERX_P5    5
#define ERX_P4    4
#define ERX_P3    3
#define ERX_P2    2
#define ERX_P1    1
#define ERX_P0    0

// Настройка длины адреса
#define SETUP_AW 0x03
#define AW       0 /* 1:0 */

// Настройка ретрансляции
#define SETUP_RETR 0x04
#define ARD        4 /* 7:4 */
#define ARC        0 /* 3:0 */

// Регистр частоты
#define RF_CH 0x05

// Регистры сетапа
#define RF_SETUP      0x06
#define CONT_WAVE     7
#define RF_DR_LOW     5
#define PLL_LOCK      4
#define RF_DR_HIGH    3
#define RF_DR_250KBPS 0b00100000
#define RF_DR_1MBPS   0b00000000
#define RF_DR_2MBPS   0b00001000
#define RF_PWR        1
#define POWER_MIN     0b00000000
#define POWER_LOW     0b00000010
#define POWER_HIGH    0b00000100
#define POWER_MAX     0b00000110

// Регистр статуса
#define STATUS  0x07
#define RX_DR   6
#define TX_DS   5
#define MAX_RT  4
#define RX_P_NO 1 /* 3:1 */
#define TX_FULL 0

// Регистры TOR
#define OBSERVE_TX 0x08
#define PLOS_CNT   4 /* 7:4 */
#define ARC_CNT    0 /* 3:0 */

#define RPD        0x09
#define RX_ADDR_P0 0x0A
#define RX_ADDR_P1 0x0B
#define RX_ADDR_P2 0x0C
#define RX_ADDR_P3 0x0D
#define RX_ADDR_P4 0x0E
#define RX_ADDR_P5 0x0F
#define TX_ADDR    0x10
#define RX_PW_P0   0x11
#define RX_PW_P1   0x12
#define RX_PW_P2   0x13
#define RX_PW_P3   0x14
#define RX_PW_P4   0x15
#define RX_PW_P5   0x16

// FIFO
#define FIFO_STATUS 0x17
#define TX_REUSE    6
#define FIFO_FULL   5
#define TX_EMPTY    4
#define RX_FULL     1
#define RX_EMPTY    0

// Динамический размер полезной информации
#define DYNPD  0x1C
#define DPL_P5 5
#define DPL_P4 4
#define DPL_P3 3
#define DPL_P2 2
#define DPL_P1 1
#define DPL_P0 0

#define FEATURE    0x1D
#define EN_DPL     2
#define EN_ACK_PAY 1
#define EN_DYN_ACK 0

class nRF_radio {
public:
  void begin(void);
  uint8_t readReg(uint8_t addr);
  uint8_t readReg(uint8_t addr, uint8_t* buf, uint8_t len);
  uint8_t writeReg(uint8_t addr, const uint8_t* buf, uint8_t len);
  uint8_t writeReg(uint8_t addr, uint8_t value, uint8_t isCmd);
  uint8_t writePayload(const void* buf, uint8_t len, const uint8_t writeType);
  void flushTx(void);
  void powerUp(void);
  uint8_t getStatus(void);
  uint8_t send(const void* buf, uint8_t len, uint8_t ack);
  uint8_t send(const void* buf, uint8_t len);

  uint8_t checkR(void);
  uint8_t checkRW(void);
  uint8_t checkW(void);
private:
  SPI_interface _spi;
  uint8_t _curData = 0x00;
  uint8_t _dynPL = 0;
};

#endif
