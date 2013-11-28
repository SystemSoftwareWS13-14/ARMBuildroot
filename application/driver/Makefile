ifneq ($(KERNELRELEASE),)
obj-m   := treiber.o
 
else
KDIR    := ../../buildroot-2013.08.1/output/build/linux-3.10.7
PWD     := $(shell pwd)
 
default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
endif
