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

void DS_readTemp(float *temp) {
  uint8_t temp[2];
  DS_readRaw(temp);
  *temp = (float)((int)temp[0] | (((int)temp[1]) << 8)) * 0.0625 + 0.03125;
}
