CROSS_COMPILE=/opt/toolchains/Sourcery-CodeBench-ARM-2013.05/bin/arm-none-linux-gnueabi-

CC=$(CROSS_COMPILE)gcc
CFLAGS=-Wall

OUT=show_uptime
OUTFILE_DYN=$(BINDIR)/$(OUT)_dynamic
OUTFILE_STAT=$(BINDIR)/$(OUT)_static
BINDIR=../bin

SOURCE=$(OUT).c
RM=rm -rf
MKDIR=mkdir -p

.PHONY: all clean

all: $(BINDIR) $(OUTFILE_STAT) $(OUTFILE_DYN)

$(OUTFILE_STAT): $(SOURCE)
	$(CC) $(CFLAGS) -static $^ -o $@

$(OUTFILE_DYN): $(SOURCE)
	$(CC) $(CFLAGS) $^ -o $@

$(BINDIR):
	$(MKDIR) $(BINDIR)

clean:
	$(RM) *.o $(OUTFILE_DYN) $(OUTFILE_STAT)
