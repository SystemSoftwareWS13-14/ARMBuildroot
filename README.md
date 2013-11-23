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
* Packages
  * HTTPD, dropbear, show_uptime, jefa_web

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

Use the armLinux script to access the specific configuration menus.

## Questions

**make source:** Download all sources needed for offline-build.

===

After make the output files are in the following directories (in the output directory):  
**images:** Kernel image, bootloader, root fs, ...  
**build:** The components were build here.  
**staging:** Contains user-space packages from the root fs with the development files.  
**target:** Like staging but without dev. files.  
**host:** Tools for buildroot.  
**toolchain:** Buil dirs for the cross-comp. toolchain.  

===

**Running processes**

<pre>
PID   USER     COMMAND
    1 root     init
    2 root     [kthreadd]
    3 root     [ksoftirqd/0]
    4 root     [kworker/0:0]
    5 root     [kworker/0:0H]
    6 root     [kworker/u2:0]
    7 root     [khelper]
    8 root     [netns]
    9 root     [writeback]
   10 root     [bioset]
   11 root     [kblockd]
   12 root     [rpciod]
   13 root     [kworker/0:1]
   14 root     [kswapd0]
   15 root     [fsnotify_mark]
   16 root     [nfsiod]
   17 root     [kworker/u2:1]
   20 root     [scsi_eh_0]
   21 root     [kworker/0:1H]
   26 root     [kpsmoused]
   27 root     [deferwq]
   28 root     [kworker/0:2]
   38 root     /sbin/syslogd -m 0
   40 root     /sbin/klogd
   65 root     /usr/sbin/dropbear -B
   75 root     udhcpc -s /etc/jefa_web/simple.script
   77 root     -sh
   80 root     httpd -h /www/
   84 root     ps -ef
</pre>

## Other useful information

The modules.dep file contains all dependencies between the kernel modules.
The modprobe tool uses this file when loading modules. Otherwise you would have to solve the
dependencies manual (with insmod).
