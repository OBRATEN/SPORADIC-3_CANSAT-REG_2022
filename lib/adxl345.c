#include "adxl345.h"

uint8_t devAddr;

void Axel_init(void) {
  I2C_init(F_CPU, F_SCL);
}

uint8_t Axel_begin(uint8_t daddr) {
  devAddr = daddr;
  if (!(Axel_addressExists())) return 0;
  I2C_writeReg(devAddr, AXEL_BW_RATE_REG, AXEL_BW_RATE_25);
  Axel_configure();
  Axel_powerOn();
  return 1;
}

uint8_t Axel_addressExists() {
  uint8_t dev = 0x00;
  I2C_readBytes(devAddr, AXEL_DEVID, 1, &dev);
  if (dev == AXEL_DEV) return 1;
  return 0;
}

void Axel_configure(void) {
  uint8_t config = AXEL_NOTEST;
  config |= AXEL_NOSPI;
  config |= AXEL_NOINTINVERT;
  config |= AXEL_FULLRES;
  config |= AXEL_NOJUSTIFY;
  config |= AXEL_RANGE16G;
  I2C_writeReg(devAddr, AXEL_DATAF, config);
}

void Axel_powerOn(void) {
  uint8_t config = AXEL_NOAUTOSLEEP;
  config |= AXEL_MEASURE;
  config |= AXEL_NOSLEEP;
  config |= AXEL_WAKEUP8;
  I2C_writeReg(devAddr, AXEL_POWERCTL, config);
}

void Axel_readXYZ(float *x, float *y, float *z) {
  int16_t X, Y, Z;
  Axel_readRaw(&X, &Y, &Z);
  *x = X * 0.03828125f;
  *y = Y * 0.03828125f;
  *z = Z * 0.03828125f;
}

void Axel_readRaw(int16_t *x, int16_t *y, int16_t *z) {
  uint8_t axisBuff[6];
  I2C_readBytes(devAddr, 0x32, 6, axisBuff);
  *x = (((int)axisBuff[1]) << 8) | axisBuff[0];
  *y = (((int)axisBuff[3]) << 8) | axisBuff[2];
  *z = (((int)axisBuff[5]) << 8) | axisBuff[4];
}
