CROSS_COMPILE=/opt/toolchains/Sourcery-CodeBench-ARM-2013.05/bin/arm-none-linux-gnueabi-

CC=$(CROSS_COMPILE)gcc
CFLAGS=-Wall -g

INSTALL_DIR=../../rootfs_files/usr/bin

OUT=show_uptime
OUTFILE_DYN=$(OUT)_dynamic
OUTFILE_STAT=$(OUT)_static
BIN_OUTFILE_DYN=$(BINDIR)/$(OUTFILE_DYN)
BIN_OUTFILE_STAT=$(BINDIR)/$(OUTFILE_STAT)
BINDIR=../bin

OUTFILE_ACCESS=access
SRC_ACCESS=$(OUTFILE_ACCESS).c
BIN_OUTFILE_ACCESS=$(BIN_DIR)/$(OUTFILE_ACCESS)

SOURCE=$(OUT).c
RM=rm -rf
MKDIR=mkdir -p
CP=cp

.PHONY: all manual clean install

all: $(OUTFILE_STAT) $(OUTFILE_DYN) $(OUTFILE_ACCESS)
manual: $(BINDIR) $(BIN_OUTFILE_STAT) $(BIN_OUTFILE_DYN) $(BIN_OUTFILE_ACCESS)

install: all $(INSTALL_DIR)
	$(CP) $(OUTFILE_STAT) $(OUTFILE_DYN) $(OUTFILE_ACCESS) $(INSTALL_DIR)
	
$(INSTALL_DIR):
	$(MKDIR) $(INSTALL_DIR)

$(OUTFILE_ACCESS): $(SRC_ACCESS)
	$(CC) $(CFLAGS) $^ -o $@

$(OUTFILE_STAT): $(SOURCE)
	$(CC) $(CFLAGS) -static $^ -o $@

$(OUTFILE_DYN): $(SOURCE)
	$(CC) $(CFLAGS) $^ -o $@

$(BIN_OUTFILE_ACCESS) : $(BINDIR) $(SRC_ACCESS)
	$(CC) $(CFLAGS) $^ -o $@

$(BIN_OUTFILE_STAT): $(SOURCE)
	$(CC) $(CFLAGS) -static $^ -o $@

$(BIN_OUTFILE_DYN): $(SOURCE)
	$(CC) $(CFLAGS) $^ -o $@

$(BINDIR):
	$(MKDIR) $(BINDIR)

clean:
	$(RM) *.o $(OUTFILE_DYN) $(OUTFILE_STAT)
