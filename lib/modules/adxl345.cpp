#include "adxl345.h"

ADXL345_self::ADXL345_self(void) {
	i2c.init(F_CPU, F_SCL);
}

uint8_t ADXL345_self::begin(uint8_t daddr) {
	devAdress = daddr;
	if (!(adressExists())) return 0;
	i2c.writeReg(devAdress, ADXL345_BW_RATE_REG, ADXL345_BW_RATE_25);
	ADXL345_self::configurate();
	ADXL345_self::powerOn();
	return 1;
}

uint8_t ADXL345_self::adressExists(void) {
	uint8_t device = 0x00;
	i2c.readBytes(devAdress, ADXL345_DEVID_REG, 1, &device);
	if (device == ADXL345_DEVICE) return 1;
	else return 0;
	}

void ADXL345_self::configurate(void) {
	uint8_t config = ADXL345_SELF_TEST_OFF;
	config |= ADXL345_SPI_OFF;
	config |= ADXL345_INT_INVERT_OFF;
	config |= ADXL345_FULL_RES_ON;
	config |= ADXL345_JUSTIFY_OFF;
	config |= ADXL345_RANGE_16G;
	i2c.writeReg(devAdress, ADXL345_DATA_FORMAT_REG, config);
}

void ADXL345_self::powerOn(void) {
	uint8_t config = ADXL345_AUTO_SLEEP_OFF;
	config |= ADXL345_MEASURE_ON;
	config |= ADXL345_SLEEP_OFF;
	config |= ADXL345_WAKEUP_8HZ;
	i2c.writeReg(devAdress, ADXL345_POWER_CTL_REG, config);
}

void ADXL345_self::readXYZ(float *x, float *y, float *z) {
	int16_t X, Y, Z;
	ADXL345_self::readRaw(&X, &Y, &Z);
	*x = X * 0.03828125f;
	*y = Y * 0.03828125f;
	*z = Z * 0.03828125f;
}

void ADXL345_self::readRaw(int16_t *x, int16_t *y, int16_t *z) {
	uint8_t axisBuff[6];
	i2c.readBytes(devAdress, 0x32, 6, axisBuff);
	*x = (((int)axisBuff[1]) << 8) | axisBuff[0];
	*y = (((int)axisBuff[3]) << 8) | axisBuff[2];
	*z = (((int)axisBuff[5]) << 8) | axisBuff[4];
}
