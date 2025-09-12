/*****************************************************************************
 * Invary Test Kit Module
 * Copyright (C) 2025 Invary, Inc.
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
#include <linux/kprobes.h>
#include "invary-test-kit.h"

/*
 * The ability to modify the sys call table was removed in 6.9 and backported to other stable kernels
 */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(6,9,0)) || (LINUX_VERSION_CODE >= KERNEL_VERSION(6,8,5)) || \
    (LINUX_VERSION_CODE >= KERNEL_VERSION(6,6,26)) || (LINUX_VERSION_CODE >= KERNEL_VERSION(6,1,85)) || \
    (LINUX_VERSION_CODE >= KERNEL_VERSION(5,15,154))
#define KPROBE_NEEDED 1
#endif

#if KPROBE_NEEDED
static int sys_kill_kprobe_pre_handler(struct kprobe *p, struct pt_regs *regs)
{
    struct pt_regs *user_regs = (struct pt_regs *) regs->di;
    pid_t pid = (pid_t) user_regs->di;
    int sig = (int) user_regs->si;
    printk(KERN_DEBUG INVARY_TEST_KIT_TAG "forwarding kill to kernel implementation: pid=%d, sig=%x\n", pid, sig);
    return 0;
}

struct kprobe sys_kill_kprobe = {
    .symbol_name = "__x64_sys_kill",
    .pre_handler = sys_kill_kprobe_pre_handler,
};

void kprobe_init(void) {
    int err;
    err = register_kprobe(&sys_kill_kprobe);
    if (err) {
        pr_err("register_kprobe() failed: %d\n", err);
    }
}

void kprobe_shutdown(void) {
    unregister_kprobe(&sys_kill_kprobe);
}
#else
// Dummy routines
void kprobe_init(void) {
}

void kprobe_shutdown(void) {
}
#endif