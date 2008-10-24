/*
 * $Id$
 *
 * Copyright (C) 2004 Embrace project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __EMBRACE_H
#define __EMBRACE_H

#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Edje.h>

#include <limits.h>

#ifndef MIN
#define MIN(x, y) (((x) > (y)) ? (y) : (x))
#endif

#ifndef MAX
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#endif

typedef struct {
	char theme[PATH_MAX + 1];
	char evas_engine[255];
	int module;
} Config;

typedef struct {
	Evas *evas;
	Ecore_Evas *ee;
	Evas_Object *edje;
	Evas_Object *container;
} Gui;

typedef struct {
	Eina_List *mailboxes;
	Eina_List *plugins;

	Ecore_Event_Handler *evt_hup;

	Gui gui;
	Config cfg;
} Embrace;

Embrace *embrace_new (void);
void embrace_free (Embrace *e);

bool embrace_init (Embrace *e);
void embrace_deinit (Embrace *e);

void embrace_run (Embrace *e);
void embrace_stop (Embrace *e);

int embrace_signal_get (void);
void embrace_expand_path (char *str, char *dest, int destlen);
char *embrace_strstrip (char *str);

#endif
