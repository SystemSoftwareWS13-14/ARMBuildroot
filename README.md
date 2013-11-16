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
  
## How To

### Compile Kernel with buildroot

* Get latest buildroot
* In buildroot folder:
  * make qemu_arm_versatile_defconfig
  * make menuconfig
  * Activate configs
  * make source
  * make
