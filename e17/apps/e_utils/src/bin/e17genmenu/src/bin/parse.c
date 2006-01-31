/* Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */

/*
 * parse.c
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
#include "category.h"
#include "eaps.h"
#include "icons.h"
#include "order.h"
#include "parse.h"

char *get_t(char *icon)
{
   char *ptr;
   char *d;
   char *dir;
   int i = 0;

   ptr = icon;
   d = strrchr(ptr, '"');
   if (!d) return NULL;

   dir = malloc(strlen(icon) + 1);
   while (ptr != d)
     {
	dir[i] = *ptr;
	ptr++;
	i++;
     }
   dir[i] = '\0';
   return strdup(dir);
}

char *parse_buffer(char *b, char *section)
{
   char *oldtoken, *token, *d;
   char t[MAX_PATH], p[MAX_PATH];
   int length, i;

   oldtoken = strdup(b);
   token = strstr(oldtoken, section);

   if (token == NULL) return NULL;

   token = strstr(token, "=");
   token = strtok(token, "=");
   snprintf(t, sizeof(t), "%s", token);

   d = strrchr(t, '\\');
   if (d)
     {
	if (t[(length = strlen(t) - 1)] == '\\') t[length] = '\0';
     }

   d = strrchr(t, '\"');
   if (d)
     {
	if (t[(length = strlen(t) - 1)] == '\"') t[length] = '\0';
     }

   d = strchr(t, '"');
   if (d)
     {
	if (t[0] == '"')
	  {
	     for (i = 0; i < strlen(t); i++)
	       {
		  t[i] = t[i+1];
	       }
	  }
     }

   if (strrchr(t, '"'))
     {
	d = get_t(t);
	snprintf(p, sizeof(p), "%s", d);
     }
   else
     {
	snprintf(p, sizeof(p), "%s", t);
     }
   return strdup(p);
}

char *parse_exec(char *exec)
{
   char *token;

   token = strdup(exec);
    /* Strip Caption From Exec */
   if (strstr(token, "caption") != NULL)
     {
	token = strdup(token);
	token = strtok(token, " ");
     }

    /* Strip %U from Exec */
   if (strstr(token, "%") != NULL)
     {
	token = strdup(token);
	token = strtok(token, " ");
     }
   return strdup(token);
}

void parse_desktop_file(char *path)
{
   char *home, *eap_name, *key, *value;
   char buffer[MAX_PATH];
   int length, overwrite;
   FILE *file;
   G_Eap *eap;

   home = get_home();
   overwrite = get_overwrite();
   eap_name = get_eap_name(path);

#ifdef DEBUG
   fprintf(stderr, "Parsing Desktop File %s\n", path);
#endif

   file = fopen(path, "r");
   if (!file)
     {
	fprintf(stderr, "ERROR: Cannot Open File %s \n", path);
	return;
     }

   eap = calloc(1, sizeof(G_Eap));
   eap->eap_name = strdup(eap_name);
   if (eap_name) free(eap_name);

   key = NULL;
   value = NULL;
   *buffer = 0;

    /* Read All Entries, Store In Struct */
   while (fgets(buffer, sizeof(buffer), file) != NULL)
     {
	/* Skip Blank Lines */
	if (!(*buffer) || (*buffer == '\n')) continue;
	/* Strip New Line Char */
	if (buffer[(length = strlen(buffer) - 1)] == '\n') buffer[length] = '\0';
	key = strtok(buffer, "=");
	value = strstr(buffer, "=");
	value = strtok(value, "=");
	if ((value != NULL) && (key != NULL))
	  {
	     if (!strcmp(key, "Name")) eap->name = strdup(value);
	     if (!strcmp(key, "GenericName")) eap->generic = strdup(value);
	     if (!strcmp(key, "Comment")) eap->comment = strdup(value);
	     if (!strcmp(key, "Categories")) eap->categories = strdup(value);
	     if (!strcmp(key, "Exec")) eap->exec = strdup(value);
	     if (!strcmp(key, "Icon")) eap->icon = strdup(value);
	     if (!strcmp(key, "StartupNotify")) eap->startup = (!strcmp(value, "true")) ? "1" : "0";
	     if (!strcmp(key, "X-KDE-StartupNotify")) eap->startup = (!strcmp(value, "true")) ? "1" : "0";
	     if (!strcmp(key, "Type")) eap->type = strdup(value);
	  }
	value = NULL;
	key = NULL;
     }
   fclose(file);

    /* Check If We Process */
   if (!eap->type) process_file(path, eap);
   if (eap->type)
     {
	if (!strcmp(eap->type, "Application")) process_file(path, eap);
     }

   /* Write Out Mapping File ? */
   if (get_mapping())
     {
	if ((!eap->icon) || (!eap->name) ||
	    (!eap->exec) || (!eap->window_class)) return;

	FILE *f;
	char buff[MAX_PATH];

	snprintf(path, sizeof(path), "%s/MAPPING", home);
	f = fopen(path, "a+");
	if (!f)
	  {
	     fprintf(stderr, "ERROR: Cannot Open Mapping File\n");
	     return;
	  }
	snprintf(buff, sizeof(buff), "%s|!%s|!%s|!%s\n",
		 eap->icon, eap->name, eap->window_class, eap->exec);
	fwrite(buff, sizeof(char), strlen(buff), f);
	fclose(f);
     }
   free(eap);
}

void process_file(char *file, G_Eap *eap)
{
   char *home, *window_class, *exec, *category, *icon;
   char path[MAX_PATH], order_path[MAX_PATH], buffer[MAX_PATH];
   int overwrite;

   if (!eap) return;

#ifdef DEBUG
   fprintf(stderr, "Processing File %s\n", file);
#endif

   home = get_home();
   overwrite = get_overwrite();

   snprintf(path, sizeof(path), "%s"EAPPDIR"/%s", home, eap->eap_name);
   window_class = get_window_class(path);

   if ((ecore_file_exists(path)) && (!overwrite))
     {
	category = NULL;
	if (eap->categories != NULL)
	  {
	     category = find_category(eap->categories);
	     if (category != NULL)
	       {
		  snprintf(order_path, sizeof(order_path), "%s"EFAVDIR"/%s", home, category);
		  modify_order(order_path, eap->eap_name);
	       }
	  }
	return;
     }

   if (eap->icon != NULL) icon = find_icon(strdup(eap->icon));
   if (eap->icon == NULL) icon = strdup(DEFAULTICON);

   /* Start Making The Eap */
   write_icon(path, icon);

   /* Set Eap Values. Trap For Name Not Being Set */
   if (eap->name != NULL)
     {
	write_eap(path, "app/info/name", eap->name);
     }
   else if (eap->eap_name != NULL)
     {
	write_eap(path, "app/info/name", eap->eap_name);
     }

   if (eap->generic != NULL) write_eap(path, "app/info/generic", eap->generic);
   if (eap->comment != NULL) write_eap(path, "app/info/comments", eap->comment);

   /* Parse Exec string for %'s that messup eap write */
   exec = NULL;
   if (eap->exec != NULL)
     {
	exec = parse_exec(eap->exec);
	if (exec != NULL)
	  {
	     write_eap(path, "app/info/exe", exec);
	     write_eap(path, "app/icon/class", exec);
	  }
     }

   if (eap->startup != NULL) write_eap(path, "app/info/startup_notify", eap->startup);
   if (window_class != NULL) write_eap(path, "app/window/class", window_class);

   snprintf(buffer, sizeof(buffer), "enlightenment_eapp_cache_gen /home/shad/.e/e/applications/all -r");
   system(buffer);

   category = NULL;
   if (eap->categories != NULL)
     {
	category = find_category(eap->categories);
	if (category != NULL)
	  {
	     snprintf(buffer, sizeof(buffer), "enlightenment_eapp_cache_gen /home/shad/.e/e/applications/favorite/%s -r", category);
	     system(buffer);
	     snprintf(order_path, sizeof(order_path), "%s"EFAVDIR"/%s", home, category);
	     modify_order(order_path, eap->eap_name);
	  }
     }
   return;
}

void parse_debian_file(char *file)
{
   char *eap_name, *name, *generic, *comment, *exec, *category, *icon;
   char buffer[MAX_PATH];
   int overwrite, length;
   FILE *f;
   G_Eap *eap;

   if (ecore_file_is_dir(file)) return;

   overwrite = get_overwrite();
   eap_name = get_eap_name(file);

#ifdef DEBUG
   fprintf(stderr, "Parsing Debian File %s\n", file);
#endif

   f = fopen(file, "r");
   if (!f)
     {
	fprintf(stderr, "ERROR: Cannot Open File %s\n", file);
	return;
     }

   eap = calloc(1, sizeof(G_Eap));
   eap->eap_name = eap_name;
   *buffer = 0;
   while (fgets(buffer, sizeof(buffer), f) != NULL)
     {
	/* Skip Blank Lines */
	if (!(*buffer) || (*buffer == '\n')) continue;
	/* Strip New Line Chars */
	if (buffer[(length = strlen(buffer) - 1)] == '\n') buffer[length] = '\0';
	if (strstr(buffer, "title"))
	  {
	     name = parse_buffer(strdup(buffer), "title=");
	     eap->name = strdup(name);
	  }
	if (strstr(buffer, "longtitle"))
	  {
	     generic = parse_buffer(strdup(buffer), "longtitle=");
	     eap->generic = strdup(generic);
	  }
	if (strstr(buffer, "description"))
	  {
	     comment = parse_buffer(strdup(buffer), "description=");
	     eap->comment = strdup(comment);
	  }
	if (strstr(buffer, "section"))
	  {
	     category = parse_buffer(strdup(buffer), "section=");
	     eap->categories = strdup(category);
	  }
	if (strstr(buffer, "command"))
	  {
	     exec = parse_buffer(strdup(buffer), "command=");
	     eap->exec = strdup(exec);
	  }
	if (strstr(buffer, "icon"))
	  {
	     icon = parse_buffer(strdup(buffer), "icon=");
	     eap->icon = strdup(icon);
	  }
     }
   buffer[0] = (char) 0;

   fclose(f);

   process_file(file, eap);
   free(eap);
}
