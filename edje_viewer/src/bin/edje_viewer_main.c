/*
 * vim:ts=8:sw=3:sts=8:et:cino=>5n-3f0^-2{20(W4
 */

#include "edje_viewer_main.h"

static void viewer_free(Viewer *v);
static void config_init(Viewer *v);
static Eina_Hash * eet_eina_hash_add_alloc(void *h, const void *key, void *data);
static int config_load(Viewer *v);
static void config_free(Viewer *v);
static int on_config_save_timer(void *data);

int
elm_main(int argc, char **argv)
{
   Viewer *v;

   v = calloc(1, sizeof(Viewer));

   config_init(v);
   if (argc >= 2)
     eina_stringshare_replace(&(v->config->edje_file), argv[1]);

   if (!v->config->edje_file || !ecore_file_exists(v->config->edje_file))
     {
	ERR("Edje file not specified\n");
	viewer_free(v);
	return 1;
     }

   create_main_win(v);
   open_edje_file(v);

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

static void
viewer_free(Viewer *v)
{
   eina_stringshare_del(v->config->edje_file);
   eina_stringshare_del(v->theme_file);
   free(v);
}

static void
config_init(Viewer *v)
{
   Eet_Data_Descriptor_Class eddc;
   
   eddc.version = EET_DATA_DESCRIPTOR_CLASS_VERSION;
   eddc.func.mem_alloc = NULL;
   eddc.func.mem_free = NULL;
   eddc.func.str_alloc = (char *(*)(const char *)) eina_stringshare_add;
   eddc.func.str_free = (void (*)(const char *)) eina_stringshare_del;
   eddc.func.list_next = (void *(*)(void *)) eina_list_next;
   eddc.func.list_append = (void *(*)(void *l, void *d)) eina_list_append;
   eddc.func.list_data = (void *(*)(void *)) eina_list_data_get;
   eddc.func.list_free = (void *(*)(void *)) eina_list_free;
   eddc.func.hash_foreach =
      (void  (*) (void *, int (*) (void *, const char *, void *, void *), void *))
      eina_hash_foreach;
   eddc.func.hash_add = (void* (*) (void *, const char *, void *)) eet_eina_hash_add_alloc;
   eddc.func.hash_free = (void  (*) (void *)) eina_hash_free;
   eddc.name = "Config";
   eddc.size = sizeof(Config);

   v->config_edd = eet_data_descriptor2_new(&eddc);
#undef T
#undef D
#define T Config 
#define D v->config_edd
#define C_VAL(edd, type, member, dtype) EET_DATA_DESCRIPTOR_ADD_BASIC(edd, type, #member, member, dtype)
   C_VAL(D, T, config_version, EET_T_INT);
   C_VAL(D, T, show_parts, EET_T_CHAR);
   C_VAL(D, T, edje_file, EET_T_STRING);

   switch (config_load(v))
     {
      case 0:
	 v->config->show_parts = 1;
      case -1:
	 /* Incremental additions */
	 v->config->config_version = CONFIG_VERSION;
	 break;
      default:
	 return;
     }
   config_save(v, 0);
}

static Eina_Hash *
eet_eina_hash_add_alloc(void *h, const void *key, void *data)
{
   Eina_Hash *hash = h;
   if (!hash) hash = eina_hash_string_superfast_new(NULL);
   if (!hash) return NULL;
   eina_hash_add(hash, key, data);
   return hash;
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
        config_free(v);
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
   FREE(v->config);
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
