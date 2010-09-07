#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif /* _GNU_SOURCE */
#include <stdio.h>
#include <string.h>

#include <Eina.h>

#include "favorite.h"
#include "private.h"

#define BOOKMARK_MENU_PREALLOC_SIZE 32

typedef struct _Bookmark_Menu_Item             Bookmark_Menu_Item;
typedef struct _Bookmark_Menu_Filter_Context   Bookmark_Menu_Filter_Context;
typedef Bookmark_Menu_Item *(*                 Bookmark_Menu_Callback)(Bookmark_Menu_Item *current_item);

static Bookmark_Menu_Item *    bookmark_menu_favorites(Bookmark_Menu_Item *);
static Bookmark_Menu_Item *    bookmark_menu_history_today(Bookmark_Menu_Item *);
static Bookmark_Menu_Item *    bookmark_menu_history_yesterday(Bookmark_Menu_Item *);
static Bookmark_Menu_Item *    bookmark_menu_history_this_week(Bookmark_Menu_Item *);
static Bookmark_Menu_Item *    bookmark_menu_history_most_visited(Bookmark_Menu_Item *);
static Bookmark_Menu_Item *    bookmark_menu_history_least_visited(Bookmark_Menu_Item *);
static Bookmark_Menu_Item *    bookmark_menu_history_by_domain(Bookmark_Menu_Item *);

static Elm_Gengrid_Item_Class gic_default, gic_new_page;

typedef enum {
   ITEM_TYPE_LAST,
   ITEM_TYPE_STATIC_FOLDER,
   ITEM_TYPE_DYNAMIC_FOLDER,
   ITEM_TYPE_PAGE,
   ITEM_TYPE_CALLBACK,
   ITEM_TYPE_SEPARATOR,
} Bookmark_Menu_Item_Type;

struct _Bookmark_Menu_Item
{
   Bookmark_Menu_Item_Type type;
   const char             *text;
   void                   *next;
   Eina_Bool               dynamic : 1;
};

struct _Bookmark_Menu_Filter_Context
{
   Bookmark_Menu_Item *current_bookmark_item;
   double              time;
};

static Bookmark_Menu_Item bookmark_menu_history[] =
{
   { ITEM_TYPE_DYNAMIC_FOLDER, "Today", bookmark_menu_history_today, EINA_FALSE },
   { ITEM_TYPE_DYNAMIC_FOLDER, "Yesterday", bookmark_menu_history_yesterday, EINA_FALSE },
   { ITEM_TYPE_DYNAMIC_FOLDER, "This week", bookmark_menu_history_this_week, EINA_FALSE },
   { ITEM_TYPE_SEPARATOR, NULL, NULL, EINA_FALSE },
   { ITEM_TYPE_DYNAMIC_FOLDER, "Most visited", bookmark_menu_history_most_visited, EINA_FALSE },
   { ITEM_TYPE_DYNAMIC_FOLDER, "Least visited", bookmark_menu_history_least_visited, EINA_FALSE },
   { ITEM_TYPE_SEPARATOR, NULL, NULL, EINA_FALSE },
   { ITEM_TYPE_DYNAMIC_FOLDER, "By domain", bookmark_menu_history_by_domain, EINA_FALSE },
   { ITEM_TYPE_LAST, NULL, NULL, EINA_FALSE }
};

static Bookmark_Menu_Item bookmark_menu_root[] =
{
   { ITEM_TYPE_STATIC_FOLDER, "History", bookmark_menu_history, EINA_FALSE },
   { ITEM_TYPE_DYNAMIC_FOLDER, "Favorites", bookmark_menu_favorites, EINA_FALSE },
   { ITEM_TYPE_SEPARATOR, NULL, NULL, EINA_FALSE },
   { ITEM_TYPE_PAGE, "ProFUSION", "http://profusion.mobi", EINA_FALSE },
   { ITEM_TYPE_PAGE, "WebKit", "http://webkit.org", EINA_FALSE },
   { ITEM_TYPE_PAGE, "Enlightenment", "http://enlightenment.org", EINA_FALSE },
#ifdef STORM_TROOPER
   { ITEM_TYPE_SEPARATOR, NULL, NULL, EINA_FALSE },
   { ITEM_TYPE_PAGE, "", "http://i.imgur.com/cJO3j.gif", EINA_FALSE },
#endif
   { ITEM_TYPE_LAST, NULL, NULL, EINA_FALSE }
};

static Eina_List *
_eina_hash_sorted_keys_get(Eina_Hash *hash, Eina_Compare_Cb compare_func)
{
   Eina_List *keyvals = NULL, *keys = NULL, *keyvals_iter;
   Eina_Iterator *iter = eina_hash_iterator_tuple_new(hash);
   Eina_Hash_Tuple *keyval;

   EINA_ITERATOR_FOREACH(iter, keyval)
   keyvals = eina_list_prepend(keyvals, keyval);
   keyvals = eina_list_sort(keyvals, 0, compare_func);
   EINA_LIST_FOREACH(keyvals, keyvals_iter, keyval)
   keys = eina_list_append(keys, keyval->key);

   eina_list_free(keyvals);
   eina_iterator_free(iter);

   return keys;
}

static Bookmark_Menu_Item *
_bookmark_menu_history(Eina_Iterator *items, Bookmark_Menu_Item *current_item, Eina_Bool (*filter)(Bookmark_Menu_Filter_Context *ctx, Hist_Item *item))
{
   Bookmark_Menu_Item *bm_item;
   Bookmark_Menu_Item *ret = NULL, *new_ret;
   Bookmark_Menu_Filter_Context ctx;
   int n_items = 0;
   const char *url;

   ctx.time = ecore_time_get();
   ctx.current_bookmark_item = current_item;

   EINA_ITERATOR_FOREACH(items, url)
   {
      Hist_Item *item = hist_items_get(hist, url);

      if (!filter(&ctx, item))
         continue;

      bm_item = calloc(1, sizeof(Bookmark_Menu_Item));
      bm_item->type = ITEM_TYPE_PAGE;
      bm_item->text = eina_stringshare_add(hist_item_title_get(item));
      bm_item->next = (char *)hist_item_url_get(item);
      bm_item->dynamic = 1;

      if (!ret)
         ret = calloc(1, sizeof(*ret) * BOOKMARK_MENU_PREALLOC_SIZE);
      else if (n_items % BOOKMARK_MENU_PREALLOC_SIZE == 0)
        {
           new_ret = realloc(ret, (BOOKMARK_MENU_PREALLOC_SIZE * n_items * sizeof(*ret)));
           if (new_ret)
              ret = new_ret;
           else
              goto realloc_error;
        }

      memcpy(&ret[n_items], bm_item, sizeof(*ret));
      free(bm_item);

      n_items++;
   }

realloc_error:

   if (!n_items)
      return NULL;

   bm_item = calloc(1, sizeof(Bookmark_Menu_Item));
   bm_item->type = ITEM_TYPE_LAST;
   new_ret = realloc(ret, (1 + n_items) * sizeof(*ret));
   if (!new_ret)
     {
        free(bm_item);
        free(ret);
        return NULL;
     }

   ret = new_ret;
   memcpy(&ret[n_items], bm_item, sizeof(*ret));
   free(bm_item);

   return ret;
}

static Eina_Bool
_domain_filter(Bookmark_Menu_Filter_Context *ctx, Hist_Item *item)
{
   char *domain = strstr(hist_item_url_get(item), "://");
   const char *filtered_domain = ctx->current_bookmark_item->text;
   if (domain)
     {
        domain += 3;
        return !strncmp(domain, filtered_domain, strlen(filtered_domain));
     }

   return EINA_FALSE;
}

static Bookmark_Menu_Item *
_bookmark_menu_history_by_domain(Bookmark_Menu_Item *current_item)
{
   Bookmark_Menu_Item *ret;
   Eina_Iterator *items = eina_hash_iterator_key_new(hist_items_hash_get(hist));

   ret = _bookmark_menu_history(items, current_item, _domain_filter);
   eina_iterator_free(items);

   return ret;
}

static Bookmark_Menu_Item *
bookmark_menu_history_by_domain(Bookmark_Menu_Item *current_item)
{
   Bookmark_Menu_Item *bm_item;
   Bookmark_Menu_Item *ret = NULL, *new_ret;
   Eina_Iterator *items = eina_hash_iterator_key_new(hist_items_hash_get(hist));
   Eina_Hash *domains = eina_hash_string_superfast_new(NULL);
   const char *url;
   int n_items = 0;

   EINA_ITERATOR_FOREACH(items, url)
   {
      char *urlcopy = strdup(url);
      char *domain, *end;

      if ((domain = strcasestr(urlcopy, "http://")))
         domain += 7;
      else if ((domain = strcasestr(urlcopy, "https://")))
         domain += 8;
      else
         goto unknown_schema;

      if ((end = strchr(domain, '/')))
        {
           *end = '\0';
           eina_hash_set(domains, strdup(domain), (void *)1);
        }

unknown_schema:
      free(urlcopy);
   }
   eina_iterator_free(items);

   items = eina_hash_iterator_key_new(domains);
   EINA_ITERATOR_FOREACH(items, url)
   {
      bm_item = calloc(1, sizeof(Bookmark_Menu_Item));
      bm_item->type = ITEM_TYPE_DYNAMIC_FOLDER;
      bm_item->text = eina_stringshare_add(url);
      bm_item->next = _bookmark_menu_history_by_domain;
      bm_item->dynamic = 1;

      if (!n_items)
         ret = calloc(1, sizeof(*ret) * BOOKMARK_MENU_PREALLOC_SIZE);
      else if (n_items % BOOKMARK_MENU_PREALLOC_SIZE == 0)
        {
           new_ret = realloc(ret, (BOOKMARK_MENU_PREALLOC_SIZE * n_items * sizeof(*ret)));
           if (new_ret)
              ret = new_ret;
           else
              goto realloc_error;
        }

      memcpy(&ret[n_items], bm_item, sizeof(*ret));
      free(bm_item);

      n_items++;
   }
realloc_error:
   eina_iterator_free(items);
   eina_hash_free(domains);

   if (!n_items)
      return NULL;

   bm_item = calloc(1, sizeof(Bookmark_Menu_Item));
   bm_item->type = ITEM_TYPE_LAST;
   new_ret = realloc(ret, (1 + n_items) * sizeof(*ret));
   if (!new_ret)
     {
        free(bm_item);
        free(ret);
        return NULL;
     }

   ret = new_ret;
   memcpy(&ret[n_items], bm_item, sizeof(*ret));
   free(bm_item);

   return ret;
}

static Eina_Bool
_this_week_filter(Bookmark_Menu_Filter_Context *ctx, Hist_Item *item)
{
   double item_time = hist_item_last_visit_get(item);
   double now = ctx->time;
   return (now - item_time) <= 7 * 24 * 3600;
}

static Eina_Bool
_today_filter(Bookmark_Menu_Filter_Context *ctx, Hist_Item *item)
{
   double item_time = hist_item_last_visit_get(item);
   double now = ctx->time;
   return (now - item_time) <= 24 * 3600;
}

static Eina_Bool
_yesterday_filter(Bookmark_Menu_Filter_Context *ctx, Hist_Item *item)
{
   double item_time = hist_item_last_visit_get(item);
   double now = ctx->time;
   return (now - item_time) > 24 * 3600 && (now - item_time) <= 48 * 3600;
}

static Bookmark_Menu_Item *
bookmark_menu_history_today(Bookmark_Menu_Item *current_item)
{
   Eina_Iterator *iter = eina_hash_iterator_key_new(hist_items_hash_get(hist));
   Bookmark_Menu_Item *items = _bookmark_menu_history(iter, current_item, _today_filter);
   eina_iterator_free(iter);
   return items;
}

static Bookmark_Menu_Item *
bookmark_menu_history_yesterday(Bookmark_Menu_Item *current_item)
{
   Eina_Iterator *iter = eina_hash_iterator_key_new(hist_items_hash_get(hist));
   Bookmark_Menu_Item *items = _bookmark_menu_history(iter, current_item, _yesterday_filter);
   eina_iterator_free(iter);
   return items;
}

static Bookmark_Menu_Item *
bookmark_menu_history_this_week(Bookmark_Menu_Item *current_item)
{
   Eina_Iterator *iter = eina_hash_iterator_key_new(hist_items_hash_get(hist));
   Bookmark_Menu_Item *items = _bookmark_menu_history(iter, current_item, _this_week_filter);
   eina_iterator_free(iter);
   return items;
}

static int
_cb_compare_hist_visit_count_decr(const void *data1, const void *data2)
{
   Hist_Item *f1 = ((Eina_Hash_Tuple *)data1)->data;
   Hist_Item *f2 = ((Eina_Hash_Tuple *)data2)->data;

   return hist_item_visit_count_get(f2) - hist_item_visit_count_get(f1);
}

static int
_cb_compare_hist_visit_count_incr(const void *data1, const void *data2)
{
   Hist_Item *f1 = ((Eina_Hash_Tuple *)data1)->data;
   Hist_Item *f2 = ((Eina_Hash_Tuple *)data2)->data;

   return hist_item_visit_count_get(f1) - hist_item_visit_count_get(f2);
}

static Bookmark_Menu_Item *
bookmark_menu_history_least_visited(Bookmark_Menu_Item *current_item)
{
   Eina_List *keys = _eina_hash_sorted_keys_get(hist_items_hash_get(hist), _cb_compare_hist_visit_count_incr);
   Eina_Iterator *iter = eina_list_iterator_new(keys);
   Bookmark_Menu_Item *items = _bookmark_menu_history(iter, current_item, _this_week_filter);
   eina_list_free(keys);
   eina_iterator_free(iter);
   return items;
}

static Bookmark_Menu_Item *
bookmark_menu_history_most_visited(Bookmark_Menu_Item *current_item)
{
   Eina_List *keys = _eina_hash_sorted_keys_get(hist_items_hash_get(hist), _cb_compare_hist_visit_count_decr);
   Eina_Iterator *iter = eina_list_iterator_new(keys);
   Bookmark_Menu_Item *items = _bookmark_menu_history(iter, current_item, _this_week_filter);
   eina_list_free(keys);
   eina_iterator_free(iter);
   return items;
}

static Bookmark_Menu_Item *
bookmark_menu_favorites(Bookmark_Menu_Item *current_item)
{
   Bookmark_Menu_Item *bm_item;
   Bookmark_Menu_Item *ret = NULL, *new_ret;
   Eina_Iterator *iter = eina_hash_iterator_key_new(fav_items_hash_get(fav));
   int n_items = 0;
   const char *url;

   EINA_ITERATOR_FOREACH(iter, url)
   {
      Fav_Item *item = fav_items_get(fav, url);

      bm_item = calloc(1, sizeof(Bookmark_Menu_Item));
      bm_item->type = ITEM_TYPE_PAGE;
      bm_item->text = eina_stringshare_add(fav_item_title_get(item));
      bm_item->next = (char *)fav_item_url_get(item);
      bm_item->dynamic = 1;

      if (!n_items)
         ret = calloc(1, sizeof(*ret) * BOOKMARK_MENU_PREALLOC_SIZE);
      else if (n_items % BOOKMARK_MENU_PREALLOC_SIZE == 0)
        {
           new_ret = realloc(ret, (BOOKMARK_MENU_PREALLOC_SIZE * n_items * sizeof(*ret)));
           if (new_ret)
              ret = new_ret;
           else
              goto realloc_error;
        }

      memcpy(&ret[n_items], bm_item, sizeof(*ret));
      free(bm_item);

      n_items++;
   }
realloc_error:
   eina_iterator_free(iter);

   if (!n_items)
     {
        free(ret);
        return NULL;
     }

   bm_item = calloc(1, sizeof(Bookmark_Menu_Item));
   bm_item->type = ITEM_TYPE_LAST;
   new_ret = realloc(ret, (1 + n_items) * sizeof(*ret));
   if (!new_ret)
     {
        free(bm_item);
        free(ret);
        return NULL;
     }

   ret = new_ret;
   memcpy(&ret[n_items], bm_item, sizeof(*ret));
   free(bm_item);

   return ret;
}

static void
on_view_mask_visible(void *data, Evas_Object *o __UNUSED__,
                     const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Browser_Window *win = data;
   Evas_Object *ed = elm_layout_edje_get(win->current_chrome);
   Evas_Object *url_entry = edje_object_part_swallow_get(ed, "url-entry");

   evas_object_focus_set(win->current_view, EINA_FALSE);
   evas_object_focus_set(url_entry, EINA_TRUE);
   elm_object_focus(url_entry);
}

static void
on_view_mask_hidden(void *data, Evas_Object *o __UNUSED__,
                    const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Browser_Window *win = data;

   evas_object_focus_set(win->current_view, EINA_TRUE);
}

static void
on_fav_on(void *data, Evas_Object *o __UNUSED__,
          const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Evas_Object *view = data;
   const char *url = ewk_view_uri_get(view);

   if (url)
     {
        const char *title = ewk_view_title_get(view);
        fav_items_add(fav, url, fav_item_new(url, title, 1));
     }
}

static void
on_fav_off(void *data, Evas_Object *o __UNUSED__,
           const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Evas_Object *view = data;
   const char *url = ewk_view_uri_get(view);

   if (url)
      fav_items_del(fav, url);
}

static void
_is_favorite_check(Evas_Object *chrome, const char *url)
{
   Evas_Object *ed = elm_layout_edje_get(chrome);
   Fav_Item *item;

   if (url && (item = fav_items_get(fav, url)))
     {
        fav_item_visit_count_set(item, fav_item_visit_count_get(item) + 1);
        edje_object_signal_emit(ed, "favorite,hilight", "");
     }
   else
      edje_object_signal_emit(ed, "favorite,default", "");
}

static void
_history_update(const char *url, const char *title)
{
   Hist_Item *item;

   if (!url)
      return;

   if ((item = hist_items_get(hist, url)))
     {
        hist_item_visit_count_set(item, hist_item_visit_count_get(item) + 1);
        hist_item_last_visit_set(item, ecore_time_get());
        hist_item_title_set(item, title);
     }
   else
      hist_items_add(hist, url, hist_item_new(title, url, 1, ecore_time_get()));
}

static void
_chrome_state_apply(Evas_Object *chrome, Evas_Object *view)
{
   const char *url = ewk_view_uri_get(view);
   const char *title = ewk_view_title_get(view);
   Evas_Object *ed = elm_layout_edje_get(chrome);
   Evas_Object *text_url;

   if (!title)
      title = url;

   edje_object_part_text_set(ed, "text.title", title ? title : "");

   text_url = edje_object_part_swallow_get(ed, "url-entry");
   elm_scrolled_entry_entry_set(text_url, url ? url : "");
   _is_favorite_check(chrome, url);
   _history_update(url, title);

   // TODO: check if actions are possible: back/forward/pause/reload
}

static void
on_view_load_progress(void *data, Evas_Object *view __UNUSED__,
                      void *event_info)
{
   Evas_Object *chrome = data;
   Evas_Object *ed = elm_layout_edje_get(chrome);

   double *progress = event_info;
   Edje_Message_Float msg = { *progress };
   edje_object_message_send(ed, EDJE_MESSAGE_FLOAT, 1, &msg);
}

static void
on_view_title_changed(void *data, Evas_Object *view,
                      void *event_info __UNUSED__)
{
   Evas_Object *chrome = data;

   _chrome_state_apply(chrome, view);
}

static void
on_view_uri_changed(void *data, Evas_Object *view, void *event_info __UNUSED__)
{
   Evas_Object *chrome = data;

   _chrome_state_apply(chrome, view);
}

static void
on_view_zoom_interactive(void *data, Evas_Object *view __UNUSED__,
                         void *event_info)
{
   Evas_Object *chrome = data;
   Evas_Object *ed = elm_layout_edje_get(chrome);

   const View_Zoom_Interactive *ev = event_info;

   char buf[256];

   snprintf(buf, sizeof(buf), "zoom=%0.1f", ev->zoom);
   edje_object_part_text_set(ed, "text.zoom", buf);
}

static void
on_view_zoom_interactive_start(void *data, Evas_Object *view __UNUSED__,
                               void *event_info __UNUSED__)
{
   Evas_Object *chrome = data;
   Evas_Object *ed = elm_layout_edje_get(chrome);

   edje_object_signal_emit(ed, "zoom,show", "");
}

static void
on_view_zoom_interactive_end(void *data, Evas_Object *view __UNUSED__,
                             void *event_info __UNUSED__)
{
   Evas_Object *chrome = data;
   Evas_Object *ed = elm_layout_edje_get(chrome);

   edje_object_signal_emit(ed, "zoom,hide", "");
}

static void
on_view_statusbar_text_set(void *data, Evas_Object *view __UNUSED__,
                           void *event_info)
{
   Evas_Object *chrome = data;
   Evas_Object *ed = elm_layout_edje_get(chrome);
   const char *text = event_info;

   edje_object_part_text_set(ed, "text.statusbar", text);
   edje_object_signal_emit(ed, "statusbar,show", "");
}

static void
on_view_link_hover_in(void *data, Evas_Object *view __UNUSED__,
                      void *event_info)
{
   Evas_Object *chrome = data;
   Evas_Object *ed = elm_layout_edje_get(chrome);
   const char **link = event_info;

   edje_object_part_text_set(ed, "text.statusbar", link[0]);
   edje_object_signal_emit(ed, "statusbar,show", "");
}

static void
on_view_link_hover_out(void *data, Evas_Object *view __UNUSED__,
                       void *event_info __UNUSED__)
{
   Evas_Object *chrome = data;
   Evas_Object *ed = elm_layout_edje_get(chrome);

   edje_object_signal_emit(ed, "statusbar,hide", "");
}

static Eina_Bool
_view_popup_delete(void *notify)
{
   evas_object_del(notify);
   return EINA_FALSE;
}

static void
on_view_popup_delete(void *data, Evas_Object *view, void *event_info)
{
   ecore_idler_add(_view_popup_delete, data);
}

static void
_popup_item_selected(void *data, Evas_Object *obj, void *event_info)
{
   Evas_Object *view = data;
   Elm_List_Item *it = elm_list_selected_item_get(obj);
   const Eina_List *itr, *list = elm_list_items_get(obj);
   void *d;
   int i = 0;

   EINA_LIST_FOREACH(list, itr, d)
   {
      if (d == it)
         break;

      i++;
   }

   ewk_view_popup_selected_set(view, i);
   ewk_view_popup_destroy(view);
}

static void
on_view_popup_new(void *data, Evas_Object *view, void *event_info)
{
   Ewk_Menu *menu = event_info;
   Ewk_Menu_Item *item;
   Eina_List *l;
   Evas_Object *li;
   Evas_Object *chrome = evas_object_data_get(view, "chrome");
   Evas_Object *notify = elm_notify_add(chrome);

   elm_object_style_set(notify, "ewebkit");
   evas_object_size_hint_weight_set(notify, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_notify_repeat_events_set(notify, EINA_FALSE);
   elm_notify_orient_set(notify, ELM_NOTIFY_ORIENT_BOTTOM);

   li = elm_list_add(view);
   elm_object_style_set(li, "ewebkit");
   elm_list_always_select_mode_set(li, 1);
   evas_object_size_hint_weight_set(li, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_fill_set(li, EVAS_HINT_FILL, EVAS_HINT_FILL);

   EINA_LIST_FOREACH(menu->items, l, item)
   elm_list_item_append(li, item->text, NULL, NULL, _popup_item_selected,
                        view);

   elm_list_go(li);
   evas_object_show(li);
   elm_notify_content_set(notify, li);

   evas_object_smart_callback_add(view, "popup,willdelete",
                                  on_view_popup_delete, notify);

   evas_object_show(notify);
}

static void
on_tab_close(void *data, Evas_Object *o,
             const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Browser_Window *win = evas_object_data_get(o, "win");
   Evas_Object *chrome = evas_object_data_get(data, "chrome");
   Elm_Gengrid_Item *item = evas_object_data_get(o, "item");
   Evas_Object *ed;

   ed = elm_layout_edje_get(win->current_chrome);
   edje_object_signal_emit(ed, "tab,item,clicked", "");

   elm_gengrid_item_del(item);
   tab_close_chrome(win, chrome);
}

static void
on_tab_gengrid_item_realized(void *data, Evas_Object *o, void *event_info)
{
   Browser_Window *win = data;
   Evas_Object *item = (Evas_Object *)elm_gengrid_item_object_get(event_info);

   evas_object_data_set(item, "item", event_info);
   evas_object_data_set(item, "win", win);
   edje_object_signal_callback_add(item, "tab,close", "", on_tab_close,
                                   elm_gengrid_item_data_get(event_info));
}

static void
on_action_back(void *data, Evas_Object *o __UNUSED__,
               const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Evas_Object *view = data;

   ewk_view_back(view);
}

static void
on_action_forward(void *data, Evas_Object *o __UNUSED__,
                  const char *emission __UNUSED__,
                  const char *source __UNUSED__)
{
   Evas_Object *view = data;

   ewk_view_forward(view);
}

static void      on_bookmark_item_click(void *data, Evas_Object *obj, void *event_info __UNUSED__);
static void      on_bookmark_item_back_click(void *data, Evas_Object *edje, const char *emission __UNUSED__, const char *source __UNUSED__);

static void
bookmark_menu_set(Evas_Object        *chrome,
                  Evas_Object        *list,
                  Bookmark_Menu_Item *root,
                  const char         *old_text)
{
   Browser_Window *win = evas_object_data_get(chrome, "win");
   Evas_Object *ed = elm_layout_edje_get(chrome);
   int i;

   if (!eina_list_data_find(win->list_history, root))
     {
        if (root == bookmark_menu_root || !root)
           win->list_history = eina_list_prepend(win->list_history, NULL);
        else
           win->list_history = eina_list_prepend(win->list_history, root);
     }

   elm_list_clear(list);

   if (!root || root == bookmark_menu_root)
     {
        root = bookmark_menu_root;
        edje_object_part_text_set(ed, "bookmark-list-title", "Bookmarks");
        edje_object_signal_emit(ed, "list,back,hide", "");
     }
   else
     {
        edje_object_part_text_set(ed, "bookmark-list-back-button-text", eina_stringshare_add(old_text ? old_text : "Bookmarks"));
        edje_object_signal_callback_del(ed, "list,back,clicked", "", on_bookmark_item_back_click);
        edje_object_signal_callback_add(ed, "list,back,clicked", "", on_bookmark_item_back_click, list);

        edje_object_signal_emit(ed, "list,back,show", "");
     }

   for (i = 0; root[i].type != ITEM_TYPE_LAST; i++) {
        if (root[i].type == ITEM_TYPE_SEPARATOR)
          {
             Elm_List_Item *item = elm_list_item_append(list, NULL, NULL, NULL, NULL, NULL);
             elm_list_item_separator_set(item, EINA_TRUE);
          }
        else
           elm_list_item_append(list, root[i].text, NULL, NULL, on_bookmark_item_click, &root[i]);
     }

   elm_list_go(list);
}

static void
on_bookmark_item_back_click(void *data, Evas_Object *edje,
                            const char *emission __UNUSED__,
                            const char *source __UNUSED__)
{
   Bookmark_Menu_Item *bmi;
   Browser_Window *win = evas_object_data_get(edje, "win");
   Evas_Object *list = data;

   edje_object_signal_emit(edje, "list,animate,right", "");
   edje_object_part_text_set(edje, "bookmark-list-title", edje_object_part_text_get(edje, "bookmark-list-back-button-text"));
   eina_stringshare_del(edje_object_part_text_get(edje, "bookmark-list-back-button-text"));

   if ((bmi = win->list_history->data) && bmi->dynamic)
     {
        eina_stringshare_del(bmi->text);
        free(bmi);
     }

   win->list_history = eina_list_remove_list(win->list_history, win->list_history);
   win->list_history_titles = eina_list_remove_list(win->list_history_titles, win->list_history_titles);

   if (!win->list_history_titles)
      bookmark_menu_set(win->current_chrome, list, win->list_history->data, "Bookmarks");
   else
      bookmark_menu_set(win->current_chrome, list, win->list_history->data, win->list_history_titles->data);
}

static void
on_bookmark_item_click(void *data, Evas_Object *obj,
                       void *event_info __UNUSED__)
{
   Evas_Object *chrome = evas_object_data_get(obj, "chrome");
   Evas_Object *ed = elm_layout_edje_get(chrome);
   Bookmark_Menu_Item *bmi = data;
   Browser_Window *win = evas_object_data_get(chrome, "win");
   const char *old_text = edje_object_part_text_get(ed, "bookmark-list-title");

   if (!bmi)
      return;

   switch (bmi->type) {
      case ITEM_TYPE_STATIC_FOLDER:
         win->list_history_titles = eina_list_prepend(win->list_history_titles, old_text);
         edje_object_signal_emit(ed, "list,animate,left", "");
         edje_object_part_text_set(ed, "bookmark-list-title", bmi->text);
         bookmark_menu_set(chrome, obj, bmi->next, old_text);
         break;

      case ITEM_TYPE_DYNAMIC_FOLDER:
      {
         Bookmark_Menu_Callback callback = bmi->next;
         if (!callback)
            return;

         Bookmark_Menu_Item *new_root = callback(bmi);
         if (new_root)
           {
              win->list_history_titles = eina_list_prepend(win->list_history_titles, old_text);
              edje_object_part_text_set(ed, "bookmark-list-title", bmi->text);
              edje_object_signal_emit(ed, "list,animate,left", "");
              bookmark_menu_set(chrome, obj, new_root, old_text);
           }
      }
      break;

      case ITEM_TYPE_LAST:
      case ITEM_TYPE_SEPARATOR:
         break;

      case ITEM_TYPE_CALLBACK:
      {
         Bookmark_Menu_Callback callback = bmi->next;
         Evas_Object *ed = elm_layout_edje_get(chrome);
         if (callback)
            callback(bmi);

         edje_object_signal_emit(ed, "bookmark,item,clicked", "");
      }
      break;

      default:
      {
         Browser_Window *win = evas_object_data_get(chrome, "win");
         Evas_Object *ed = elm_layout_edje_get(chrome);
         if (win)
            ewk_view_uri_set(win->current_view, bmi->next);

         edje_object_signal_emit(ed, "bookmark,item,clicked", "");
      }
      break;
     }
}

Evas_Object *
view_screenshot_add(Evas *evas, const Evas_Object *view)
{
   Evas_Object *img;
   Ewk_View_Smart_Data *sd;
   Ewk_View_Private_Data *priv;
   Eina_Rectangle rect = { 0, 0, 480, 800 };
   cairo_surface_t *surface;
   cairo_format_t format;
   cairo_t *cairo;
   int stride;
   void *pixels, *dest;

   sd = (Ewk_View_Smart_Data *)evas_object_smart_data_get(view);
   priv = (Ewk_View_Private_Data *)sd->_priv;

   /* assuming colorspace is EVAS_COLORSPACE_ARGB8888 */
   stride = rect.w * 4;
   format = CAIRO_FORMAT_RGB24;

   img = evas_object_image_filled_add(evas);
   evas_object_resize(img, rect.w, rect.h);
   evas_object_image_size_set(img, rect.w, rect.h);
   evas_object_image_colorspace_set(img, EVAS_COLORSPACE_ARGB8888);
   evas_object_image_alpha_set(img, EINA_FALSE);
   pixels = evas_object_image_data_get(img, EINA_TRUE);

   surface =
      cairo_image_surface_create_for_data(pixels, format, rect.w, rect.h,
                                          stride);
   if (cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS)
      goto error_cairo_surface_create;

   cairo = cairo_create(surface);
   if (cairo_status(cairo) != CAIRO_STATUS_SUCCESS)
      goto error_cairo_create;

   if (!ewk_view_paint_contents(priv, cairo, &rect))
     {
        evas_object_del(img);
        img = NULL;
     }
   else
     {
        dest = evas_object_image_data_get(img, EINA_TRUE);
        memmove(dest, pixels, rect.h * stride);
        evas_object_image_data_set(img, dest);
     }

error_cairo_create:
   cairo_destroy(cairo);
error_cairo_surface_create:
   cairo_surface_destroy(surface);

   return img;
}

static void
tab_grid_item_click(void *data, Evas_Object *obj, void *event_info)
{
   Evas_Object *chrome = data;
   Browser_Window *win = evas_object_data_get(chrome, "win");
   Evas_Object *ed = elm_layout_edje_get(chrome);

   edje_object_signal_emit(ed, "tab,item,clicked", "");
   tab_focus_chrome(win, chrome);
}

static void
tab_grid_new_tab_click(void *data, Evas_Object *obj, void *event_info)
{
   Evas_Object *chrome = data;
   Browser_Window *win = evas_object_data_get(chrome, "win");
   Evas_Object *ed = elm_layout_edje_get(chrome);

   edje_object_signal_emit(ed, "tab,item,clicked", "");
   tab_add(win, DEFAULT_URL);
}

static void
on_action_tab_show(void *data, Evas_Object *o __UNUSED__,
                   const char *emission __UNUSED__,
                   const char *source __UNUSED__)
{
   Evas_Object *chrome = data;
   Evas_Object *grid = evas_object_data_get(chrome, "tab-grid");
   Browser_Window *win = evas_object_data_get(chrome, "win");
   Eina_List *itr;
   Evas_Object *itr_chrome;

   elm_gengrid_clear(grid);

   elm_gengrid_item_append(grid, &gic_new_page, NULL, tab_grid_new_tab_click, chrome);

   EINA_LIST_FOREACH(win->chromes, itr, itr_chrome)
   {
      Evas_Object *itr_view = evas_object_data_get(itr_chrome, "view");
      elm_gengrid_item_append(grid, &gic_default, itr_view, tab_grid_item_click, itr_chrome);
   }
}

static void
on_action_tab_hide(void *data, Evas_Object *o __UNUSED__,
                   const char *emission __UNUSED__,
                   const char *source __UNUSED__)
{
}

static void
on_action_bookmark_hide(void *data, Evas_Object *o __UNUSED__,
                        const char *emission __UNUSED__,
                        const char *source __UNUSED__)
{
   Evas_Object *chrome = data;
   Evas_Object *edje = elm_layout_edje_get(chrome);
   Bookmark_Menu_Item *bmi;
   Browser_Window *win = evas_object_data_get(chrome, "win");

   EINA_LIST_FREE(win->list_history, bmi)
   if (bmi && bmi->dynamic)
     {
        eina_stringshare_del(bmi->text);
        free(bmi);
     }

   eina_stringshare_del(edje_object_part_text_get(edje, "bookmark-list-back-button-text"));
   eina_list_free(win->list_history_titles);
   win->list_history = NULL;
   win->list_history_titles = NULL;
}

static void
on_action_bookmark_show(void *data, Evas_Object *o __UNUSED__,
                        const char *emission __UNUSED__,
                        const char *source __UNUSED__)
{
   Evas_Object *chrome = data;
   Evas_Object *hl = evas_object_data_get(chrome, "bookmark-list");

   bookmark_menu_set(chrome, hl, NULL, NULL);
}

static void
on_action_pause(void *data, Evas_Object *o __UNUSED__,
                const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Evas_Object *view = data;

   ewk_view_stop(view);
}

static void
on_action_reload(void *data, Evas_Object *o __UNUSED__,
                 const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Evas_Object *view = data;

   ewk_view_reload(view);
}

static void
on_action_home(void *data, Evas_Object *o __UNUSED__,
               const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Evas_Object *view = data;

   ewk_view_uri_set(view, DEFAULT_URL);
}

static void
on_action_load_page(void *data, Evas_Object *view, void *event_info __UNUSED__)
{
   Evas_Object *ewk_view = data;

   const char *entry_data = elm_scrolled_entry_entry_get(view);

   char uri[2048];

   snprintf(uri, 2048, "%s%s",
            (strstr(entry_data, "://") ? "" : "http://"), entry_data);

   ewk_view_uri_set(ewk_view, uri);
   evas_object_focus_set(ewk_view, EINA_TRUE);
}

static void
on_view_load_error(void *data __UNUSED__, Evas_Object *view __UNUSED__,
                   void *event_info)
{
   const Ewk_Frame_Load_Error *error = event_info;
   Evas_Object *frame = error->frame;
   char *msg;
   int len;
   const char template[] = ""
                           "<html>\n"
                           "  <head>\n"
                           "    <title>Error loading page.</title>\n"
                           "  </head>\n"
                           "  <body>\n"
                           "    <h1>Error loading page</h1>\n"
                           "    <p>Error description: <strong>%s</strong></p>\n"
                           "    <p>Failing address: <strong>%s</strong></p>\n"
                           "    <p>Go <a href=\"javascript:history.go(-1);\">back</a></p>"
                           "  </body>\n" "</html>\n";

   if (error->is_cancellation)
      return;

   if (!frame)
     {
        ERR("error loading '%s': %s", error->failing_url, error->description);
        return;
     }

   len = asprintf(&msg, template, error->description, error->failing_url);
   if (len < 0)
     {
        ERR("error loading '%s': %s", error->failing_url, error->description);
        return;
     }

   ewk_frame_contents_alternate_set
      (frame, msg, len, "text/html", NULL, NULL, error->failing_url);
   free(msg);
}

static void
on_view_download_request(void *data, Evas_Object *view __UNUSED__,
                         void *event_info)
{
   Ewk_Download *download = event_info;

   INF("Download: %s", download->url);
}

static void
on_inputmethods_changed(void *data, Evas_Object *view, void *event_info)
{
   Eina_Bool active = (Eina_Bool)(long)event_info;
   Evas_Object *win = data;
   unsigned int imh;
   INF("IM changed: active=%d", active);

   if (!active)
     {
        elm_win_keyboard_mode_set(win, ELM_WIN_KEYBOARD_OFF);
        return;
     }

   imh = ewk_view_imh_get(view);
   INF("Imh:%d", imh);
   if (imh & EWK_IMH_TELEPHONE)
      elm_win_keyboard_mode_set(win, ELM_WIN_KEYBOARD_PHONE_NUMBER);
   else if (imh & EWK_IMH_NUMBER)
      elm_win_keyboard_mode_set(win, ELM_WIN_KEYBOARD_NUMERIC);
   else if (imh & EWK_IMH_URL)
      elm_win_keyboard_mode_set(win, ELM_WIN_KEYBOARD_URL);
   else if (imh & EWK_IMH_PASSWORD)
      elm_win_keyboard_mode_set(win, ELM_WIN_KEYBOARD_PASSWORD);

#if 0
   else if (imh & EWK_IMH_EMAIL)
      elm_win_keyboard_mode_set(win, ELM_WIN_KEYBOARD_
#endif
   else
      elm_win_keyboard_mode_set(win, ELM_WIN_KEYBOARD_ON);
}

#if 0
static void
win_title_set(Browser_Window *win)
{
   const char *view_title = ewk_view_title_get(win->view);
   int p = ewk_view_load_progress_get(win->view) * 100;
   char win_title[1024];

   if (!view_title)
      view_title = ewk_view_uri_get(win->view);

   if (p < 100)
      snprintf(win_title, sizeof(win_title), "(%d%%) %s  - " PACKAGE_STRING,
               p, view_title);
   else
      snprintf(win_title, sizeof(win_title), "%s  - " PACKAGE_STRING,
               view_title);

   elm_win_title_set(win->win, win_title);
}

static void
on_view_load_progress(void *data, Evas_Object *view __UNUSED__,
                      void *event_info __UNUSED__)
{
   Browser_Window *win = data;

   win_title_set(win);
}

static void
on_view_title_changed(void *data, Evas_Object *view __UNUSED__,
                      void *event_info __UNUSED__)
{
   Browser_Window *win = data;

   win_title_set(win);
}

#endif

static void
on_key_down(void *data, Evas *e __UNUSED__, Evas_Object *o __UNUSED__,
            void *event_info)
{
   Browser_Window *win = data;
   Evas_Object *view = win->current_view;
   Evas_Object *frame = ewk_view_frame_main_get(view);
   Evas_Event_Key_Down *ev = event_info;
   const char *k = ev->keyname;

   INF("keyname=%s, key=%s, string=%s\n", ev->keyname, ev->key, ev->string);
   if ((strcmp(k, "Keycode-122") == 0) || (strcmp(k, "F5") == 0))
     {
        ev->event_flags &= EVAS_EVENT_FLAG_ON_HOLD;
        ewk_frame_scroll_add(frame, 0, 50);
     }
   else if ((strcmp(k, "Keycode-123") == 0) || (strcmp(k, "F6") == 0))
     {
        ev->event_flags &= EVAS_EVENT_FLAG_ON_HOLD;
        ewk_frame_scroll_add(frame, 0, -50);
     }
   else if ((strcmp(k, "Keycode-185") == 0) || (strcmp(k, "F7") == 0))
     {
        ev->event_flags &= EVAS_EVENT_FLAG_ON_HOLD;
        view_zoom_next_up(view);
     }
   else if ((strcmp(k, "Keycode-186") == 0) || (strcmp(k, "F8") == 0))
     {
        ev->event_flags &= EVAS_EVENT_FLAG_ON_HOLD;
        view_zoom_next_down(view);
     }
}

static char *
tab_grid_label_get(const void *data, Evas_Object *obj __UNUSED__, const char *part __UNUSED__)
{
   if (data)
     {
        const char *title = ewk_view_title_get(data);
        return strdup(title ? title : "");
     }

   return NULL;
}

static Evas_Object *
tab_grid_icon_get(const void *data, Evas_Object *obj __UNUSED__, const char *part __UNUSED__)
{
   if (data)
     {
        const Evas_Object *view = data;
        Evas *evas = evas_object_evas_get(view);
        return view_screenshot_add(evas, view);
     }

   return NULL;
}

static Eina_Bool
tab_grid_state_get(const void *data __UNUSED__, Evas_Object *obj __UNUSED__, const char *part __UNUSED__)
{
   return EINA_FALSE;
}

static void
tab_grid_del(const void *data, Evas_Object *obj)
{
}

Evas_Object *
chrome_add(Browser_Window *win, const char *url)
{
   Evas_Object *chrome = elm_layout_add(win->win);
   Evas_Object *ed = elm_layout_edje_get(chrome);
   Evas_Object *view;

   if (!elm_layout_file_set(chrome, PACKAGE_DATA_DIR "/default.edj", "chrome"))
     {
        int err = edje_object_load_error_get(ed);

        const char *msg = edje_load_error_str(err);

        CRITICAL("Could not load chrome theme: %s", msg);
        evas_object_del(chrome);
        return NULL;
     }

   view = view_add(win->win);
   if (!view)
     {
        CRITICAL("Could not create view");
        goto error_view_create;
     }

   evas_object_focus_set(view, 1);
   elm_layout_content_set(chrome, "view", view);

   evas_object_data_set(chrome, "view", view);
   evas_object_data_set(chrome, "win", win);
   evas_object_data_set(view, "chrome", chrome);
   evas_object_data_set(ed, "win", win);

   if (win->app->user_agent)
     {
        INF("using custom user agent string: %s\n", win->app->user_agent);
        ewk_view_setting_user_agent_set(view, win->app->user_agent);
     }

   ewk_view_setting_enable_plugins_set(view, !win->app->disable_plugins);

   evas_object_event_callback_add(view, EVAS_CALLBACK_KEY_DOWN, on_key_down,
                                  win);
   evas_object_smart_callback_add(view, "load,error", on_view_load_error, win);
   evas_object_smart_callback_add(view, "download,request",
                                  on_view_download_request, win);
   evas_object_smart_callback_add(view, "inputmethods,changed",
                                  on_inputmethods_changed, win->win);
#if 0
   evas_object_smart_callback_add(view, "load,progress", on_view_load_progress,
                                  win);
   evas_object_smart_callback_add(view, "title,changed", on_view_title_changed,
                                  win);
#endif

   if (url)
      ewk_view_uri_set(view, url);

   Evas_Object *text_url = elm_scrolled_entry_add(ed);
   elm_object_style_set(text_url, "ewebkit/url");
   elm_scrolled_entry_single_line_set(text_url, EINA_TRUE);
   elm_layout_content_set(chrome, "url-entry", text_url);
   evas_object_smart_callback_add
      (text_url, "activated", on_action_load_page, view);

   Evas_Object *bookmark_list = elm_list_add(ed);
   elm_list_scroller_policy_set(bookmark_list,
                                ELM_SCROLLER_POLICY_OFF,
                                ELM_SCROLLER_POLICY_AUTO);
   elm_object_style_set(bookmark_list, "ewebkit");
   evas_object_data_set(bookmark_list, "chrome", chrome);
   evas_object_data_set(chrome, "bookmark-list", bookmark_list);
   elm_layout_content_set(chrome, "bookmark-list-swallow", bookmark_list);

   Evas_Object *tab_grid = elm_gengrid_add(ed);
   elm_object_style_set(tab_grid, "ewebkit");
   elm_gengrid_item_size_set(tab_grid, 140, 174);
   elm_gengrid_horizontal_set(tab_grid, EINA_FALSE);
   elm_gengrid_multi_select_set(tab_grid, EINA_FALSE);
   evas_object_data_set(chrome, "tab-grid", tab_grid);
   evas_object_data_set(tab_grid, "win", win);
   elm_layout_content_set(chrome, "tab-grid-swallow", tab_grid);
   evas_object_smart_callback_add(tab_grid, "realized",
                                  on_tab_gengrid_item_realized, win);

   gic_default.func.label_get = tab_grid_label_get;
   gic_default.func.icon_get = tab_grid_icon_get;
   gic_default.func.state_get = tab_grid_state_get;
   gic_default.func.del = tab_grid_del;

   memcpy(&gic_new_page, &gic_default, sizeof(gic_default));
   gic_new_page.item_style = "new_page";
   gic_default.item_style = "default";

   edje_object_signal_callback_add(ed, "action,back", "back", on_action_back, view);
   edje_object_signal_callback_add(ed, "action,forward", "forward", on_action_forward,
                                   view);
   edje_object_signal_callback_add(ed, "action,stop", "stop", on_action_pause,
                                   view);
   edje_object_signal_callback_add(ed, "action,reload", "reload", on_action_reload,
                                   view);
   edje_object_signal_callback_add(ed, "action,home", "home", on_action_home,
                                   view);

   edje_object_signal_callback_add(ed, "action,fav_on", "", on_fav_on, view);
   edje_object_signal_callback_add(ed, "action,fav_off", "", on_fav_off, view);

   edje_object_signal_callback_add(ed, "view,mask,visible", "", on_view_mask_visible, win);
   edje_object_signal_callback_add(ed, "view,mask,hidden", "", on_view_mask_hidden, win);

   edje_object_signal_callback_add(ed, "bookmark,show", "",
                                   on_action_bookmark_show, chrome);
   edje_object_signal_callback_add(ed, "bookmark,hide", "",
                                   on_action_bookmark_hide, chrome);
   edje_object_signal_callback_add(ed, "tab,show", "",
                                   on_action_tab_show, chrome);
   edje_object_signal_callback_add(ed, "tab,hide", "",
                                   on_action_tab_hide, chrome);
   evas_object_smart_callback_add(view, "load,progress", on_view_load_progress,
                                  chrome);
   evas_object_smart_callback_add(view, "title,changed", on_view_title_changed,
                                  chrome);
   evas_object_smart_callback_add(view, "uri,changed", on_view_uri_changed,
                                  chrome);
   evas_object_smart_callback_add(view, "zoom,interactive",
                                  on_view_zoom_interactive, chrome);
   evas_object_smart_callback_add(view, "zoom,interactive,start",
                                  on_view_zoom_interactive_start, chrome);
   evas_object_smart_callback_add(view, "zoom,interactive,end",
                                  on_view_zoom_interactive_end, chrome);
   evas_object_smart_callback_add(view, "statusbar,text,set",
                                  on_view_statusbar_text_set, chrome);
   evas_object_smart_callback_add(view, "link,hover,in", on_view_link_hover_in,
                                  chrome);
   evas_object_smart_callback_add(view, "link,hover,out",
                                  on_view_link_hover_out, chrome);
   evas_object_smart_callback_add(view, "popup,create", on_view_popup_new,
                                  win);

   edje_object_signal_emit(ed, "panels,reset,hide", "");
   _chrome_state_apply(chrome, view);

   elm_pager_content_push(win->pager, chrome);
   return chrome;

error_view_create:
   evas_object_del(chrome);
   return NULL;
}

