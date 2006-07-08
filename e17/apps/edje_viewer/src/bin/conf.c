/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */

#include "edje_viewer.h"

void edje_viewer_config_init(void)
{
   ecore_config_init("edje_viewer");
   edje_viewer_config_load();

   ecore_config_save();
}

void edje_viewer_config_load(void)
{
   ecore_config_int_default("/recent/count", 0);
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
   int count, key_length, i;
   char *key, *val, *cwd, *new_path;

   if (!path) return;
   count = edje_viewer_config_count_get();

   new_path = malloc(PATH_MAX);
   if (!(path[0] == '/')) {
	cwd = malloc(PATH_MAX);
	getcwd(cwd, PATH_MAX);

	snprintf(new_path, PATH_MAX, "%s/%s", cwd, path);

	FREE(cwd);
   } else {
	new_path = strdup(path);
   }

   key_length = strlen("/recent/") + 8;
   key = malloc(key_length * sizeof(key));
   for (i = 0; i < count; i++)
     {
	snprintf(key, key_length, "/recent/%i", i + 1);
	val = ecore_config_string_get(key);
	if (!strcmp(new_path, val))
	  {
	     FREE(key);
	     FREE(new_path);
	     if (val) FREE(val);
	     return;
	  }
	if (val) FREE(val);
     }
   count++;
   i = count;

   if (count > 20) 
     {
	count = 20;
	i = 1;
     }

   edje_viewer_config_count_set(count);
   snprintf(key, key_length, "/recent/%i", i);
   ecore_config_string_set(key, new_path);

   FREE(key);
   FREE(new_path);
}
