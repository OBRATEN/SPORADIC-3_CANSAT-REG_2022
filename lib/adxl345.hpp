#ifndef ADXL345_H
#define ADXL345_H

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#ifndef F_SCL
#define F_SCL 100000
#endif

#define AXEL_DEF_ADDR 0x1d
#define AXEL_ALT_ADDR 0x53
#define AXEL_DEVID 0x00
#define AXEL_DEV 0xe5

#define AXEL_BW_RATE_REG 0x2C
#define AXEL_BW_RATE_100 0b1010
#define AXEL_BW_RATE_50 0b1001
#define AXEL_BW_RATE_25 0b1000
#define AXEL_THRESH_TAP 0x1D
#define AXEL_DUR 0x21
#define AXEL_LATENT 0x22
#define AXEL_WINDOW 0x23
#define AXEL_INTMAP 0x2F
#define AXEL_INTEN 0x2E
#define AXEL_POWERCTL 0x2D
#define AXEL_TAPSTAT 0x2B
#define AXEL_TAPAXES 0x2A
#define AXEL_INTSRC 0x30
#define AXEL_DATAF 0x31

#define AXEL_TEST 0b10000000
#define AXEL_NOTEST 0b00000000
#define AXEL_SPI 0b01000000
#define AXEL_NOSPI 0b00000000
#define AXEL_INTINVERT 0b00100000
#define AXEL_NOINTINVERT 0b00000000
#define AXEL_FULLRES 0b00001000
#define AXEL_NOFULLRES 0b00000000
#define AXEL_JUSTIFY 0b00000100
#define AXEL_NOJUSTIFY 0b00000000

#define AXEL_TAPAXESX 0b00000100
#define AXEL_INTSINGLETAP 0b01000000
#define AXEL_INTDOUBLETAP 0b00100000

#define AXEL_RANGE2G 0b00
#define AXEL_RANGE4G 0b01
#define AXEL_RANGE8G 0b10
#define AXEL_RANGE16G 0b11

#define AXEL_AUTOSLEEP 0b00010000
#define AXEL_NOAUTOSLEEP 0b00000000
#define AXEL_MEASURE 0b00001000
#define AXEL_NOMEASURE 0b00000000
#define AXEL_SLEEP 0b00000100
#define AXEL_NOSLEEP 0b00000000

#define AXEL_WAKEUP8 0b00
#define AXEL_WAKEUP4 0b01
#define AXEL_WAKEUP2 0b10
#define AXEL_WAKEUP1 0b11

#include "i2c.hpp"

class ADXL_gyro : public I2C_interface {
public:
	uint8_t initI2C(uint32_t f_cpu, uint32_t f_scl);
  uint8_t begin(uint8_t addr); 
  uint8_t addressExists();
  void configure(void);
  void powerOn(void);
  void readXYZ(float *x, float *y, float *z);
  void readRawData(int16_t *x, int16_t *y, int16_t *z);
  void changeDevAddr(uint8_t addr);
private:
  uint8_t _devAddr;
};

#endif
