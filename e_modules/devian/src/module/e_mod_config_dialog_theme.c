#include "dEvian.h"

#define THEME_GENERAL 0
#define THEME_PICTURE 1
#define THEME_RSS 2
#define THEME_POPUP 3

static void *_create_data(E_Config_Dialog *cfd);
static void _fill_data(E_Config_Dialog_Data *cfdata);
static void _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static Evas_Object *_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int _basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static Evas_Object *_advanced_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int _advanced_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);

static void _ilist_themes_general_append(E_Config_Dialog *cfd, Evas_Object *il, Evas *evas);

#ifdef HAVE_PICTURE
static void _ilist_themes_picture_append(E_Config_Dialog *cfd, Evas_Object *il, Evas *evas);
#endif
#ifdef HAVE_RSS
static void _ilist_themes_rss_append(E_Config_Dialog *cfd, Evas_Object *il, Evas *evas);
#endif
#ifdef HAVE_FILE
static void _ilist_themes_file_append(E_Config_Dialog *cfd, Evas_Object *il, Evas *evas);
#endif
static void _ilist_themes_popup_append(E_Config_Dialog *cfd, Evas_Object *il, Evas *evas);
static void _ilist_cb_theme_general_selected(void *data);

#ifdef HAVE_PICTURE
static void _ilist_cb_theme_picture_selected(void *data);
#endif
#ifdef HAVE_RSS
static void _ilist_cb_theme_rss_selected(void *data);
#endif
#ifdef HAVE_FILE
static void _ilist_cb_theme_file_selected(void *data);
#endif
static void _ilist_cb_theme_popup_selected(void *data);

static Evas_List *_theme_list_gen(void);
static int _is_theme_valid(char *path);
static void _invalid_theme_error_msg(void);

typedef struct _dEvian_Theme dEvian_Theme;

struct _dEvian_Theme
{
   int valid;
   const char *name;
   const char *file;
   Evas_Object *icon;
};

struct _E_Config_Dialog_Data
{
   /* list of devian_theme */
   Evas_List *themes;
   /* path for icons */
   char *icon_ok;

   /* basic */
   char *theme_general;
   /* advanced */
   char *theme_rss;
   char *theme_picture;
   char *theme_file;
   char *theme_popup;
};


/* PUBLIC FUNCTIONS */

E_Config_Dialog *DEVIANF(config_dialog_theme) (void)
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_View *v;
   char buf[4096];

   /* if was alreay open, return */
   if (DEVIANM->dialog_conf_theme)
     if (!e_object_is_del(E_OBJECT(DEVIANM->dialog_conf_theme)))
       if (e_object_ref_get(E_OBJECT(DEVIANM->dialog_conf_theme)) > 0)
         return NULL;

   v = E_NEW(E_Config_Dialog_View, 1);

   /* methods */
   v->create_cfdata = _create_data;
   v->free_cfdata = _free_data;
   v->basic.apply_cfdata = _basic_apply_data;
   v->basic.create_widgets = _basic_create_widgets;
   v->advanced.apply_cfdata = _advanced_apply_data;
   v->advanced.create_widgets = _advanced_create_widgets;

   snprintf(buf, sizeof(buf), "%s/module.eap", e_module_dir_get(DEVIANM->module));
   cfd = e_config_dialog_new(DEVIANM->container, _(MODULE_NAME " Module Configuration - Theme"), "dEvian", "_e_modules_devian_theme_config_dialog", buf, 0, v, NULL);

   e_object_ref(E_OBJECT(cfd));

   return cfd;
}

void DEVIANF(config_dialog_theme_shutdown) (void)
{
   if (DEVIANM->dialog_conf_theme)
     {
        while (e_object_unref(E_OBJECT(DEVIANM->dialog_conf_theme)) > 0);
        DEVIANM->dialog_conf_theme = NULL;
     }
}


/* PRIVATE FUNCTIONS */

static void *
_create_data(E_Config_Dialog *cfd)
{
   E_Config_Dialog_Data *cfdata;

   cfdata = E_NEW(E_Config_Dialog_Data, 1);

   cfdata->themes = NULL;
   cfdata->icon_ok = NULL;
   cfdata->theme_general = NULL;
#ifdef HAVE_PICTURE
   cfdata->theme_picture = NULL;
#endif
#ifdef HAVE_RSS
   cfdata->theme_rss = NULL;
#endif
#ifdef HAVE_FILE
   cfdata->theme_file = NULL;
#endif
   cfdata->theme_popup = NULL;

   _fill_data(cfdata);

   return cfdata;
}

static void
_fill_data(E_Config_Dialog_Data *cfdata)
{
   char buf[DEVIAN_MAX_PATH];

   cfdata->themes = _theme_list_gen();

   snprintf(buf, sizeof(buf), "%s/%s", e_module_dir_get(DEVIANM->module), "module_icon.png");
   cfdata->icon_ok = strdup(buf);

#ifdef HAVE_PICTURE
   cfdata->theme_general = strdup(DEVIANM->conf->theme_picture);
#else
#ifdef HAVE_RSS
   cfdata->theme_general = strdup(DEVIANM->conf->theme_rss);
#else
#ifdef HAVE_FILE
   cfdata->theme_general = strdup(DEVIANM->conf->theme_file);
#else
   cfdata->theme_general = strdup(DEVIANM->conf->theme_popup);  /* ...TODO: change with file theme */
#endif
#endif
#endif
#ifdef HAVE_PICTURE
   cfdata->theme_picture = strdup(DEVIANM->conf->theme_picture);
#endif
#ifdef HAVE_RSS
   cfdata->theme_rss = strdup(DEVIANM->conf->theme_rss);
#endif
#ifdef HAVE_FILE
   cfdata->theme_file = strdup(DEVIANM->conf->theme_file);
#endif
   cfdata->theme_popup = strdup(DEVIANM->conf->theme_popup);
}

static void
_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   Evas_List *l;
   dEvian_Theme *t;

   for (l = cfdata->themes; l; l = evas_list_next(l))
     {
        t = evas_list_data(l);
        if (t->icon)
          evas_object_del(t->icon);
        evas_stringshare_del(t->name);
     }

   E_FREE(cfdata->icon_ok);

   E_FREE(cfdata->theme_general);
#ifdef HAVE_PICTURE
   E_FREE(cfdata->theme_picture);
#endif
#ifdef HAVE_RSS
   E_FREE(cfdata->theme_rss);
#endif
#ifdef HAVE_FILE
   E_FREE(cfdata->theme_file);
#endif
   E_FREE(cfdata->theme_popup);

   E_FREE(cfdata);
}

static int
_basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   if (!cfdata->theme_general)
     {
        _invalid_theme_error_msg();
        return 0;
     }

#ifdef HAVE_PICTURE
   E_FREE(cfdata->theme_picture);
   cfdata->theme_picture = strdup(cfdata->theme_general);
#endif
#ifdef HAVE_RSS
   E_FREE(cfdata->theme_rss);
   cfdata->theme_rss = strdup(cfdata->theme_general);
#endif
#ifdef HAVE_FILE
   E_FREE(cfdata->theme_file);
   cfdata->theme_file = strdup(cfdata->theme_general);
#endif
   E_FREE(cfdata->theme_popup);
   cfdata->theme_popup = strdup(cfdata->theme_general);

#ifdef HAVE_PICTURE
   if (strcmp(cfdata->theme_general, DEVIANM->conf->theme_picture))
     {
        evas_stringshare_del(DEVIANM->conf->theme_picture);
        DEVIANM->conf->theme_picture = evas_stringshare_add(cfdata->theme_general);
        DEVIANF(container_theme_change) (NULL, SOURCE_PICTURE, &DEVIANM->conf->theme_picture);
     }
#endif
#ifdef HAVE_RSS
   if (strcmp(cfdata->theme_general, DEVIANM->conf->theme_rss))
     {
        evas_stringshare_del(DEVIANM->conf->theme_rss);
        DEVIANM->conf->theme_rss = evas_stringshare_add(cfdata->theme_general);
        DEVIANF(container_theme_change) (NULL, SOURCE_RSS, &DEVIANM->conf->theme_rss);
     }
#endif
#ifdef HAVE_FILE
   if (strcmp(cfdata->theme_general, DEVIANM->conf->theme_file))
     {
        evas_stringshare_del(DEVIANM->conf->theme_file);
        DEVIANM->conf->theme_file = evas_stringshare_add(cfdata->theme_general);
        DEVIANF(container_theme_change) (NULL, SOURCE_FILE, &DEVIANM->conf->theme_file);
     }
#endif
   if (strcmp(cfdata->theme_general, DEVIANM->conf->theme_popup))
     {
        evas_stringshare_del(DEVIANM->conf->theme_popup);
        DEVIANM->conf->theme_popup = evas_stringshare_add(cfdata->theme_general);
        DEVIANF(popup_warn_theme_change) ();
     }

   e_config_save_queue();
   return 1;
}

static int
_advanced_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   if (
#ifdef HAVE_PICTURE
       !cfdata->theme_picture ||
#endif
#ifdef HAVE_RSS
       !cfdata->theme_rss ||
#endif
#ifdef HAVE_FILE
       !cfdata->theme_file ||
#endif
       !cfdata->theme_popup)
     {
        _invalid_theme_error_msg();
        return 0;
     }

#ifdef HAVE_PICTURE
   if (strcmp(cfdata->theme_picture, DEVIANM->conf->theme_picture))
     {
        evas_stringshare_del(DEVIANM->conf->theme_picture);
        DEVIANM->conf->theme_picture = evas_stringshare_add(cfdata->theme_picture);
        DEVIANF(container_theme_change) (NULL, SOURCE_PICTURE, &DEVIANM->conf->theme_picture);
     }
#endif
#ifdef HAVE_RSS
   if (strcmp(cfdata->theme_rss, DEVIANM->conf->theme_rss))
     {
        evas_stringshare_del(DEVIANM->conf->theme_rss);
        DEVIANM->conf->theme_rss = evas_stringshare_add(cfdata->theme_rss);
        DEVIANF(container_theme_change) (NULL, SOURCE_RSS, &DEVIANM->conf->theme_rss);
     }
#endif
#ifdef HAVE_FILE
   if (strcmp(cfdata->theme_file, DEVIANM->conf->theme_file))
     {
        evas_stringshare_del(DEVIANM->conf->theme_file);
        DEVIANM->conf->theme_file = evas_stringshare_add(cfdata->theme_file);
        DEVIANF(container_theme_change) (NULL, SOURCE_FILE, &DEVIANM->conf->theme_file);
     }
#endif
   if (strcmp(cfdata->theme_popup, DEVIANM->conf->theme_popup))
     {
        evas_stringshare_del(DEVIANM->conf->theme_popup);
        DEVIANM->conf->theme_popup = evas_stringshare_add(cfdata->theme_popup);
        DEVIANF(popup_warn_theme_change) ();
     }

   e_config_save_queue();
   return 1;
}

static Evas_Object *
_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata)
{
   Evas_Object *o, *of, *ob;

   o = e_widget_table_add(evas, 0);
   of = e_widget_frametable_add(evas, _("General"), 0);

   ob = e_widget_ilist_add(evas, 20, 20, &(cfdata->theme_general));
   e_widget_ilist_selector_set(ob, 1);
   e_widget_min_size_set(ob, 100, 100);
   _ilist_themes_general_append(cfd, ob, evas);
   e_widget_ilist_go(ob);
   e_widget_frametable_object_append(of, ob, 0, 0, 1, 1, 1, 1, 1, 1);

   e_widget_table_object_append(o, of, 0, 0, 1, 1, 1, 1, 1, 1);

   return o;
}

static Evas_Object *
_advanced_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata)
{
   Evas_Object *o, *of, *ob;

   o = e_widget_table_add(evas, 0);

#ifdef HAVE_PICTURE
   of = e_widget_frametable_add(evas, _("Picture"), 0);

   ob = e_widget_ilist_add(evas, 20, 20, &(cfdata->theme_picture));
   e_widget_ilist_selector_set(ob, 1);
   e_widget_min_size_set(ob, 130, 100);
   _ilist_themes_picture_append(cfd, ob, evas);
   e_widget_ilist_go(ob);
   e_widget_frametable_object_append(of, ob, 0, 0, 1, 1, 1, 1, 1, 1);
   e_widget_table_object_append(o, of, 0, 0, 1, 1, 1, 1, 1, 1);
#endif

#ifdef HAVE_RSS
   of = e_widget_frametable_add(evas, _("Rss"), 0);

   ob = e_widget_ilist_add(evas, 20, 20, &(cfdata->theme_rss));
   e_widget_ilist_selector_set(ob, 1);
   e_widget_min_size_set(ob, 130, 100);
   _ilist_themes_rss_append(cfd, ob, evas);
   e_widget_ilist_go(ob);
   e_widget_frametable_object_append(of, ob, 0, 0, 1, 1, 1, 1, 1, 1);
   e_widget_table_object_append(o, of, 1, 0, 1, 1, 1, 1, 1, 1);
#endif

#ifdef HAVE_FILE
   of = e_widget_frametable_add(evas, _("Log"), 0);

   ob = e_widget_ilist_add(evas, 20, 20, &(cfdata->theme_file));
   e_widget_ilist_selector_set(ob, 1);
   e_widget_min_size_set(ob, 130, 100);
   _ilist_themes_file_append(cfd, ob, evas);
   e_widget_ilist_go(ob);
   e_widget_frametable_object_append(of, ob, 0, 0, 1, 1, 1, 1, 1, 1);
   e_widget_table_object_append(o, of, 2, 0, 1, 1, 1, 1, 1, 1);
#endif

   of = e_widget_frametable_add(evas, _("Popup"), 0);

   ob = e_widget_ilist_add(evas, 20, 20, &(cfdata->theme_popup));
   e_widget_ilist_selector_set(ob, 1);
   e_widget_min_size_set(ob, 130, 100);
   _ilist_themes_popup_append(cfd, ob, evas);
   e_widget_ilist_go(ob);
   e_widget_frametable_object_append(of, ob, 0, 0, 1, 1, 1, 1, 1, 1);

   e_widget_table_object_append(o, of, 3, 0, 1, 1, 1, 1, 1, 1);

   return o;
}

static void
_ilist_themes_general_append(E_Config_Dialog *cfd, Evas_Object *il, Evas *evas)
{
   Evas_List *l;
   Evas_Object *ico;
   dEvian_Theme *theme;
   E_Config_Dialog_Data *cfdata;
   int i = 0;

   cfdata = cfd->cfdata;

   e_widget_ilist_clear(il);
   for (l = cfdata->themes; l; l = evas_list_next(l))
     {
        theme = evas_list_data(l);
        ico = e_icon_add(evas);
        if (theme->valid)
          e_icon_file_set(ico, cfdata->icon_ok);
        e_widget_ilist_append(il, ico, (char *)theme->name, _ilist_cb_theme_general_selected, cfd, (char *)theme->file);
        if (!strcmp(theme->name,
#ifdef HAVE_PICTURE
                    DEVIANM->conf->theme_picture
#else
#ifdef HAVE_RSS
                    DEVIANM->conf->theme_rss
#else
#ifdef HAVE_FILE
                    DEVIANM->conf->theme_file
#else
                    DEVIANM->conf->theme_popup  /* ...TODO: change with file theme */
#endif
#endif
#endif
                    ))
          {
             e_widget_ilist_selected_set(il, i);
             _ilist_cb_theme_general_selected(cfd);
          }
        i++;
     }
}

#ifdef HAVE_PICTURE
static void
_ilist_themes_picture_append(E_Config_Dialog *cfd, Evas_Object *il, Evas *evas)
{
   Evas_List *l;
   Evas_Object *ico;
   dEvian_Theme *theme;
   E_Config_Dialog_Data *cfdata;
   int i = 0;

   cfdata = cfd->cfdata;

   e_widget_ilist_clear(il);
   for (l = cfdata->themes; l; l = evas_list_next(l))
     {
        theme = evas_list_data(l);
        ico = e_icon_add(evas);
        if (theme->valid)
          e_icon_file_set(ico, cfdata->icon_ok);
        e_widget_ilist_append(il, ico, (char *)theme->name, _ilist_cb_theme_picture_selected, cfd, (char *)theme->file);
        if (!strcmp(theme->name, DEVIANM->conf->theme_picture))
          {
             e_widget_ilist_selected_set(il, i);
             _ilist_cb_theme_picture_selected(cfd);
          }
        i++;
     }
}
#endif

#ifdef HAVE_RSS
static void
_ilist_themes_rss_append(E_Config_Dialog *cfd, Evas_Object *il, Evas *evas)
{
   Evas_List *l;
   Evas_Object *ico;
   dEvian_Theme *theme;
   E_Config_Dialog_Data *cfdata;
   int i = 0;

   cfdata = cfd->cfdata;

   e_widget_ilist_clear(il);
   for (l = cfdata->themes; l; l = evas_list_next(l))
     {
        theme = evas_list_data(l);
        ico = e_icon_add(evas);
        if (theme->valid)
          e_icon_file_set(ico, cfdata->icon_ok);
        e_widget_ilist_append(il, ico, (char *)theme->name, _ilist_cb_theme_rss_selected, cfd, (char *)theme->file);
        if (!strcmp(theme->name, DEVIANM->conf->theme_rss))
          {
             e_widget_ilist_selected_set(il, i);
             _ilist_cb_theme_rss_selected(cfd);
          }
        i++;
     }
}
#endif

#ifdef HAVE_FILE
static void
_ilist_themes_file_append(E_Config_Dialog *cfd, Evas_Object *il, Evas *evas)
{
   Evas_List *l;
   Evas_Object *ico;
   dEvian_Theme *theme;
   E_Config_Dialog_Data *cfdata;
   int i = 0;

   cfdata = cfd->cfdata;

   e_widget_ilist_clear(il);
   for (l = cfdata->themes; l; l = evas_list_next(l))
     {
        theme = evas_list_data(l);
        ico = e_icon_add(evas);
        if (theme->valid)
          e_icon_file_set(ico, cfdata->icon_ok);
        e_widget_ilist_append(il, ico, (char *)theme->name, _ilist_cb_theme_file_selected, cfd, (char *)theme->file);
        if (!strcmp(theme->name, DEVIANM->conf->theme_file))
          {
             e_widget_ilist_selected_set(il, i);
             _ilist_cb_theme_file_selected(cfd);
          }
        i++;
     }
}
#endif

static void
_ilist_themes_popup_append(E_Config_Dialog *cfd, Evas_Object *il, Evas *evas)
{
   Evas_List *l;
   Evas_Object *ico;
   dEvian_Theme *theme;
   E_Config_Dialog_Data *cfdata;
   int i = 0;

   cfdata = cfd->cfdata;

   e_widget_ilist_clear(il);
   for (l = cfdata->themes; l; l = evas_list_next(l))
     {
        theme = evas_list_data(l);
        ico = e_icon_add(evas);
        if (theme->valid)
          e_icon_file_set(ico, cfdata->icon_ok);
        e_widget_ilist_append(il, ico, (char *)theme->name, _ilist_cb_theme_popup_selected, cfd, (char *)theme->file);
        if (!strcmp(theme->name, DEVIANM->conf->theme_popup))
          {
             e_widget_ilist_selected_set(il, i);
             _ilist_cb_theme_popup_selected(cfd);
          }
        i++;
     }
}

static void
_ilist_cb_theme_general_selected(void *data)
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_Data *cfdata;

   cfd = data;
   cfdata = cfd->cfdata;

   DMAIN(("Selected: %s", cfdata->theme_general));
}

#ifdef HAVE_PICTURE
static void
_ilist_cb_theme_picture_selected(void *data)
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_Data *cfdata;

   cfd = data;
   cfdata = cfd->cfdata;

   DMAIN(("Selected: %s", cfdata->theme_picture));
}
#endif

#ifdef HAVE_RSS
static void
_ilist_cb_theme_rss_selected(void *data)
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_Data *cfdata;

   cfd = data;
   cfdata = cfd->cfdata;

   DMAIN(("Selected: %s", cfdata->theme_rss));
}
#endif

#ifdef HAVE_FILE
static void
_ilist_cb_theme_file_selected(void *data)
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_Data *cfdata;

   cfd = data;
   cfdata = cfd->cfdata;

   DMAIN(("Selected: %s", cfdata->theme_file));
}
#endif

static void
_ilist_cb_theme_popup_selected(void *data)
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_Data *cfdata;

   cfd = data;
   cfdata = cfd->cfdata;

   DMAIN(("Selected: %s", cfdata->theme_popup));
}

static Evas_List *
_theme_list_gen(void)
{
   dEvian_Theme *theme;
   Ecore_List *list = NULL;
   Evas_List *themes = NULL;
   char *file, *ext;
   char buf[4096];
   char name[DEVIAN_MAX_PATH];
   char themes_path[DEVIAN_MAX_PATH];
   int name_l;

   snprintf(themes_path, DEVIAN_MAX_PATH, "%s%s", e_module_dir_get(DEVIANM->module), "/themes");

   if (!ecore_file_is_dir(themes_path))
     {
        snprintf(buf, sizeof(buf), _("<hilight>Directory %s doesnt exists.</hilight><br><br>" "No themes found !!!"), themes_path);
        e_module_dialog_show(DEVIANM->module, _(MODULE_NAME " Module Error"), buf);
        return NULL;
     }

   list = ecore_file_ls(themes_path);
   if (ecore_list_is_empty(list) || !list)
     {
        snprintf(buf, sizeof(buf), _("<hilight>Directory %s is empty !</hilight><br><br>" "No themes found !!!"), themes_path);
        e_module_dialog_show(DEVIANM->module, _(MODULE_NAME " Module Error"), buf);
        return NULL;
     }

   while ((file = (char *)ecore_list_next(list)))
     {
        theme = E_NEW(dEvian_Theme, 1);

        ext = strrchr(file, '.');
        if (!ext)
          continue;
        name_l = strlen(file) - strlen(ext);
        if (name_l <= 0)
          continue;
        strncpy(name, file, name_l);
        name[name_l] = '\0';

        snprintf(buf, sizeof(buf), "%s/%s", themes_path, file);

        theme->valid = _is_theme_valid(buf);
        theme->name = evas_stringshare_add(name);
        if (theme->valid)
          theme->file = evas_stringshare_add(name);
        else
          theme->file = NULL;
        theme->icon = NULL;

        themes = evas_list_append(themes, theme);
     }

   return themes;
}

static int
_is_theme_valid(char *path)
{
   char *version_c;
   int version;

   version_c = edje_file_data_get(path, "version");
   if (!version_c)
     return 0;
   version = atoi(version_c);
   if (version != CONFIG_THEME_VERSION)
     return 0;

   return 1;
}

static void
_invalid_theme_error_msg(void)
{
   char buf[4096];

   snprintf(buf, sizeof(buf),
            _("<hilight>Invalid theme</hilight><br><br>"
              "You have selected an invalid theme.<br>" "A valid theme has a dEvian icon in front of it."));
   e_module_dialog_show(DEVIANM->module, _(MODULE_NAME " Module Error"), buf);
}
