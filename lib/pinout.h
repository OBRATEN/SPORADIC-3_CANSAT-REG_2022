#ifndef PINOUT_H
#define PINOUT_H

#define SPI_P
#define OW_P
#define NRF_P
#define SD_P
#define RGB_P
#define ONCHIPLED_P

#ifdef SPI_P
#define SPI_DDR  DDRB
#define SPI_PORT PORTB
#define CSN      PINB0 // Дефолтный SS с ПЭС роскансата
#define SCK      PINB1
#define MOSI     PINB2
#define MISO     PINB3
#endif // SPI_P

#ifdef OW_P
#define OWPORT PORTD
#define OWDIR  DDRD
#define OWPIN  PIND
#define OWPINN PD7
#endif // OW_P

#if defined NRF_P && defined SPI_P
#define NRF_SS PINB0
#define NRF_CE PINB4
#define NSS_low()  PORTB |= (1 << NRF_SS);
#define NSS_high() PORTB &= ~(1 << NRF_SS);
#define NCE_high() PORTB |= (1 << NRF_CE);
#define NCE_low()  PORTB &= ~(1 << NRF_CE);
#endif // NRF_P & SPI_P

#if defined SD_P && defined SPI_P
#define SD_SS PINC1
#define SDSS_DDR DDRC
#define SDSS_high()  PORTC &= ~(1 << PINC1);
#define SDSS_low()   PORTC |=  (1 << PINC1);
#endif // SD_P & SPI_P

#ifdef RGB_P
#define RGB_DDR   DDRA
#define RGB_PORT  PORTA
#define RGB_RED   PINA4
#define RGB_GREEN PINA3
#define RGB_BLUE  PINA5
#endif // RGB_P

#ifdef ONCHIPLED_P
#define ONCHIPLED_PIN  PING3
#define ONCHIPLED_DDR  DDRG
#define ONCHIPLED_PORT PORTG
#endif // ONCHIPLED_P

#endif // PINOUT_H