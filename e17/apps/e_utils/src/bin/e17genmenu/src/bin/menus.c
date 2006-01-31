/* Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */

/*
 * menus.c
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
#include "parse.h"
#include "menus.h"

void make_menus()
{
   char *d;

   d = get_desktop_dir();
   if (d) check_for_dirs(strdup(d));
   
   if (!d)
     {
	/* Check desktop files in these directories */
	check_for_dirs(GNOME_DIRS);
	check_for_dirs(KDE_DIRS);
	check_for_dirs(DEBIAN_DIRS);
     }
}

void check_for_dirs(char *path)
{
   char *dir;
   char dirs[MAX_PATH];

   snprintf(dirs, sizeof(dirs), path);
   dir = strdup(path);
   if (dirs)
     {
	if (strstr(dirs, ":")) dir = strtok(dirs, ":");
     }
   if (!dir) return;
   while (dir)
     {
#ifdef DEBUG
	fprintf(stderr, "\nChecking For %s...\n", dir);
#endif
	if (ecore_file_exists(dir))
	     check_for_files(strdup(dir));
	
	if (!strstr(dirs, ":")) break;
	dir = strtok(NULL, ":");
     }
}

void check_for_files(char *dir)
{
   char *file;
   char path[MAX_PATH];
   Ecore_List *files = NULL;

#ifdef DEBUG
   fprintf(stderr, "Checking For Files In %s\n", dir);
#endif

   files = ecore_file_ls(dir);
   ecore_list_goto_first(files);
   while ((file = ecore_list_next(files)) != NULL)
     {
	snprintf(path, sizeof(path), "%s/%s", dir, file);
	if (!ecore_file_is_dir(path))
	  {
	     if (!strcmp(file + strlen(file) - 8, ".desktop"))
	       {
		  if (!strcmp(file, "panel_appearance.desktop")) return;
		  if (!strcmp(file, "ktip.desktop")) return;
		  if (!strcmp(file, "kalarm.desktop")) return;
		  /* Parse Desktop File */
		  parse_desktop_file(strdup(path));
	       }
	     else if (strstr(DEBIAN_DIRS, dir) != NULL)
	       {
		  /* Parse Debian File */
		  if (strstr(file, "README") == NULL) parse_debian_file(strdup(path));
	       }
	  }
     }
   if (files) ecore_list_destroy(files);
}
