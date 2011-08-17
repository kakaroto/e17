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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <Eina.h>
#include <Ecore_File.h>
#include <Elementary.h>

#include "enna.h"
#include "enna_config.h"
#include "utils.h"
#include "buffer.h"
#include "utils.h"



/****************************************************************************/
/*                       Config File Main Section                           */
/****************************************************************************/


static void
config_load_theme (void)
{
   if (!enna_config->theme)
     goto err_theme;

   enna_config->theme_file = (char *)
     enna_config_theme_file_get(enna_config->theme);

   if (!enna_config->theme_file)
     goto err_theme;

   elm_theme_overlay_add(enna_config->eth, enna_config->theme_file);
   return;

 err_theme:
   printf("couldn't load theme file!\n");
}


/****************************************************************************/
/*                                Theme                                     */
/****************************************************************************/

void
enna_config_load_theme (void)
{
   config_load_theme();
}

const char *
enna_config_theme_get()
{
   return enna_config->theme_file;
}

const char *
enna_config_theme_file_get(const char *s)
{
   char tmp[4096];
   memset(tmp, 0, sizeof(tmp));

   if (!s)
     return NULL;
   if (s[0]=='/')
     snprintf(tmp, sizeof(tmp), "%s", s);

   if (!ecore_file_exists(tmp))
     snprintf(tmp, sizeof(tmp), PACKAGE_DATA_DIR "/enna/theme/%s.edj", s);
   if (!ecore_file_exists(tmp))
     snprintf(tmp, sizeof(tmp), "%s", PACKAGE_DATA_DIR "/enna/theme/default.edj");

   if (ecore_file_exists(tmp))
     return strdup(tmp);
   else
     return NULL;
}

void
enna_config_init(void)
{
   enna_config = calloc(1, sizeof(Enna_Config));
}


void
enna_config_shutdown(void)
{
   free(enna_config);
   enna_config = NULL;
}

