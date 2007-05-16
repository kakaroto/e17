#include "News.h"


/*
 * Public functions
 */

int
news_theme_init(void)
{
   const char *path;
   char buf[4096];

   path = e_theme_edje_file_get(NEWS_THEME_IN_E, NEWS_THEME_ITEM);
   if (path && path[0])
     news->theme = NULL;
   else
     {
        snprintf(buf, sizeof(buf), "%s/e-module-news.edj", e_module_dir_get(news->module));
        news->theme = strdup(buf);
     }

   return 1;
}

void
news_theme_shutdown(void)
{
   E_FREE(news->theme);
}

void
news_theme_edje_set(Evas_Object *obj, char *key)
{
   if (!news->theme)
     e_theme_edje_object_set(obj, NEWS_THEME_IN_E, key);
   else
     edje_object_file_set(obj, news->theme, key);
}

void
news_theme_icon_set(Evas_Object *ic, char *key)
{
   if (!news->theme)
     e_util_edje_icon_set(ic, key);
   else
     e_icon_file_edje_set(ic, news->theme, key);
}

void
news_theme_menu_icon_set(E_Menu_Item *mi, char *key)
{
   if (!news->theme)
     e_util_menu_item_edje_icon_set(mi, key);
   else
     e_menu_item_icon_edje_set(mi, news->theme, key);
}

/*
 * Private functions
 */
