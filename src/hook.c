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

#include <linux/types.h>
#include <linux/syscalls.h>
#include <linux/printk.h>
#include <linux/string.h>
#include "symbols.h"
#include "hook.h"
#include "mem.h"
#include "invary-test-kit.h"

#define __NR_sys_call_hooks 1000

static unsigned long* sys_call_table_ptr;
static void *sys_call_table_orig[__NR_sys_call_hooks];

void hook_init(void) {
    sys_call_table_ptr = (unsigned long*)kernel_symbol_lookup("sys_call_table");
    memset(sys_call_table_orig, 0, (__NR_sys_call_hooks * sizeof(void*)));
}

void hook_shutdown(void) {
    int i;
    for (i = 0; i < __NR_sys_call_hooks; i++) {
        unhook_syscall(i);
    }
}

static void update_syscall(uint16_t syscall, void* addr) {
    long unsigned int cr0;
    cr0 = unprotect_memory();
    sys_call_table_ptr[syscall] = (unsigned long)addr;
    protect_memory(cr0);
}

void* hook_syscall(void *addr, uint16_t syscall) {
    void* existing;
    if (syscall >= __NR_sys_call_hooks) {
        return NULL;
    }

    existing = (void*)sys_call_table_ptr[syscall];
    if (!sys_call_table_orig[syscall]) {
        sys_call_table_orig[syscall] = existing;
    }
    update_syscall(syscall, addr);
    printk(KERN_WARNING INVARY_TEST_KIT_TAG "hooked system call: %d -> %pS (was %pS)\n", syscall, addr, existing);

    return existing;
}


void unhook_syscall(uint16_t syscall) {
    void* orig;

    orig = sys_call_table_orig[syscall];
    sys_call_table_orig[syscall] = NULL;
    if (!orig) {
        return;
    }
    update_syscall(syscall, orig);
    printk(KERN_WARNING INVARY_TEST_KIT_TAG "unhooked system call: %d -> %pS\n", syscall, orig);
}
