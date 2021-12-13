CC = avr-g++
OCP = avr-objcopy
CFLAGS = -g -Wall -Os -mmcu=atmega128a
CLIBS = -I lib/
OFLAGS = -j .text -j .data -O ihex
TARGET = main
PROGR = usbasp
UPL = avrdude
MCU = m128

all: $(TARGET)
$(TARGET) : src/$(TARGET).cpp
	$(RM) build/$(TARGET).bin
	$(RM) build/$(TARGET).hex
	$(CC) $(CFLAGS) $(CLIBS) -o build/$(TARGET).bin src/$(TARGET).cpp
	$(OCP) $(OFLAGS) build/$(TARGET).bin build/$(TARGET).hex
clean: $(TARGET)
$(TARGET) : src/$(TARGET).cpp
	$(RM) build/$(TARGET).bin
upload: $(TARGET)
$(TARGET) : src/$(TARGET).cpp
	$(RM) build/$(TARGET).bin
	$(RM) build/$(TARGET).hex
	$(CC) $(CFLAGS) $(CLIBS) -o build/$(TARGET).bin src/$(TARGET).cpp
	$(OCP) $(OFLAGS) build/$(TARGET).bin build/$(TARGET).hex
	$(UPL) -c $(PROGR) -p $(MCU) -U flash:w:build/$(TARGET).hex:a
