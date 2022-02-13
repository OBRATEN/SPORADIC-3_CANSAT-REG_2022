#include "adxl345.h"

uint8_t ADXL_self::addressExists() {
  uint8_t dev = 0x00;
  this->readReg(devAddr, AXEL_DEVID, 1, &dev);
  if (dev == AXEL_DEV) return 1;
  return 0;
}

void ADXL_self::configure(void) {
  uint8_t config = AXEL_NOTEST;
  config |= AXEL_NOSPI;
  config |= AXEL_NOINTINVERT;
  config |= AXEL_FULLRES;
  config |= AXEL_NOJUSTIFY;
  config |= AXEL_RANGE16G;
  this->writeReg(devAddr, AXEL_DATAF, config);
}

void ADXL_self::powerOn(void) {
  uint8_t config = AXEL_NOAUTOSLEEP;
  config |= AXEL_MEASURE;
  config |= AXEL_NOSLEEP;
  config |= AXEL_WAKEUP8;
  this->writeReg(devAddr, AXEL_POWERCTL, config);
}

uint8_t ADXL_self::begin(uint8_t addr) {
  if (!(this->addressExists())) return 0;
  devAddr = addr;
  this->writeReg(devAddr, AXEL_BW_RATE_REG, AXEL_BW_RATE_25);
  this->configure();
  this->powerOn();
  return 1;
}

void ADXL_self::readXYZ(float *x, float *y, float *z) {
  int16_t X, Y, Z;
  this->readRawData(&X, &Y, &Z);
  *x = X * 0.03828125f;
  *y = Y * 0.03828125f;
  *z = Z * 0.03828125f;
}

void ADXL_self::readRawData(int16_t *x, int16_t *y, int16_t *z) {
  uint8_t axisBuff[6];
  this->readReg(devAddr, 0x32, 6, axisBuff);
  *x = (((int)axisBuff[1]) << 8) | axisBuff[0];
  *y = (((int)axisBuff[3]) << 8) | axisBuff[2];
  *z = (((int)axisBuff[5]) << 8) | axisBuff[4];
}

