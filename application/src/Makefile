CROSS_COMPILE=/opt/toolchains/Sourcery-CodeBench-ARM-2013.05/bin/arm-none-linux-gnueabi-

CC=$(CROSS_COMPILE)gcc
CFLAGS=-Wall

OUT=show_uptime
OUTFILE_DYN=$(OUT)_dynamic
OUTFILE_STAT=$(OUT)_static
BIN_OUTFILE_DYN=$(BINDIR)/$(OUTFILE_DYN)
BIN_OUTFILE_STAT=$(BINDIR)/$(OUTFILE_STAT)
BINDIR=../bin

SOURCE=$(OUT).c
RM=rm -rf
MKDIR=mkdir -p

.PHONY: all manual clean

all: $(OUTFILE_STAT) $(OUTFILE_DYN)
manual: $(BINDIR) $(BIN_OUTFILE_STAT) $(BIN_OUTFILE_DYN)

$(OUTFILE_STAT): $(SOURCE)
	$(CC) $(CFLAGS) -static $^ -o $@

$(OUTFILE_DYN): $(SOURCE)
	$(CC) $(CFLAGS) $^ -o $@

$(BIN_OUTFILE_STAT): $(SOURCE)
	$(CC) $(CFLAGS) -static $^ -o $@

$(BIN_OUTFILE_DYN): $(SOURCE)
	$(CC) $(CFLAGS) $^ -o $@

$(BINDIR):
	$(MKDIR) $(BINDIR)

clean:
	$(RM) *.o $(OUTFILE_DYN) $(OUTFILE_STAT)
