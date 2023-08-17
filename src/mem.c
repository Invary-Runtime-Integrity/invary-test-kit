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

#include <linux/version.h> 

#if IS_ENABLED(CONFIG_X86) || IS_ENABLED(CONFIG_X86_64)
#if LINUX_VERSION_CODE > KERNEL_VERSION(4, 16, 0)
static inline void write_cr0_forced(unsigned long val)
{
    unsigned long __force_order;
    asm volatile(
        "mov %0, %%cr0" : "+r"(val), "+m"(__force_order));
}
#endif
#endif

#if IS_ENABLED(CONFIG_X86) || IS_ENABLED(CONFIG_X86_64)
#if LINUX_VERSION_CODE > KERNEL_VERSION(4, 16, 0)
void protect_memory(long unsigned int cr0) {
    write_cr0_forced(cr0);
}
#else
void protect_memory(long unsigned int cr0) {
    write_cr0(cr0);
}
#endif
#endif

#if IS_ENABLED(CONFIG_X86) || IS_ENABLED(CONFIG_X86_64)
#if LINUX_VERSION_CODE > KERNEL_VERSION(4, 16, 0)
long unsigned int unprotect_memory(void) {
    long unsigned int cr0;
    cr0 = read_cr0();
    write_cr0_forced(cr0 & ~0x00010000);
    return cr0;
}
#else
long unsigned int unprotect_memory(void) {
    long unsigned int cr0;
    cr0 = read_cr0();
    write_cr0(cr0 & ~0x00010000);
    return cr0;
}
#endif
#endif
