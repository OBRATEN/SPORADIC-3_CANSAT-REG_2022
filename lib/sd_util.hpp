#ifndef SD_H
#define SD_H

/* Библиотека "сырого" взаимодействия с SD V1, V2
 * Автор: Гарагуля Артур, "SPORADIC", г. Курск
 * Интерфейс: SPI
 * Зависимости: avr, spi
 */

#include <stdint.h>
#include "spi.hpp"
#include "timer.h"

// IDLE | инит по SPI если на CS низкий уровень
#define CMD0 0x00
// SEND_IF_COND | verify SD interface operating condition
#define CMD8 0x08
// SEND_CSD | считать регистр CSD
#define CMD9 0x09
// SEND_CID | считать регистр CID
#define CMD10 0x0A
// SEND_STATUS | считать статусный регистр
#define CMD13 0x0D
// READ_BLOCK | считать одиночный блок данных
#define CMD17 0x11
// WRITE_BLOCK | записать одиночный блок данных
#define CMD24 0x18
// WRITE_MULTIPLE_BLOCK | записывать блоки данных до STOP_TRANSMISSION
#define CMD25 0x19
// ERASE_WR_BLK_START | задать адрес нулевого блока для стирания
#define CMD32 0x20
// ERASE_WR_BLK_END | задать адрес последнего блока для стирания
#define CMD33 0x21
// ERASE | стереть все ранее выделеные блоки
#define CMD38 0x26
// APP_CMD | "побег" для специфичных команд
#define CMD55 0x37
// READ_OCR | считать регистр OCR
#define CMD58 0x3A
// SET_WR_BLK_ERASE_COUNT | задать количество блоков для чистки перед записью
#define ACMD23 0x17
// SD_SEND_OP_COMD | Отправить данные о состоянии хоста и активировать процесс инициализации
#define ACMD41 0x29
//------------------------------------------------------------------------------
// Статус карты в состоянии готовности
#define R1_READY_STATE 0x00
// Статус карты в состоянии ожидания
#define R1_IDLE_STATE 0x01
// Статусный бит для неверной команды
#define R1_ILLEGAL_COMMAND 0x04
// Метка начала для чтения или записи одиночного блока
#define DATA_START_BLOCK 0xFE
// Метка конца для записи множества блоков
#define STOP_TRAN_TOKEN 0xFD
// Метка начала для записи множества блоков
#define WRITE_MULTIPLE_TOKEN 0xFC
// Маска для метки ответа после записи блока
#define DATA_RES_MASK 0x1F
// Запись метки успешности принятых данных
#define DATA_RES_ACCEPTED 0x05

typedef struct CID {
  // Байт 0
  uint8_t mid;  // ID производителя
  // Байты 1-2
  char oid[2];  // OEM/Application ID
  // Байты 3-7
  char pnm[5];  // Название продукта
  // Байт 8
  unsigned prv_m : 4;  // Редакция продукта
  unsigned prv_n : 4;
  // Байты 9-12
  uint32_t psn;  // Серийный номер продукта
  // Байты 13
  unsigned mdt_year_high : 4;  // Дата выпуска
  unsigned reserved : 4;
  // Байты 14
  unsigned mdt_month : 4;
  unsigned mdt_year_low :4;
  // Байты 15
  unsigned always1 : 1;
  unsigned crc : 7;
} cid_t;

// CSD для карт версии 1.0
typedef struct CSDV1 {
  // Байт 0
  unsigned reserved1 : 6;
  unsigned csd_ver : 2;
  // Байт 1
  uint8_t taac;
  // Байт 2
  uint8_t nsac;
  // Байт 3
  uint8_t tran_speed;
  // Байт 4
  uint8_t ccc_high;
  // Байт 5
  unsigned read_bl_len : 4;
  unsigned ccc_low : 4;
  // Байт 6
  unsigned c_size_high : 2;
  unsigned reserved2 : 2;
  unsigned dsr_imp : 1;
  unsigned read_blk_misalign :1;
  unsigned write_blk_misalign : 1;
  unsigned read_bl_partial : 1;
  // Байт 7
  uint8_t c_size_mid;
  // Байт 8
  unsigned vdd_r_curr_max : 3;
  unsigned vdd_r_curr_min : 3;
  unsigned c_size_low :2;
  // Байт 9
  unsigned c_size_mult_high : 2;
  unsigned vdd_w_cur_max : 3;
  unsigned vdd_w_curr_min : 3;
  // Байт 10
  unsigned sector_size_high : 6;
  unsigned erase_blk_en : 1;
  unsigned c_size_mult_low : 1;
  // Байт 11
  unsigned wp_grp_size : 7;
  unsigned sector_size_low : 1;
  // Байт 12
  unsigned write_bl_len_high : 2;
  unsigned r2w_factor : 3;
  unsigned reserved3 : 2;
  unsigned wp_grp_enable : 1;
  // Байт 13
  unsigned reserved4 : 5;
  unsigned write_partial : 1;
  unsigned write_bl_len_low : 2;
  // Байт 14
  unsigned reserved5: 2;
  unsigned file_format : 2;
  unsigned tmp_write_protect : 1;
  unsigned perm_write_protect : 1;
  unsigned copy : 1;
  unsigned file_format_grp : 1;
  // Байт 15
  unsigned always1 : 1;
  unsigned crc : 7;
} csd1_t;
//------------------------------------------------------------------------------
// CSD для карт 2.0
typedef struct CSDV2 {
  // Байт 0
  unsigned reserved1 : 6;
  unsigned csd_ver : 2;
  // Байт 1
  uint8_t taac;
  // Байт 2
  uint8_t nsac;
  // Байт 3
  uint8_t tran_speed;
  // Байт 4
  uint8_t ccc_high;
  // Байт 5
  unsigned read_bl_len : 4;
  unsigned ccc_low : 4;
  // Байт 6
  unsigned reserved2 : 4;
  unsigned dsr_imp : 1;
  unsigned read_blk_misalign :1;
  unsigned write_blk_misalign : 1;
  unsigned read_bl_partial : 1;
  // Байт 7
  unsigned reserved3 : 2;
  unsigned c_size_high : 6;
  // Байт 8
  uint8_t c_size_mid;
  // Байт 9
  uint8_t c_size_low;
  // Байт 10
  unsigned sector_size_high : 6;
  unsigned erase_blk_en : 1;
  unsigned reserved4 : 1;
  // Байт 11
  unsigned wp_grp_size : 7;
  unsigned sector_size_low : 1;
  // Байт 12
  unsigned write_bl_len_high : 2;
  unsigned r2w_factor : 3;
  unsigned reserved5 : 2;
  unsigned wp_grp_enable : 1;
  // Байт 13
  unsigned reserved6 : 5;
  unsigned write_partial : 1;
  unsigned write_bl_len_low : 2;
  // Байт 14
  unsigned reserved7: 2;
  unsigned file_format : 2;
  unsigned tmp_write_protect : 1;
  unsigned perm_write_protect : 1;
  unsigned copy : 1;
  unsigned file_format_grp : 1;
  // Байт 15
  unsigned always1 : 1;
  unsigned crc : 7;
} csd2_t;
//------------------------------------------------------------------------------
// unoin старого и нового типа
union csd_t {
  csd1_t v1;
  csd2_t v2;
};
//------------------------------------------------------------------------------

// SCK равен максимальному значению F_CPU/2
#define SPI_FULL_SPEED 0
// SCK равен половине максимума F_CPU/4
#define SPI_HALF_SPEED 1
// SCK равен четверти максимума F_CPU/8
#define SPI_QUARTER_SPEED 2
// Защищает нулевой блок 
#define SD_PROTECT_BLOCK_ZERO 1
// Таймаут инициализации
#define SD_INIT_TIMEOUT 2000
// Таймаут стирания
#define SD_ERASE_TIMEOUT 10000
// Таймаут чтения
#define SD_READ_TIMEOUT 300
// Таймаут записи
#define SD_WRITE_TIMEOUT 600
//------------------------------------------------------------------------------
// Ошибки карты
// Ошибка таймаута для команды CMD0
#define SD_CARD_ERROR_CMD0 0x1
// CMD8 не принята => карта не поддерживается
#define SD_CARD_ERROR_CMD8 0x2
// Карта вернула ошибку при чтении блока CMD17
#define SD_CARD_ERROR_CMD17 0x3
// Карта вернула ошибку при записи блока CMD24
#define SD_CARD_ERROR_CMD24 0x4
// Ошибка записи множества блоков
#define SD_CARD_ERROR_CMD25 0x05
// Ошибка чтения OCR CMD58
#define SD_CARD_ERROR_CMD58 0x06
// Ошибка при определении количества блоков для стирания
#define SD_CARD_ERROR_ACMD23 0x07
// Ошибка таймаута инициализации ACMD41
#define SD_CARD_ERROR_ACMD41 0x08
// Карта вернула битое значение CSR версии
#define SD_CARD_ERROR_BAD_CSD 0x09
// Ошибка от команды чистки множества блоков
#define SD_CARD_ERROR_ERASE 0x0A
// Карта не может стерать одиночные блоки
#define SD_CARD_ERROR_ERASE_SINGLE_BLOCK 0x0B
// Ошибка таймаута чистки последовательности множества блоков
#define SD_CARD_ERROR_ERASE_TIMEOUT 0x0C
// Карта вернула метку ошибки во время чтения данных
#define SD_CARD_ERROR_READ 0x0D
// Ошибка чтения CID или CSD
#define SD_CARD_ERROR_READ_REG 0x0E
// Таймаут ожидания старта чтения данных
#define SD_CARD_ERROR_READ_TIMEOUT 0x0F
// Карта не принимает STOP_TRAN_TOKEN
#define SD_CARD_ERROR_STOP_TRAN 0x10
// Карта вернула метку ошибки как ответ на операцию записи данных
#define SD_CARD_ERROR_WRITE 0x11
// Попытка записи заблокированного нулевого блока
#define SD_CARD_ERROR_WRITE_BLOCK_ZERO 0x12
// Карта не переходит в состояние готовности для записи множества блоков
#define SD_CARD_ERROR_WRITE_MULTIPLE 0x13
// Карта вернула ошибку на CMD13 чтение статуса после записи
#define SD_CARD_ERROR_WRITE_PROGRAMMING 0x14
// Таймаут во время записи
#define SD_CARD_ERROR_WRITE_TIMEOUT 0x15
// Скорость указана неверно
#define SD_CARD_ERROR_SCK_RATE  0x16
//------------------------------------------------------------------------------
// Типы карт
// Стандартная ёмкость карт V1
#define SD_CARD_TYPE_SD1  1
// Сандартная ёмкость карт V2
#define SD_CARD_TYPE_SD2  2
// Высокоёмкостная карта
#define SD_CARD_TYPE_SDHC 3

class SD_card {
public:
    SD_card(void) : _errorCode(0), _inBlock(0), _partialBlockRead(0), _type(0) {}
    uint32_t cardSize(void);
    uint8_t erase(uint32_t fBlock, uint32_t lBlock);
    uint32_t eraseSingleBlockEnable(void);
    uint8_t errorCode(void) const {return _errorCode;}
    uint8_t errorData(void) const {return _status;}

    uint8_t init(void) {return init(SPI_FULL_SPEED, 0);}
    uint8_t init(uint8_t rateId) {return init(rateId, 0);}
    uint8_t init(uint8_t rateId, uint8_t spiInited);
    
    void partialBlockRead(uint8_t value);
    uint8_t partialBlockRead(void) const {return _partialBlockRead;}
    uint8_t readBlock(uint32_t block, uint8_t* dst);
    uint8_t readData(uint32_t block, uint16_t offset, uint16_t count, uint8_t* dst);
    uint8_t readCID(cid_t* cid) {return readRegister(CMD10, cid);}
    uint8_t readCSD(csd_t* csd) {return readRegister(CMD9, csd);}
    void readEnd(void);
    uint8_t setSckRate(uint8_t sckRateID);
    uint8_t type(void) const {return _type;}
    uint8_t writeBlock(uint32_t blockNumber, const uint8_t* src);
    uint8_t writeData(const uint8_t* src);
    uint8_t writeStart(uint32_t blockNumber, uint32_t eraseCount);
    uint8_t writeStop(void);
private:
    SPI_interface _spi;
    uint32_t _block;
    uint8_t _errorCode;
    uint8_t _inBlock;
    uint16_t _offset;
    uint8_t _partialBlockRead;
    uint8_t _status;
    uint8_t _type;

    uint8_t cardAcmd(uint8_t cmd, uint32_t arg) {
        cardCommand(CMD55, 0);
        return cardCommand(cmd, arg);
    }

    uint8_t cardCommand(uint8_t cmd, uint32_t arg);
    void error(uint8_t code) {_errorCode = code;}
    uint8_t readRegister(uint8_t cmd, void* buf);
    uint8_t sendWriteCommand(uint32_t blockNumber, uint32_t eraseCount);
    void chipSelectHigh(void);
    void chipSelectLow(void);
    void type(uint8_t value) {_type = value;}
    uint8_t waitNotBusy(uint16_t timeoutMillis);
    uint8_t writeData(uint8_t token, const uint8_t* src);
    uint8_t waitStartBlock(void);
};

#endif
