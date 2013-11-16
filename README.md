ARMBuildroot
============

## Tools

* Buildroot
  * With ulibc
* ccache

## Config Settings

* Build options:
  * 4 parallel jobs for building
  * Activated ccache
* System configuration:
  * System hostname
  * System banner
* Toolchain
  * Enable WCHAR support
  * Build cross gdb for the host
  * Set external toolchain
* Filesystem
  * cpio the root fs with bzip2

**Removed**
* ext2/3/4 root fs
  
## How To

### Compile Kernel with buildroot

* Get latest buildroot
* In buildroot folder:
  * make qemu_arm_versatile_defconfig
  * make menuconfig
  * Activate configs
  * make source
  * make

## Questions

**make source:** Download all sources needed for offline-build.

After make the output files are in the following directories (in the output directory):  
**images:** Kernel image, bootloader, root fs, ...  
**build:** The components were build here.  
**staging:** Contains user-space packages from the root fs with the development files.  
**target:** Like staging but without dev. files.  
**host:** Tools for buildroot.  
**toolchain:** Buil dirs for the cross-comp. toolchain.  
