/*
 * Enlightement Hidden Ninjas.
 *
 * Copyright 2012 Hermet (ChunEon Park)
 *
 * This application is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This application is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef __DEFINES__H__
#define __DEFINES__H__

#define PRINT_DBG(string) \
   do \
     { \
        fprintf(stderr, "%s(%d), %s\n", __func__, __LINE__, string); \
     } \
   while(0)

#define ID_BLOCK         1
#define ID_BOMB          2



/*
typedef float VAR_TYPE;


typename <T>
struct Vector2
{
   T x;
   T y;
   vector2() x(0), y(0) {}
}
*/

#endif

