/*
 * @file
 *
 * Copyright (C) 2010 by ProFUSION embedded systems
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the  GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * @author Fabiano Fidêncio <fidencio@profusion.mobi>
 */

#ifndef GUI_H
#define GUI_H

#include <Elementary.h>

typedef struct _Pginfo Pginfo;

struct _Pginfo
{
   Evas_Object *pager, *pg1, *pg2, *pg3;
};

typedef struct _Measurements_Lists  Measurements_Lists;

struct _Measurements_Lists
{
   Evas_Object *list_in, *list_out;
};

void create_pager(Evas_Object *parent, Evas_Object *pg, Pginfo *info,
		Measurements_Lists *ml);
void populate_pager(Evas_Object *parent, Evas_Object *pg, Pginfo *info);
void populate_converters_list(Evas_Object *list, Pginfo *info);
void populate_measurements_list_in(Evas_Object *list, Evas_Object *ed,
		Pginfo *info);
void populate_measurements_list_out(Evas_Object *list, Evas_Object *ed,
		Pginfo *info);

#endif
