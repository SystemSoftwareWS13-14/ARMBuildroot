export ARCH = arm
export CROSS_COMPILE = /opt/toolchains/Sourcery-CodeBench-ARM-2013.05/bin/arm-none-linux-gnueabi-
OUT = $(shell pwd)/../bin

SSH_IP = 192.168.29.30

KERNELVERSION=3.10.7
KDIR := ../../buildroot-2013.08.1/output/build/linux-$(KERNELVERSION)/
MODDIR = ../../rootfs_files/lib/modules/$(KERNELVERSION)
MKDIR = mkdir -p
CP = cp
SCP = scp

.PHONY: all clean install

all:
	$(MAKE) -C $(KDIR) M=$$PWD

clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean

install: all $(MODDIR)
	$(CP) *.ko $(MODDIR)

install_live: all
	$(SCP) -oBatchMode=yes -oStrictHostKeyChecking=no *.ko root@$(SSH_IP):/lib/modules/$(KERNELVERSION)/extra/

$(OUT):
	$(MKDIR) $(OUT)

$(MODDIR):
	$(MKDIR) $(MODDIR)
