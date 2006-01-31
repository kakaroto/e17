/* Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */

/*
 * eaps.c
 * Copyright (C) Christopher Michael 2005 <devilhorns@comcast.net>
 *
 * e17genmenu is free software copyrighted by Christopher Michael.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name ``Christopher Michael'' nor the name of any other
 *    contributor may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * e17genmenu IS PROVIDED BY Christopher Michael ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL Christopher Michael OR ANY OTHER CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "global.h"
#include "config.h"
#include "icons.h"
#include "eaps.h"

/* Create a .directory.eap for this dir */
void create_dir_eap(char *path, char *cat)
{
   char path2[MAX_PATH];
   char *icon;

   snprintf(path2, sizeof(path2), "%s/.directory.eap", path);
   if (!ecore_file_exists(path2))
     {
	icon = set_icon(cat);
	if (!icon)
	  {
	     fprintf(stderr, "ERROR: Cannot Find Icon For %s\n", cat);
	     return;
	  }
	write_icon(path2, icon);
	write_eap(path2, "app/info/name", cat);
     }
}

char *get_window_class(char *file)
{
   char *tmp, *cls;
   int i;
   Eet_File *ef;

   if (!ecore_file_exists(file)) return NULL;

   ef = eet_open(file, EET_FILE_MODE_READ);
   if (!ef) return NULL;

   tmp = eet_read(ef, "app/window/class", &i);
   if (!tmp)
     {
	if (ef) eet_close(ef);
	return NULL;
     }

   /* Allocate string for window class */
   cls = malloc(i + 1);
   memcpy(cls, tmp, i);
   cls[i] = 0;

   free(tmp);
   eet_close(ef);

   if (cls != NULL) return strdup(cls);
   return NULL;
}

void write_icon(char *f, char *i)
{
   Engrave_File *eet;
   Engrave_Image *image;
   Engrave_Group *grp;
   Engrave_Part *part;
   Engrave_Part_State *ps;

   char *idir, *ifile, *icomp;

#ifdef DEBUG
   fprintf(stderr, "\tWriting Icon %s\n", i);
#endif
   if (!i) ifile = ecore_file_get_file(DEFAULTICON);
   if (i) ifile = ecore_file_get_file(i);

   idir = ecore_file_get_dir(i);
	   
   eet = engrave_file_new();
   engrave_file_image_dir_set(eet, idir);

   icomp = get_icon_compression();
   if (!strcmp(icomp, "COMP"))
     {
	image = engrave_image_new(ifile, ENGRAVE_IMAGE_TYPE_COMP, 0);
     }
   else
     {
	image = engrave_image_new(ifile, ENGRAVE_IMAGE_TYPE_LOSSY, 0);
     }
   engrave_file_image_add(eet, image);

   grp = engrave_group_new();
   engrave_group_name_set(grp, "icon");
   engrave_group_max_size_set(grp, 48, 48);
   engrave_file_group_add(eet, grp);

   part = engrave_part_new(ENGRAVE_PART_TYPE_IMAGE);
   engrave_part_name_set(part, "image");
   engrave_part_mouse_events_set(part, 0);
   engrave_group_part_add(grp, part);

   ps = engrave_part_state_new();
   engrave_part_state_name_set(ps, "default", 0.0);
   engrave_part_state_aspect_set(ps, 1.0, 1.0);
   engrave_part_state_image_normal_set(ps, image);
   engrave_part_state_add(part, ps);

   engrave_edj_output(eet, f);
   engrave_file_free(eet);
}

void write_eap(char *file, char *section, char *value)
{
   int i;
   Eet_File *ef;

#ifdef DEBUG
   fprintf(stderr, "\tWriting %s:%s\n", strdup(section), strdup(value));
#endif

   ef = eet_open(file, EET_FILE_MODE_READ_WRITE);
   if (!ef) return;

   if (!strcmp(section, "app/info/startup_notify"))
     {
	if (!value) eet_delete(ef, section);
	if (value)
	  {
	     i = atoi(value);
	     eet_write(ef, strdup(section), &i, 1, 0);
	  }
     }
   else
     {
	if (!value) eet_delete(ef, section);
	if (value)
	  {
	     i = eet_write(ef, strdup(section), strdup(value), strlen(value), 0);
	     if (i == 0) fprintf(stderr, "Failed To Write %s To %s Of %s\n", value, section, file);
	  }
     }
   eet_close(ef);
}
