#ifndef CONFIG_H
#define CONFIG_H

#include <avr/io.h>

#define F_CPU     8000000UL
#define F_SCL     400000UL
#define UART_BAUD 9600

// Биты байта статуса
#define STATUS_PREFIX 7
#define STATUS_I2C    6
#define STATUS_SPI    5
#define STATUS_SD     4
#define STATUS_ADXL   3
#define STATUS_BMP    2
#define STATUS_DS     1
#define STATUS_BN     0
#define STATUS_CHECK  (0 | (1 << STATUS_I2C) | (1 << STATUS_SPI))

#define SD_BLOCK_LEN 512
#define SD_SECTORS   3903488

#define DS_TIME_INTERVAL    770
#define NRF_TIME_INTERVAL   10
#define BMP_TIME_INTERVAL   50
#define ADXL_TIME_INTERVAL  25
#define SD_TIME_INTERVAL    100

#define AXEL_DEF_ADDR 0x1d
#define AXEL_ALT_ADDR 0x53

#define BMP_DEF_ADDR 0x77
#define BMP_ALT_ADDR 0x76

#define TESTING_MODE

#endif // CONFIG_H