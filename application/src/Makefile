CROSS_COMPILE=/opt/toolchains/Sourcery-CodeBench-ARM-2013.05/bin/arm-none-linux-gnueabi-

CC=$(CROSS_COMPILE)gcc
CFLAGS=-Wall -g

INSTALL_DIR=../../rootfs_files/usr/bin
BIN_DIR=../bin

OUT=show_uptime
SOURCE=$(OUT).c

OUTFILE_DYN=$(OUT)_dynamic
OUTFILE_STAT=$(OUT)_static
BIN_OUTFILE_DYN=$(BIN_DIR)/$(OUTFILE_DYN)
BIN_OUTFILE_STAT=$(BIN_DIR)/$(OUTFILE_STAT)

RM=rm -rf
MKDIR=mkdir -p
CP=cp

.PHONY: all clean install

all: $(BIN_DIR) $(BIN_OUTFILE_STAT) $(BIN_OUTFILE_DYN)

install: all $(INSTALL_DIR)
	$(CP) $(BIN_OUTFILE_STAT) $(BIN_OUTFILE_DYN) $(INSTALL_DIR)

$(BIN_OUTFILE_STAT): $(SOURCE)
	$(CC) $(CFLAGS) -static $^ -o $@

$(BIN_OUTFILE_DYN): $(SOURCE)
	$(CC) $(CFLAGS) $^ -o $@

$(BIN_DIR):
	$(MKDIR) $(BIN_DIR)

$(INSTALL_DIR):
	$(MKDIR) $(INSTALL_DIR)

clean:
	$(RM) $(BIN_OUTFILE_DYN) $(BIN_OUTFILE_STAT)
