CPPC = avr-g++
CC = avr-gcc
DEVICE = m128
DEVICEID = atmega128a
CFLAGS = -g -Wall -Os -mmcu=$(DEVICEID)
CLIBS = -I lib/
PROGRAMMER = usbasp
UPLOADER = avrdude

TARGET = main

default: ccompile upload clean

ccompile:
	$(CC) $(CFLAGS) -Wl,-u,vfprintf -lprintf_flt -lm $(CLIBS) -o build/$(TARGET).o -c src/$(TARGET).c
	$(CC) $(CFLAGS) -Wl,-u,vfprintf -lprintf_flt -lm -o build/$(TARGET).elf build/$(TARGET).o
	avr-objcopy -j .text -j .data -O ihex build/$(TARGET).elf build/$(TARGET).hex
	avr-size --format=avr --mcu=$(DEVICE) build/$(TARGET).elf
cppcompile:
	$(CPPC) $(CFLAGS) $(CLIBS) -c src/$(TARGET).cpp -o build/$(TARGET).o
	$(CPPC) $(CFLAGS) -o build/$(TARGET).elf build/$(TARGET).o
	avr-objcopy -j .text -j .data -O ihex build/$(TARGET).elf build/$(TARGET).hex
	avr-size --format=avr --mcu=$(DEVICE) build/$(TARGET).elf
upload:
	$(UPLOADER) -v -p $(DEVICE) -c $(PROGRAMMER) -U flash:w:build/$(TARGET).hex:i
clean:
	rm build/$(TARGET).o
	rm build/$(TARGET).elf
	rm build/$(TARGET).hex
