#include "bmp280.h"

uint8_t BMP_devAddr;
uint16_t t1, p1;
int16_t t2, t3, p2, p3, p4, p5, p6, p7, p8, p9;
int32_t t_fine;

uint8_t BMP_init(void);

void BMP_writeReg(uint8_t addr, uint8_t data) {
  I2C_writeReg(BMP_devAddr, addr, data);
}

uint8_t BMP_readReg1B(uint8_t addr) {
  uint8_t data;
  I2C_readBytes(BMP_devAddr, addr, 1, &data);
  return data;
}

uint16_t BMP_readReg2B(uint8_t addr) {
  uint8_t buff[2];
  I2C_readBytes(BMP_devAddr, addr, 2, buff);
  return ((uint16_t)((buff[0] << 8) | buff[1]));
}

uint16_t BMP_readReg2BLE(uint8_t reg) {
  uint16_t buff = BMP_readReg2B(reg);
  return (buff >> 8) | (buff << 8);
}

int16_t BMP_readRegS2BLE(uint8_t reg) {
  return (int16_t)BMP_readReg2BLE(reg);
}

uint32_t BMP_readReg3B(uint8_t addr) {
  uint8_t buff[3];
  uint32_t temp;
  I2C_readBytes(BMP_devAddr, addr, 3, buff);
  temp = buff[0];
  temp <<= 8;
  temp |= buff[1];
  temp <<= 8;
  temp |= buff[2];
  return temp;
}

void BMP_readCalData(void) {
  t1 = BMP_readReg2BLE(BMP_T1REG);
  t2 = BMP_readRegS2BLE(BMP_T2REG);
  t3 = BMP_readRegS2BLE(BMP_T3REG);
  p1 = BMP_readReg2BLE(BMP_P1REG);
  p2 = BMP_readRegS2BLE(BMP_P2REG);
  p3 = BMP_readRegS2BLE(BMP_P3REG);
  p4 = BMP_readRegS2BLE(BMP_P4REG);
  p5 = BMP_readRegS2BLE(BMP_P5REG);
  p6 = BMP_readRegS2BLE(BMP_P6REG);
  p7 = BMP_readRegS2BLE(BMP_P7REG);
  p8 = BMP_readRegS2BLE(BMP_P8REG);
  p9 = BMP_readRegS2BLE(BMP_P9REG);
}

uint8_t BMP_begin(uint8_t addr) {
  BMP_devAddr = addr;
  if (BMP_readReg1B(BMP_CHIPIDREG) != BMP_CHIPIDVAL) return 0;
  BMP_readCalData();
  BMP_writeReg(BMP_CFGREG, BMP_CFG);
  return 1;
}

int32_t BMP_readTemp(void) {
  int32_t dig, dec;
  int32_t aTemp, temp;
  aTemp = (BMP_readReg3B(BMP_TEMPDATAREG) >> 4);
  dig = ((((aTemp >> 3) - ((int32_t)t1 << 1))) * ((int32_t)t2)) >> 11;
  dec = (((((aTemp >> 4) - ((int32_t)t1)) * ((aTemp >> 4) - ((int32_t)t1))) >> 12) *
         ((int32_t)t3)) >> 14;
  t_fine = dig + dec;
  temp = ((t_fine) * 5 + 128) >> 8;
  return temp;
}

void BMP_readFloatPress(int32_t *temp, float *press) {
  float d1, d2;
  int32_t aPress;
  *temp = BMP_readTemp();
  aPress = BMP_readReg3B(BMP_PRESDATAREG);
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

float BMP_readAlt(float *press) {
  float temp, alt;
  temp = (float)*press / 101325;
  temp = 1 - pow(temp, 0.19029);
  alt = 44330 * temp;
  return alt;
}

uint8_t BMP_getData(int32_t *temp, float *press, float *alt) {
  BMP_writeReg(BMP_CTRLREG, BMP_MEAS);
  _delay_ms(44);
  BMP_readFloatPress(temp, press);
  *alt = BMP_readAlt(press);
  return 1;
}

