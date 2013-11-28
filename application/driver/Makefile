#CROSS_COMPILE=/opt/toolchains/Sourcery-CodeBench-ARM-2013.05/bin/arm-none-linux-gnueabi-
#CC=$(CROSS_COMPILE)gcc

ifneq ($(KERNELRELEASE),)
obj-m   := mod1.o
 
else
KDIR    := ../../buildroot-2013.08.1/output/build/linux-3.10.7
PWD     := $(shell pwd)
 
default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
endif
