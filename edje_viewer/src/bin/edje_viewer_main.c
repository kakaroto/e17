/*
 * vim:ts=8:sw=3:sts=8:et:cino=>5n-3f0^-2{2(0W4
 */

#include "edje_viewer_main.h"

static void viewer_free(Viewer *v);
static void config_init(Viewer *v);
static int config_load(Viewer *v);
static void config_free(Viewer *v);
static void config_groups_free(Viewer *v);
static int on_config_save_timer(void *data);

int
elm_main(int argc, char **argv)
{
   Viewer *v;

   v = calloc(1, sizeof(Viewer));

   config_init(v);
   if (argc >= 2)
     {
        if ((argc > 2) || strcmp(v->config->edje_file, argv[1]))
          config_groups_free(v);
        eina_stringshare_replace(&(v->config->edje_file), argv[1]);
     }

   if (!v->config->edje_file || !ecore_file_exists(v->config->edje_file))
     {
	ERR("Edje file not specified\n");
	viewer_free(v);
	return 1;
     }

   create_main_win(v);
   open_edje_file(v);

   if (argc > 2)
     {
        int i;

        for (i = 2; i < argc; i++)
          edje_group_activate(v, argv[i]);
     }

   elm_run();

   viewer_free(v);

   elm_shutdown();
   return 0;
}

void
config_save(Viewer *v, Eina_Bool immediate)
{
   if (v->config_save_timer)
     ecore_timer_del(v->config_save_timer);
   if (immediate)
     on_config_save_timer(v);
   else
     v->config_save_timer = ecore_timer_add(30, on_config_save_timer, v);
}

void
config_group_change(Viewer *v, const char *name, Eina_Bool visible, Eina_Bool del)
{
   Config_Group *grp;

   grp = config_group_get(v, name);
   if (grp)
     {
        if (del)
          {
             v->config->groups = eina_list_remove(v->config->groups, grp);
             return;
          }

        grp->visible = visible;
        return;
     }

   if (del) return;

   grp = calloc(1, sizeof(Config_Group));
   grp->name = eina_stringshare_ref(name);
   grp->visible = visible;
   v->config->groups = eina_list_append(v->config->groups, grp);

   config_save(v, 0);
}

Config_Group *
config_group_get(Viewer *v, const char *name)
{
   Config_Group *grp;
   Eina_List *l;

   EINA_LIST_FOREACH(v->config->groups, l, grp)
     {
        if (!strcmp(name, grp->name))
          return grp;
     }

   return NULL;
}

static void
viewer_free(Viewer *v)
{
   eina_stringshare_del(v->theme_file);

   eina_list_free(v->signals);
   eina_list_free(v->hoversel_items);

   if (v->config_edd)
     eet_data_descriptor_free(v->config_edd);

   if (v->groups_edd)
     eet_data_descriptor_free(v->groups_edd);

   config_free(v);

   free(v);
}

static void
config_init(Viewer *v)
{
   Eet_Data_Descriptor_Class eddc, eddc2;
   
   eddc2.version = eddc.version = EET_DATA_DESCRIPTOR_CLASS_VERSION;
   eddc2.func.mem_alloc = eddc.func.mem_alloc = NULL;
   eddc2.func.mem_free = eddc.func.mem_free = NULL;
   eddc2.func.str_alloc = eddc.func.str_alloc = (char *(*)(const char *)) eina_stringshare_add;
   eddc2.func.str_free = eddc.func.str_free = (void (*)(const char *)) eina_stringshare_del;
   eddc2.func.list_next = eddc.func.list_next = (void *(*)(void *)) eina_list_next;
   eddc2.func.list_append = eddc.func.list_append = (void *(*)(void *l, void *d)) eina_list_append;
   eddc2.func.list_data = eddc.func.list_data = (void *(*)(void *)) eina_list_data_get;
   eddc2.func.list_free = eddc.func.list_free = (void *(*)(void *)) eina_list_free;
   eddc2.func.hash_foreach = eddc.func.hash_foreach = NULL;
   eddc2.func.hash_add = eddc.func.hash_add = NULL;
   eddc2.func.hash_free = eddc.func.hash_free = NULL;
   eddc.name = "Config";
   eddc.size = sizeof(Config);
   eddc2.name = "Groups";
   eddc2.size = sizeof(Config_Group);

#define C_VAL(edd, type, member, dtype) EET_DATA_DESCRIPTOR_ADD_BASIC(edd, type, #member, member, dtype)
#define C_LIST(edd, type, member, dtype) EET_DATA_DESCRIPTOR_ADD_LIST(edd, type, #member, member, dtype)

   v->groups_edd = eet_data_descriptor2_new(&eddc2);
#undef T
#undef D
#define T Config_Group
#define D v->groups_edd
   C_VAL(D, T, name, EET_T_STRING);
   C_VAL(D, T, visible, EET_T_CHAR);

   v->config_edd = eet_data_descriptor2_new(&eddc);
#undef T
#undef D
#define T Config 
#define D v->config_edd
   C_VAL(D, T, config_version, EET_T_INT);
   C_VAL(D, T, show_parts, EET_T_CHAR);
   C_VAL(D, T, show_signals, EET_T_CHAR);
   C_VAL(D, T, edje_file, EET_T_STRING);
   C_LIST(D, T, groups, v->groups_edd); /* the list */

   switch (config_load(v))
     {
      case 0:
	 v->config->show_parts = 1;
	 v->config->show_signals = 1;
      case -1:
	 /* Incremental additions */
	 v->config->config_version = CONFIG_VERSION;
	 break;
      default:
	 return;
     }
   config_save(v, 0);
}

static int
config_load(Viewer *v)
{
   Eet_File *ef;
   char buf[4096], buf2[4096];

   snprintf(buf2, sizeof(buf2), "%s/.config/edje_viewer", getenv("HOME"));
   ecore_file_mkpath(buf2);
   snprintf(buf, sizeof(buf), "%s/edje_viewer.cfg", buf2);

   ef = eet_open(buf, EET_FILE_MODE_READ);
   if (!ef)
     {
        v->config = calloc(1, sizeof(Config));
        return 0;
     }

   v->config = eet_data_read(ef, v->config_edd, "config");
   eet_close(ef);

   if (v->config->config_version > CONFIG_VERSION)
     {
        config_free(v);
        v->config = calloc(1, sizeof(Config));
        return 0;
     }

   if (v->config->config_version < CONFIG_VERSION)
     return -1;

   return 1;
}

static void
config_free(Viewer *v)
{
   config_groups_free(v);
   eina_stringshare_del(v->config->edje_file);
   FREE(v->config);
}

static void
config_groups_free(Viewer *v)
{
   Config_Group *grp;

   EINA_LIST_FREE(v->config->groups, grp)
     {
        eina_stringshare_del(grp->name);
        FREE(grp);
     }
}

static int
on_config_save_timer(void *data)
{
   Viewer *v = data;
   Eet_File *ef;
   char buf[4096], buf2[4096];
   int ret;

   snprintf(buf, sizeof(buf), "%s/.config/edje_viewer/edje_viewer.cfg", getenv("HOME"));
   snprintf(buf2, sizeof(buf2), "%s.tmp", buf);

   ef = eet_open(buf2, EET_FILE_MODE_WRITE);
   if (ef)
     {
        eet_data_write(ef, v->config_edd, "config", v->config, 1);

        if (eet_close(ef))
          goto save_end;

        ret = ecore_file_mv(buf2, buf);
        if (!ret)
          goto save_end;

        ecore_file_unlink(buf2);
     }

save_end:
   if (v->config_save_timer)
     ecore_timer_del(v->config_save_timer);
   v->config_save_timer = NULL;
   return ECORE_CALLBACK_CANCEL;
}

ELM_MAIN()
