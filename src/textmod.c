/*****************************************************************************
 * Invary Test Kit Module
 * Copyright (C) 2024 Invary, Inc.
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
#include "invary-test-kit.h"
#include "mem.h"
#include "textmod.h"

static unsigned long sys_listen_location = 0;
static unsigned long textmod_listen = 0;
static unsigned char orig_data[5] = { 0 };
static unsigned char call_data[] = { 0xe8, 0, 0, 0, 0};

void modified_listen(void);

void modified_listen(void)
{
    printk("Hooked sys_listen!\n");
}

static void textmod_enable(void)
{
    // Modify sys_listen fentry
    if (sys_listen_location) {
        long unsigned int cr0;
        cr0 = unprotect_memory();
        memcpy((void*)sys_listen_location, call_data, 5);
        protect_memory(cr0);
        printk(KERN_INFO INVARY_TEST_KIT_TAG "Modified sys_listen function entry!\n");
    }
}

static void textmod_disable(void)
{
    // Restore sys_listen
    if (sys_listen_location) {
        long unsigned int cr0;
        cr0 = unprotect_memory();
        memcpy((void*)sys_listen_location, orig_data, 5);
        protect_memory(cr0);
        printk(KERN_INFO INVARY_TEST_KIT_TAG "Restored sys_listen function entry\n");
    }
}


void textmod_init(void)
{
    int offset;

    sys_listen_location = kernel_symbol_lookup("__sys_listen");
    if (!sys_listen_location) {
        printk(KERN_WARNING INVARY_TEST_KIT_TAG "No __sys_listen!\n");
        // For older kernels
        sys_listen_location = kernel_symbol_lookup("sys_listen");
        if (!sys_listen_location) {
            printk(KERN_WARNING INVARY_TEST_KIT_TAG "No sys_listen!\n");
            return;
        }
    }

    textmod_listen = kernel_symbol_lookup("modified_listen");
    if (!textmod_listen) {
        printk(KERN_WARNING INVARY_TEST_KIT_TAG "No textmod_listen!\n");
        return;
    }

    // Create function entry data
    offset = textmod_listen - sys_listen_location - 5;
    memcpy(call_data+1, (void*)&offset, 4);

    // Save original data
    memcpy(orig_data, (void*)sys_listen_location, 5);

    // Modify kernel text
    textmod_enable();
}

void textmod_shutdown(void)
{
    // Restore modifications
    textmod_disable();
}
