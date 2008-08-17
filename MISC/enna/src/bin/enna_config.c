/*
 * enna_config.c
 * Copyright (C) Nicolas Aguirre 2006,2007,2008 <aguirre.nicolas@gmail.com>
 *
 * enna_config.c is free software copyrighted by Nicolas Aguirre.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name ``Nicolas Aguirre'' nor the name of any other
 *    contributor may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * enna_config.c IS PROVIDED BY Nicolas Aguirre ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL Nicolas Aguirre OR ANY OTHER CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "enna_config.h"
#include "enna_util.h"
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <Ecore.h>
#include <Ecore_Data.h>
#include <Ecore_File.h>
#include <Evas.h>
#include <string.h>
#include <stdio.h>

Enna_Config         config;
static char        *theme_filename = NULL;

EAPI char          *
enna_config_theme_get()
{

   return theme_filename;
}

EAPI void
enna_config_theme_set(char *theme_name)
{
   char                tmp[4096];

   if (!theme_filename)
     {
	if (theme_name)
	  {
	     sprintf(tmp, PACKAGE_DATA_DIR "/enna/theme/%s.edj", theme_name);
	     if (!ecore_file_exists(tmp))
	       {
		  char               *theme;

		  theme =
		     enna_config_get_conf_value_or_default("theme", "name",
							   "default");
		  sprintf(tmp, PACKAGE_DATA_DIR "/enna/theme/%s.edj", theme);
		  if (!ecore_file_exists(tmp))
		    {
		       theme_filename =
			  strdup(PACKAGE_DATA_DIR "/enna/theme/default.edj");
		       dbg("ERROR : theme define in enna.cfg (%s) does not exists on your system (%s)!\n", theme, tmp);
		       dbg("ERROR : Default theme is used instead.\n");
		    }
		  else
		     theme_filename = strdup(tmp);
	       }
	     else
		theme_filename = strdup(tmp);
	  }
	else
	  {
	     char               *theme;

	     theme =
		enna_config_get_conf_value_or_default("theme", "name",
						      "default");
	     sprintf(tmp, PACKAGE_DATA_DIR "/enna/theme/%s.edj", theme);
	     if (!ecore_file_exists(tmp))
	       {
		  theme_filename =
		     strdup(PACKAGE_DATA_DIR "/enna/theme/default.edj");
		  dbg("ERROR : theme define in enna.cfg (%s) does not exists on your system (%s)!\n", theme, tmp);
		  dbg("ERROR : Default theme is used instead.\n");
	       }
	     else
		theme_filename = strdup(tmp);
	  }
     }
   else
      dbg("Warning try to define new theme, but another theme is already in use : %s\n", theme_filename);

   dbg("Using theme : %s\n", theme_filename);

}

EAPI Evas_List     *
enna_config_theme_available_get(void)
{
   return NULL;
}

EAPI Evas_List     *
enna_config_extensions_get(char *type)
{

   if (!type)
      return NULL;

   if (!strcmp(type, "music"))
      return config.music_extensions;
   else if (!strcmp(type, "video"))
      return config.video_extensions;
   else if (!strcmp(type, "radio"))
      return config.radio_extensions;
   else if (!strcmp(type, "photo"))
      return config.photo_extensions;
   else
      return NULL;
}

EAPI void
enna_config_extensions_set(char *type, Evas_List * ext)
{
}

static struct conf_section *
enna_config_load_conf(char *conffile, int size)
{
   struct conf_section *current_section = NULL;
   struct conf_section *sections = NULL;
   char               *current_line = conffile;

   while (current_line < conffile + size)
     {
	char               *eol = strchr(current_line, '\n');

	if (eol)
	   *eol = 0;
	else			// Maybe the end of file
	   eol = conffile + size;

	// Removing the leading spaces
	while (*current_line && *current_line == ' ')
	   current_line++;

	// Maybe an empty line
	if (!(*current_line))
	  {
	     current_line = eol + 1;
	     continue;
	  }

	// Maybe a comment line
	if (*current_line == '#')
	  {
	     current_line = eol + 1;
	     continue;
	  }

	// We are at a section definition
	if (*current_line == '[')
	  {
	     // ']' must be the last char of this line
	     char               *end_of_section_name =
		strchr(current_line + 1, ']');
	     if (end_of_section_name[1] != 0)
	       {
		  dbg("malformed section name %s\n", current_line);
		  return NULL;
	       }
	     current_line++;
	     *end_of_section_name = '\0';

	     // Building the section
	     current_section = malloc(sizeof(*current_section));
	     current_section->section_name = strdup(current_line);
	     current_section->values = NULL;
	     current_section->next_section = NULL;
	     if (sections)
	       {
		  current_section->next_section = sections;
		  sections = current_section;
	       }
	     else
		sections = current_section;

	     current_line = eol + 1;
	     continue;

	  }

	// Must be in a section to provide a key/value pair
	if (!current_section)
	  {
	     dbg("No section for this line %s\n", current_line);
	     return NULL;
	  }

	// Building the key/value string pair
	char               *key = current_line;
	char               *value = strchr(current_line, '=');

	if (!value)
	  {
	     dbg("Malformed line %s\n", current_line);
	     return NULL;
	  }
	*value = '\0';
	value++;

	// Building the key/value pair
	struct conf_pair   *newpair = malloc(sizeof(*newpair));

	newpair->key = strdup(key);
	newpair->value = strdup(value);
	newpair->next_pair = current_section->values;
	current_section->values = newpair;

	current_line = eol + 1;
     }
   free(conffile);

   return sections;
}

static struct conf_section *
enna_config_load_conf_file(char *filename)
{
   int                 fd;
   FILE               *f;
   struct stat         st;
   char                tmp[4096];

   if (stat(filename, &st))
     {
	dbg("Cannot stat file %s\n", filename);
	sprintf(tmp, "%s/.enna", enna_util_user_home_get());
	if (!ecore_file_is_dir(tmp))
	   ecore_file_mkdir(tmp);

	if (!(f = fopen(filename, "w")))
	   return NULL;
	else
	  {
	     fprintf(f, "[tv_module]\n"
		     "used=0\n\n"
		     "[video_module]\n"
		     "base_path=%s\n"
		     "used=1\n\n"
		     "[music_module]\n"
		     "base_path=%s\n"
		     "used=1\n\n"
		     "[photo_module]\n"
		     "base_path=%s\n"
		     "used=1\n\n"
		     "[playlist_module]\n"
		     "base_path=%s\n"
		     "used=1\n\n"
		     "[theme]\n"
		     "name=default\n",
		     enna_util_user_home_get(),
		     enna_util_user_home_get(),
		     enna_util_user_home_get(), enna_util_user_home_get());
	     fclose(f);
	  }

     }
   if (stat(filename, &st))
     {
	dbg("Cannot stat file %s\n", filename);
	return NULL;
     }

   char               *conffile = malloc(st.st_size);

   if (!conffile)
     {
	dbg("Cannot malloc %d bytes\n", (int)st.st_size);
	return NULL;
     }

   if ((fd = open(filename, O_RDONLY)) < 0)
     {
	dbg("Cannot open file\n");
	return NULL;
     }

   int                 ret = read(fd, conffile, st.st_size);

   if (ret != st.st_size)
     {
	dbg("Cannot read conf file entirely, read only %d bytes\n", ret);
	return NULL;
     }

   return enna_config_load_conf(conffile, st.st_size);
}

EAPI void
enna_config_init(char *filename, char *theme_name)
{

   char                buf[4096];
   int                 i = 0;
   char               *me[] = { "ogg", "mp3", "aac", "wav", "flac" };
   char               *re[] = { "pls", "m3u" };
   char               *ve[] =
      { "avi", "mpg", "mpeg", "mkv", "ts", "mp4", "asf", "mov", "iso", "wmv",
"ogg" };
   char               *pe[] = { "jpg", "jpeg", "png", "tif" };

   sprintf(buf, "%s/.enna/enna.cfg", enna_util_user_home_get());

   config.sections = enna_config_load_conf_file((filename ? filename : buf));
   if (config.sections == NULL)
     {
	dbg("Cannot load conf file\n");
	exit(0);
     }
   /* if theme_name is defined, we load this theme */
   /* else is maybe define in enna.cfg file          */
   /* else we get default.edj as default theme     */

   enna_config_theme_set(theme_name);
   config.theme = enna_config_theme_get();
   config.music_extensions = NULL;
   config.video_extensions = NULL;
   config.photo_extensions = NULL;
   config.radio_extensions = NULL;
   for (i = 0; i < 5; i++)
      config.music_extensions =
	 evas_list_append(config.music_extensions, strdup(me[i]));

   for (i = 0; i < 11; i++)
      config.video_extensions =
	 evas_list_append(config.video_extensions, strdup(ve[i]));

   for (i = 0; i < 4; i++)
      config.photo_extensions =
	 evas_list_append(config.photo_extensions, strdup(pe[i]));

   for (i = 0; i < 2; i++)
      config.radio_extensions =
	 evas_list_append(config.radio_extensions, strdup(re[i]));

}

EAPI char          *
enna_config_get_conf_value(char *section_name, char *key_name)
{
   struct conf_section *sections = config.sections;

   while (sections)
     {
	if (!strcmp(section_name, sections->section_name))
	  {
	     struct conf_pair   *p = sections->values;

	     while (p)
	       {
		  if (!strcmp(p->key, key_name))
		     return p->value;
		  p = p->next_pair;
	       }
	     return NULL;
	  }
	sections = sections->next_section;
     }
   return NULL;
}
