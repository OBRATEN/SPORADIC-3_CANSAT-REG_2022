CPPC = avr-g++
DEVICE = m128
DEVICEID = atmega128a
CFLAGS = -g -Wall -Os -mmcu=$(DEVICEID)
PRINTFLAGS = -Wl,-u,vfprintf -lprintf_flt -lm
CLIBS = -I lib/
CCODE = lib/i2c.cpp lib/onewire.cpp lib/uart.c lib/spi.cpp lib/sd_util.cpp lib/fat.cpp lib/timer.c lib/adxl345.cpp lib/ds18b20.cpp lib/bmp280.cpp lib/nrf24l01.cpp
PROGRAMMER = usbasp
UPLOADER = avrdude

TARGET = main

default: cppcompile upload clean

cppcompile:
	$(CPPC) $(CFLAGS) $(CLIBS) src/$(TARGET).cpp ${CCODE} -o build/$(TARGET).elf
	avr-objcopy -j .text -j .data -O ihex build/$(TARGET).elf build/$(TARGET).hex
	avr-size --format=avr --mcu=$(DEVICE) build/$(TARGET).elf
upload:
	$(UPLOADER) -v -p $(DEVICE) -c $(PROGRAMMER) -U lfuse:w:0xc4:m -U hfuse:w:0x99:m -U efuse:w:0xff:m -U flash:w:build/$(TARGET).hex:i 
clean:
	rm build/$(TARGET).elf
	rm build/$(TARGET).hex
