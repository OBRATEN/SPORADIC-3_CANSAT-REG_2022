#ifndef ADC_h
#define ADC_h

#include <avr/io.h>

class ADC_self {
public:
  ADC_self(void);
  uint8_t getData(uint8_t pin);
  bool getCapacity(void);

private:
  bool m_capacity;
};

#endif
