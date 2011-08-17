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

#include <string.h>

#include <Eina.h>
#include <Edje.h>
#include <Elementary.h>

#include "browser.h"
#include "enna.h"
#include "enna_config.h"
#include "mainmenu.h"

typedef struct _Smart_Data Smart_Data;
typedef struct _Activated_Cb_Data Activated_Cb_Data;

struct _Activated_Cb_Data
{
   Smart_Data *sd;
   Enna_File *file;
};

struct _Smart_Data
{
   Enna_File *selected;
   Ecore_Event_Handler *act_handler;
   Eina_Bool visible;
   Enna_Browser *browser;
};

/* Local subsystem functions */

static void
_add_cb(void *data, Enna_File *file)
{

}

static void
_enna_mainmenu_load_from_activities(Smart_Data *sd)
{
   sd->browser = enna_browser_add(_add_cb, sd, NULL, NULL, NULL, NULL, "/");
   enna_browser_browse(sd->browser);
}

static void
_enna_mainmenu_item_activate(void *data)
{
   Activated_Cb_Data *cb_data = data;


   // run the activity_show cb. that is responsable of showing the
   // content using enna_content_select("name")
   enna_activity_show(cb_data->file->name);
   cb_data->sd->selected = cb_data->file;
}

/* Local subsystem callbacks */

/* externally accessible functions */
Evas_Object *
enna_mainmenu_add(Evas_Object *parent)
{
   Smart_Data *sd;
   sd = ENNA_NEW(Smart_Data, 1);
   if (!sd) return NULL;

   /* connect to the input signal */

   _enna_mainmenu_load_from_activities(sd);

   return NULL;
}

void
enna_mainmenu_shutdown(Evas_Object *obj)
{
   Smart_Data *sd;

   sd =evas_object_data_get(obj, "mainmenu_data");
   ENNA_EVENT_HANDLER_DEL(sd->act_handler);

   enna_browser_del(sd->browser);
   ENNA_FREE(sd);
}

void
enna_mainmenu_append(Evas_Object *obj, Enna_File *f)
{
   Smart_Data *sd;
   Activated_Cb_Data *cb_data;

   sd = evas_object_data_get(obj, "mainmenu_data");

   if (!f) return;

   cb_data = malloc(sizeof(Activated_Cb_Data));
   cb_data->sd = sd;
   cb_data->file = f;

   _enna_mainmenu_item_activate(cb_data);
}

Enna_File *
enna_mainmenu_selected_activity_get(Evas_Object *obj)
{
   Smart_Data *sd;

   sd = evas_object_data_get(obj, "mainmenu_data");

   if (!sd) return 0;
   return sd->selected;
}

void
enna_mainmenu_show(Evas_Object *obj)
{
   Smart_Data *sd;

   sd = evas_object_data_get(obj, "mainmenu_data");

   if (!sd) return;
   sd->visible = 1;
   sd->selected = NULL;
}

void
enna_mainmenu_hide(Evas_Object *obj)
{
   Smart_Data *sd;

   sd = evas_object_data_get(obj, "mainmenu_data");

   if (!sd) return;
   sd->visible = 0;
}


Eina_Bool
enna_mainmenu_visible(Evas_Object *obj)
{
   Smart_Data *sd;

   sd = evas_object_data_get(obj, "mainmenu_data");

   if (!sd) return EINA_FALSE;
   return sd->visible;
}

Evas_Object *
enna_mainmenu_selected_obj_get(Evas_Object *obj)
{

   return NULL;
}
