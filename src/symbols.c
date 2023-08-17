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

#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/ptrace.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/version.h>

#include <generated/autoconf.h>
#include <linux/kprobes.h>

#include <asm/io.h>
#include <asm/page_types.h>
#include <asm/setup.h>
#include <asm-generic/sections.h>

#include <asm/desc.h>
#include "symbols.h"

MODULE_LICENSE("GPL");
//MODULE_DESCRIPTION("Example Rogue Module");
//MODULE_AUTHOR("Wesley Peck (wesleypeck@gmail.com)");
//MODULE_VERSION(VERSION_STR);

typedef unsigned long (*kallsyms_lookup_name_t)(const char *name);
kallsyms_lookup_name_t lookup_symbol = NULL;

static struct kprobe kp = {
    .symbol_name = "kallsyms_lookup_name"
};

void kernel_symbol_init(void) {
    register_kprobe(&kp);
    lookup_symbol = (kallsyms_lookup_name_t) kp.addr;
    unregister_kprobe(&kp);
}

unsigned long kernel_symbol_lookup(const char* symbol) {
    if (lookup_symbol == NULL) {
        return 0;
    }

    return lookup_symbol(symbol);
}
