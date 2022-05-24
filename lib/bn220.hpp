#ifndef BN220_H
#define BN220_H

#include "avr/io.h"

class BN220_gps {
private:
	char gpsTitleBuf[5];
	int8_t parserState = -1;
	uint8_t dataIn = 0;
public:
	void parse(char data) {parse(&data, 1);}
	void parse(char *data, uint16_t size);
	void getData(char data[], uint8_t size);
	uint8_t haveData(void) {return dataIn;}
};

#endif // BN220_H