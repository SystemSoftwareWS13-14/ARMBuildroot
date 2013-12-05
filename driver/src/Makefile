export ARCH = arm
export CROSS_COMPILE = /opt/toolchains/Sourcery-CodeBench-ARM-2013.05/bin/arm-none-linux-gnueabi-
OUT = $(shell pwd)/../bin

KDIR := ../../buildroot-2013.08.1/output/build/linux-3.10.7/
MODDIR = ../../rootfs_files/lib/modules/extra
MKDIR = mkdir -p
CP = cp

.PHONY: all clean install

all: $(OUT)
	$(MAKE) -C $(KDIR) M=$$PWD

clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean

install: all $(MODDIR)
	$(CP) *.ko $(MODDIR)

$(OUT):
	$(MKDIR) $(OUT)

$(MODDIR):
	$(MKDIR) $(MODDIR)
