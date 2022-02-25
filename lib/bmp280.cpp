#include "bmp280.hpp"

void BMP_press::writeReg1B(uint8_t addr, uint8_t data) {
  I2C_interface::writeReg(devAddr, addr, data);
}

uint8_t BMP_press::readReg1B(uint8_t addr) {
  uint8_t data;
  I2C_interface::readReg(devAddr, addr, 1, &data);
  return data;
}

uint16_t BMP_press::readReg2B(uint8_t addr) {
  uint8_t buff[2];
  I2C_interface::readReg(devAddr, addr, 2, buff);
  return ((uint16_t)((buff[0] << 8) | buff[1]));
}

uint16_t BMP_press::readReg2BLE(uint8_t reg) {
  uint16_t buff = this->readReg2B(reg);
  return (buff >> 8) | (buff << 8);
}

int16_t BMP_press::readRegS2BLE(uint8_t reg) {
  return (int16_t)this->readReg2BLE(reg);
}

uint32_t BMP_press::readReg3B(uint8_t addr) {
  uint8_t buff[3];
  uint32_t temp;
  I2C_interface::readReg(devAddr, addr, 3, buff);
  temp = buff[0];
  temp <<= 8;
  temp |= buff[1];
  temp <<= 8;
  temp |= buff[2];
  return temp;
}

void BMP_press::readCalData(void) {
  t1 = this->readReg2BLE(BMP_T1REG);
  t2 = this->readRegS2BLE(BMP_T2REG);
  t3 = this->readRegS2BLE(BMP_T3REG);
  p1 = this->readReg2BLE(BMP_P1REG);
  p2 = this->readRegS2BLE(BMP_P2REG);
  p3 = this->readRegS2BLE(BMP_P3REG);
  p4 = this->readRegS2BLE(BMP_P4REG);
  p5 = this->readRegS2BLE(BMP_P5REG);
  p6 = this->readRegS2BLE(BMP_P6REG);
  p7 = this->readRegS2BLE(BMP_P7REG);
  p8 = this->readRegS2BLE(BMP_P8REG);
  p9 = this->readRegS2BLE(BMP_P9REG);
}

uint8_t BMP_press::begin(uint8_t addr) {
  devAddr = addr;
  if (this->readReg1B(BMP_CHIPIDREG) != BMP_CHIPIDVAL) return 0;
  this->readCalData();
  this->writeReg1B(BMP_CFGREG, BMP_CFG);
  return 1;
}

int32_t BMP_press::readTemp(void) {
  int32_t dig, dec;
  int32_t aTemp, temp;
  aTemp = (this->readReg3B(BMP_TEMPDATAREG) >> 4);
  dig = ((((aTemp >> 3) - ((int32_t)t1 << 1))) * ((int32_t)t2)) >> 11;
  dec = (((((aTemp >> 4) - ((int32_t)t1)) * ((aTemp >> 4) - ((int32_t)t1))) >> 12) *
         ((int32_t)t3)) >> 14;
  t_fine = dig + dec;
  temp = ((t_fine) * 5 + 128) >> 8;
  return temp;
}

void BMP_press::readFloatPress(int32_t *temp, float *press) {
  float d1, d2;
  int32_t aPress;
  *temp = this->readTemp();
  aPress = this->readReg3B(BMP_PRESDATAREG);
  aPress >>= 4;
  d1 = ((float)t_fine / 2.0) - 64000.0;
  d2 = d1 * d1 * ((float)p6) / 32768.0;
  d2 = d2 + d1 * ((float)p5) * 2.0;
  d2 = (d2 / 4.0) + (((float)p4) * 65536.0);
  d1 = (((float)p3) * d1 * d1 / 524288.0 + ((float)p2) * d1) / 524288.0;
  d1 = (1.0 + d1 / 32768.0) * ((float)p1);
  if (d1 == 0.0) {
    *press = 0;
    return;
  }
  *press = 1048576.0 - (float)aPress;
  *press = (*press - (d2 / 4096.0)) * 6250.0 / d1;
  d1 = ((float)p9) * (*press) * (*press) / 2147483648.0;
  d2 = *press * ((float)p8) / 32768.0;
  *press = *press + (d1 + d2 + ((float)p7)) / 16.0;
  return;
}

float BMP_press::readAlt(float *press) {
  float temp, alt;
  temp = (float)*press / 101325;
  temp = 1 - pow(temp, 0.19029);
  alt = 44330 * temp;
  return alt;
}

uint8_t BMP_press::getData(int32_t *temp, float *press, float *alt) {
  this->writeReg1B(BMP_CTRLREG, BMP_MEAS);
  _delay_ms(44);
  this->readFloatPress(temp, press);
  *alt = this->readAlt(press);
  return 1;
}

