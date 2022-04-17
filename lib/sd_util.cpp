#include "sd_util.hpp"

/* Предисловие
 * Библиотека соответствует шаблону о переменных С11
 * Булиновая функция возвращает 0 при успешном завершении и 1 при ошибке
 * Подробнее о библиотеке можно узнать из sd_util.hpp
 */

uint8_t SD_card::readRegister(uint8_t cmd, void* buf) {
  uint8_t* dst = reinterpret_cast<uint8_t*>(buf);
  if (cardCommand(cmd, 0)) {
    error(SD_CARD_ERROR_READ_REG);
    goto fail;
  }
  if (!waitStartBlock()) goto fail;
  // Отправка данных
  for (uint16_t i = 0; i < 16; i++) dst[i] = _spi.readByte();
  _spi.readByte();  // CRC 0
  _spi.readByte();  // CRC 1
  SS_high();
  return true;
fail:
  SS_high();
  return false;
}

uint8_t SD_card::waitStartBlock(void) {
  uint16_t t0 = millis();
  while ((_status = _spi.readByte()) == 0xFF) {
    if (((uint16_t)millis() - t0) > SD_READ_TIMEOUT) {
      error(SD_CARD_ERROR_READ_TIMEOUT);
      goto fail;
    }
  }
  if (_status != DATA_START_BLOCK) {
    error(SD_CARD_ERROR_READ);
    goto fail;
  }
  return true;
fail:
  SS_high();
  return false;
}

// Пропуск оставшихся данных в блоке в режиме считывания частями
void SD_card::readEnd(void) {
  if (_inBlock) {
    while (_offset++ < 514) _spi.readByte();
    SS_high();
    _inBlock = 0;
  }
}

uint8_t SD_card::waitNotBusy(uint16_t timeout) {
  uint16_t cT = millis();
  do {
    if (_spi.readByte() == 0xFF) return true;
  } while (((uint16_t)millis() - cT) < timeout);
  return false;
}

uint8_t SD_card::cardCommand(uint8_t cmd, uint32_t arg) {
  //readEnd();
  SS_low();
  waitNotBusy(300);
  // Отправка команды и аргумента
  _spi.writeByte(cmd | 0x40);
  for (int8_t s = 24; s >= 0; s -= 8) _spi.writeByte(arg >> s);
  // Отправка однобайтовой контрольной суммы
  uint8_t crc = 0xFF;
  if (cmd == CMD0) crc = 0x95;
  if (cmd == CMD8) crc = 0x87;
  _spi.writeByte(crc);
  // Ожидание ответа и возврат статуса
  for (uint8_t i = 0; ((_status = _spi.readByte()) & 0X80) && i != 0xFF; i++);
  return _status;
}

uint32_t SD_card::cardSize(void) {
  csd_t csd;
  if (!(readCSD(&csd))) return 0;
  if (csd.v1.csd_ver == 0) {
    uint8_t read_bl_len = csd.v1.read_bl_len;
    uint16_t c_size = (csd.v1.c_size_high << 10)
                    | (csd.v1.c_size_mid << 2) | csd.v1.c_size_low;
    uint8_t c_size_mult = (csd.v1.c_size_mult_high << 1)
                        | csd.v1.c_size_mult_low;
    return (uint32_t)(c_size + 1) << (c_size_mult + read_bl_len - 7);
  } else if (csd.v2.csd_ver == 1) {
    uint32_t c_size = ((uint32_t)csd.v2.c_size_high << 16)
                    | (csd.v2.c_size_mid << 8) | csd.v2.c_size_low;
    return (c_size + 1) << 10;
  } else {
    error(SD_CARD_ERROR_BAD_CSD);
    return 0;
  }
}

uint32_t SD_card::eraseSingleBlockEnable(void) {
  csd_t csd;
  return readCSD(&csd) ? csd.v1.erase_blk_en : 0;
}

uint8_t SD_card::erase(uint32_t fBlock, uint32_t lBlock) {
  if (!(eraseSingleBlockEnable())) {
    error(SD_CARD_ERROR_ERASE_SINGLE_BLOCK);
    goto fail;
  }
  if (_type != SD_CARD_TYPE_SDHC) {
    fBlock <<= 9;
    lBlock <<= 9;
  }
  if (cardCommand(CMD32, fBlock)
      || cardCommand(CMD33, lBlock)
      || cardCommand(CMD38, 0)) {
    error(SD_CARD_ERROR_ERASE);
    goto fail;
  }
  if (!waitNotBusy(SD_ERASE_TIMEOUT)) {
    error(SD_CARD_ERROR_ERASE_TIMEOUT);
    goto fail;
  }
  SS_high();
  return true;
fail:
  SS_high();
  return false;
}

uint8_t SD_card::init(uint8_t rateId, uint8_t spiInited) {
  _errorCode = _inBlock = _partialBlockRead = _type = 0;
  uint16_t t0 = (uint16_t)millis();
  uint32_t arg;
  if (!(spiInited)) _spi.init();
  SS_high();
  asm("nop");
  SS_low();
  while((_status != cardCommand(CMD0, 0)) != R1_IDLE_STATE) {
    if (((uint16_t)(millis() - t0)) > SD_INIT_TIMEOUT) {
      error(SD_CARD_ERROR_CMD0);
      goto fail;
    }
  }
  if ((cardCommand(CMD8, 0x1AA) & R1_ILLEGAL_COMMAND)) {
    type(SD_CARD_TYPE_SD1);
  } else {
    for (uint8_t i = 0; i < 4; i++) _status = _spi.readByte();
    if (_status != 0xAA) {
      error(SD_CARD_ERROR_CMD8);
      goto fail;
    } type(SD_CARD_TYPE_SD2);
  } arg = type() == SD_CARD_TYPE_SD2 ? 0x40000000 : 0;
  while ((_status = cardAcmd(ACMD41, arg)) != R1_READY_STATE) {
    if (((uint16_t)(millis() - t0)) > SD_INIT_TIMEOUT) {
      error(SD_CARD_ERROR_ACMD41);
      goto fail;
    }
  } if (type() == SD_CARD_TYPE_SD2) {
    if (cardCommand(CMD58, 0)) {
      error(SD_CARD_ERROR_CMD58);
      goto fail;
    }
    if ((_spi.readByte() & 0xC0) == 0xC0) type(SD_CARD_TYPE_SDHC);
    // Тут вольтаж, но он нам не нужен
    for (uint8_t i = 0; i < 3; i++) _spi.readByte();
  } SS_high();
  return true;
fail:
  SS_high();
  return false;
}

void SD_card::partialBlockRead(uint8_t value) {
  readEnd();
  _partialBlockRead = value;
}

uint8_t SD_card::readBlock(uint32_t block, uint8_t* dst) {
  return readData(block, 0, 512, dst);
}

uint8_t SD_card::readData(uint32_t block, uint16_t offset, uint16_t count, uint8_t* dst) {
  if (count == 0) return true;
  if ((count + offset) > 512) goto fail;
  if (!_inBlock || block != _block || offset < _offset) {
    _block = block;
    // Если не SDHC, то используем адрес
    if (type() != SD_CARD_TYPE_SDHC) block <<= 9;
    if (cardCommand(CMD17, block)) {
      error(SD_CARD_ERROR_CMD17);
      goto fail;
    }
    if (!waitStartBlock()) {
      goto fail;
    }
    _offset = 0;
    _inBlock = 1;
  }
  // Пропуск данных до компенсатора
  for (;_offset < offset; _offset++) _spi.readByte();
  // Перемещаем данные
  for (uint16_t i = 0; i < count; i++) dst[i] = _spi.readByte();
  _offset += count;
  if (!_partialBlockRead || _offset >= 512) readEnd();
  return true;
fail:
  SS_high();
  return false;
}

uint8_t SD_card::writeBlock(uint32_t blockNumber, const uint8_t* src) {
#if SD_PROTECT_BLOCK_ZERO
  // Не позволяем записать нулевой блок
  if (blockNumber == 0) {
    error(SD_CARD_ERROR_WRITE_BLOCK_ZERO);
    goto fail;
  }
#endif  // SD_PROTECT_BLOCK_ZERO
  if (type() != SD_CARD_TYPE_SDHC) blockNumber <<= 9;
  if (cardCommand(CMD24, blockNumber)) {
    error(SD_CARD_ERROR_CMD24);
    goto fail;
  }
  if (!writeData(DATA_START_BLOCK, src)) goto fail;
  // wait for flash programming to complete
  if (!waitNotBusy(SD_WRITE_TIMEOUT)) {
    error(SD_CARD_ERROR_WRITE_TIMEOUT);
    goto fail;
  }
  // response is r2 so get and check two bytes for nonzero
  if (cardCommand(CMD13, 0) || _spi.readByte()) {
    error(SD_CARD_ERROR_WRITE_PROGRAMMING);
    goto fail;
  }
  SS_high();
  return true;

fail:
  SS_high();
  return false;
}

uint8_t SD_card::writeData(const uint8_t* src) {
  if (!waitNotBusy(SD_WRITE_TIMEOUT)) {
    error(SD_CARD_ERROR_WRITE_MULTIPLE);
    SS_high();
    return false;
  }
  return writeData(WRITE_MULTIPLE_TOKEN, src);
}
uint8_t SD_card::writeData(uint8_t token, const uint8_t* src) {
  _spi.writeByte(token);
  for (uint16_t i = 0; i < 512; i++) _spi.writeByte(src[i]);
  _spi.writeByte(0xff); // Фейковая CRC
  _spi.writeByte(0xff); // Фейковая CRC
  _status = _spi.readByte();
  if ((_status & DATA_RES_MASK) != DATA_RES_ACCEPTED) {
    error(SD_CARD_ERROR_WRITE);
    SS_high();
    return false;
  } return true;
}

uint8_t SD_card::writeStart(uint32_t blockNumber, uint32_t eraseCount) {
#if SD_PROTECT_BLOCK_ZERO
  // don't allow write to first block
  if (blockNumber == 0) {
    error(SD_CARD_ERROR_WRITE_BLOCK_ZERO);
    goto fail;
  }
#endif  // SD_PROTECT_BLOCK_ZERO
  // send pre-erase count
  if (cardAcmd(ACMD23, eraseCount)) {
    error(SD_CARD_ERROR_ACMD23);
    goto fail;
  }
  // use address if not SDHC card
  if (type() != SD_CARD_TYPE_SDHC) blockNumber <<= 9;
  if (cardCommand(CMD25, blockNumber)) {
    error(SD_CARD_ERROR_CMD25);
    goto fail;
  } return true;
fail:
  SS_high();
  return false;
}

uint8_t SD_card::writeStop(void) {
  if (!waitNotBusy(SD_WRITE_TIMEOUT)) goto fail;
  _spi.writeByte(STOP_TRAN_TOKEN);
  if (!waitNotBusy(SD_WRITE_TIMEOUT)) goto fail;
  SS_high();
  return true;
fail:
  error(SD_CARD_ERROR_STOP_TRAN);
  SS_high();
  return false;
}
