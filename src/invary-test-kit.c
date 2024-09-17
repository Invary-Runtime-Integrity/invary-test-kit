/*****************************************************************************
 * Invary Test Kit Module
 * Copyright (C) 2023 Invary, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * For the complete terms of the GNU General Public License, please see this URL:
 * http://www.gnu.org/licenses/gpl-2.0.html
 *
 ****************************************************************************/

#include <linux/module.h>
#include "symbols.h"
#include "hook.h"
#include "textmod.h"
#include "mem.h"
#include "invary-test-kit.h"

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Invary Test Kit Module");
MODULE_AUTHOR("Paul Couto (paul@invary.com)");
MODULE_VERSION(VERSION_STR);

#include "symbols.c"
#include "hook.c"
#include "textmod.c"
#include "mem.c"

#if LINUX_VERSION_CODE > KERNEL_VERSION(4, 16, 0)
asmlinkage long invary_kill(const struct pt_regs* pt_regs);
typedef asmlinkage long (*kernel_kill_t)(const struct pt_regs*);
#else
asmlinkage int invary_kill(pid_t pid, int sig);
typedef asmlinkage int (*kernel_kill_t)(pid_t, int);
#endif

static kernel_kill_t kernel_kill;

int invary_test_kit_init(void);
void invary_test_kit_exit(void);

int invary_test_kit_init(void) {
    printk(KERN_ALERT INVARY_TEST_KIT_TAG "initializing\n");
    kernel_symbol_init();
    hook_init();
    textmod_init();
    kernel_kill = (kernel_kill_t)hook_syscall(invary_kill, __NR_kill);
    printk(KERN_ALERT INVARY_TEST_KIT_TAG "initialized\n");
    return 0;
}

void invary_test_kit_exit(void) {
    printk(KERN_ALERT INVARY_TEST_KIT_TAG "exiting\n");
    textmod_shutdown();
    hook_shutdown();
    printk(KERN_ALERT INVARY_TEST_KIT_TAG "exited\n");
}

#if LINUX_VERSION_CODE > KERNEL_VERSION(4, 16, 0)
asmlinkage long invary_kill(const struct pt_regs *pt_regs) {
    pid_t pid = (pid_t) pt_regs->di;
    int sig = (int) pt_regs->si;
#else
asmlinkage int invary_kill(pid_t pid, int sig) {
#endif
    printk(KERN_DEBUG INVARY_TEST_KIT_TAG "forwarding kill to kernel implementation: pid=%d, sig=%x\n", pid, sig);
#if LINUX_VERSION_CODE > KERNEL_VERSION(4, 16, 0)
    return kernel_kill(pt_regs);
#else
    return kernel_kill(pid, sig);
#endif
}

module_init(invary_test_kit_init);
module_exit(invary_test_kit_exit);
