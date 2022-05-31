#include "sd.hpp"

uint8_t SD_card::init(void) {
  SDSS_DDR |= (1 << SD_SS);
  uint8_t res[5], cnt = 0;
  powerOn();
  while((res[0] = idle()) != SD_IN_IDLE_STATE) if(cnt++ >= CMD0_MAX_ATTEMPTS) return 0;
  _delay_ms(1);
  sendIfCond(res);
  if(res[0] != SD_IN_IDLE_STATE) return SD_ERROR;
  if(res[4] != 0xAA) return SD_ERROR;
  cnt = 0;
  do {
    if(cnt == CMD55_MAX_ATTEMPTS) return SD_ERROR;
    res[0] = sendApp();
    if(SD_R1_NO_ERROR(res[0])) res[0] = sendOpCond();
    _delay_ms(1);
    cnt++;
  } while (res[0] != SD_READY);
  _delay_ms(1);
  readOCR(res);
  return SD_SUCCESS;
}

void SD_card::powerOn(void) {
  SDSS_low();
  _delay_ms(10);
  _spi.transfer(0xFF);
  SDSS_low();
  for (uint8_t i = 0; i < SD_INIT_CYCLES; i++) _spi.transfer(0xFF);
}

void SD_card::sendCMD(uint8_t cmd, uint32_t arg, uint8_t crc) {
  _spi.transfer(cmd | 0x40);
  _spi.transfer((uint8_t)(arg >> 24));
  _spi.transfer((uint8_t)(arg >> 16));
  _spi.transfer((uint8_t)(arg >> 8));
  _spi.transfer((uint8_t)(arg));
  _spi.transfer(crc | 0x01);
}

uint8_t SD_card::readRes1() {
  uint8_t i = 0, res1;
  while ((res1 = _spi.transfer(0xFF)) == 0xFF) {
    i++;
    if(i > 8) break;
  } return res1;
}

void SD_card::readRes2(uint8_t *res) {
  res[0] = readRes1();
  res[1] = _spi.transfer(0xFF);
}

void SD_card::readRes3(uint8_t *res) {
  res[0] = readRes1();
  if(res[0] > 1) return;
  readBytes(res + 1, R3_BYTES);
}

void SD_card::readRes7(uint8_t *res) {
  res[0] = readRes1();
  if(res[0] > 1) return;
  readBytes(res + 1, R7_BYTES);
}

void SD_card::readBytes(uint8_t *res, uint8_t count) {
  while(count--) *res++ = _spi.transfer(0xFF);
}

uint8_t SD_card::idle(void) {
  _spi.transfer(0xFF);
  SDSS_high();
  _spi.transfer(0xFF);
  sendCMD(CMD0, CMD0_ARG, CMD0_CRC);
  uint8_t res1 = readRes1();
  _spi.transfer(0xFF);
  SDSS_low();
  _spi.transfer(0xFF);
  return res1;
}

void SD_card::sendIfCond(uint8_t *res) {
  _spi.transfer(0xFF);
  SDSS_high();
  _spi.transfer(0xFF);
  sendCMD(CMD8, CMD8_ARG, CMD8_CRC);
  readRes7(res);
  _spi.transfer(0xFF);
  SDSS_low();
  _spi.transfer(0xFF);
}

void SD_card::sendStatus(uint8_t *res) {
  _spi.transfer(0xFF);
  SDSS_high();
  _spi.transfer(0xFF);
  sendCMD(CMD13, CMD13_ARG, CMD13_CRC);
  readRes2(res);
  _spi.transfer(0xFF);
  SDSS_low();
  _spi.transfer(0xFF);
}

#define SD_MAX_WRITE_ATTEMPTS   3907

/*******************************************************************************
token = 0x00 - busy timeout
token = 0x05 - data accepted
token = 0xFF - response timeout
*******************************************************************************/
uint8_t SD_card::writeBlock(uint32_t addr, uint8_t *data, uint8_t *token) {
  uint16_t readAttempts;
  uint8_t res1, read;
  *token = 0xFF;
  _spi.transfer(0xFF);
  SDSS_high();
  _spi.transfer(0xFF);
  sendCMD(CMD24, addr, CMD24_CRC);
  res1 = readRes1();
  if(res1 == SD_READY) {
    _spi.transfer(SD_START_TOKEN);
    for (uint16_t i = 0; i < SD_BLOCK_LEN; i++) _spi.transfer(data[i]);
    readAttempts = 0;
    while (++readAttempts != SD_MAX_WRITE_ATTEMPTS)
    if ((read = _spi.transfer(0xFF)) != 0xFF) { *token = 0xFF; break; }
    if((read & 0x1F) == 0x05) {
      *token = 0x05;
      readAttempts = 0;
      while (_spi.transfer(0xFF) == 0x00) if (++readAttempts == SD_MAX_WRITE_ATTEMPTS) { *token = 0x00; break; }
    }
  }
  _spi.transfer(0xFF);
  SDSS_low();
  _spi.transfer(0xFF);
  return res1;
}

void SD_card::readOCR(uint8_t *res) {
  uint16_t cnt = 0;
  _spi.transfer(0xFF);
  SDSS_high();
  uint8_t tmp = _spi.transfer(0xFF);
  if (tmp != 0xFF) while (_spi.transfer(0xFF) != 0xFF) if (cnt++ > 2000) break;
  sendCMD(CMD58, CMD58_ARG, CMD58_CRC);
  readRes3(res);
  _spi.transfer(0xFF);
  SDSS_low();
  _spi.transfer(0xFF);
}

uint8_t SD_card::sendApp(void) {
  _spi.transfer(0xFF);
  SDSS_high();
  _spi.transfer(0xFF);
  sendCMD(CMD55, CMD55_ARG, CMD55_CRC);
  uint8_t res1 = readRes1();
  _spi.transfer(0xFF);
  SDSS_low();
  _spi.transfer(0xFF);
  return res1;
}

uint8_t SD_card::sendOpCond(void) {
  _spi.transfer(0xFF);
  SDSS_high();
  _spi.transfer(0xFF);
  sendCMD(ACMD41, ACMD41_ARG, ACMD41_CRC);
  uint8_t res1 = readRes1();
  _spi.transfer(0xFF);
  SDSS_low();
  _spi.transfer(0xFF);
  return res1;
}

// 0 - виртуальный сектор дополнен, 1 - виртуальный сектор записан на SD и обновлён
uint8_t SD_card::writeData(const void* buff, uint16_t len) {
  if (len + _bytesInSec >= 512) {
    writeBlock(_curAddr, _sector, &_token);
    _curAddr++;
    _bytesInSec = len;
    clearBuff();
    memcpy(_sector, buff, len);
    return 1;
  } else memcpy(_sector+len, buff, len);
  return 0;
}

void SD_card::updateAddr(uint32_t addr) {
  _curAddr = addr;
}

void SD_card::clearBuff(void) {
  memset(_sector, 0, 512);
}

uint8_t SD_card::getStatus(void) {
  return _token;
}