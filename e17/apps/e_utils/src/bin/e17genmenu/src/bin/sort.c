/* Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */

/*
 * sort.c
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

#include "config.h"
#include "global.h"
#include "order.h"
#include "sort.h"

Ecore_List *add_list(Ecore_List *list, char *tmp)
{
   int i, r;
   char *name;

   i = 0;
   r = 0;
   if (!ecore_list_nodes(list)) ecore_list_prepend(list, tmp);
   ecore_list_goto_first(list);
   if (ecore_list_nodes(list) >= 1)
     {
	for (i = 0; i < ecore_list_nodes(list); i++)
	  {
	     name = ecore_list_goto_index(list, i);
	     r = strcmp(tmp, name);
	     if (r < 0) break;
	     if (r == 0) return list;
	  }
	if (r < 0) ecore_list_insert(list, tmp);
	if (r >= 0) ecore_list_append(list, tmp);
     }
   return list;
}

char *get_name(char *path)
{
   int ret;
   char *n, *nbuff, *name;
   Eet_File *ef;

   ef = eet_open(path, EET_FILE_MODE_READ);
   if (!ef) return NULL;

   n = (char *) eet_read(ef, "app/info/name", &ret);
   nbuff = (char *) malloc(ret + 1);
   snprintf(nbuff, ret + 1, "%s", n);
   name = strdup(nbuff);

   eet_close(ef);
   return strdup(name);
}

void sort_menu(char *path)
{
   char *home, *name, *v, *token;
   char path2[MAX_PATH], buffer[MAX_PATH];
   int length, j;
   FILE *f;
   Ecore_List *eaps = NULL;

#ifdef DEBUG
   fprintf(stderr, "Sorting Menu %s...\n", path);
#endif

   home = get_home();
   snprintf(path2, sizeof(path2), "%s/.order", path);
   if (!ecore_file_exists(path2)) create_order(path2);

   f = fopen(path2, "r");
   if (!f) return;

    /* Sort Names */
   eaps = ecore_list_new();
   *buffer = 0;
   while (fgets(buffer, MAX_PATH, f) != NULL)
     {
	if (!(*buffer) || (*buffer == '\n')) continue;
	if (buffer[(length = strlen(buffer) -1)] == '\n') buffer[length] = '\0';
	snprintf(path2, sizeof(path2), "%s"EAPPDIR"/%s", home, buffer);
	name = get_name(strdup(path2));
	if (name != NULL)
	  {
	     snprintf(path2, sizeof(path2), "%s:%s", strdup(name), strdup(buffer));
	     if (!search_list(eaps, strdup(path2))) eaps = add_list(eaps, strdup(path2));
	  }
     }
   fclose(f);

    /* Order File Empty */
   if (!ecore_list_nodes(eaps)) return;

   ecore_list_goto_first(eaps);

    /* Rewrite Order File */
   snprintf(path2, sizeof(path2), "%s/.order", path);
   f = fopen(path2, "w");
   if (!f) return;

   for (j = 0; j < ecore_list_nodes(eaps); j++)
     {
	v = ecore_list_goto_index(eaps, j);
	/* Split eaps */
	token = strstr(v, ":");
	token = strtok(token, ":");
	snprintf(buffer, sizeof(buffer), "%s\n", token);
	if (buffer != NULL) fwrite(buffer, sizeof(char), strlen(buffer), f);
     }

   fclose(f);

   if (eaps) ecore_list_destroy(eaps);
   return;
}

void sort_menus()
{
   char *home, *file;
   char path[MAX_PATH];
   Ecore_List *list = NULL;

#ifdef DEBUG
   fprintf(stderr, "Sorting Menus...\n");
#endif

   home = get_home();
   snprintf(path, sizeof(path), "%s"EFAVDIR, home);
   list = (Ecore_List *)ecore_file_ls(path);
   if (!list) return;

   ecore_list_goto_first(list);
   while ((file = ecore_list_next(list)) != NULL)
     {
	if (!strcmp(file, ".") || !strcmp(file, "..")) continue;
	snprintf(path, sizeof(path), "%s"EFAVDIR"/%s", home, file);
	if (ecore_file_is_dir(path)) sort_menu(path);
     }

   if (list) ecore_list_destroy(list);
}

void sort_favorites()
{
   int length;
   char *home, *file;
   char path[MAX_PATH], buffer[MAX_PATH];
   Ecore_List *list = NULL, *dir = NULL;
   FILE *f;

#ifdef DEBUG
   fprintf(stderr, "\nSorting Favorites Menu...\n");
#endif

   home = get_home();

   snprintf(path, sizeof(path), "%s"EFAVDIR"/.order", home);
   if (!ecore_file_exists(path)) create_order(path);

   f = fopen(path, "r");
   if (!f) return;

   list = ecore_list_new();
   *buffer = 0;
   while (fgets(buffer, sizeof(buffer), f) != NULL)
     {
	if (!(*buffer) || (*buffer == '\n')) continue;
	/* Strip New Line Char */
	if (buffer[(length = strlen(buffer) - 1)] == '\n') buffer[length] = '\0';
	if (!search_list(list, strdup(buffer)))
	  {
	     if (!ecore_list_append(list, strdup(buffer)))
	       {
#ifdef DEBUG
		  fprintf(stderr, "ERROR: Ecore List Append Failed !!\n");
#endif
		  return;
	       }
	  }
     }
   fclose(f);

   f = fopen(path, "w");
   if (!f) return;

   snprintf(path, sizeof(path), "%s"EFAVDIR, home);
   dir = (Ecore_List *)ecore_file_ls(path);
   if (dir) ecore_list_goto_first(dir);
   while ((file = ecore_list_next(dir)) != NULL)
     {
	if (!strcmp(file, ".") || !strcmp(file, "..")) continue;
	if (!strcmp(file, ".order")) continue;
	if (!search_list(list, file))
	  {
	     snprintf(path, sizeof(path), "%s"EFAVDIR"/%s", home, file);
	     snprintf(buffer, sizeof(buffer), "%s\n", file);
	     if (buffer != NULL) fwrite(buffer, sizeof(char), strlen(buffer), f);
	  }
     }

   fclose(f);
   if (list) ecore_list_destroy(list);
   if (dir) ecore_list_destroy(dir);
}
