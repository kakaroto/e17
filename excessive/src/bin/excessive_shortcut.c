#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "excessive_private.h"

typedef struct _Excessive_Shortcut Excessive_Shortcut;
struct _Excessive_Shortcut
{
   Evas_Object *list;
   Efreet_Desktop *ef;

   char *target;
};

static char *
_excessive_shortcut_favorite_label_get(void *data __UNUSED__, Evas_Object *obj __UNUSED__, const char *part __UNUSED__)
{
   return strdup("Favorites");
}

static Evas_Object *
_excessive_shortcut_favorite_icon_get(void *data __UNUSED__, Evas_Object *obj, const char *part __UNUSED__)
{
   Evas_Object *ic = elm_icon_add(obj);

   elm_icon_standard_set(ic, "favorites");
   return ic;
}

static Elm_Genlist_Item_Class itc_favorite_group = {
  "group_index",
  {
    _excessive_shortcut_favorite_label_get,
    _excessive_shortcut_favorite_icon_get,
    NULL,
    NULL,
    NULL
  },
  NULL
};

static char *
_excessive_shortcut_desktop_label_get(void *data, Evas_Object *obj __UNUSED__, const char *part __UNUSED__)
{
   Excessive_Shortcut *es = data;

   return strdup(es->ef->name);
}

static Evas_Object *
_excessive_shortcut_desktop_icon_get(void *data __UNUSED__, Evas_Object *obj, const char *part __UNUSED__)
{
   Excessive_Shortcut *es = data;
   Evas_Object *ic = elm_icon_add(obj);

   elm_icon_standard_set(ic, es->ef->icon);
   return ic;
}

static void
_excessive_shortcut_desktop_select(void *data, Evas_Object *obj __UNUSED__, void *event_info)
{
   Elm_Genlist_Item *item = event_info;
   Excessive_Shortcut *es;
   Evas_Object *grid = data;

   es = elm_genlist_item_data_get(item);
   elm_genlist_item_selected_set(item, EINA_FALSE);
   if (!es) return ;
   excessive_browse_directory(grid, es->target);
}

static Elm_Genlist_Item_Class itc_desktop_group = {
  "default",
  {
    _excessive_shortcut_desktop_label_get,
    _excessive_shortcut_desktop_icon_get,
    NULL,
    NULL,
    NULL
  },
  NULL
};

static void
_excessive_shortcut_cleanup(Excessive_Shortcut *es)
{
   efreet_desktop_free(es->ef);
   free(es->target);
   free(es);
}

static void
_desktop_stat_ok(void *data, Eio_File *handler __UNUSED__, const struct stat *st)
{
   Excessive_Shortcut *es = data;
   Elm_Genlist_Item *egi;

   if (!eio_file_is_dir(st))
     {
        _excessive_shortcut_cleanup(es);
        return ;
     }

   egi = evas_object_data_get(es->list, "excessive/favorites");

   elm_genlist_item_append(es->list, &itc_desktop_group, es, egi, ELM_GENLIST_ITEM_NONE, NULL, NULL);
}

static void
_desktop_stat_error(void *data, Eio_File *handler __UNUSED__, int error __UNUSED__)
{
   _excessive_shortcut_cleanup(data);
}

static Eina_Bool
_desktop_filter_cb(void *data __UNUSED__, Eio_File *handler __UNUSED__, const char *file)
{
   if (!strcmp(file + eina_stringshare_strlen(file) - 8, ".desktop"))
     return EINA_TRUE;
   return EINA_FALSE;
}

static char *
_desktop_flatten_env(const char *file)
{
   Eina_Strbuf *buf;
   int i, length;
   char *steal;

   length = strlen(file);
   buf = eina_strbuf_new();

   for (i = 0; i < length; ++i)
     {
        if (file[i] == '$')
          {
             char *tmp;
             int j = ++i;

             for (; i < length && (isalnum(file[i]) || file[i] == '_'); ++i)
               ;

             if (i > length)
               break ;

             if (i - j <= 0) continue ;

             tmp = alloca(i - j + 1);
             memcpy(tmp, file + j, i - j);
             tmp[i - j] = '\0';

             if (getenv(tmp))
               eina_strbuf_append(buf, getenv(tmp));

             i--;
          }
        else
          {
             eina_strbuf_append_char(buf, file[i]);
          }
     }

   steal = eina_strbuf_string_steal(buf);
   eina_strbuf_free(buf);

   return steal;
}

static void
_desktop_main_cb(void *data, Eio_File *handler __UNUSED__, const char *file)
{
   Excessive_Shortcut *es;
   Efreet_Desktop *ef;
   char *replace;

   ef = efreet_desktop_get(file);
   if (!ef) return ;

   if (ef->type != EFREET_DESKTOP_TYPE_LINK)
     goto end;

   if (!ef->url)
     goto end;

   if (strncmp(ef->url, "file:", 5))
     goto end;

   replace = _desktop_flatten_env(ef->url + 5);

   es = malloc(sizeof (Excessive_Shortcut));
   if (!es) goto nomem;

   es->ef = ef;
   es->target = replace;
   es->list = data;

   eio_file_direct_stat(replace, _desktop_stat_ok, _desktop_stat_error, es);
   return ;

 nomem:
   free(replace);

 end:
   efreet_desktop_free(ef);
}

static void
_desktop_end_cb(void *data __UNUSED__, Eio_File *handler __UNUSED__)
{
}

static void
_desktop_error_cb(void *data __UNUSED__, Eio_File *handler __UNUSED__, int error __UNUSED__)
{
}

Eina_Bool
excessive_shortcut_init(Evas_Object *list, Evas_Object *grid)
{
   Elm_Genlist_Item *egi;

   if (!list) return EINA_FALSE;

   egi = elm_genlist_item_append(list, &itc_favorite_group, NULL, NULL,
                                 ELM_GENLIST_ITEM_GROUP, NULL, NULL);
   evas_object_data_set(list, "excessive/favorites", egi);

   evas_object_smart_callback_add(list, "selected", _excessive_shortcut_desktop_select, grid);

   eio_file_ls("/home/cedric/.e/e/fileman/favorites/",
               _desktop_filter_cb,
               _desktop_main_cb,
               _desktop_end_cb,
               _desktop_error_cb,
               list);

   return EINA_TRUE;
}

Eina_Bool
excessive_shortcut_shutdown(void)
{
   return EINA_TRUE;
}
