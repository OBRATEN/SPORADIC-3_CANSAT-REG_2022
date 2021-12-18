#include "ds18b20.h"

void DS_init(void) {
  OW_init();
}

void DS_readRaw(uint8_t *data) {
  OW_reset();
  OW_writeByte(0xCC);
  OW_writeByte(0x44);
  while (!(OW_readBit()));
  OW_reset();
  OW_writeByte(0xCC);
  OW_writeByte(0xBE);
  data[0] = OW_readByte();
  data[1] = OW_readByte();
  OW_reset();
}

void DS_readTemp(char *buff) {
  uint8_t temp[2];
  int8_t digit;
  uint16_t dec;
  DS_readRaw(temp);
  digit = temp[0] >> 4;
  digit |= (temp[1] & 0x7) << 4;
  dec = temp[0] & 0xf;
  dec *= .0625;
  sprintf(buff, "%d.%02u", digit, dec);
}
