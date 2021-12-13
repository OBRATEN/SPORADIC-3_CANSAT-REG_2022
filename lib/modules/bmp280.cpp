#include "bmp280.h"
#include <util/delay.h>

I2C_self i2c(8000000, 100000);

uint8_t BMP280_dev_addr;

int32_t t_fine;

uint16_t dig_T1;
int16_t  dig_T2;
int16_t  dig_T3;

uint16_t dig_P1;
int16_t  dig_P2;
int16_t  dig_P3;
int16_t  dig_P4;
int16_t  dig_P5;
int16_t  dig_P6;
int16_t  dig_P7;
int16_t  dig_P8;
int16_t  dig_P9;

void BMP280_self::write8(uint8_t address, uint8_t data) {
	i2c.writeReg(BMP280_dev_addr, address, data);
}

uint8_t BMP280_self::read8(uint8_t address) {
	uint8_t BMP280_buffer;
	i2c.readBytes(BMP280_dev_addr, address, 1, &BMP280_buffer);
  return BMP280_buffer;
}

uint16_t BMP280_self::read16(uint8_t address) {
  uint8_t BMP280_buffer[2];
  i2c.readBytes(BMP280_dev_addr, address, 2, BMP280_buffer);
  return ((uint16_t) ((BMP280_buffer[0]<<8)|BMP280_buffer[1]));    
}

uint32_t BMP280_self::read24(uint8_t address) {
  uint32_t ret_value;
  uint8_t BMP280_buffer[3];
  i2c.readBytes(BMP280_dev_addr, address, 3, BMP280_buffer);
  //ret_value = (uint32_t) ((BMP280_buffer[0]<<16)|(BMP280_buffer[1]<<8)|BMP280_buffer[2]);
  ret_value = BMP280_buffer[0];
  ret_value <<=8;
  ret_value |= BMP280_buffer[1];
  ret_value <<=8;
  ret_value |= BMP280_buffer[2];
  return ret_value;
}

uint16_t BMP280_self::read16_LE(uint8_t reg) {
  uint16_t temp = BMP280_self::read16(reg);
  return (temp >> 8) | (temp << 8);
}

int16_t BMP280_self::readS16_LE(uint8_t reg) {
  return (int16_t)BMP280_self::read16_LE(reg);
}

void BMP280_self::readCoefficients(void) {
    dig_T1 = BMP280_self::read16_LE(BMP280_REGISTER_DIG_T1);
    dig_T2 = BMP280_self::readS16_LE(BMP280_REGISTER_DIG_T2);
    dig_T3 = BMP280_self::readS16_LE(BMP280_REGISTER_DIG_T3);

    dig_P1 = BMP280_self::read16_LE(BMP280_REGISTER_DIG_P1);
    dig_P2 = BMP280_self::readS16_LE(BMP280_REGISTER_DIG_P2);
    dig_P3 = BMP280_self::readS16_LE(BMP280_REGISTER_DIG_P3);
    dig_P4 = BMP280_self::readS16_LE(BMP280_REGISTER_DIG_P4);
    dig_P5 = BMP280_self::readS16_LE(BMP280_REGISTER_DIG_P5);
    dig_P6 = BMP280_self::readS16_LE(BMP280_REGISTER_DIG_P6);
    dig_P7 = BMP280_self::readS16_LE(BMP280_REGISTER_DIG_P7);
    dig_P8 = BMP280_self::readS16_LE(BMP280_REGISTER_DIG_P8);
    dig_P9 = BMP280_self::readS16_LE(BMP280_REGISTER_DIG_P9);
}

uint8_t BMP280_self::begin(uint8_t daddr) {
  BMP280_dev_addr  = daddr;
  if (BMP280_self::read8(BMP280_REGISTER_CHIPID) != BMP280_CHIPID_VALUE) return 0;
  BMP280_self::readCoefficients();
  BMP280_self::write8(BMP280_REGISTER_CONFIG, BMP280_CONFIG);
  m_capacity = 1;
  return 1;
}

int32_t BMP280_self::readTemperature(void) {
  int32_t var1, var2;
  int32_t adc_T;
  int32_t T;
  adc_T = BMP280_self::read24(BMP280_REGISTER_TEMPDATA);
  adc_T >>= 4;
  var1  = ((((adc_T>>3) - ((int32_t)dig_T1 <<1))) *
       ((int32_t)dig_T2)) >> 11;
  var2  = (((((adc_T>>4) - ((int32_t)dig_T1)) *
         ((adc_T>>4) - ((int32_t)dig_T1))) >> 12) *
       ((int32_t)dig_T3)) >> 14;
  t_fine = var1 + var2;
  T  = (t_fine * 5 + 128) >> 8;
  return T;
}

void BMP280_self::readPressure(int32_t *temp, float *press) {
    float var1, var2;
    int32_t adc_P;
    *temp = BMP280_self::readTemperature();
    adc_P = BMP280_self::read24(BMP280_REGISTER_PRESSUREDATA);
    adc_P >>= 4;
    
    var1 = ((float)t_fine/2.0)-64000.0;
    var2 = var1 * var1 * ((float)dig_P6) / 32768.0;
    var2 = var2 + var1 * ((float)dig_P5) * 2.0;
    var2 = (var2/4.0)+(((float)dig_P4) * 65536.0);
    var1 = (((float)dig_P3) * var1 * var1 / 524288.0 + ((float)dig_P2) * var1) / 524288.0;
    var1 = (1.0 + var1 / 32768.0)*((float)dig_P1);
    if (var1 == 0.0) {
      *press = 0;
      return; // avoid exception caused by division by zero
    }
    *press = 1048576.0-(float)adc_P;
    *press = (*press-(var2 / 4096.0)) * 6250.0 / var1;
    var1 = ((float)dig_P9) * (*press) * (*press) / 2147483648.0;
    var2 = *press * ((float)dig_P8) / 32768.0;
    *press = *press + (var1 + var2 + ((float)dig_P7)) / 16.0;
    return;
}

float BMP280_self::altitude(float *press) {
  float temp;
  float  altitude;
  temp = (float) * press/101325;
  temp = 1-pow(temp, 0.19029);
  altitude = 44330 * temp;
  return altitude;
}

uint8_t BMP280_self::receiveData(int32_t *temp, float *press, float *alt) {
  BMP280_self::write8(BMP280_REGISTER_CONTROL, BMP280_MEAS);
  _delay_ms(44);
  BMP280_self::readPressure(temp, press);
  *alt = BMP280_self::altitude(press);
  return 1;
}

bool BMP280_self::getCapacity(void) {
	return m_capacity;
}