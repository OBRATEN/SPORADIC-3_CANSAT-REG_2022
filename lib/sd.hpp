#ifndef SD_H
#define SD_H

#define F_CPU 8000000UL

/* Библиотека "сырого" взаимодействия с SD V1, V2
 * Автор: Гарагуля Артур, "SPORADIC", г. Курск
 * Интерфейс: SPI
 * Зависимости: avr, spi
 */

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include "spi.hpp"

#define SDSS_low()  PORTC |=  (1 << PINC1)
#define SDSS_high() PORTC &= ~(1 << PINC1)

#define GO_IDLE_STATE            0
#define SEND_OP_COND             1
#define SEND_IF_COND       8
#define SEND_CSD                 9
#define STOP_TRANSMISSION        12
#define SEND_STATUS              13
#define SET_BLOCK_LEN            16
#define READ_SINGLE_BLOCK        17
#define READ_MULTIPLE_BLOCKS     18
#define WRITE_SINGLE_BLOCK       24
#define WRITE_MULTIPLE_BLOCKS    25
#define ERASE_BLOCK_START_ADDR   32
#define ERASE_BLOCK_END_ADDR     33
#define ERASE_SELECTED_BLOCKS    38
#define SD_SEND_OP_COND      41   //ACMD
#define APP_CMD          55
#define READ_OCR         58
#define CRC_ON_OFF               59

class SD_card {
private:
  SPI_interface _spi;
  uint8_t _SDHC = 0;
  uint8_t _type = 0;
public:
  uint8_t init(void);
  uint8_t sendCMD(uint8_t cmd, uint32_t arg);
  uint8_t writeSingleBlock(uint32_t addr, uint8_t *buf);
};

#endif