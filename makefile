CC = gcc
CFLAGS = -Wall -Wextra -O2

SRC_DIR = src
BUILD_DIR = build

COMMON_SRCS = $(SRC_DIR)/barcode.c $(SRC_DIR)/pbm.c
ENCODE_SRC  = $(SRC_DIR)/main_encode.c
DECODE_SRC  = $(SRC_DIR)/main_decode.c

ENCODE_EXE = $(BUILD_DIR)/encode.exe
DECODE_EXE = $(BUILD_DIR)/decode.exe

all: $(ENCODE_EXE) $(DECODE_EXE)

$(ENCODE_EXE): $(ENCODE_SRC) $(COMMON_SRCS)
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(ENCODE_EXE) $(ENCODE_SRC) $(COMMON_SRCS)

$(DECODE_EXE): $(DECODE_SRC) $(COMMON_SRCS)
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(DECODE_EXE) $(DECODE_SRC) $(COMMON_SRCS)

clean:
	@if exist $(BUILD_DIR) rmdir /s /q $(BUILD_DIR)

rebuild: clean all

.PHONY: all clean rebuild