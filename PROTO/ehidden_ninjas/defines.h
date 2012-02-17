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

//MACRO FUNCTIONS
#define PRINT_DBG(string) \
   do \
     { \
        fprintf(stderr, "%s - %s(%d)\n", string, __func__, __LINE__); \
     } \
   while(0)

#define PACKAGE_BUILD_DIR

//ID VALUES
#define ID_BLOCK         1
#define ID_BOMB          2

//CONST VARIABLES
#define GRID_SIZE        50

typedef float ELEMENT_TYPE;

//STRUCTURES
template<typename T>
struct Vector2
{
   T x;
   T y;
   Vector2<T>(): x(0), y(0) {}
   Vector2<T>(T& x, T& y): x(x), y(y) {}
   //TODO: override operators.
};

#endif

