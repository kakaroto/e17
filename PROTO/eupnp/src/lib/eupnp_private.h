/* Eupnp - UPnP library
 *
 * Copyright (C) 2009 Andre Dieb Martins <andre.dieb@gmail.com>
 *
 * This file is part of Eupnp.
 *
 * Eupnp is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Eupnp is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Eupnp.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef _EUPNP_PRIVATE_H
#define _EUPNP_PRIVATE_H

#include <stdio.h>
#include "eupnp_log.h"

#define STR(x) #x
#define XSTR(x) STR(x)

#define CHECK_NULL_RET(x)                                               \
    do {                                                                \
        if (x == NULL) {                                                \
            WARN("%s == NULL!\n", XSTR(x));                             \
            return;                                                     \
        }                                                               \
    } while (0)

#define CHECK_NULL_RET_VAL(x, val)                                      \
    do {                                                                \
        if (x == NULL) {                                                \
            WARN("%s == NULL!\n", XSTR(x));                             \
            return val;                                                 \
        }                                                               \
    } while (0)

#endif /* _EUPNP_PRIVATE_H */
