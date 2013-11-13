CROSS_COMPILE=/opt/toolchains/Sourcery-CodeBench-ARM-2013.05/bin/arm-none-linux-gnueabi-

CC=$(CROSS_COMPILE)gcc
CFLAGS=-Wall

HWEXEC=show_uptime
OUTFILE_DYN=$(BINDIR)/$(HWEXEC)_dyn
OUTFILE_STAT=$(BINDIR)/$(HWEXEC)_stat
BINDIR=../bin

HWSOURCE=$(HWEXEC).c
RM=rm -rf
MKDIR=mkdir -p

.PHONY: all clean

all: $(BINDIR) $(OUTFILE_STAT) $(OUTFILE_DYN)

$(OUTFILE_STAT): $(HWSOURCE)
	$(CC) $(CFLAGS) -static $^ -o $(@)

$(OUTFILE_DYN): $(HWSOURCE)
	$(CC) $(CFLAGS) $^ -o $(@)

$(BINDIR):
	$(MKDIR) $(BINDIR)

clean:
	$(RM) *.o $(OUTFILE)
