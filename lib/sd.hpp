#ifndef SD_H
#define SD_H

#define F_CPU 8000000UL

/* Библиотека "сырого" взаимодействия с SD V1, V2
 * Автор: Гарагуля Артур, "SPORADIC", г. Курск
 * Интерфейс: SPI
 * Зависимости: avr, spi
 */

#include "config.h"
#include "pinout.h"
#include "spi.hpp"
#include <string.h>
#include <stdint.h>
#include <util/delay.h>

#define CMD0                0
#define CMD0_ARG            0x00000000
#define CMD0_CRC            0x94
#define CMD8                8
#define CMD8_ARG            0x0000001AA
#define CMD8_CRC            0x86
#define CMD9                9
#define CMD9_ARG            0x00000000
#define CMD9_CRC            0x00
#define CMD10               9
#define CMD10_ARG           0x00000000
#define CMD10_CRC           0x00
#define CMD13               13
#define CMD13_ARG           0x00000000
#define CMD13_CRC           0x00
#define CMD17               17
#define CMD17_CRC           0x00
#define CMD24               24
#define CMD24_CRC           0x00
#define CMD55               55
#define CMD55_ARG           0x00000000
#define CMD55_CRC           0x00
#define CMD58               58
#define CMD58_ARG           0x00000000
#define CMD58_CRC           0x00
#define ACMD41              41
#define ACMD41_ARG          0x40000000
#define ACMD41_CRC          0x00

#define SD_IN_IDLE_STATE    0x01
#define SD_READY            0x00
#define SD_R1_NO_ERROR(X)   X < 0x02

#define R3_BYTES            4
#define R7_BYTES            4

#define CMD0_MAX_ATTEMPTS       255
#define CMD55_MAX_ATTEMPTS      255
#define SD_ERROR                0
#define SD_SUCCESS              1
#define SD_MAX_READ_ATTEMPTS    1563
#define SD_READ_START_TOKEN     0xFE
#define SD_INIT_CYCLES          80

#define SD_START_TOKEN          0xFE
#define SD_ERROR_TOKEN          0x00

#define SD_DATA_ACCEPTED        0x05
#define SD_DATA_REJECTED_CRC    0x0B
#define SD_DATA_REJECTED_WRITE  0x0D

class SD_card {
private:
  SPI_interface _spi;
  uint8_t  _sector[SD_BLOCK_LEN];
  uint16_t _bytesInSec;
  uint32_t _curAddr;
  uint8_t _token;
public:
  SD_card(void) : _bytesInSec(0), _curAddr(0), _token(0xFF) { memset(_sector, 0, SD_BLOCK_LEN); }
  uint8_t init(void);
  uint8_t readBlock(uint32_t addr, uint8_t *buff, uint8_t *err);
  uint8_t writeBlock(uint32_t addr, uint8_t *data, uint8_t *err);
  // Функции "потоковой" записи
  uint8_t writeData(const void* buff, uint16_t len);
  void updateAddr(uint32_t addr);
  void clearBuff(void);
  uint8_t getStatus(void);
private:
  void powerOn(void);
  uint8_t idle(void);
  void sendCMD(uint8_t cmd, uint32_t arg, uint8_t crc);
  void sendIfCond(uint8_t *res);
  void sendStatus(uint8_t *res);
  void readOCR(uint8_t *res);
  uint8_t sendApp(void);
  uint8_t sendOpCond(void);
  uint8_t readRes1(void);
  void readRes2(uint8_t *res);
  void readRes3(uint8_t *res);
  void readRes7(uint8_t *res);
  void readBytes(uint8_t *res, uint8_t count);
};

#endif