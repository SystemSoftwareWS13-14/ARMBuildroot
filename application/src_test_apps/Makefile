CROSS_COMPILE=/opt/toolchains/Sourcery-CodeBench-ARM-2013.05/bin/arm-none-linux-gnueabi-

CC=$(CROSS_COMPILE)gcc
CFLAGS=-Wall -g

INSTALL_DIR=../../rootfs_files/usr/bin_testapps
BIN_DIR=../bin

SOURCES=$(wildcard *.c)
BIN_FILES=$(patsubst %.c,$(BIN_DIR)/%,$(SOURCES))

RM=rm -rf
MKDIR=mkdir -p
CP=cp

.PHONY: all clean install

all: $(BIN_DIR) $(BIN_FILES)

install: all $(INSTALL_DIR)
	$(CP) $(BIN_FILES) $(INSTALL_DIR)

$(BIN_DIR)/access_multi: access_multi.c
	$(CC) $(CFLAGS) -pthread $^ -o $@

$(BIN_DIR)/%: %.c
	$(CC) $(CFLAGS) $^ -o $@

$(BIN_DIR):
	$(MKDIR) $(BIN_DIR)

$(INSTALL_DIR):
	$(MKDIR) $(INSTALL_DIR)

clean:
	$(RM) $(BIN_FILES)
