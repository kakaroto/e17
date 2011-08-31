#include <Elementary.h>
#include <Eio.h>
#include "enna.h"


typedef struct _Enna_Shortcut Enna_Shortcut;
struct _Enna_Shortcut
{
   Evas_Object *list;
   Efreet_Desktop *ef;

   char *target;
};

static char *
_enna_shortcut_favorite_label_get(void *data __UNUSED__, Evas_Object *obj __UNUSED__, const char *part __UNUSED__)
{
   return strdup("Favorites");
}

static Evas_Object *
_enna_shortcut_favorite_icon_get(void *data __UNUSED__, Evas_Object *obj, const char *part)
{
   /* if (!strcmp(part, "elm.swallow.icon")) */
   /*   { */
   /*      Evas_Object *ic = elm_icon_add(obj); */
   /*      elm_icon_standard_set(ic, "emblem-favorite"); */
   /*      return ic; */
   /*   } */

   return NULL;
}

static Elm_Genlist_Item_Class itc_favorite_group = {
  "default",
  {
    _enna_shortcut_favorite_label_get,
    _enna_shortcut_favorite_icon_get,
    NULL,
    NULL,
    NULL
  },
  NULL
};

static char *
_enna_shortcut_desktop_label_get(void *data, Evas_Object *obj __UNUSED__, const char *part __UNUSED__)
{
   Enna_Shortcut *es = data;

   if (es && es->ef)
     return strdup(es->ef->name);
   else if (es)
     return strdup(ecore_file_file_get(es->target));
   else
     return strdup("Test");
}

static Evas_Object *
_enna_shortcut_desktop_icon_get(void *data __UNUSED__, Evas_Object *obj, const char *part __UNUSED__)
{
   Enna_Shortcut *es = data;
   Evas_Object *ic = elm_icon_add(obj);
   elm_icon_order_lookup_set(ic, ELM_ICON_LOOKUP_FDO_THEME);
   if (es && es->ef)
     elm_icon_standard_set(ic, es->ef->icon);
   else
     elm_icon_standard_set(ic, "emblem-favorite");
   return ic;
}

#if 0
static void
_enna_shortcut_desktop_select(void *data, Evas_Object *obj __UNUSED__, void *event_info)
{
   Elm_Genlist_Item *item = event_info;
   Enna_Shortcut *es;
   Evas_Object *grid = data;

   es = elm_genlist_item_data_get(item);
   elm_genlist_item_selected_set(item, EINA_FALSE);
   if (!es) return ;
   enna_browse_directory(grid, es->target);
}
#endif

static Elm_Genlist_Item_Class itc_desktop_group = {
  "default",
  {
    _enna_shortcut_desktop_label_get,
    _enna_shortcut_desktop_icon_get,
    NULL,
    NULL,
    NULL
  },
  NULL
};

static void
_enna_shortcut_cleanup(Enna_Shortcut *es)
{
   if (es->ef)
     efreet_desktop_free(es->ef);
   free(es->target);
   free(es);
}

static void
_desktop_stat_ok(void *data, Eio_File *handler __UNUSED__, const struct stat *st)
{
   Enna_Shortcut *es = data;
   Elm_Genlist_Item *egi;

   if (!eio_file_is_dir(st))
     {
        _enna_shortcut_cleanup(es);
        return ;
     }

   egi = evas_object_data_get(es->list, "enna/favorites");

   elm_genlist_item_append(es->list, &itc_desktop_group, es, egi, ELM_GENLIST_ITEM_NONE, NULL, NULL);
}

static void
_desktop_stat_error(void *data, Eio_File *handler __UNUSED__, int error __UNUSED__)
{
   _enna_shortcut_cleanup(data);
}

static Eina_Bool
_desktop_filter_cb(void *data __UNUSED__, Eio_File *handler __UNUSED__, const char *file)
{
   

    return EINA_TRUE;

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
   Enna_Shortcut *es;
   Efreet_Desktop *ef;
   char *replace;

   if (!strcmp("inode/symlink", efreet_mime_type_get(file)))
     {
	ef = NULL;
	replace = strdup(file);
     }
   else
     {

	ef = efreet_desktop_get(file);
	if (!ef) return ;

	if (ef->type != EFREET_DESKTOP_TYPE_LINK)
	  goto end;

	if (!ef->url)
	  goto end;

	if (strncmp(ef->url, "file:", 5))
	  goto end;

	replace = _desktop_flatten_env(ef->url + 5);
     }

   es = malloc(sizeof (Enna_Shortcut));
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

Evas_Object *
enna_shortcut_add(Evas_Object *parent)
{
   Elm_Genlist_Item *egi;
   const char *home = "/root/";
   char buffer[PATH_MAX];
   Evas_Object *list;

   list = elm_genlist_add(parent);

   egi = elm_genlist_item_append(list, &itc_favorite_group, NULL, NULL,
                                 ELM_GENLIST_ITEM_SUBITEMS, NULL, NULL);

   elm_genlist_item_append(list, &itc_desktop_group, NULL, egi, ELM_GENLIST_ITEM_NONE, NULL, NULL);
   elm_genlist_item_append(list, &itc_desktop_group, NULL, egi, ELM_GENLIST_ITEM_NONE, NULL, NULL);
   elm_genlist_item_append(list, &itc_desktop_group, NULL, egi, ELM_GENLIST_ITEM_NONE, NULL, NULL);

   egi = elm_genlist_item_append(list, &itc_favorite_group, NULL, NULL,
                                 ELM_GENLIST_ITEM_SUBITEMS, NULL, NULL);
   egi = elm_genlist_item_append(list, &itc_favorite_group, NULL, NULL,
                                 ELM_GENLIST_ITEM_SUBITEMS, NULL, NULL);
   egi = elm_genlist_item_append(list, &itc_favorite_group, NULL, NULL,
                                 ELM_GENLIST_ITEM_SUBITEMS, NULL, NULL);
   evas_object_data_set(list, "enna/favorites", egi);

   //evas_object_smart_callback_add(list, "selected", _enna_shortcut_desktop_select, grid);

   if (getenv("HOME"))
     home = getenv("HOME");

   snprintf(buffer, PATH_MAX - 1, "%s/.e/e/fileman/favorites/", home);

   eio_file_ls(buffer,
               _desktop_filter_cb,
               _desktop_main_cb,
               _desktop_end_cb,
               _desktop_error_cb,
               list);

   return list;
}
