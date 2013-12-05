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

CONFIG_DIR="../configs"
SAV_BR_CONFIG="$CONFIG_DIR/br.config"

#--------Script-------

cd $ROOT_DIR

rebuild()
{
	clean
	repack
	compile
}

installD()
{
	# !!
	compile
}

compile()
{
	loadBR
        make
}

clean()
{
	saveConfigs
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
	make busybox-menuconfig
	saveBB
}

configK()
{
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
	rm -f dl/show_uptime.tar.gz
	rm -f dl/jefa_web.tar.gz

	rm -f -r output/build/show_uptime-1.0/
	rm -f -r output/build/jefa_web-1.0/
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

loadBR()
{	
	make defconfig BR2_DEFCONFIG="$SAV_BR_CONFIG"
}

saveConfigs()
{
	saveBR
	saveK
	saveBB
}

saveK()
{
	make linux-update-defconfig
}

saveBB()
{
	make busybox-update-config
}

saveBR()
{
	make savedefconfig
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
                        echo "Option requires an argument." 1>&2
                        usage
                        ;;
                ?)
                        echo "Invalid option." 1>&2
                        usage
                        ;;
                *)
                        echo "Unimplemented option." 1>&2
                        command=$DEFAULT_CMD
                        ;;
        esac
done
shift $(($OPTIND - 1))

if [ -z $command ]
then
        command=$DEFAULT_CMD
fi

if [ $(type -t "$command") == "function" ]
then
	echo "Running $command"

	$command
else
	echo "Unrecognized command." 1>&2
	usage
fi

