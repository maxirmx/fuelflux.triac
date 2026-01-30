# Makefile for 2-CH SCR HAT - Orange Pi Zero 2W / Ubuntu
#
# Build: make
# Clean: make clean
# Run:   sudo ./main

DIR_BIN = ./bin
DIR_Config = ./lib/Config
DIR_Examples = ./examples
DIR_Drive = ./lib/2-CH_SCR_HAT

OBJ_C = $(wildcard ${DIR_Examples}/*.c ${DIR_Config}/*.c ${DIR_Drive}/*.c)
OBJ_O = $(patsubst %.c,${DIR_BIN}/%.o,$(notdir ${OBJ_C}))

TARGET = main

CC = gcc
CFLAGS = -g -O2 -Wall
LIB = -lm -lpthread

# Create bin directory
$(shell mkdir -p $(DIR_BIN))

${TARGET}: ${OBJ_O}
	$(CC) $(CFLAGS) $(OBJ_O) -o $@ $(LIB)
	@echo "Build complete: $(TARGET)"
	@echo "Run with: sudo ./$(TARGET)"

${DIR_BIN}/%.o: $(DIR_Examples)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ -I $(DIR_Config) -I $(DIR_Drive)

${DIR_BIN}/%.o: $(DIR_Drive)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ -I $(DIR_Config)

${DIR_BIN}/%.o: $(DIR_Config)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(DIR_BIN)
	rm -f $(TARGET)

.PHONY: clean
