#ifndef SD_H
#define SD_H

#include <avr/io.h>
#include <avr/pgmspace.h>
#include "spi.hpp"

#define CS PINB3

#define GO_IDLE_STATE          0
#define SEND_OP_COND           1
#define SEND_IF_COND			 		 8
#define SEND_CSD               9
#define STOP_TRANSMISSION      12
#define SEND_STATUS            13
#define SET_BLOCK_LEN          16
#define READ_SINGLE_BLOCK      17
#define READ_MULTIPLE_BLOCKS   18
#define WRITE_SINGLE_BLOCK     24
#define WRITE_MULTIPLE_BLOCKS  25
#define ERASE_BLOCK_START_ADDR 32
#define ERASE_BLOCK_END_ADDR   33
#define ERASE_SELECTED_BLOCKS  38
#define SD_SEND_OP_COND			 	 41
#define APP_CMD					 			 55
#define READ_OCR				 			 58
#define CRC_ON_OFF             59

#define ON     1
#define OFF    0

class SD_card {
public:
	uint8_t init(void);
	uint8_t erase(uint32_t start, uint32_t total);
	uint8_t sendCmd(uint8_t cmd, uint32_t arg);
	uint8_t writeBlock(uint32_t start);
	uint8_t readBlock(uint32_t start);
	uint8_t writeMltBlock(uint32_t start, uint32_t total);
	uint8_t readMltBlock(uint32_t start, uint32_t total);
private:
	SPI_interface _spi;
	uint32_t _start, _total;
	uint8_t _SDHC_flag, _cardType;
	uint8_t _buff[512];
};

#endif SD_H
