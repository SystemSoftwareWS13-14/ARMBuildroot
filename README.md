ARMBuildroot
============

## Tools

* Buildroot
  * With ulibc
* ccache

## Config Settings

### Buildroot

* Build options:
  * 4 parallel jobs for building
  * Activated ccache
  * Removed "build packages with debug infos"
* System configuration:
  * System hostname
  * System banner
  * /dev management with devtmpfs
* Toolchain
  * Enable WCHAR support
  * Build cross gdb for the host
  * Set external toolchain
* Filesystem
  * cpio the root fs with bzip2
* The rootfs_files directory is used as overlay filesystem for rootfs
* Packages
  * <pre>
BR2_PACKAGE_BUSYBOX=y
BR2_PACKAGE_STRACE=y
BR2_PACKAGE_SHOW_UPTIME=y
BR2_PACKAGE_JEFA_WEB=y
BR2_PACKAGE_DROPBEAR=y
BR2_PACKAGE_DROPBEAR_SMALL=y
</pre>

**Removed**
* ext2/3/4 root fs

===

### Kernel

* kernel debug infos

### Busybox

* strace
* httpd

----
  
## How To

### Compile Kernel with buildroot

* Get latest buildroot
* In buildroot folder:
  * make qemu_arm_versatile_defconfig
  * make menuconfig
  * Activate configs
  * make source
  * make

Use the **al** script to access the specific configuration menus.

## Questions

**make source:** Download all sources needed for offline-build.  
**Downloaded files:** In directory "dl".  
**Used Cross-Toolchain:** Sourcery-CodeBench-ARM-2013.05.

===

After make the output files are in the following directories (in the output directory):  
**images:** Kernel image, bootloader, root fs, ...  
**build:** The components were extracted and build here.  
**staging:** Contains user-space packages from the root fs with the development files.  
**target:** Like root fs but without dev. files.  
**host:** Tools for buildroot.  
**toolchain:** Build dirs for the cross-comp. toolchain.  

===

Boot time is displayed at start-up. (UTC time is used).

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

**Available RAM**

<pre>
# free
             total         used         free       shared      buffers
Mem:        125904        18752       107152            0            0
-/+ buffers:              18752       107152
Swap:            0            0            0
</pre>

**Users already on system**

<pre>
root:x:0:0:root:/root:/bin/sh
daemon:x:1:1:daemon:/usr/sbin:/bin/sh
bin:x:2:2:bin:/bin:/bin/sh
sys:x:3:3:sys:/dev:/bin/sh
sync:x:4:100:sync:/bin:/bin/sync
mail:x:8:8:mail:/var/spool/mail:/bin/sh
proxy:x:13:13:proxy:/bin:/bin/sh
www-data:x:33:33:www-data:/var/www:/bin/sh
backup:x:34:34:backup:/var/backups:/bin/sh
operator:x:37:37:Operator:/var:/bin/sh
haldaemon:x:68:68:hald:/:/bin/sh
dbus:x:81:81:dbus:/var/run/dbus:/bin/sh
ftp:x:83:83:ftp:/home/ftp:/bin/sh
nobody:x:99:99:nobody:/home:/bin/sh
sshd:x:103:99:Operator:/var:/bin/sh
default:x:1000:1000:Default non-root user:/home/default:/bin/sh
</pre>

**Hard disks**

<pre>
# df
Filesystem                Size      Used Available Use% Mounted on
</pre>

No hard disks are available.

=====

output of modules (dmesg):

<pre>
mmci-pl18x: probe of fpga:05 failed with error -38
mmci-pl18x: probe of fpga:0b failed with error -38
i2c /dev entries driver
aaci-pl041 fpga:04: ARM AC'97 Interface PL041 rev0 at 0x10004000, irq 56
aaci-pl041 fpga:04: FIFO 512 entries
</pre>

commands:

<pre>
modprobe kernel/drivers/mmc/host/mmci.ko
modprobe kernel/drivers/char/hw_random/rng-core.ko
modprobe kernel/drivers/misc/eeprom/eeprom.ko
modprobe kernel/drivers/i2c/i2c-dev.ko
modprobe kernel/sound/core/snd-page-alloc.ko
modprobe kernel/sound/pci/ac97/snd-ac97-codec.ko
modprobe kernel/fs/fat/fat.ko
modprobe kernel/fs/nls/nls_iso8859-1.ko
modprobe kernel/sound/arm/snd-aaci.ko
modprobe kernel/sound/ac97_bus.ko
</pre>

same as they write on stdout!

===

**Network device**
<pre>
model=smc91c111
</pre>

===

**Strace of show_uptime**

<pre>
rt_sigprocmask(SIG_BLOCK, [CHLD], [], 8) = 0
rt_sigaction(SIGCHLD, NULL, {SIG_DFL, [], 0}, 8) = 0
rt_sigprocmask(SIG_SETMASK, [], NULL, 8) = 0
nanosleep({1, 0}, 0xbea20a5c)           = 0
sysinfo({uptime=112, loads=[20448, 11008, 4160] totalram=128925696, freeram=109408256, sharedram=0, bufferram=0} totalswap=0, freeswap=0, procs=30}) = 0
write(1, "=====================\n", 22=====================
) = 22
write(1, "Uptime = 112\n", 13Uptime = 112
)          = 13
write(1, "=====================\n", 22=====================
) = 22
</pre>

## Other useful information

The modules.dep file contains all dependencies between the kernel modules.
The modprobe tool uses this file when loading modules. Otherwise you would have to solve the
dependencies manual (with insmod).

-----

#Module

## How To

Makefile:
* set ARCH and CROSS_COMPILE on execution
* set KDIR to directory with kernel-sources
* set obj-m to module-object-name you want

run module on qemu:
* copy into target dir / rootfs (quick and dirty)
* create package
* use overlay fs

## Questions

access modes of driver:
<pre>
-rw-r--r--    1 root     root         68133 Dec  5  2013 treiber.ko
</pre>

===

log output in /var/log/messages using cat on openclose:
<pre>
Jan  1 00:01:45 JeFa_Buildroot user.warn kernel: MODERN mod_init called
Jan  1 00:01:45 JeFa_Buildroot user.warn kernel: Major: 254
Jan  1 00:01:45 JeFa_Buildroot user.info kernel: Registered driver
Jan  1 00:02:23 JeFa_Buildroot user.debug kernel: Opened openclose!
Jan  1 00:02:23 JeFa_Buildroot user.debug kernel: Closed openclose!
</pre>

===

**hello.c:** cat /dev/driver returns "Hello world" in a loop, because cat does not get a EOF.

strace cat /dev/driver  
<pre>
read(3, "Hello\n\0", 4096)              = 7
write(1, "Hello\n\0", 7Hello
)                = 7
</pre>

cat calls the read system call and gets the number of bytes read. However, it does not get
a EOF, so it calls the read again. When the read driver function returns 0, it indicates the
calling program that EOF is reached an 0 Bytes are read. So the driver needs to return at first
the number of Bytes read, and in the second read a EOF.

Correct strace cat /dev/driver  
<pre>
open("/dev/myDriver", O_RDONLY|O_LARGEFILE) = 3
read(3, "Hello\n\0", 4096)              = 7
write(1, "Hello\n\0", 7Hello
)                = 7
read(3, "", 4096)                       = 0
close(3)                                = 0
</pre>

===

tasklet:
* soft-irq
* executes function later -> after next hardware interrupt

===

timer:
* soft-itq
* executes function after time intervall

===

kthread in module:
* not killable
* removed correctly on mod_exit

===

Flags for kmalloc:  
* GFP_ATOMIC (For kernel sapce, never sleeps)
* GFP_KERNEL (For kernel space, may sleeps)
* GFP_USER (For user space, may sleeps)
* Flags to restrict the memory allocation:
 * GFP_HIGHUSER
 * GFP_NOIO
 * GFP_NOFS

===

Options for communication between user-space programs and kernel modules:
* With insmod var="value" and the macro module_param()
* With the device files in /dev
* With the /proc or /sysfs (module_param_named()) filesystems

## Other useful information

The implementation of the /dev/null and /dev/zero device drivers are in:  
**source/drivers/char/mem.c**

##Useful Links

http://www.linux-magazine.com/Online/Features/Qemu-and-the-Kernel

tasklet & workqueue : http://www.ibm.com/developerworks/library/l-tasklets/

timer & list: http://www.ibm.com/developerworks/library/l-timers-list/
