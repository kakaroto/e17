/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */

#include <edje_viewer_main.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#define CONF_VERSION 1

#define NEWD(str, typ) \
     eet_data_descriptor_new(str, sizeof(typ), \
				(void *(*) (void *))eina_list_next, \
				(void *(*) (void *, void *))eina_list_append, \
				(void *(*) (void *))eina_list_data_get, \
				(void *(*) (void *))eina_list_free, \
				(void  (*) (void *, int (*) (void *, const char *, void *, void *), void *))eina_hash_foreach, \
				(void *(*) (void *, const char *, void *))eina_hash_add, \
				(void  (*) (void *))eina_hash_free)

#define CFG_OPTIONS_NEWB(str, it, type) EET_DATA_DESCRIPTOR_ADD_BASIC(_ev_config_edd, Edje_Viewer_Config, str, it, type)
#define CFG_OPTIONS_NEWL(str, it, subtype) EET_DATA_DESCRIPTOR_ADD_LIST(_ev_config_edd, Edje_Viewer_Config, str, it, subtype)

#define FREED(eed) \
	 if (eed) \
	     { \
		eet_data_descriptor_free((eed)); \
		(eed) = NULL; \
	     }

static Eet_Data_Descriptor *_ev_config_edd = NULL;

int edje_viewer_config_init(void)
{
   char     *home;
   char      buf[PATH_MAX];

   Eet_Data_Descriptor *recent;

   home = getenv("HOME");
   if(!home)
     {
	fprintf(stderr, "Can't find home directory!\n");
	return 0;
     }

   /* make sure ~/.e exists and is a dir */
   snprintf(buf, sizeof(buf), "%s/.e", home);
   if(!ecore_file_is_dir(buf))
     {
	if(ecore_file_exists(buf))
	  {
	     fprintf(stderr, "Can't create config path!\n");
	     return 0;
	  }

	if(!ecore_file_mkdir(buf))
	  {
	     fprintf(stderr, "Can't create config path!");
	     return 0;
	  }
     }

   /* make sure ~/.e/apps exists and is a dir */
   snprintf(buf, sizeof(buf), "%s/.e/apps", home);
   if(!ecore_file_is_dir(buf))
     {
	if(ecore_file_exists(buf))
	  {
	     fprintf(stderr, "Can't create config path!");
	     return 0;
	  }

	if(!ecore_file_mkdir(buf))
	  {
	     fprintf(stderr, "Can't create config path!");
	     return 0;
	  }
     }

   /* make sure ~/.e/apps/edje_viewer/ exists and is a dir */
   snprintf(buf, sizeof(buf), "%s/.e/apps/edje_viewer", home);
   if(!ecore_file_is_dir(buf))
     {
	if(ecore_file_exists(buf))
	  {
	     fprintf(stderr, "Can't create config path!");
	     return 0;
	  }

	if(!ecore_file_mkdir(buf))
	  {
	     fprintf(stderr, "Can't create config path!");
	     return 0;
	  }
     }

   _ev_config_edd = NEWD("Edje_Viewer_Config", Edje_Viewer_Config);

   CFG_OPTIONS_NEWB("config_version", config_version, EET_T_INT);
   CFG_OPTIONS_NEWB("open_last", open_last, EET_T_INT);
   CFG_OPTIONS_NEWB("sort_parts", sort_parts, EET_T_INT);
   CFG_OPTIONS_NEWL("recent", recent, _ev_config_edd);

   return 1;
}

int edje_viewer_config_shutdown(void)
{
   FREED(_ev_config_edd);
   return 1;
}

int edje_viewer_config_save(Gui *gui)
{
   Eet_File  *ef;
   char       buf[PATH_MAX];
   char      *home;
   int        ret;

   home = getenv("HOME");
   if(!home)
     return 0;

   snprintf(buf, sizeof(buf), "%s/.e/apps/edje_viewer/config.eet", home);
   
   ef = eet_open(buf, EET_FILE_MODE_WRITE);
   if(!ef)
     return 0;

   ret = eet_data_write(ef, _ev_config_edd, "config", gui->config, 1);
   if(!ret)
     fprintf(stderr, "Problem saving config!");

   eet_close(ef);
   return ret;   
}

int edje_viewer_config_load(Gui *gui)
{
   Eet_File *ef;
   char      buf[PATH_MAX];
   char     *home;
   
   home = getenv("HOME");
   if(!home)
     {
	fprintf(stderr, "Can't find home directory!\n");
	return -1;
     }

   snprintf(buf, sizeof(buf), "%s/.e/apps/edje_viewer/config.eet", home);

   if(!ecore_file_exists(buf) || ecore_file_size(buf) == 0)
     {
	/* no saved config */
	edje_viewer_config_defaults(gui);
	return 0;
     }
   
   ef = eet_open(buf, EET_FILE_MODE_READ);
   if(!ef)
     {
	fprintf(stderr, "Cant open configuration file! Using program defaults.\n");
	edje_viewer_config_defaults(gui);
	return 0;
     }

   gui->config = eet_data_read(ef, _ev_config_edd, "config");

   if ((!gui->config->config_version) || (gui->config->config_version != CONF_VERSION))
     {
	fprintf(stderr, "Incompatible configuration file! Creating new one.\n");
	eet_close(ef);
	edje_viewer_config_defaults(gui);
	return 0;
     }

   eet_close(ef);
   return 1;
}

void edje_viewer_config_defaults(Gui *gui)
{
   if (!gui->config)
     gui->config = calloc(1, sizeof(Edje_Viewer_Config));

   gui->config->config_version = CONF_VERSION;
   gui->config->open_last = 0;
   gui->config->sort_parts = 1;
   gui->config->recent = NULL;
}

void edje_viewer_config_recent_set(Gui *gui, const char *file)
{

   gui->config->recent = eina_list_prepend(gui->config->recent, strdup(file));
   if (10 == eina_list_count(gui->config->recent))
     {
	Eina_List *l;

	l = eina_list_last(gui->config->recent);

	gui->config->recent = eina_list_remove_list(gui->config->recent, l);
	free(l->data);
     }
}
