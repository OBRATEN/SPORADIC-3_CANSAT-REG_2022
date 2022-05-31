CPPC = avr-g++
ASMC = avra
DEVICE = m128
WDEVICE = t2313
DEVICEID = atmega128a
CFLAGS = -g -Wall -O1 -mmcu=$(DEVICEID)
PRINTFLAGS = -Wl,-u,vfprintf -lprintf_flt -lm
CLIBS = -I lib/
CCODE = lib/i2c.cpp lib/onewire.cpp lib/uart.c lib/spi.cpp lib/timer.c lib/sd.cpp lib/adxl345.cpp lib/ds18b20.cpp lib/bmp280.cpp lib/nrf24l01.cpp
PROGRAMMER = usbasp
UPLOADER = avrdude

TARGET = main
WATCHDOG = watchdog

default: cppcompile upload
watchdog: asmcompile asmupload

testcompile:
	g++ -o build/test src/test.cpp
asmcompile:
	$(ASMC) src/$(WATCHDOG).asm -o build/$(WATCHDOG).hex
asmupload:
	$(UPLOADER) -v -p $(WDEVICE) -c $(PROGRAMMER) -U lfuse:w:0x46:m -U hfuse:w:0xCF:m -U efuse:w:0xFF:m -U lock:w:0xFF:m -U flash:w:build/$(WATCHDOG).hex:i
cppcompile:
	@echo "Compiling for board $(DEVICEID)..."
	@echo ""
	$(CPPC) $(CFLAGS) $(CLIBS) src/$(TARGET).cpp ${CCODE} -o build/$(TARGET).elf
	@avr-objcopy -j .text -j .data -O ihex build/$(TARGET).elf build/$(TARGET).hex
	@echo ""
	@echo "Compilation for board $(DEVICEID) success. Size data:"
	@echo ""
	@avr-size --format=avr --mcu=atmega128 build/$(TARGET).elf
upload:
	$(UPLOADER) -v -p $(DEVICE) -c $(PROGRAMMER) -U lfuse:w:0xc4:m -U hfuse:w:0x99:m -U efuse:w:0xff:m -U flash:w:build/$(TARGET).hex:i 
clean:
	rm build/$(TARGET).elf
	rm build/$(TARGET).hex