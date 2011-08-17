/*
 * GeeXboX Enna Media Center.
 * Copyright (C) 2005-2010 The Enna Project
 *
 * This file is part of Enna.
 *
 * Enna is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * Enna is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Enna; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef BUFFER_H
#define BUFFER_H

typedef struct _Enna_Buffer {
   char *buf;
   size_t len;
   size_t capacity;
} Enna_Buffer;

Enna_Buffer *enna_buffer_new(void);
void enna_buffer_free(Enna_Buffer *buffer);

void enna_buffer_append(Enna_Buffer *buffer, const char *str);
void enna_buffer_appendf(Enna_Buffer *buffer, const char *format, ...);

#endif /* BUFFER_H */
