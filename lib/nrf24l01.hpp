#ifndef NRF_H
#define NRF_H

#define F_CPU 8000000UL

#include "spi.hpp"
#include <util/delay.h>
#include <stddef.h>

/* Библиотека управления радиомодулем nRF24L01+
 * Автор: Гарагуля Артур, "SPORADIC", г. Курск
 * Интерфейс: SPI + 1 digital pin
 * Зависимости: avr, spi
 */

#define RF24_SPI_SPEED 10000000
#define RF24_POWERUP_DELAY  5000

#define NRF_CONFIG  0x00
#define EN_AA       0x01
#define EN_RXADDR   0x02
#define SETUP_AW    0x03
#define SETUP_RETR  0x04
#define RF_CH       0x05
#define RF_SETUP    0x06
#define NRF_STATUS  0x07
#define OBSERVE_TX  0x08
#define CD          0x09
#define RX_ADDR_P0  0x0A
#define RX_ADDR_P1  0x0B
#define RX_ADDR_P2  0x0C
#define RX_ADDR_P3  0x0D
#define RX_ADDR_P4  0x0E
#define RX_ADDR_P5  0x0F
#define TX_ADDR     0x10
#define RX_PW_P0    0x11
#define RX_PW_P1    0x12
#define RX_PW_P2    0x13
#define RX_PW_P3    0x14
#define RX_PW_P4    0x15
#define RX_PW_P5    0x16
#define FIFO_STATUS 0x17
#define DYNPD       0x1C
#define FEATURE     0x1D

#define MASK_RX_DR  6
#define MASK_TX_DS  5
#define MASK_MAX_RT 4
#define EN_CRC      3
#define CRCO        2
#define PWR_UP      1
#define PRIM_RX     0
#define ENAA_P5     5
#define ENAA_P4     4
#define ENAA_P3     3
#define ENAA_P2     2
#define ENAA_P1     1
#define ENAA_P0     0
#define ERX_P5      5
#define ERX_P4      4
#define ERX_P3      3
#define ERX_P2      2
#define ERX_P1      1
#define ERX_P0      0
#define AW          0
#define ARD         4
#define ARC         0
#define PLL_LOCK    4
#define CONT_WAVE   7
#define RF_DR       3
#define RF_PWR      6
#define RX_DR       6
#define TX_DS       5
#define MAX_RT      4
#define RX_P_NO     1
#define TX_FULL     0
#define PLOS_CNT    4
#define ARC_CNT     0
#define TX_REUSE    6
#define FIFO_FULL   5
#define TX_EMPTY    4
#define RX_FULL     1
#define RX_EMPTY    0
#define DPL_P5      5
#define DPL_P4      4
#define DPL_P3      3
#define DPL_P2      2
#define DPL_P1      1
#define DPL_P0      0
#define EN_DPL      2
#define EN_ACK_PAY  1
#define EN_DYN_ACK  0

#define R_REGISTER    0x00
#define W_REGISTER    0x20
#define REGISTER_MASK 0x1F
#define ACTIVATE      0x50
#define R_RX_PL_WID   0x60
#define R_RX_PAYLOAD  0x61
#define W_TX_PAYLOAD  0xA0
#define W_ACK_PAYLOAD 0xA8
#define FLUSH_TX      0xE1
#define FLUSH_RX      0xE2
#define REUSE_TX_PL   0xE3
#define RF24_NOP      0xFF

#define LNA_HCURR   0

#define RPD                  0x09
#define W_TX_PAYLOAD_NO_ACK  0xB0

#define RF_DR_LOW   5
#define RF_DR_HIGH  3
#define RF_PWR_LOW  1
#define RF_PWR_HIGH 2

typedef enum {
  //-18 dBm | -6 dBm | -12 dBm
  RF24_PA_MIN = 0,
  //-12 dBm | 0 dBm | -4 dBm
  RF24_PA_LOW,
  //-6 dBm | 3 dBm | 1 dBm
  RF24_PA_HIGH,
  //0 dBm | 7 dBm | 4 dBm
  RF24_PA_MAX,
  RF24_PA_ERROR
} rf24_pa_dbm_e;

typedef enum {
    RF24_1MBPS = 0,
    RF24_2MBPS,
    RF24_250KBPS
} rf24_datarate_e;

typedef enum {
    RF24_CRC_DISABLED = 0,
    RF24_CRC_8,
    RF24_CRC_16
} rf24_crclength_e;

class nRF24_radio {
private:
  SPI_interface _spi;
  uint32_t _spiSpeed;
  uint8_t _status;
  uint8_t _payloadSize;
  uint8_t pipe0_reading_address[5];
  uint8_t _configReg;
  bool _is_p_variant;
  bool _is_p0_rx;
protected:
  inline void beginTransaction();
  inline void endTransaction();
  bool ack_payloads_enabled;
  uint8_t addr_width;
  bool dynamic_payloads_enabled;
  void readReg(uint8_t reg, uint8_t* buf, uint8_t len);
  uint8_t readReg(uint8_t reg);
public:
  uint32_t csDelay;
  uint8_t txDelay;
  nRF24_radio(uint32_t spiSpeed = RF24_SPI_SPEED);
  bool begin(void);
  bool isChipConnected();
  void startListening(void);
  void stopListening(void);
  bool available(void);
  bool available(uint8_t* pipe_num);
  void read(void* buf, uint8_t len);
  bool write(const void* buf, uint8_t len);
  void openWritingPipe(uint64_t address);
  void openReadingPipe(uint8_t number, const uint8_t* address);
  void printDetails(void);
  void printPrettyDetails(void);
  bool rxFifoFull();
  void powerDown(void);
  void powerUp(void);
  bool write(const void* buf, uint8_t len, const bool multicast);
  bool writeFast(const void* buf, uint8_t len);
  bool writeFast(const void* buf, uint8_t len, const bool multicast);
  bool writeBlocking(const void* buf, uint8_t len, uint32_t timeout);
  bool writeAckPayload(uint8_t pipe, const void* buf, uint8_t len);
  bool txStandBy();
  bool txStandBy(uint32_t timeout, bool startTx = 0);
  void whatHappened(bool& tx_ok, bool& tx_fail, bool& rx_ready);
  void startFastWrite(const void* buf, uint8_t len, const bool multicast, bool startTx = 1);
  bool startWrite(const void* buf, uint8_t len, const bool multicast);
  void reUseTX();
  uint8_t flush_tx(void);
  uint8_t flush_rx(void);
  bool testCarrier(void);
  bool testRPD(void);
  bool isValid();
  void closeReadingPipe(uint8_t pipe);
  void setChannel(uint8_t channel);
  void setRadiation(uint8_t level, rf24_datarate_e speed, bool lnaEnable = true);
  void setRetries(uint8_t delay, uint8_t count);
  void setPayloadSize(uint8_t size);
  void setAddressWidth(uint8_t);
  void toggleFeatures(void);
  bool setDataRate(rf24_datarate_e speed);
  void setPALevel(uint8_t level, bool lnaEnable = 1);
private:
  void _init_obj();
  bool _init_radio();
  bool _init_pins();
  inline uint8_t _pa_level_reg_value(uint8_t level, bool lnaEnable);
  uint8_t _data_rate_reg_value(rf24_datarate_e speed);
  void csn(bool mode);
  void ce(bool level);
  void writeReg(uint8_t reg, const uint8_t* buf, uint8_t len);
  void writeReg(uint8_t reg, uint8_t value, bool is_cmd_only = false);
  void writePayload(const void* buf, uint8_t len, const uint8_t writeType);
  void readPayload(uint8_t *buf, uint8_t len);
  void readPayload(void* buf, uint8_t len);
  uint8_t getStatus(void);
};

#endif // NRF_H