/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <Ecore_Config.h>

#include "conf.h"

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#define FREE(ptr) do { if(ptr) { free(ptr); ptr = NULL; }} while (0);

void edje_viewer_config_init(void)
{
   ecore_config_init("edje_viewer");
   edje_viewer_config_load();

   ecore_config_save();
}

void edje_viewer_config_load(void)
{
   ecore_config_int_default("/recent/count", 0);
   ecore_config_int_default("/startup/open_last", 1);
   ecore_config_int_default("/startup/sort_parts", 1);
   ecore_config_load();
}

void edje_viewer_config_shutdown(void)
{
   ecore_config_save();
   ecore_config_shutdown();
}

int edje_viewer_config_count_get(void)
{
   int count = ecore_config_int_get("/recent/count");
   return count;
}

void edje_viewer_config_count_set(int count)
{
   if (count < 0) return;
   ecore_config_int_set("/recent/count", count);
}

char *edje_viewer_config_recent_get(int number)
{
   int key_length;
   char *key, *val;

   key_length = strlen("/recent/") + 4;
   key = malloc(key_length * sizeof(key));
   snprintf(key, key_length, "/recent/%i", number);
   val = ecore_config_string_get(key);

   FREE(key);
   return val;
}

void edje_viewer_config_recent_set(const char *path)
{
   int count, i;
   char key[sizeof("/recent/") + 8], *key_v, *val;
   char new_path[PATH_MAX];

   if (!path) return;
   count = edje_viewer_config_count_get();

   realpath(path, new_path);

   strcpy(key, "/recent/");
   key_v = key + sizeof("/recent/") - 1;
   for (i = 0; i < count; i++)
     {
	snprintf(key_v, 8, "%i", i + 1);
	val = ecore_config_string_get(key);
	if (!strcmp(new_path, val))
	  {
	     FREE(val);
	     return;
	  }
	FREE(val);
     }
   count++;
   i = count;

   if (count > 20) 
     {
	count = 20;
	i = 1;
     }

   edje_viewer_config_count_set(count);
   snprintf(key_v, 8, "%i", i);
   ecore_config_string_set(key, new_path);
}

char *edje_viewer_config_last_get(void)
{
   return ecore_config_string_get("/recent/last");
}

void edje_viewer_config_last_set(const char *path)
{
   char new_path[PATH_MAX];

   if (!path) return;

   realpath(path, new_path);
   ecore_config_string_set("/recent/last", new_path);
}

unsigned char edje_viewer_config_open_last_get(void)
{
   unsigned char check;
   check = ecore_config_int_get("/startup/open_last");

   return check;
}

void edje_viewer_config_open_last_set(unsigned char check)
{
   ecore_config_int_set("/startup/open_last", check);
}

unsigned char edje_viewer_config_sort_parts_get(void)
{
   unsigned char check;
   check = ecore_config_int_get("/startup/sort_parts");

   return check;
}

void edje_viewer_config_sort_parts_set(unsigned char check)
{
   ecore_config_int_set("/startup/sort_parts", check);
}
