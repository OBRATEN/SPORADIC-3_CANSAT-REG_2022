#include "adxl345.hpp"

ADXL_gyro::ADXL_gyro(void) {
  _i2c = I2C_interface();
}

uint8_t ADXL_gyro::addressExists() {
  uint8_t dev = 0x00;
  _i2c.readReg(_devAddr, AXEL_DEVID, 1, &dev);
  if (dev == AXEL_DEV) return 1;
  return 0;
}

void ADXL_gyro::configure(void) {
  uint8_t config = AXEL_NOTEST;
  config |= AXEL_NOSPI;
  config |= AXEL_NOINTINVERT;
  config |= AXEL_FULLRES;
  config |= AXEL_NOJUSTIFY;
  config |= AXEL_RANGE16G;
  _i2c.writeReg(_devAddr, AXEL_DATAF, config);
}

void ADXL_gyro::powerOn(void) {
  uint8_t config = AXEL_NOAUTOSLEEP;
  config |= AXEL_MEASURE;
  config |= AXEL_NOSLEEP;
  config |= AXEL_WAKEUP8;
  //_i2c.writeReg(_devAddr, AXEL_POWERCTL, config);
  _i2c.writeReg(_devAddr, 0x2D, 0x08);
}

uint8_t ADXL_gyro::begin(uint8_t addr) {
  _devAddr = addr;
  if (!(addressExists())) return 0;
  _i2c.writeReg(_devAddr, AXEL_BW_RATE_REG, AXEL_BW_RATE_25);
  configure();
  powerOn();
  return 1;
}

void ADXL_gyro::readXYZ(float *x, float *y, float *z) {
  int16_t X, Y, Z;
  readRawData(&X, &Y, &Z);
  *x = X * 0.03828125f;
  *y = Y * 0.03828125f;
  *z = Z * 0.03828125f;
  _i2c.writeReg(_devAddr, 0x2D, 0x08);
}

void ADXL_gyro::readRawData(int16_t *x, int16_t *y, int16_t *z) {
  *x = _i2c.readReg(_devAddr, 0x32, 2);
  *y = _i2c.readReg(_devAddr, 0x34, 2);
  *z = _i2c.readReg(_devAddr, 0x36, 2);
}
