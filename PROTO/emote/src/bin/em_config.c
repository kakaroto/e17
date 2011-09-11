#include "em_global.h"

/* local function prototypes */
static void _em_config_cb_free(void);
static void *_em_config_domain_load(const char *domain, Em_Config_DD *edd);
static int _em_config_domain_save(const char *domain, Em_Config_DD *edd, const void *data);
static void _em_config_win_create(Evas_Object *parent);
static void _em_config_cb_win_del(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event __UNUSED__);
static void _em_config_cb_close(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event __UNUSED__);

/* local variables */
static Em_Config_DD *_em_config_edd = NULL;
static Evas_Object *_em_config_win;

/* global variables */
EM_INTERN Em_Config *em_config = NULL;

EM_INTERN Eina_Bool 
em_config_init(void)
{
   _em_config_edd = EM_CONFIG_DD_NEW("Em_Config", Em_Config);
   #undef T
   #undef D
   #define T Em_Config
   #define D _em_config_edd
   EM_CONFIG_VAL(D, T, version, EET_T_INT);

   em_config_load();
   em_config_save();
   return EINA_TRUE;
}

EM_INTERN Eina_Bool 
em_config_shutdown(void)
{
   _em_config_cb_free();
   EM_CONFIG_DD_FREE(_em_config_edd);
   return EINA_TRUE;
}

EM_INTERN Em_Config_DD *
em_config_descriptor_new(const char *name, int size)
{
   Eet_Data_Descriptor_Class eddc;

   if (!eet_eina_stream_data_descriptor_class_set(&eddc, sizeof(eddc),
                                                  name, size))
     return NULL;
   return (Em_Config_DD *)eet_data_descriptor_stream_new(&eddc);
}

EM_INTERN void
em_config_load(void)
{
   em_config = _em_config_domain_load("emote", _em_config_edd);
   if (em_config)
     {
        int reload;

        if ((em_config->version >> 16) < EM_CONFIG_FILE_EPOCH)
          {
             /* config too old */
             reload = 1;
          }
        else if (em_config->version > EM_CONFIG_FILE_VERSION)
          {
             /* config too new, WTF ? */
             reload = 1;
          }
        if (reload)
          em_config = _em_config_domain_load("emote", _em_config_edd);
     }
   if (!em_config) em_config = EM_NEW(Em_Config, 1);

   /* define some convenient macros */
#define IFCFG(v) if ((em_config->version & 0xffff) < (v)) {
#define IFCFGELSE } else {
#define IFCFGEND }

   /* setup defaults */
   IFCFG(0x008d);
   IFCFGEND;

   /* limit config values so they are sane */

   em_config->version = EM_CONFIG_FILE_VERSION;
}

EM_INTERN int
em_config_save(void)
{
   return _em_config_domain_save("emote", _em_config_edd, em_config);
}

EM_INTERN void
em_config_show(Evas_Object *parent)
{
   if (!_em_config_win) _em_config_win_create(parent);
   evas_object_show(_em_config_win);
   elm_win_activate(_em_config_win);
}

/* local functions */
static void
_em_config_cb_free(void)
{
   EM_FREE(em_config);
}

static void *
_em_config_domain_load(const char *domain, Em_Config_DD *edd)
{
   Eet_File *ef;
   char buff[PATH_MAX];

   if (!domain) return NULL;
   snprintf(buff, sizeof(buff),
            "%s/config/%s.cfg", em_util_user_dir_get(), domain);
   ef = eet_open(buff, EET_FILE_MODE_READ);
   if (ef)
     {
        void *data;

        data = eet_data_read(ef, edd, "config");
        eet_close(ef);
        if (data) return data;
     }
   return NULL;
}

static int
_em_config_domain_save(const char *domain, Em_Config_DD *edd, const void *data)
{
   Eet_File *ef;
   char buff[PATH_MAX];
   const char *userdir;
   int ret = 0;

   if (!domain) return 0;
   userdir = em_util_user_dir_get();
   snprintf(buff, sizeof(buff), "%s/config/", userdir);
   if (!ecore_file_exists(buff)) ecore_file_mkpath(buff);
   snprintf(buff, sizeof(buff), "%s/config/%s.tmp", userdir, domain);
   ef = eet_open(buff, EET_FILE_MODE_WRITE);
   if (ef)
     {
        char buff2[PATH_MAX];
        int err;

        snprintf(buff2, sizeof(buff2), "%s/config/%s.cfg", userdir, domain);
        ret = eet_data_write(ef, edd, "config", data, 1);
        err = eet_close(ef);
        ret = ecore_file_mv(buff, buff2);
     }
   return ret;
}

static void
_em_config_win_create(Evas_Object *parent)
{
   Evas_Object *o, *bx, *bbx, *tb;

   _em_config_win =
     elm_win_add(parent, "emote::config", ELM_WIN_DIALOG_BASIC);
   elm_win_title_set(_em_config_win, _("Emote - Configuration"));
   elm_win_keyboard_mode_set(_em_config_win, ELM_WIN_KEYBOARD_ALPHA);
   evas_object_smart_callback_add(_em_config_win, "delete-request",
                                  _em_config_cb_win_del, NULL);

   o = elm_bg_add(_em_config_win);
   evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_win_resize_object_add(_em_config_win, o);
   evas_object_show(o);

   bx = elm_box_add(_em_config_win);
   elm_box_horizontal_set(bx, EINA_FALSE);
   elm_box_homogeneous_set(bx, EINA_FALSE);
   evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(bx, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_win_resize_object_add(_em_config_win, bx);
   evas_object_show(bx);

   tb = elm_toolbar_add(_em_config_win);
   elm_toolbar_icon_size_set(tb, 16);
   elm_toolbar_align_set(tb, 0.0);
//   elm_toolbar_scrollable_set(tb, EINA_TRUE);
   evas_object_size_hint_weight_set(tb, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(tb, EVAS_HINT_FILL, 0.5);
   elm_box_pack_end(bx, tb);
   evas_object_show(tb);

   elm_toolbar_item_append(tb, "preferences-system", _("General"), NULL, NULL);
   elm_toolbar_item_append(tb, "go-home", _("Servers"), NULL, NULL);

   bbx = elm_box_add(_em_config_win);
   elm_box_horizontal_set(bbx, EINA_TRUE);
   elm_box_homogeneous_set(bbx, EINA_TRUE);
   evas_object_size_hint_weight_set(bbx, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(bbx, EVAS_HINT_FILL, 0.5);
   elm_box_pack_end(bx, bbx);
   evas_object_show(bbx);

   o = elm_button_add(_em_config_win);
   elm_object_text_set(o, _("Apply"));
   elm_box_pack_end(bbx, o);
   evas_object_show(o);

   o = elm_button_add(_em_config_win);
   elm_object_text_set(o, _("Close"));
   evas_object_smart_callback_add(o, "clicked", _em_config_cb_close, NULL);
   elm_box_pack_end(bbx, o);
   evas_object_show(o);

   evas_object_size_hint_min_set(_em_config_win, 200, 100);
   evas_object_resize(_em_config_win, 200, 100);
}

static void
_em_config_cb_win_del(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event __UNUSED__)
{
   evas_object_del(_em_config_win);
   _em_config_win = NULL;
}

static void
_em_config_cb_close(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event __UNUSED__)
{
   evas_object_del(_em_config_win);
   _em_config_win = NULL;
}
