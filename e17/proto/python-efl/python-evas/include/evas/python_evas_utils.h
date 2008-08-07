/*
 * Copyright (C) 2007-2008 Gustavo Sverzut Barbieri
 *
 * This file is part of Python-Evas.
 *
 * Python-Evas is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * Python-Evas is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this Python-Evas.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _PYTHON_EVAS_UTILS_H_
#define _PYTHON_EVAS_UTILS_H_

#include <Evas.h>

typedef void (*evas_event_callback_t)(void *data, Evas *e, Evas_Object *obj, void *event_info);

typedef struct
{
    int x;
    int y;
} Evas_Point;

typedef struct
{
    Evas_Point output;
    Evas_Point canvas;
} Evas_Position;

#define PY_REFCOUNT(obj) (((PyObject *)(obj))->ob_refcnt)


#endif /* _PYTHON_EVAS_UTILS_H_ */
