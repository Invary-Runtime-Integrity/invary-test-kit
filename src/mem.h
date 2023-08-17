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

#ifndef _ROGUE_PUBLIC_MEM_H
#define _ROGUE_PUBLIC_MEM_H

inline void protect_memory(long unsigned int cr0);
inline long unsigned int unprotect_memory(void);

#endif
