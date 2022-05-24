#include <iostream>
#include <string.h>

using namespace std;

struct txData {
  uint8_t prefix = 0;
  int16_t aX = 0;
  int16_t aY = 0;
  int16_t aZ = 0;
  int16_t DStemp = 0;
  int16_t BMPtemp = 0;
  int32_t press = 0;
  int16_t alt = 0;
  uint32_t packageID = 0;
  uint32_t gTime = 0;
  uint8_t status = 0;
} tx_data;

int main() {
  cout << sizeof(tx_data) << endl;
  uint8_t data[26] = {0x00, 0x8b, 0x03, 0xe6, 0xff, 0x5b, 0x00, 0x37, 0x21, 0xc0, 0x09, 0x18, 0x7d, 0x01, 0x00, 0x4b, 0x7c, 0x04, 0x00, 0x00, 0x00, 0x31, 0x00, 0x00, 0x00, 0xc1};
  memcpy(&tx_data, data, sizeof(tx_data));
  cout << sizeof(data) << endl;
  cout << tx_data.aX << endl;
}