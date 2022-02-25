#include "adxl345.hpp"

uint8_t ADXL_gyro::initI2C(uint32_t f_cpu, uint32_t f_scl) {
  if (this->inited()) return 2;
  I2C_interface::init(f_cpu, f_scl);
  if (this->inited()) return 1;
  return 0;
}

uint8_t ADXL_gyro::addressExists() {
  uint8_t dev = 0x00;
  I2C_interface::readReg(_devAddr, AXEL_DEVID, 1, &dev);
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
  I2C_interface::writeReg(_devAddr, AXEL_DATAF, config);
}

void ADXL_gyro::powerOn(void) {
  uint8_t config = AXEL_NOAUTOSLEEP;
  config |= AXEL_MEASURE;
  config |= AXEL_NOSLEEP;
  config |= AXEL_WAKEUP8;
  I2C_interface::writeReg(_devAddr, AXEL_POWERCTL, config);
}

uint8_t ADXL_gyro::begin(uint8_t addr) {
  _devAddr = addr;
  if (!(this->addressExists())) return 0;
  I2C_interface::writeReg(_devAddr, AXEL_BW_RATE_REG, AXEL_BW_RATE_25);
  this->configure();
  this->powerOn();
  return 1;
}

void ADXL_gyro::readXYZ(float *x, float *y, float *z) {
  int16_t X, Y, Z;
  this->readRawData(&X, &Y, &Z);
  *x = X * 0.03828125f;
  *y = Y * 0.03828125f;
  *z = Z * 0.03828125f;
}

void ADXL_gyro::readRawData(int16_t *x, int16_t *y, int16_t *z) {
  uint8_t axisBuff[6];
  I2C_interface::readReg(_devAddr, 0x32, 6, axisBuff);
  *x = (((int)axisBuff[1]) << 8) | axisBuff[0];
  *y = (((int)axisBuff[3]) << 8) | axisBuff[2];
  *z = (((int)axisBuff[5]) << 8) | axisBuff[4];
}

