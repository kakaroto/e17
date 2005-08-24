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

typedef struct {
        Evas *evas;
        Evas_Object *edje;
        Evas_Object *container;
} Gui;

typedef struct {
	char *theme;
	char *evas_engine;
} Config;

typedef struct {
	E_Menu *config_menu;

	Evas_List *mailboxes;
	Evas_List *plugins;

	Config *conf;

	Gui gui;

	Ecore_Idler *idler;
} Embrace;

Embrace *embrace_new ();
void embrace_free (Embrace *e);

bool embrace_init (Embrace *e);
void embrace_deinit (Embrace *e);

void embrace_run (Embrace *e);
void embrace_stop (Embrace *e);

int embrace_signal_get ();
void embrace_expand_path (char *str, char *dest, int destlen);
char *embrace_strstrip (char *str);

#endif
