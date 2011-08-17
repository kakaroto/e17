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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>

#include "buffer.h"

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

#define BUFFER_DEFAULT_CAPACITY 32768

Enna_Buffer *
enna_buffer_new(void)
{
   return calloc(1, sizeof (Enna_Buffer));
}

void
enna_buffer_append(Enna_Buffer *buffer, const char *str)
{
   size_t len;

   if (!buffer || !str)
     return;

   if (!buffer->buf)
     {
        buffer->capacity = BUFFER_DEFAULT_CAPACITY;
        buffer->buf = calloc(1, buffer->capacity);
     }

   len = buffer->len + strlen (str);
   if (len >= buffer->capacity)
     {
        buffer->capacity = MAX(len + 1, 2 * buffer->capacity);
        buffer->buf = realloc(buffer->buf, buffer->capacity);
     }

   strcat(buffer->buf, str);
   buffer->len += strlen(str);
}

void
enna_buffer_appendf(Enna_Buffer *buffer, const char *format, ...)
{
   char str[BUFFER_DEFAULT_CAPACITY];
   int size;
   va_list va;

   if (!buffer || !format)
     return;

   va_start(va, format);
   size = vsnprintf(str, BUFFER_DEFAULT_CAPACITY, format, va);
   if (size >= BUFFER_DEFAULT_CAPACITY)
     {
        char *dynstr = malloc(size + 1);
        vsnprintf(dynstr, size + 1, format, va);
        enna_buffer_append(buffer, dynstr);
        free(dynstr);
     }
   else
     enna_buffer_append(buffer, str);
   va_end(va);
}

void
enna_buffer_free(Enna_Buffer *buffer)
{
   if (!buffer)
     return;

   if (buffer->buf)
     free(buffer->buf);
   free(buffer);
}
