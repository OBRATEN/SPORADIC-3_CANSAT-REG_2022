#include "bmp280.hpp"

BMP_press::BMP_press(void) {
  _i2c = I2C_interface();
}

void BMP_press::writeReg(uint8_t addr, uint8_t data) {
  _i2c.writeReg(devAddr, addr, data);
}

uint8_t BMP_press::readReg(uint8_t addr) {
  uint8_t data;
  _i2c.readReg(devAddr, addr, 1, &data);
  return data;
}

int32_t BMP_press::readReg24b(uint8_t addr) {
  uint32_t result;
  uint8_t buffer[3];
  _i2c.readReg(devAddr, addr, 3, buffer);
  result = buffer[0];
  result <<= 8;
  result |= buffer[1];
  result <<= 8;
  result |= buffer[2];
  return result;
}

uint16_t BMP_press::read16_LE(uint8_t reg) {
  uint16_t temp = _i2c.readReg(devAddr, reg, 2);
  return (temp >> 8) | (temp << 8);
}

void BMP_press::readCalData(void) {
  t1 = (uint16_t) _i2c.readReg(0x77, 0x88, 2);
  t2 = _i2c.readReg(0x77, 0x8A, 2);
  t3 = _i2c.readReg(0x77, 0x8C, 2);
  p1 = (uint16_t) _i2c.readReg(0x77, 0x8E, 2);
  p2 = _i2c.readReg(0x77, 0x90, 2);
  p3 = _i2c.readReg(0x77, 0x92, 2);
  p4 = _i2c.readReg(0x77, 0x94, 2);
  p5 = _i2c.readReg(0x77, 0x96, 2);
  p6 = _i2c.readReg(0x77, 0x98, 2);
  p7 = _i2c.readReg(0x77, 0x9A, 2);
  p8 = _i2c.readReg(0x77, 0x9C, 2);
  p9 = _i2c.readReg(0x77, 0x9E, 2);
}

uint8_t BMP_press::begin(uint8_t addr) {
  devAddr = addr;
  if (readReg(BMP_CHIPIDREG) != BMP_CHIPIDVAL) return 0;
  readCalData();
  writeReg(BMP_CFGREG, BMP_CFG);
  writeReg(BMP_CTRLREG, 0xFD);
  _delay_ms(44);
  return 1;
}

double BMP_press::readTemp(void) {
  double dig, dec, temp;
  int32_t aTemp;
  aTemp = (readReg24b(BMP_TEMPDATAREG) >> 4);
  dig = (((double)aTemp)/16384.0-((double)t1)/1024.0)*((double)t2);
  dec = ((((double)aTemp)/131072.0-((double)t1)/8192.0)*(((double)aTemp)/131072.0-((double)t1)/8192.0))*((double)t3);
  t_fine = dig + dec;
  temp = (dig + dec) / 5120.0;
  return temp;
}

void BMP_press::readFloatPress(double *temp, double *press) {
  int32_t aPress;
  *temp = readTemp();
  aPress = readReg24b(BMP_PRESDATAREG) >> 4;
  double var1 = ((double)t_fine/2.0) - 64000.0;
  double var2 = var1*var1*((double)p6)/32768.0;
  var2 = var2 + var1*((double)p5)*2.0;
  var2 = (var2/4.0) + (((double)p4)*65536.0);
  var1 = (((double)p3)*var1*var1/524288.0 + ((double)p2)*var1)/524288.0;
  var1 = (1.0 + var1/32768.0)*((double)p1);
  double p = 1048576.0 - (double)aPress;
  p = (p - (var2/4096.0))*6250.0/var1;
  var1 = ((double)p9)*p*p/2147483648.0;
  var2 = p*((double)p8)/32768.0;
  p = p + (var1 + var2 + ((double)p7))/16.0;
  *press = p;
}

void BMP_press::readAlt(double *press, double *altR) {
  double temp, alt;
  temp = (double)*press / 101325;
  temp = 1 - pow(temp, 0.19029);
  alt = 44330 * temp;
  *altR = alt;
}

uint8_t BMP_press::getData(double *temp, double *press, double *alt) {
  readFloatPress(temp, press);
  readAlt(press, alt);
  _i2c.writeReg(devAddr, BMP_CTRLREG, 0xFD);
  return 1;
}
