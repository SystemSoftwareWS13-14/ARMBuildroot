export ARCH = arm
export CROSS_COMPILE = /opt/toolchains/Sourcery-CodeBench-ARM-2013.05/bin/arm-none-linux-gnueabi-
#OUT = $(shell pwd)/../bin
KDIR := ../../buildroot-2013.08.1/output/build/linux-3.10.7
 
default:
	$(MAKE) -C $(KDIR) M=$$PWD

clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean

install: default
	$(MAKE) -C $(KDIR) M=$$PWD modules_install
