#!/bin/bash
# Helper script for the arm linux kernel and quemu.
# To compile the kernel buildroot is used.

# Configure this script here:
DEFAULT_CMD=emulate
ROOT_DIR="buildroot-2013.08.1"
IMG_PATH="output/images/zImage"
ROOTFS_PATH="output/images/rootfs.cpio.bz2"
BOARD="versatilepb"

#-------Configs------

KERNEL_CONFIG_DIR="output/build/linux-3.10.7"
KERNEL_CONFIG="$KERNEL_CONFIG_DIR/.config"
BB_CONFIG_DIR="output/build/busybox-1.21.1"
BB_CONFIG="$BB_CONFIG_DIR/.config"

CONFIG_DIR="../configs"
SAV_KERNEL_CONFIG="$CONFIG_DIR/kernel.config"
SAV_BB_CONFIG="$CONFIG_DIR/bb.config"
SAV_BR_CONFIG="$CONFIG_DIR/br.config"

#--------Script-------

cd $ROOT_DIR

rebuild()
{
	clean
	repack
	compile
}

compile()
{
	loadConfigs
        make
}

clean()
{
        make clean
}

config()
{
	loadBR
        make menuconfig
	saveBR
}

configBB()
{
	loadBB
	make busybox-menuconfig
	saveBB
}

configK()
{
	loadK
	make linux-menuconfig
	saveK
}

repack()
{
	rmPack
	pack
}

rmPack()
{
	rm dl/show_uptime.tar.gz
	rm dl/jefa_web.tar.gz

	rm -r output/build/show_uptime-1.0/
	rm -r output/build/jefa_web-1.0/
}

pack()
{
        cd ../application
        tar -czf show_uptime.tar.gz src
        cd ../$ROOT_DIR

        cd ../website
        tar -czf jefa_web.tar.gz src
        cd ../$ROOT_DIR
}

download()
{
	make source
}

emulate()
{
        QEMU_AUDIO_DRV=none qemu-system-arm -kernel $IMG_PATH \
        -M $BOARD -nographic \
	-initrd $ROOTFS_PATH \
	-append "console=ttyAMA0" \
        -net nic,macaddr=00:00:00:00:00:1B,vlan=0 \
        -net vde,sock="/tmp/vde2-tap0.ctl",vlan=0

        # -nographic: Redirect output (inclusive serial) to command line
        # -serial: Redirect serial port (standard is stdio)
        # -append "rdinit=/bin/sh" to start shell from initramfs (not /init)
}

debug()
{
	QEMU_AUDIO_DRV=none qemu-system-arm -kernel $IMG_PATH \
        -M $BOARD -nographic \
        -initrd $ROOTFS_PATH \
        -append "console=ttyAMA0" \
	-S \
	-gdb tcp::12345 \
        -net nic,macaddr=00:00:00:00:00:1B,vlan=0 \
        -net vde,sock="/tmp/vde2-tap0.ctl",vlan=0
}

gdb()
{
	cd output/build/linux-3.10.7/
	/opt/toolchains/Sourcery-CodeBench-ARM-2013.05/bin/arm-none-linux-gnueabi-gdb vmlinux
	cd ../../..
}
#---------Config----------

loadConfigs()
{
	loadBR
	loadK
	loadBB
}

loadK()
{
	mkdir -p $KERNEL_CONFIG_DIR
	cp -f $SAV_KERNEL_CONFIG $KERNEL_CONFIG
}

loadBB()
{
	mkdir -p $BB_CONFIG_DIR
	cp -f $SAV_BB_CONFIG $BB_CONFIG
}

loadBR()
{	
	cp -f $SAV_BR_CONFIG .config
}

saveConfigs()
{
	saveBR
	saveK
	saveBB
}

saveK()
{
	cp -f $KERNEL_CONFIG $SAV_KERNEL_CONFIG
}

saveBB()
{
	cp -f $BB_CONFIG $SAV_BB_CONFIG
}

saveBR()
{
	cp -f .config $SAV_BR_CONFIG
}

#---------Usage-----------

usage()
{
        echo "Usage $0 [-c <command>]" 1>&2
        echo -e "-c:\t\tCommand to execute:" 1>&2
        echo -e "\t\tcompile, emulate, config, configBB, configKernel, refreshPackages, clean, defconfig, download, pack" 1>&2
        echo -e "\t\t(Default is -c emulate)" 1>&2
        exit 1
}

if [ $# -eq 0 ]
then
        command=$DEFAULT_CMD
fi

while getopts ":c:h" opt; do
        case $opt in
                c)
                        command=$OPTARG
                        ;;
                h)
                        usage
                        ;;
                :)
                        echo "Option requires an argument."
                        usage
                        ;;
                ?)
                        echo "Invalid option."
                        usage
                        ;;
                *)
                        echo "Unimplemented option."
                        command=$DEFAULT_CMD
                        ;;
        esac
done
shift $(($OPTIND - 1))

if [ -z $command ]
then
        command=$DEFAULT_CMD
fi

echo "Running $command"

$command
