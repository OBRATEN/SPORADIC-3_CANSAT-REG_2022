#include "bn220.hpp"

void BN220_gps::parse(char *data, uint16_t size) {
	for (uint16_t i = 0; i < size; i++) {
		if (data[i] == '$') {
			parserState = 0;
			continue;
		}
		if (parserState == -1) continue;
		if (parserState == 0) {
			if (data[i] == ',') {
				if (gpsTitleBuf[0] == 'G' && (gpsTitleBuf[1] == 'N' || gpsTitleBuf[1] == 'P') && gpsTitleBuf[2] == 'G' && gpsTitleBuf[3] == 'G' && gpsTitleBuf[4] == 'A') {
					parserState = 1;
					dataIn = 1;
					continue;
				}
				parserState = -1;
				continue;
			}
			gpsTitleBuf[0] = gpsTitleBuf[1];
			gpsTitleBuf[1] = gpsTitleBuf[2];
			gpsTitleBuf[2] = gpsTitleBuf[3];
			gpsTitleBuf[3] = gpsTitleBuf[4];
			gpsTitleBuf[4] = data[i];
			continue;
		}
	}
}

void BN220_gps::getData(char data[], uint8_t size) {
	for (uint8_t i = 0; i < size; i++) data[i] = gpsTitleBuf[i];
}