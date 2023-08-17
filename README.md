# invary-test-kit

The Invary Test Kit is a Linux kernel module which hijacks the <b>kill</b> system call, resulting in an error detected by the Invary sensor software.  It does not modify the running system in any other way, and only logs that <b>kill</b> has been called before passing control back to the normal system call.

This code is based on ideas presented in the following github projects:
https://github.com/m0nad/Diamorphine
https://github.com/xcellerator/linux_kernel_hacking

## Finding the Pre-built Module for your System

Invary maintains pre-built modules for a large number of Linux distributions/versions and their supported kernel versions.  These are maintained in the <b>archive</b> directory of this repository.

To see if your Linux environment is supported, run the <b>uname -a</b> command:

```
$ uname -a
Linux testMachine 6.2.0-26-generic #26~22.04.1-Ubuntu SMP PREEMPT_DYNAMIC Thu Jul 13 16:27:29 UTC 2 x86_64 x86_64 x86_64 GNU/Linux
```
From this example, the machine is a x86_64 system running Ubuntu-22.04 with kernel version 6.2.0.26-generic.  If you navigate to <b>archive/ubuntu/22.04/x86_64</b>, you will find the compressed tar file with the pre-built module.

```
$ cp ~/invary-test-kit/archive/ubuntu/22.04/x86_64/invary-test-kit-ubuntu-22.04-6.2.0.26-generic.tar.gz .
$ tar xvfz invary-test-kit-ubuntu-22.04-6.2.0.26-generic.tar.gz
$ cd invary-test-kit
$ ls
invary-test-kit.ko
```

## Building the Module

If your Linux system is currently not supported by Invary, you can easily build the kernel module from the provided sources.

First, you will need to install the required packages to support building a Linux kernel module.  For Debian/Ubuntu, these are:
```
# Debian
sudo apt-get install linux-headers-$(uname -r) build-essential

# Ubuntu - later versions may also need gcc-12
sudo apt-get install build-essential linux-headers-$(uname -r)
```
For RHEL, Centos, Amazon Linux, Rocky and AlmaLinux, these are:
```
sudo yum install gcc kernel-devel make kernel-devel-$(uname -r)
```

Once these packages have been installed, you will just need to run <b>make</b> in the source folder:

```
$ cd ~/invary-test-kit/src/
$ make
make W=1 -C /lib/modules/`uname -r`/build M=$PWD
make[1]: Entering directory '/usr/src/linux-headers-6.2.0-26-generic'
  CC [M]  /home/tester/invary-test-kit/src/invary-test-kit.o
  MODPOST /home/tester/invary-test-kit/src/Module.symvers
  CC [M]  /home/tester/invary-test-kit/src/invary-test-kit.mod.o
  LD [M]  /home/tester/invary-test-kit/src/invary-test-kit.ko
  BTF [M] /home/tester/invary-test-kit/src/invary-test-kit.ko
make[1]: Leaving directory '/usr/src/linux-headers-6.2.0-26-generic'
```

## Installing the Module

Now that you have the module, from the archive file or a local build, to install just use the <b>insmod</b> command as root:

```
sudo insmod invary-test-kit.ko
```
If you did build the module locally, there is a short-cut:
```
make load
```


## Issues with Secure Boot

If you run into an error such as this when attempting to load the module, your system requires modules to be signed to be loaded:

```
$ sudo insmod invary-test-kit.ko
insmod: ERROR: could not insert module invary-test-kit.ko: Key was rejected by service
```

The details of signing a kernel module are beyond the scope of this document and can vary depending on the Linux distribution.  Please consult the Linux kernel documentation (https://www.kernel.org/doc/html/v4.15/admin-guide/module-signing.html) or information provided by your Linux distribution (e.g. - https://ubuntu.com/blog/how-to-sign-things-for-secure-boot).


## Testing the Module

To confirm that the module did load correctly, run the following commands:
```
$ lsmod | grep invary_test_kit
invary_test_kit        24576  0

$ sudo cat /proc/kallsyms | grep invary_test_kit
ffffffffc24ed024 r _note_10	[invary_test_kit]
ffffffffc24ed03c r _note_9	[invary_test_kit]
ffffffffc24ee548 b kernel_kill	[invary_test_kit]
ffffffffc24ee100 d kp	[invary_test_kit]
ffffffffc24ee560 b sys_call_table_orig	[invary_test_kit]
ffffffffc24f04a0 b sys_call_table_ptr	[invary_test_kit]
ffffffffc24ee180 d __UNIQUE_ID___addressable_cleanup_module301	[invary_test_kit]
ffffffffc24ec0f0 t hook_init	[invary_test_kit]
ffffffffc24ec440 t hook_shutdown	[invary_test_kit]
ffffffffc24ee1c0 d __this_module	[invary_test_kit]
ffffffffc24ec0e0 t __pfx_hook_init	[invary_test_kit]
ffffffffc24ec480 t cleanup_module	[invary_test_kit]
ffffffffc24ec4e0 t protect_memory	[invary_test_kit]
ffffffffc24ec150 t hook_syscall	[invary_test_kit]
ffffffffc24ec270 t init_module	[invary_test_kit]
ffffffffc24ec270 t invary_test_kit_init	[invary_test_kit]
ffffffffc24ec260 t __pfx_init_module	[invary_test_kit]
ffffffffc24ee540 b lookup_symbol	[invary_test_kit]
ffffffffc24ec330 t unhook_syscall	[invary_test_kit]
ffffffffc24ec010 t invary_kill	[invary_test_kit]
ffffffffc24ec470 t __pfx_cleanup_module	[invary_test_kit]
ffffffffc24ec0b0 t kernel_symbol_lookup	[invary_test_kit]
ffffffffc24ec0a0 t __pfx_kernel_symbol_lookup	[invary_test_kit]
ffffffffc24ec530 t unprotect_memory	[invary_test_kit]
ffffffffc24ec520 t __pfx_unprotect_memory	[invary_test_kit]
ffffffffc24ec480 t invary_test_kit_exit	[invary_test_kit]
ffffffffc24ec050 t __pfx_kernel_symbol_init	[invary_test_kit]
ffffffffc24ec000 t __pfx_invary_kill	[invary_test_kit]
ffffffffc24ec430 t __pfx_hook_shutdown	[invary_test_kit]
ffffffffc24ec320 t __pfx_unhook_syscall	[invary_test_kit]
ffffffffc24ec060 t kernel_symbol_init	[invary_test_kit]
ffffffffc24ec4d0 t __pfx_protect_memory	[invary_test_kit]
ffffffffc24ec140 t __pfx_hook_syscall	[invary_test_kit]
```
<b>Note:</b> The module name is changed from <b>invary-test-kit</b> to <b>invary_test_kit</b> by the Linux kernel.

If the Invary Sensor is running, the results of a failed appraisal can be found in the Invary console (https://console.invary.com).  If you do not have an account, you can sign up at https://invary.com/services or use Invary RISe at https://invary.com/rise.


## Removing the Module

The module can easily be removed with the <b>rmmod</b> command as root:
```
sudo rmmod invary_test_kit
```
If the module was built locally, there is a short-cut:
```
make unload
```
