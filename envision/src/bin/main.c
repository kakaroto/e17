/*
 * Envision
 *
 * Copyright (C) 2010, Thiago Ribeiro Masaki <contact@profusion.mobi>
 * Copyright (C) 2010, Otavio Pontes <otavio@profusion.mobi>
 *
 * License LGPL-2.1, see COPYING file at project folder.
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <Elementary.h>
#include <Elementary_Cursor.h>
#include <Epdf.h>
#include <stdlib.h>
#include <string.h>
#ifndef ELM_LIB_QUICKLAUNCH

#include "gettext.h"

/*--------------------GLOBAL VALUES STORAGE------------------------*/
typedef struct _App            App;
typedef struct _Item_Data      Item_Data;
typedef struct _Highlight_Data Highlight_Data;
typedef enum _PageViewMode     PageViewMode;

struct _App
{
   Evas_Object *win;
   Evas_Object *fs_inwin;
   Evas_Object *bg;
   Evas_Object *grid;
   Evas_Object *entry;
   Evas_Object *ed;
   Evas_Object *fs;
   Evas_Object *clipper;
   Evas_Object *gl_index;
   Evas_Object *spinner;
   Evas_Object *toolbar;
   struct {
        Elm_Toolbar_Item *down;
        Elm_Toolbar_Item *up;
        Elm_Toolbar_Item *zoom_in;
        Elm_Toolbar_Item *zoom_out;
        Elm_Toolbar_Item *open;
        Elm_Toolbar_Item *fullscreen;
   } action;

   struct _file_info
   {
      Epdf_Document *document;
      int            pages_count;
      const char    *filename;
      Eina_List     *index;
   } file_info;

   const char       *text_to_search;

   Elm_Gengrid_Item *current_item_page;
   Ecore_Job        *update_job;
   Eina_Bool         single_page;
   Eina_Bool         bring_in_cur_page : 1;
   Eina_List        *visible_items;
};

/*----------------------PAGE DATA---------------------------------*/
struct _Item_Data
{
   Elm_Gengrid_Item *item;
   Epdf_Page        *page;
   int               page_width, page_height, page_number;
   App              *base;

   struct _search
   {
      Eina_List *matches;
      Eina_List *highlights;
   } search;
};

struct _Highlight_Data
{
   Evas_Coord   x, y, w, h;
   Evas_Object *rect;
   Evas_Object *item_obj;
};

enum _PageViewMode { SINGLE_PAGE, WINDOW_WIDTH };

/*-------------------------EINA LOG--------------------------------*/
static int _log_domain = -1;
#define CRITICAL(...) EINA_LOG_DOM_CRIT(_log_domain, __VA_ARGS__)
/********************************************************************/

static Elm_Gengrid_Item_Class gic;
static Elm_Genlist_Item_Class glc2;

static void
_viewport_resize(void        *data,
                 Evas *e      __UNUSED__,
                 Evas_Object *obj,
                 void *einfo  __UNUSED__)
{
   App *app = data;
   Evas_Coord w, h;
   evas_object_geometry_get(obj, NULL, NULL, &w, &h);
   evas_object_resize(app->clipper, w, h);
}

static void
_viewport_move(void        *data,
               Evas *e      __UNUSED__,
               Evas_Object *obj,
               void *einfo  __UNUSED__)
{
   App *app = data;
   Evas_Coord x, y;
   evas_object_geometry_get(obj, &x, &y, NULL, NULL);
   evas_object_move(app->clipper, x, y);
}

static void
_viewport_del(void            *data,
              Evas *e          __UNUSED__,
              Evas_Object *obj __UNUSED__,
              void *einfo      __UNUSED__)
{
   App *app = data;
   evas_object_del((Evas_Object *)app->clipper);
}

static void
_page_move(void        *data,
           Evas *e      __UNUSED__,
           Evas_Object *obj,
           void *einfo  __UNUSED__)
{
   Highlight_Data *item = data;
   Evas_Coord x, y;
   evas_object_geometry_get(obj, &x, &y, NULL, NULL);
   evas_object_move(item->rect, item->x + x, item->y + y);
}

static void
_page_del(void            *data,
          Evas *e          __UNUSED__,
          Evas_Object *obj __UNUSED__,
          void *einfo      __UNUSED__)
{
   Highlight_Data *item = data;
   evas_object_del(item->rect);
   free(item);
}

/*-------------------DRAW SEARCH RESULTS - HIGHLIGHTS----------------------*/
void
draw_search_highlights(Elm_Gengrid_Item *it)
{
   Evas_Object *item_obj;
   double sh, sv;
   Eina_List *l;
   App *app;
   Item_Data *idata;
   Epdf_Rectangle *rec;

   idata = elm_gengrid_item_data_get(it);
   if (!idata->search.matches) return;
   app = idata->base;
   epdf_page_scale_get(idata->page, &sh, &sv);

   item_obj = (Evas_Object *)elm_gengrid_item_object_get(it);
   EINA_LIST_FOREACH(idata->search.matches, l, rec)
     {
        Highlight_Data *item = malloc(sizeof(Highlight_Data));
        Evas_Object *r;
        r = elm_icon_add(app->win);
        elm_icon_file_set(r, PACKAGE_DATA_DIR "/default.edj",
                          "highlight");

        item->x = rec->x1 * sh + 25;
        item->y = rec->y1 * sh + 20;
        item->rect = r;
        evas_object_color_set(r, 255, 255, 255, 255);
        evas_object_move(r, (rec->x1 * sh), (2 + (rec->y1 * sh)));
        evas_object_resize(r, (rec->x2 - rec->x1) * sh, (rec->y2 - rec->y1) *
                           sv);
        evas_object_clip_set(r, app->clipper);
        evas_object_show(r);
        evas_object_event_callback_add(item_obj, EVAS_CALLBACK_MOVE, _page_move,
                                       item);
        evas_object_event_callback_add(item_obj, EVAS_CALLBACK_DEL, _page_del,
                                       item);
     }
}

/*-------------------UPDATE ITEMS - REPAINT PDF----------------------*/
static void
_update_items(void *data)
{
   App *app = data;
   Elm_Gengrid_Item *it;
   Eina_List *l;
   app->update_job = NULL;

   EINA_LIST_FOREACH(app->visible_items, l, it)
     elm_gengrid_item_update(it);
}

/*---------------------LABEL GET - SHOW LABEL------------------------*/
static char *
grid_label_get(void *data       __UNUSED__,
               Evas_Object *obj __UNUSED__,
               const char *part __UNUSED__)
{
   return NULL;
}

static void
grid_item_realized(void            *data,
                   Evas_Object *obj __UNUSED__,
                   void            *event_info)
{
   App *app = data;
   app->visible_items = eina_list_append(app->visible_items, event_info);
}

static void
grid_item_unrealized(void            *data,
                     Evas_Object *obj __UNUSED__,
                     void            *event_info)
{
   App *app = data;
   app->visible_items = eina_list_remove(app->visible_items, event_info);
}

static void
grid_changed(void            *data,
             Evas_Object *obj __UNUSED__,
             void *event_info __UNUSED__)
{
   App *app = data;
   if (!app->bring_in_cur_page) return;
   app->bring_in_cur_page = EINA_FALSE;
   elm_gengrid_item_bring_in(app->current_item_page);
}

/*---------------------ICON GET - RENDER PDF-------------------------*/
static Evas_Object *
grid_icon_get(void        *data,
              Evas_Object *obj,
              const char  *part)
{
   const Item_Data *idata = data;
   Evas_Coord w, h;

   elm_gengrid_item_size_get(obj, &w, &h);

   if (!strcmp(part, "elm.swallow.icon"))
     {
        Evas_Object *rect = evas_object_image_add(evas_object_evas_get(obj));
        epdf_page_scale_set(idata->page, ((double)w - 50) /
                            (double)idata->page_width,
                            ((double)h - 50) / (double)idata->page_height);
        epdf_page_render(idata->page, rect);
        if (idata->base->text_to_search) draw_search_highlights(idata->item);
        return rect;
     }
   return NULL;
}

/*--------------------------ITEM SELECT CB---------------------------*/
static void
grid_sel(void            *data,
         Evas_Object *obj __UNUSED__,
         void            *event_info)
{
   App *app = data;
   Elm_Gengrid_Item *item = event_info;
   Item_Data *idata;
   app->current_item_page = item;
   idata = elm_gengrid_item_data_get(item);

   elm_spinner_value_set(app->spinner, idata->page_number + 1);
}

/*--------------------------GRID ITEM DEL CB-------------------------*/
static void
grid_data_del(void            *data,
              Evas_Object *obj __UNUSED__)
{
   Item_Data *idata = data;
   epdf_page_delete(idata->page);
   free(idata);
}

/* FIXME */
static void
page_view_mode_set(App         *app,
                   PageViewMode mode)
{
   Evas_Coord h, w, item_w, item_h;
   Item_Data *idata;

   if (!app->current_item_page) return;

   idata = elm_gengrid_item_data_get(app->current_item_page);
   evas_object_geometry_get(app->grid, NULL, NULL, NULL, &h);

   switch(mode)
     {
      case SINGLE_PAGE:
        elm_gengrid_item_size_get(app->grid, &item_w, &item_h);
        item_w = ((h - 10) * idata->page_width) / idata->page_height;
        evas_object_size_hint_align_set(app->grid, 0.5, 0.5);
        elm_gengrid_item_size_set(app->grid, item_w, h - 10);
        if (app->update_job) ecore_job_del(app->update_job);
        app->update_job = ecore_job_add(_update_items, app);
        break;

      case WINDOW_WIDTH:
        evas_object_geometry_get(app->win, NULL, NULL, &w, NULL);
        item_h = (w * idata->page_height) / idata->page_width;
        elm_gengrid_item_size_set(app->grid, w, item_h);
        if (app->update_job) ecore_job_del(app->update_job);
        app->update_job = ecore_job_add(_update_items, app);
        break;

      default:
        break;
     }
}

/*------------------------ITEM DOUBLE CLICK CB-----------------------*/
static void
grid_item_db_double_clicked(void            *data,
                     Evas_Object *obj __UNUSED__,
                     void            *event_info)
{
   App *app = data;
   app->current_item_page = event_info;
   app->bring_in_cur_page = EINA_TRUE;
   page_view_mode_set((App *)data, WINDOW_WIDTH);
   elm_gengrid_item_bring_in(app->current_item_page);
}

/*--------------------------ZOOM IN CB-------------------------------*/
static void
_zoom_in(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   App *app = data;
   Item_Data *idata;
   Evas_Coord item_w, item_h;

   if (!app->current_item_page)
     return;

   elm_gengrid_item_size_get(app->grid, &item_w, &item_h);

   if (item_h > 2400)
     return;

   idata = (Item_Data *)elm_gengrid_item_data_get(app->current_item_page);
   if (!idata)
     return;

   item_h += 60;

   item_w = (item_h * idata->page_width) / idata->page_height;

   elm_gengrid_item_size_set(app->grid, item_w, item_h);

   if (app->update_job) ecore_job_del(app->update_job);
   app->update_job = ecore_job_add(_update_items, app);
   elm_gengrid_item_show(app->current_item_page);
}

/*--------------------------ZOOM OUT CB------------------------------*/
static void
_zoom_out(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   App *app = data;
   Item_Data *idata;
   Evas_Coord item_w, item_h;

   if (!app->current_item_page)
     return;

   elm_gengrid_item_size_get(app->grid, &item_w, &item_h);

   if (item_h < 240)
     return;

   idata = (Item_Data *)elm_gengrid_item_data_get(app->current_item_page);
   if (!idata)
     return;

   item_h -= 60;
   item_w = (item_h * idata->page_width) / idata->page_height;

   elm_gengrid_item_size_set(app->grid, item_w, item_h);

   if (app->update_job) ecore_job_del(app->update_job);
   app->update_job = ecore_job_add(_update_items, app);
}

static void
_page_up(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   App *app = data;
   const Elm_Gengrid_Item *it;
   Elm_Gengrid_Item *prev;
   it = elm_gengrid_selected_item_get(app->grid);
   if (!it)
     return;
   prev = elm_gengrid_item_prev_get(it);
   if (!prev)
     return;
   elm_gengrid_item_selected_set(prev, EINA_TRUE);
   elm_gengrid_item_show(prev);
}

/*--------------------------PAGE_DOWN CB-------------------------------*/
static void
_page_down(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   App *app = data;
   const Elm_Gengrid_Item *it;
   Elm_Gengrid_Item *next;
   it = elm_gengrid_selected_item_get(app->grid);
   if (!it)
     return;
   next = elm_gengrid_item_next_get(it);
   if (!next)
     return;
   elm_gengrid_item_selected_set(next, EINA_TRUE);
   elm_gengrid_item_show(next);
}

/*--------------------------FULL SCREEN CB-----------------------------*/
static void
_fullscreen(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   App *app = data;
   Eina_Bool fullscreen;

   fullscreen = !elm_win_fullscreen_get(app->win);

   elm_win_fullscreen_set(app->win, fullscreen);
   page_view_mode_set((App *)data, SINGLE_PAGE);
}

/*--------------------------OPEN FILE_SELECTOR CB----------------------*/
static void
_file_open(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   App *app = data;
   evas_object_show(app->fs_inwin);
}

/*--------------------------SEARCH BUTTON CB---------------------------*/
static void
_search(void            *data,
        Evas *e          __UNUSED__,
        Evas_Object *obj __UNUSED__,
        void *einfo      __UNUSED__)
{
   App *app = data;
   Elm_Gengrid_Item *it;
   Eina_Bool matched;

   if (!app->text_to_search)
     app->text_to_search =
       eina_stringshare_add(elm_scrolled_entry_entry_get(app->entry));
   else
     eina_stringshare_replace(&app->text_to_search,
                              elm_scrolled_entry_entry_get(app->entry));

   if (!strlen(app->text_to_search))
     {
        if (evas_object_visible_get(app->clipper))
          {
             evas_object_clip_unset(app->grid);
             evas_object_hide(app->clipper);
          }
        eina_stringshare_del(app->text_to_search);
        app->text_to_search = NULL;
        return;
     }

   it = elm_gengrid_first_item_get(app->grid);

   matched = EINA_FALSE;
   while (it)
     {
        Item_Data *idata = elm_gengrid_item_data_get(it);
        idata->search.matches = epdf_page_text_find(idata->page,
                                                    app->text_to_search, 0);
        it = elm_gengrid_item_next_get(it);

        matched |= (idata->search.matches != NULL);
     }

   if (matched)
     {
        if (!evas_object_visible_get(app->clipper))
          {
             evas_object_show(app->clipper);
             evas_object_clip_set(app->grid, app->clipper);
          }
     }
   else if (evas_object_visible_get(app->clipper))
     {
        evas_object_clip_unset(app->grid);
        evas_object_hide(app->clipper);
     }

   if (app->update_job) ecore_job_del(app->update_job);
   app->update_job = ecore_job_add(_update_items, app);
}

static void
_on_key_down(void            *data,
             Evas *e          __UNUSED__,
             Evas_Object *obj __UNUSED__,
             void            *einfo)
{
   Evas_Event_Key_Down *event = einfo;
   App *app = data;
   if (!strcmp(event->keyname, "F11"))
     _fullscreen(app, NULL, NULL);
}

/*--------------------------EPDF_LOAD--------------------------------*/
/**
 * @brief Load the Documment on memory
 *
 * @param App Application state variable
 * @param filename The file name of the file to be loaded
 */
static Eina_Bool
epdf_load(App        *app,
          const char *filename)
{
   if (!filename)
     return EINA_FALSE;

   app->file_info.document = epdf_document_new(filename);
   if (!app->file_info.document)
     {
        EINA_LOG_CRIT("Bad pdf file");
        return EINA_FALSE;
     }
   app->file_info.pages_count =
     epdf_document_page_count_get(app->file_info.document);

   app->file_info.index = epdf_index_new(app->file_info.document);
   return EINA_TRUE;
}

/* Epdf Page index still doesn't work*/
static void
_goto_page(void *data       __UNUSED__,
           Evas_Object *obj __UNUSED__,
           void *event_info __UNUSED__)
{
}

void
load_index_genlist(App              *app,
                   Elm_Genlist_Item *ancestor,
                   Eina_List        *children)
{
   Eina_List *l;
   Epdf_Index_Item *it;
   EINA_LIST_FOREACH(children, l, it)
     {
        Eina_List *ch;
        ch = epdf_index_item_children_get(it);
        if (ch)
          {
             Elm_Genlist_Item *parent;
             parent = elm_genlist_item_append(app->gl_index, &glc2, it,
                                              ancestor,
                                              ELM_GENLIST_ITEM_SUBITEMS,
                                              _goto_page, app->grid);
             load_index_genlist(app, parent, ch);
          }
        else
          {
             elm_genlist_item_append(app->gl_index, &glc2, it, ancestor,
                                     ELM_GENLIST_ITEM_NONE,
                                     _goto_page, app->grid);
          }
     }
}

/*--------------------------PAGE_SET---------------------------------*/
/**
 * @brief Load the page on a page item
 *
 * @param App Application state variable
 * @param page_number The number of the page to load
 */
static Item_Data *
page_set(App *app,
         int  page_number)
{
   Epdf_Page *page;
   Item_Data *idata;

   if (!app->file_info.document)
     return NULL;

   if (page_number >= epdf_document_page_count_get(app->file_info.document))
     {
        EINA_LOG_CRIT("Page number does not exist");
        return NULL;
     }

   page = epdf_page_new(app->file_info.document);
   if (!page)
     {
        EINA_LOG_CRIT("Bad page");
        epdf_document_delete(app->file_info.document);
        app->file_info.document = NULL;
        return NULL;
     }

   idata = malloc(sizeof(Item_Data));
   idata->page = page;
   idata->base = app;
   idata->page_number = page_number;
   epdf_page_page_set(idata->page, page_number);
   epdf_page_size_get(idata->page, &idata->page_width, &idata->page_height);

   return idata;
}

static void
_bt_close(void            *data,
          Evas_Object *obj __UNUSED__,
          void *event_info __UNUSED__)
{
   Evas_Object *notify = data;
   evas_object_hide(notify);
   evas_object_del(notify);
}

/*--------------------------GENGRID LOAD--------------------------*/
/**
 * @brief Clear gengrid items
 *
 * @param App Application state variable
 */
static void
unload_gengrid(App *app)
{
   if (!app->grid)
     return;

   if (!app->file_info.document)
     return;

   /* Clear the items */
   elm_gengrid_clear(app->grid);
   epdf_document_delete(app->file_info.document);
}

/**
 * @brief Populate the gengrid with pdf pages
 *
 * @param App Application state variable
 */
static Eina_Bool
load_gengrid(App *app)
{
   int i;
   Item_Data *idata;
   Elm_Gengrid_Item *first;
   char buf[256];

   unload_gengrid(app);

   if (!epdf_load(app, app->file_info.filename))
     return EINA_FALSE;

   first = NULL;

   for (i = 0; i < app->file_info.pages_count; i++)
     {
        idata = page_set(app, i);

        if (!idata) return EINA_FALSE;
        idata->item = elm_gengrid_item_append(app->grid, &gic, idata,
                                              grid_sel, app);

        elm_gengrid_item_cursor_set(idata->item, ELM_CURSOR_HAND1);
     }

   /* Select the first item */
   first = elm_gengrid_first_item_get(app->grid);
   elm_gengrid_item_selected_set(first, EINA_TRUE);
   app->current_item_page = first;
   idata = elm_gengrid_item_data_get(first);

   elm_gengrid_item_size_set(app->grid, idata->page_width, idata->page_height);

   snprintf(buf, sizeof(buf), "%%1.0f of %d", app->file_info.pages_count);
   elm_spinner_min_max_set(app->spinner, 1, app->file_info.pages_count);
   elm_spinner_label_format_set(app->spinner, buf);

   return EINA_TRUE;
}

static void
_change_selection(void            *data,
                  Evas_Object *obj __UNUSED__,
                  void *event_info __UNUSED__)
{
   App *app = data;
   int page;
   Elm_Gengrid_Item *it;
   Item_Data *idata;
   int range, i;

   if (!app->current_item_page)
     return;

   it = app->current_item_page;

   idata = elm_gengrid_item_data_get(it);
   if (!idata)
     return;

   page = (int)elm_spinner_value_get(app->spinner) - 1;
   if (idata->page_number < page)
     {
        range = (page - idata->page_number);
        for (i = 0; i < range; i++)
          it = elm_gengrid_item_next_get(it);
     }
   else if (idata->page_number > page)
     {
        range = idata->page_number - page;
        for (i = 0; i < range; i++)
          it = elm_gengrid_item_prev_get(it);
     }

   elm_gengrid_item_selected_set(it, EINA_TRUE);
   elm_gengrid_item_show(it);
}

/*--------------------------FILE SELECTOR OK CB-----------------------*/
static void
fileselector_done(void *data, Evas_Object *obj __UNUSED__, void *event_info)
{
   App *app = data;
   const char *selected = event_info;

   evas_object_hide(app->fs_inwin);

   if (!selected) return;

   eina_stringshare_replace(&app->file_info.filename, selected);

   if (!load_gengrid(app))
      CRITICAL("Failed to open file");

   edje_object_part_text_set(app->ed, "page-title", app->file_info.filename);
}

/*--------------------------WINDOW CLOSE CB--------------------------*/
static void
on_win_del_req(void *data       __UNUSED__,
               Evas_Object *obj __UNUSED__,
               void *event_info __UNUSED__)
{
   elm_exit();
}

char *
gl_label_get(void            *data,
             Evas_Object *obj __UNUSED__,
             const char *part __UNUSED__)
{
   char *label = data;
   return strdup(label);
}

Evas_Object *
gl_icon_get(void        *data,
            Evas_Object *obj,
            const char  *part)
{
   Evas_Object *ic = NULL;

   if (strcmp(part, "elm.swallow.icon"))
     return NULL;

   ic = elm_icon_add(obj);
   if (!strcmp(data, "Open"))
     elm_icon_file_set(ic, PACKAGE_DATA_DIR "/default.edj", "openfile");
   else
     elm_icon_file_set(ic, PACKAGE_DATA_DIR "/default.edj", "fullscreen");
   return ic;
}

static Elm_Toolbar_Item *
_toolbar_item_add(App *app, const char *icon, const char *label, int priority, Evas_Smart_Cb cb)
{
   Elm_Toolbar_Item *item = elm_toolbar_item_append(app->toolbar, icon, label,
                                                    cb, app);
   elm_toolbar_item_priority_set(item, priority);
   return item;
}

static Eina_Bool
create_main_win(App *app)
{
   Evas_Object *layout, *btsearch;
   Evas_Coord x, y, w, h;

   /* Window */
   app->win = elm_win_add(NULL, "Envision", ELM_WIN_BASIC);
   if (!app->win)
     {
        CRITICAL("Could not create window.");
        goto gui_error;
     }

   evas_object_resize(app->win, 480, 640);
   elm_win_title_set(app->win, "Envision");
   evas_object_smart_callback_add
     (app->win, "delete-request", on_win_del_req, app);
   elm_win_autodel_set(app->win, EINA_TRUE);

   /* Background */
   app->bg = elm_bg_add(app->win);
   if (!app->bg)
     {
        CRITICAL("Could not create background.");
        goto gui_error;
     }
   evas_object_size_hint_weight_set(app->bg, EVAS_HINT_EXPAND,
                                    EVAS_HINT_EXPAND);
   elm_win_resize_object_add(app->win, app->bg);
   evas_object_show(app->bg);

   /* Theme Extension */
   elm_theme_extension_add(NULL, PACKAGE_DATA_DIR "/default.edj");

   /* Layout */
   layout = elm_layout_add(app->win);
   if (!layout)
     {
        CRITICAL("Could not create Layout");
        goto gui_error;
     }

   if (!elm_layout_file_set(layout, PACKAGE_DATA_DIR "/default.edj", "main"))
     {
        CRITICAL("Can't load Edje Layout");
        goto gui_error;
     }

   app->ed = elm_layout_edje_get(layout);
   app->toolbar = edje_object_part_external_object_get
     (app->ed, "elm.external.toolbar");
   if (!app->toolbar)
     {
        CRITICAL("no toolbar in layout!");
        goto gui_error;
     }

   elm_toolbar_no_select_mode_set(app->toolbar, EINA_TRUE);
   elm_toolbar_menu_parent_set(app->toolbar, app->win);
   app->action.down = _toolbar_item_add
      (app, "go-down", "Down", 200, _page_down);
   app->action.up = _toolbar_item_add
      (app, "go-up", "Up", 190, _page_up);
   app->action.zoom_in = _toolbar_item_add
      (app, "zoom-in", "Zoom", 180, _zoom_in);
   app->action.zoom_out = _toolbar_item_add
      (app, "zoom-out", "Zoom", 170, _zoom_out);
   app->action.open = _toolbar_item_add
      (app, "document-open", "Open", 160, _file_open);
   app->action.fullscreen = _toolbar_item_add
      (app, "view-fullscreen", "Fullscreen", 150, _fullscreen);

   evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(app->win, layout);
   evas_object_show(layout);
   if (app->file_info.filename)
     edje_object_part_text_set(app->ed, "page-title", app->file_info.filename);

   app->spinner = elm_spinner_add(app->win);
   elm_object_style_set(app->spinner, "efenniht");
   elm_spinner_step_set(app->spinner, 1);
   elm_spinner_min_max_set(app->spinner, 0, 0);
   elm_layout_content_set(layout, "page-spinner", app->spinner);
   evas_object_smart_callback_add(app->spinner, "changed", _change_selection,
                                  app);

   app->entry = elm_scrolled_entry_add(app->ed);
   elm_object_style_set(app->entry, "efenniht/search");
   elm_scrolled_entry_single_line_set(app->entry, EINA_TRUE);
   btsearch = elm_icon_add(app->win);
   elm_icon_file_set(btsearch, PACKAGE_DATA_DIR "/default.edj",
                     "search_button");
   evas_object_size_hint_min_set(btsearch, 20, 20);
   evas_object_show(btsearch);
   evas_object_event_callback_add(btsearch, EVAS_CALLBACK_MOUSE_DOWN, _search,
                                  app);

   elm_scrolled_entry_end_set(app->entry, btsearch);
   elm_layout_content_set(layout, "search-entry", app->entry);

   /* Open file */
   app->fs_inwin = elm_win_inwin_add(app->win);
   if(!app->fs_inwin)
     {
        CRITICAL("Could not create Inwin");
        goto gui_error;
     }

   app->fs = elm_fileselector_add(app->win);
   if(!app->fs)
     {
        CRITICAL("Could not create file selector");
        goto gui_error;
     }

   elm_fileselector_expandable_set(app->fs, EINA_TRUE);
   elm_fileselector_path_set(app->fs, getenv("HOME"));
   evas_object_size_hint_weight_set(app->fs, EVAS_HINT_EXPAND,
                                    EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(app->fs, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_win_inwin_content_set(app->fs_inwin, app->fs);
   evas_object_show(app->fs);
   evas_object_smart_callback_add(app->fs, "done", fileselector_done, app);

   /*  Gengrid  */

   app->grid = elm_gengrid_add(app->win);
   if(!app->grid)
     {
        CRITICAL("Could not create file selector");
        goto gui_error;
     }
   elm_object_style_set(app->grid, "efenniht");
   elm_gengrid_align_set(app->grid, 0.5, 0.0);
   elm_gengrid_horizontal_set(app->grid, EINA_FALSE);
   elm_gengrid_multi_select_set(app->grid, EINA_FALSE);
   elm_gengrid_bounce_set(app->grid, EINA_FALSE, EINA_FALSE);
   evas_object_smart_callback_add(app->grid, "clicked,double", grid_item_db_double_clicked,
                                  app);
   evas_object_smart_callback_add(app->grid, "realized", grid_item_realized,
                                  app);
   evas_object_smart_callback_add(app->grid, "unrealized", grid_item_unrealized,
                                  app);
   evas_object_smart_callback_add(app->grid, "changed", grid_changed,
                                  app);
   gic.func.label_get = grid_label_get;
   gic.func.content_get = grid_icon_get;
   gic.func.del = grid_data_del;

   app->clipper = evas_object_rectangle_add(evas_object_evas_get(app->grid));
   evas_object_geometry_get(app->grid, &x, &y, &w, &h);
   evas_object_resize(app->clipper, w, h);
   evas_object_move(app->clipper, x, y);
   evas_object_event_callback_add(app->grid, EVAS_CALLBACK_MOVE, _viewport_move,
                                  app);
   evas_object_event_callback_add(app->grid, EVAS_CALLBACK_RESIZE,
                                  _viewport_resize, app);
   evas_object_event_callback_add(app->grid, EVAS_CALLBACK_DEL, _viewport_del,
                                  app);

   elm_layout_content_set(layout, "view", app->grid);
   evas_object_event_callback_add(app->win, EVAS_CALLBACK_KEY_DOWN,
                                  _on_key_down, app);
   evas_object_event_callback_add(layout, EVAS_CALLBACK_KEY_DOWN,
                                  _on_key_down, app);
   evas_object_event_callback_add(app->grid, EVAS_CALLBACK_KEY_DOWN,
                                  _on_key_down, app);
   evas_object_show(app->win);

   return EINA_TRUE;

gui_error:
   if (app->win) evas_object_del(app->win);
   if (app->bg) evas_object_del(app->bg);
   if (layout) evas_object_del(layout);
   if (app->fs_inwin) evas_object_del(app->fs_inwin);
   if (app->fs) evas_object_del(app->fs);
   if (app->grid) evas_object_del(app->grid);
   return EINA_FALSE;
}

EAPI int
elm_main(int argc, char **argv)
{
   App app;
   int r = 0;

   _log_domain = eina_log_domain_register("Envision", NULL);
   if (_log_domain < 0)
     {
        EINA_LOG_CRIT("could not create log domain 'Envision'.");
        return -1;
     }

   if (!epdf_init())
     {
        EINA_LOG_CRIT("Epdf can't be initialized");
        r = -1;
        goto end;
     }

   memset(&app, 0, sizeof(app));

   if (!create_main_win(&app))
     {
        r = -1;
        goto end;
     }

   if (argc > 1)
     {
        app.file_info.filename = eina_stringshare_add(argv[1]);
        load_gengrid(&app);
     }

   elm_run();

   if (app.file_info.document)
     epdf_document_delete(app.file_info.document);

end:
   eina_stringshare_del(app.file_info.filename);
   eina_stringshare_del(app.text_to_search);

   if (app.update_job)
     ecore_job_del(app.update_job);

   epdf_shutdown();
   eina_log_domain_unregister(_log_domain);
   _log_domain = -1;
   elm_shutdown();

   return r;
}

#endif
ELM_MAIN()

