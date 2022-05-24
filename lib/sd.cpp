#include "sd.hpp"

uint8_t SD_card::init(void) {
  uint8_t i, response, SD_version;
  uint16_t retry = 0 ;
  for(i = 0; i < 10; i++) _spi.transfer(0xff);
  SDSS_high();
  do {
    response = sendCMD(GO_IDLE_STATE, 0);
    retry++;
    if(retry>0x20) return 1;
  } while(response != 0x01);
  SDSS_low();
  _spi.transfer(0xff);
  _spi.transfer(0xff);
  retry = 0;
  SD_version = 2;
  do {
    response = sendCMD(SEND_IF_COND,0x000001AA);
    retry++;
    if(retry>0xfe) {
      SD_version = 1;
      _type = 1;
      break;
    }
  } while(response != 0x01);
  retry = 0;
  do {
    response = sendCMD(APP_CMD,0);
    response = sendCMD(SD_SEND_OP_COND,0x40000000);
    retry++;
    if(retry>0xfe) return 2;
  } while(response != 0x00);
  retry = 0;
  _SDHC = 0;
  if (SD_version == 2) { 
    do {
      response = sendCMD(READ_OCR,0);
      retry++;
      if(retry>0xfe)  {
        _type = 0;
        break;
      }
    } while(response != 0x00);
    if(_SDHC == 1) _type = 2;
    else _type = 3;
  } return 0;
}

uint8_t SD_card::sendCMD(uint8_t data, uint32_t arg) {
  uint8_t response, retry=0, status;
  if(_SDHC == 0)   
  if(data == READ_SINGLE_BLOCK     ||
     data == READ_MULTIPLE_BLOCKS  ||
     data == WRITE_SINGLE_BLOCK    ||
     data == WRITE_MULTIPLE_BLOCKS ||
     data == ERASE_BLOCK_START_ADDR|| 
     data == ERASE_BLOCK_END_ADDR ) 
    data <<= 9;
  SDSS_high();
  _spi.transfer(data | 0x40);
  _spi.transfer(arg>>24);
  _spi.transfer(arg>>16);
  _spi.transfer(arg>>8);
  _spi.transfer(arg);
  if(data == SEND_IF_COND) _spi.transfer(0x87);
  else _spi.transfer(0x95); 

  while((response = _spi.readByte()) == 0xff) if(retry++ > 0xfe) break;

  if(response == 0x00 && data == 58) {
    status = _spi.readByte() & 0x40;
    if(status == 0x40) _SDHC = 1;
    else _SDHC = 0;
    _spi.readByte();
    _spi.readByte();
    _spi.readByte();
  }
  _spi.readByte();
  SDSS_low();

  return response;
}

uint8_t SD_card::writeSingleBlock(uint32_t startBlock, uint8_t *buff) {
  uint8_t response;
  uint16_t i, retry=0;
  response = sendCMD(WRITE_SINGLE_BLOCK, startBlock);
  if(response != 0x00) return response;
  SDSS_high();
  _spi.transfer(0xfe);
  for(i=0; i<512; i++) _spi.transfer(buff[i]);
  _spi.transfer(0xff);
  _spi.transfer(0xff);
  response = _spi.readByte();
  if( (response & 0x1f) != 0x05) {
    SDSS_low();
    return response;
  }
  while(!_spi.readByte()) if (retry++ > 0xfffe) { SDSS_low(); return 1; }
  SDSS_low();
  _spi.transfer(0xff);
  SDSS_high();
  while(!_spi.readByte())
  if(retry++ > 0xfffe) {
    SDSS_low();
    return 1;
  }
  SDSS_low();
  return 0;
}