#include <Elementary_Cursor.h>
#include "Clouseau.h"
#include <Ecore_Ipc.h>

#include "clouseau_private.h"

#define CLIENT_NAME         "Clouseau Client"

#define SHOW_SCREENSHOT     "/images/show-screenshot.png"
#define TAKE_SCREENSHOT     "/images/take-screenshot.png"
#define SCREENSHOT_MISSING  "/images/screenshot-missing.png"

struct _app_data_st
{
   Variant_st *app;  /* app->data is (app_info_st *)   */
   Variant_st *td;   /* tree->data is (tree_data_st *) */
};
typedef struct _app_data_st app_data_st;

struct _bmp_node
{
   unsigned int ctr;           /* Current refresh_ctr */
   unsigned long long object;  /* Evas ptr        */
   Evas_Object *bt;            /* Button of BMP_REQ */
};
typedef struct _bmp_node bmp_node;

struct _gui_elements
{
   Evas_Object *win;
   Evas_Object *bx;     /* The main box */
   Evas_Object *hbx;    /* The top menu box */
   Evas_Object *panel;  /* Button panel */
   Evas_Object *bt_load;
   Evas_Object *bt_save;
   Evas_Object *dd_list;
   Evas_Object *gl;
   Evas_Object *lb;  /* Label showing backtrace */
   Evas_Object *prop_list;
   Evas_Object *connect_inwin;
   Evas_Object *save_inwin;
   Evas_Object *en;
   Evas_Object *pb; /* Progress wheel shown when waiting for TREE_DATA */
   char *address;
   app_data_st *sel_app; /* Currently selected app data */
};
typedef struct _gui_elements gui_elements;

static int _load_list(gui_elements *g);
static void _bt_load_file(void *data, Evas_Object *obj EINA_UNUSED, void *event_info);
static void _show_gui(gui_elements *g, Eina_Bool work_offline);

/* Globals */
static gui_elements *gui = NULL;
static Eina_List *apps = NULL;    /* List of (app_data_st *)  */
static Eina_List *bmp_req = NULL; /* List of (bmp_node *)     */

static Elm_Genlist_Item_Class itc;
static Eina_Bool list_show_clippers = EINA_TRUE, list_show_hidden = EINA_TRUE;
static Eina_Bool do_highlight = EINA_TRUE;
static Ecore_Ipc_Server *svr = NULL;
static Eina_Bool _add_callback_called = EINA_FALSE;
static void _cancel_bt_clicked(void *data, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED);
static void _ofl_bt_clicked(void *data, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED);

static void
_titlebar_string_set(gui_elements *g, Eina_Bool online)
{
   if (online)
     {
        char *str = malloc(strlen(CLIENT_NAME) + strlen(g->address) + 32);
        sprintf(str, "%s - %s", CLIENT_NAME, g->address);
        elm_win_title_set(g->win, str);
        free(str);
     }
   else
     {
        char *str = malloc(strlen(CLIENT_NAME) + 32);
        sprintf(str, "%s - Offline", CLIENT_NAME);
        elm_win_title_set(g->win, str);
        free(str);
     }
}

Eina_Bool
_add(void *data EINA_UNUSED, int type EINA_UNUSED, void *event)
{
   Ecore_Ipc_Event_Server_Add *ev = event;
   void *p;
   int size = 0;

   _add_callback_called = EINA_TRUE;
   ecore_ipc_server_data_size_max_set(ev->server, -1);

   if (svr)
     {
        connect_st t = { getpid(), __FILE__ };
        p = clouseau_data_packet_compose(CLOUSEAU_GUI_CLIENT_CONNECT,
              &t, sizeof(t), &size, NULL, 0);

        if (p)
          {
             ecore_ipc_server_send(ev->server, 0,0,0,0,EINA_FALSE, p, size);
             ecore_ipc_server_flush(ev->server);
             free(p);
          }

        _titlebar_string_set(gui, EINA_TRUE);
     }

   return ECORE_CALLBACK_RENEW;
}

static void
_set_button(Evas_Object *w, Evas_Object *bt,
      char *ic_name, char *tip, Eina_Bool en)
{  /* Update button icon and tooltip */
   char buf[1024];
   Evas_Object *ic = elm_icon_add(w);
   snprintf(buf, sizeof(buf), "%s%s", PACKAGE_DATA_DIR, ic_name);
   elm_image_file_set(ic, buf, NULL);
   elm_object_part_content_set(bt, "icon", ic);
   elm_object_tooltip_text_set(bt, tip);
   elm_object_disabled_set(bt, en);
   evas_object_show(ic);
}

static void
_work_offline_popup(void)
{
   Evas_Object *bxx, *lb, *bt_bx, *bt_ofl, *bt_exit;
   /* START - Popup asking user to close client or work offline */
   gui->connect_inwin = elm_win_inwin_add(gui->win);
   evas_object_show(gui->connect_inwin);

   bxx = elm_box_add(gui->connect_inwin);
   elm_object_style_set(gui->connect_inwin, "minimal_vertical");
   evas_object_size_hint_weight_set(bxx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bxx);

   lb = elm_label_add(bxx);
   evas_object_size_hint_weight_set(lb, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(lb, EVAS_HINT_FILL, 0.0);
   elm_object_text_set(lb, "Connection to server failed.");
   elm_box_pack_end(bxx, lb);
   evas_object_show(lb);

   bt_bx = elm_box_add(bxx);
   elm_box_horizontal_set(bt_bx, EINA_TRUE);
   elm_box_homogeneous_set(bt_bx, EINA_TRUE);
   evas_object_size_hint_align_set(bt_bx, 0.5, 0.5);
   evas_object_size_hint_weight_set(bt_bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bt_bx);
   elm_box_pack_end(bxx, bt_bx);

   /* Add the exit button */
   bt_exit = elm_button_add(bt_bx);
   elm_object_text_set(bt_exit, "Exit");
   evas_object_smart_callback_add(bt_exit, "clicked",
         _cancel_bt_clicked, (void *) gui);

   elm_box_pack_end(bt_bx, bt_exit);
   evas_object_show(bt_exit);

   bt_ofl = elm_button_add(bt_bx);
   elm_object_text_set(bt_ofl, "Work Offline");
   evas_object_smart_callback_add(bt_ofl, "clicked",
         _ofl_bt_clicked, (void *) gui);

   elm_box_pack_end(bt_bx, bt_ofl);
   evas_object_show(bt_ofl);

   elm_win_inwin_content_set(gui->connect_inwin, bxx);
   /* END   - Popup asking user to close client or work offline */
}

Eina_Bool
_del(void *data EINA_UNUSED, int type EINA_UNUSED, void *event)
{
   Ecore_Ipc_Event_Server_Del *ev = event;

   if ((!_add_callback_called) || (!ev->server))
     {  /* if initial connection with daemon failed - exit */
        ecore_ipc_server_del(ev->server);
        svr = NULL; /* Global svr var */
        _work_offline_popup();
        return ECORE_CALLBACK_RENEW;
     }

   printf("Lost server with ip %s!\n", ecore_ipc_server_ip_get(ev->server));

   ecore_ipc_server_del(ev->server);
   svr = NULL; /* Global svr var */
   _show_gui(gui, EINA_TRUE);

   return ECORE_CALLBACK_RENEW;
}

static void
clouseau_lines_free(bmp_info_st *st)
{  /* Free lines asociated with a bmp */
   if (st->lx)
     evas_object_del(st->lx);

   if (st->ly)
     evas_object_del(st->ly);

   st->lx = st->ly = NULL;
}

static void
clouseau_bmp_blob_free(bmp_info_st *st)
{  /* We also free all lines drawn in this bmp canvas */
   clouseau_lines_free(st);

   if (st->bmp)
     free(st->bmp);
}

static Eina_Bool
_load_gui_with_list(gui_elements *g, Eina_List *trees)
{
   Eina_List *l;
   Clouseau_Tree_Item *treeit;

   elm_object_text_set(g->lb, NULL); /* Clear backtrace label */

   if (!trees)
     return EINA_TRUE;

   /* Stop progress wheel as we load tree data */
   elm_progressbar_pulse(g->pb, EINA_FALSE);
   evas_object_hide(g->pb);

   EINA_LIST_FOREACH(trees, l, treeit)
     {  /* Insert the base ee items */
        Elm_Genlist_Item_Type glflag = (treeit->children) ?
           ELM_GENLIST_ITEM_TREE : ELM_GENLIST_ITEM_NONE;
        elm_genlist_item_append(g->gl, &itc, treeit, NULL,
              glflag, NULL, NULL);
     }

   return EINA_TRUE;
}

static char *
_app_name_get(app_info_st *app)
{
   char *str = NULL;
   if (app->file)
     {
        char *tmp = strdup(app->file);
        char *bname = basename(tmp);
        str = malloc(strlen(bname) + strlen(app->name) + 32);
        sprintf(str, "%s:%s [%d]", bname, app->name, app->pid);
        free(tmp);
     }
   else
     {
        str = malloc(strlen(app->name)+32);
        sprintf(str, "%s [%d]", app->name, app->pid);
     }

   return str;  /* User has to free(str) */
}

static void
_close_app_views(app_info_st *app, Eina_Bool clr)
{  /* Close any open-views if this app */
   Eina_List *l;
   Variant_st *view;
   EINA_LIST_FOREACH(app->view, l, view)
     {
        bmp_info_st *b = view->data;
        if (b->win)
          evas_object_del(b->win);

        if (b->bt)
          elm_object_disabled_set(b->bt, EINA_FALSE);

        b->win = b->bt = NULL;
     }

   if (clr)
     {  /* These are cleared when app data is reloaded */
        EINA_LIST_FREE(app->view, view)
          {  /* Free memory allocated to show any app screens */
             clouseau_bmp_blob_free(view->data);
             clouseau_data_variant_free(view);
          }

        app->view = NULL;
     }
}

static void
_set_selected_app(void *data, Evas_Object *pobj,
      void *event_info EINA_UNUSED)
{  /* Set hovel label */
   app_data_st *st = data;
   elm_progressbar_pulse(gui->pb, EINA_FALSE);
   evas_object_hide(gui->pb);

   if (gui->sel_app)
     _close_app_views(gui->sel_app->app->data, EINA_FALSE);

   if (st)
     {
        if (!svr)
          {  /* Got TREE_DATA from file, update this immidately */
              gui->sel_app = st;
             char *str = _app_name_get(st->app->data);
             elm_object_text_set(pobj, str);
             free(str);
             _load_list(gui);
             return;
          }

        if (gui->sel_app != st)
          {  /* Reload only of selected some other app */
             gui->sel_app = st;
             char *str = _app_name_get(st->app->data);
             elm_object_text_set(pobj, str);
             free(str);

             elm_progressbar_pulse(gui->pb, EINA_FALSE);
             evas_object_hide(gui->pb);
             _load_list(gui);
          }
     }
   else
     {  /* If we got a NULL ptr, reset lists and dd_list text */
        elm_object_text_set(pobj, "SELECT APP");
        elm_genlist_clear(gui->gl);
        elm_genlist_clear(gui->prop_list);
        gui->sel_app = NULL;
     }

   if (svr)
     {  /* Enable/Disable buttons only if we are online */
        elm_object_disabled_set(gui->bt_load, (gui->sel_app == NULL));
        elm_object_disabled_set(gui->bt_save, (gui->sel_app == NULL));
     }
}

static int
_app_ptr_cmp(const void *d1, const void *d2)
{
   const app_data_st *info = d1;
   app_info_st *app = info->app->data;

   return ((app->ptr) - (unsigned long long) (uintptr_t) d2);
}

static void
_add_app_to_dd_list(Evas_Object *dd_list, app_data_st *st)
{  /* Add app to Drop Down List */
   char *str = _app_name_get(st->app->data);
   elm_hoversel_item_add(dd_list, str, NULL, ELM_ICON_NONE,
         _set_selected_app, st);

   free(str);
}

static int
_bmp_object_ptr_cmp(const void *d1, const void *d2)
{  /* Comparison according to Evas ptr of BMP struct */
   const Variant_st *info = d1;
   bmp_info_st *bmp = info->data;

   return ((bmp->object) - (unsigned long long) (uintptr_t) d2);
}

static int
_bmp_app_ptr_cmp(const void *d1, const void *d2)
{  /* Comparison according to app ptr of BMP struct */
   const Variant_st *info = d1;
   bmp_info_st *bmp = info->data;

   return ((bmp->app) - (unsigned long long) (uintptr_t) d2);
}

static Eina_List *
_remove_bmp(Eina_List *view, void *ptr)
{  /* Remove app bitmap from bitmaps list */
   Variant_st *v = (Variant_st *)
      eina_list_search_unsorted(view, _bmp_app_ptr_cmp,
            (void *) (uintptr_t) ptr);

   if (v)
     {
        bmp_info_st *st = v->data;
        if (st->win)
          evas_object_del(st->win);

        if (st->bmp)
          free(st->bmp);

        clouseau_data_variant_free(v);
        return eina_list_remove(view, v);
     }

   return view;
}

static app_data_st *
_add_app(gui_elements *g, Variant_st *v)
{
   app_data_st *st;

   st = malloc(sizeof(app_data_st));
   if (!st) return NULL;

   st->app = v;
   st->td = NULL; /* Will get this on TREE_DATA message */
   apps = eina_list_append(apps, st);

   _add_app_to_dd_list(g->dd_list, st);

   return st;
}

static void
_free_app_tree_data(Variant_st *td)
{
   tree_data_st *ftd;

   if (!td) return ;

   ftd = td->data;
   clouseau_data_tree_free(ftd->tree);
   clouseau_data_variant_free(td);
}

static void
_free_app(app_data_st *st)
{
   Variant_st *view;
   app_info_st *app = st->app->data;
   if (app->file)
     free(app->file);

   EINA_LIST_FREE(app->view, view)
     {  /* Free memory allocated to show any app screens */
        bmp_info_st *b = view->data;
        if (b->win)
          evas_object_del(b->win);

        if (b->bmp)
          free(b->bmp);

        clouseau_data_variant_free(view);
     }

   clouseau_data_variant_free(st->app);
   _free_app_tree_data(st->td);
   free(st);
}

static void
_remove_app(gui_elements *g, Variant_st *v)
{
   app_closed_st *app = v->data;
   /* Handle the case that NO app is selected, set sel_app to NULL */
   app_info_st *sel_app = (g->sel_app) ? g->sel_app->app->data : NULL;
   app_data_st *st = (app_data_st *)
      eina_list_search_unsorted(apps, _app_ptr_cmp,
            (void *) (uintptr_t) app->ptr);

   /* if NO app selected OR closing app is the selected one, reset display */
   if ((!sel_app) || (app->ptr == sel_app->ptr))
     _set_selected_app(NULL, g->dd_list, NULL);

   if (st)
     {  /* Remove from list and free all variants */
        Eina_List *l;
        apps = eina_list_remove(apps, st);
        _free_app(st);

        if (elm_hoversel_expanded_get(g->dd_list))
          elm_hoversel_hover_end(g->dd_list);

        elm_hoversel_clear(g->dd_list);
        EINA_LIST_FOREACH(apps, l , st)
           _add_app_to_dd_list(g->dd_list, st);
     }

   clouseau_data_variant_free(v);
}

static void
_update_tree_offline(gui_elements *g, Variant_st *v)
{
   tree_data_st *td = v->data;
   elm_genlist_clear(g->gl);
   _load_gui_with_list(g, td->tree);
}

static void
_update_tree(gui_elements *g, Variant_st *v)
{  /* Update Tree for app, then update GUI if its displayed */
   tree_data_st *td = v->data;
   app_info_st *selected = g->sel_app->app->data;

   /* Update only if tree is from APP on our list */
   app_data_st *st = (app_data_st *)
      eina_list_search_unsorted(apps, _app_ptr_cmp,
            (void *) (uintptr_t) td->app);

   if (st)
     {  /* Free app TREE_DATA then set ptr to new data */
        _free_app_tree_data(st->td);
        st->td = v;

        if (selected->ptr == td->app)
          {  /* Update GUI only if TREE_DATA is from SELECTED app */
             elm_genlist_clear(g->gl);
             _load_gui_with_list(g, td->tree);
          }
     }
   else
     {  /* Happens when TREE_DATA of app that already closed has arrived */
        _free_app_tree_data(v);                   /* Dispose unused info */
     }
}

static int
_bmp_node_cmp(const void *d1, const void *d2)
{  /* Compare accoring to Evas ptr */
   const bmp_node *info = d1;

   return ((info->object) - (unsigned long long) (uintptr_t) d2);
}

static bmp_node *
_get_bmp_node(bmp_info_st *st, app_info_st *app)
{  /* Find the request of this bmp info, in the req list         */
   /* We would like to verify this bmp_info_st is still relevant */
   Eina_List *req_list = bmp_req;
   bmp_node *nd = NULL;

   if (!app)
     return NULL;

   do
     { /* First find according to Evas ptr, then match ctr with refresh_ctr */
        req_list = eina_list_search_unsorted_list(req_list, _bmp_node_cmp,
              (void *) (uintptr_t) st->object);

        if (req_list)
          nd = (bmp_node *) eina_list_data_get(req_list);

        if (nd)
          {  /* if found this object in list, compare ctr */
             if (nd->ctr == app->refresh_ctr)
               return nd;

             /* ctr did not match, look further in list */
             req_list = eina_list_next(req_list);
          }
     }
   while(req_list);

   return NULL;
}

static void
_add_bmp(gui_elements *g EINA_UNUSED, Variant_st *v)
{  /* Remove bmp if exists (according to obj-ptr), then add the new one */
   bmp_info_st *st = v->data;
   st->zoom_val = 1.0; /* Init zoom value */

   app_data_st *app = (app_data_st *)
      eina_list_search_unsorted(apps, _app_ptr_cmp,
            (void *) (uintptr_t) st->app);

   /* Check for relevant bmp req in the bmp_req list */
   bmp_node *nd = _get_bmp_node(st, app->app->data);

   if (!st->bmp)
     {  /* We consider a case out request will be answered with empty bmp
           this may happen if we have a sub-window of app
           (like checks in elementary test)
           if the user closed it just as we send our BMP_REQ
           this Evas is no longer valid and we get NULL ptr for BMP.
           This code ignores this case. */
        elm_progressbar_pulse(g->pb, EINA_FALSE);
        evas_object_hide(g->pb);
        clouseau_data_variant_free(v);

        /* Make refresh button display: screenshot NOT available */
        if (nd)
          _set_button(g->win, nd->bt,
                SCREENSHOT_MISSING,
                "Screenshot not available", EINA_TRUE);
        return;
     }

   if (app && nd)
     {  /* Remove app bmp data if exists, then update */
        elm_progressbar_pulse(g->pb, EINA_FALSE);
        evas_object_hide(g->pb);

        app_info_st *info = app->app->data;
        info->view = _remove_bmp(info->view,
              (void *) (uintptr_t) (st->object));
        info->view = eina_list_append(info->view, v);

        /* Now we need to update refresh button, make it open-window */
        _set_button(g->win, nd->bt,
              SHOW_SCREENSHOT,
              "Show App Screenshot", EINA_FALSE);

        bmp_req = eina_list_remove(bmp_req, nd);
        free(nd);
     }
   else
     {  /* Dispose bmp info if app no longer in the list of apps */
        /* or the bmp_info is no longer relevant */
        if (st->bmp)
          free(st->bmp);

        clouseau_data_variant_free(v);
     }
}

static void
clouseau_make_lines(bmp_info_st *st, Evas_Coord xx, Evas_Coord yy)
{  /* and no, we are NOT talking about WHITE lines */
   Evas_Coord x_rgn, y_rgn, w_rgn, h_rgn;

   clouseau_lines_free(st);

   elm_scroller_region_get(st->scr, &x_rgn, &y_rgn, &w_rgn, &h_rgn);

   st->lx = evas_object_line_add(evas_object_evas_get(st->o));
   st->ly = evas_object_line_add(evas_object_evas_get(st->o));
   evas_object_repeat_events_set(st->lx, EINA_TRUE);
   evas_object_repeat_events_set(st->ly, EINA_TRUE);

   evas_object_line_xy_set(st->lx, 0, yy, w_rgn, yy);
   evas_object_line_xy_set(st->ly, xx, 0, xx, h_rgn);

   evas_object_color_set(st->lx, HIGHLIGHT_R, HIGHLIGHT_G, HIGHLIGHT_B,
         HIGHLIGHT_A);
   evas_object_color_set(st->ly, HIGHLIGHT_R, HIGHLIGHT_G, HIGHLIGHT_B,
         HIGHLIGHT_A);
   evas_object_show(st->lx);
   evas_object_show(st->ly);
}

static void
clouseau_lines_cb(void *data,
      Evas *e EINA_UNUSED, Evas_Object *obj EINA_UNUSED,
      void *event_info)
{
   if (((Evas_Event_Mouse_Down *) event_info)->button == 1)
     return; /* Draw line only if not left mouse button */

   clouseau_make_lines(data, 
         (((Evas_Event_Mouse_Move *) event_info)->cur.canvas.x),
         (((Evas_Event_Mouse_Move *) event_info)->cur.canvas.y));
}

static void
_mouse_out(void *data,
      Evas *e EINA_UNUSED, Evas_Object *obj EINA_UNUSED,
      EINA_UNUSED void *event_info)
{
   bmp_info_st *st = data;
   elm_object_text_set(st->lb_mouse, " ");
   elm_object_text_set(st->lb_rgba, " ");
}

static void
_mouse_move(void *data,
      Evas *e EINA_UNUSED, Evas_Object *obj EINA_UNUSED,
      void *event_info)
{  /* Event info is label getting mouse pointer cords */
   bmp_info_st *st = data;
   unsigned char *pt;
   char s_bar[64];
   float dx, dy;
   Evas_Coord mp_x, mp_y, xx, yy;
   Evas_Coord x, y, w, h;

   mp_x = (((Evas_Event_Mouse_Move *) event_info)->cur.canvas.x);
   mp_y = (((Evas_Event_Mouse_Move *) event_info)->cur.canvas.y);
   evas_object_geometry_get(st->o, &x, &y, &w, &h);

   dx = ((float) (mp_x - x)) / ((float) w);
   dy = ((float) (mp_y - y)) / ((float) h);

   xx = dx * st->w;
   yy = dy * st->h;

   sprintf(s_bar, "%dx%d", xx, yy);

   elm_object_text_set(st->lb_mouse, s_bar);

   if (((Evas_Event_Mouse_Move *) event_info)->buttons > 1)
     clouseau_make_lines(st, mp_x, mp_y);

   if (((xx >= 0) && (xx < ((Evas_Coord) st->w))) &&
         ((yy >= 0) && (yy < ((Evas_Coord) st->h))))
     { /* Need to test borders, because image may be scrolled */
        pt = ((unsigned char *) st->bmp) + (((yy * st->w) + xx) * sizeof(int));
        sprintf(s_bar, "rgba(%d,%d,%d,%d)", pt[2], pt[1], pt[0], pt[3]);
        elm_object_text_set(st->lb_rgba, s_bar);
     }
   else
     elm_object_text_set(st->lb_rgba, " ");
}

static void
_app_win_del(void *data,
      Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{  /* when closeing view, set view ptr to NULL, and enable open button */
   bmp_info_st *st = data;
   elm_object_disabled_set(st->bt, EINA_FALSE);
   evas_object_event_callback_del(st->o, EVAS_CALLBACK_MOUSE_MOVE,
         _mouse_move);
   evas_object_event_callback_del(st->o, EVAS_CALLBACK_MOUSE_OUT,
         _mouse_out);
   evas_object_event_callback_del(st->o, EVAS_CALLBACK_MOUSE_DOWN,
         clouseau_lines_cb);
   st->win = st->bt = st->lb_mouse = st->o = NULL;
}

/* START - Callbacks to handle zoom on app window (screenshot) */
static Evas_Event_Flags
reset_view(void *data , void *event_info EINA_UNUSED)
{  /* Cancel ZOOM and remove LINES on double tap */
   bmp_info_st *st = data;
   st->zoom_val = 1.0;
   clouseau_lines_free(st);
   evas_object_size_hint_min_set(st->o, st->w, st->h);

   return EVAS_EVENT_FLAG_ON_HOLD;
}

static void
_update_zoom(Evas_Object *img, Evas_Object *scr, Evas_Coord zx,
      Evas_Coord zy, double zoom, Evas_Coord origw, Evas_Coord origh)
{
   Evas_Coord origrelx = 0, origrely= 0;
   Evas_Coord offx = 0, offy= 0;

   Evas_Coord sx, sy, sw, sh;
   elm_scroller_region_get(scr, &sx, &sy, &sw, &sh);

   /* Get coords on pic. */
     {
        Evas_Coord x, y, w, h;
        evas_object_geometry_get(img, &x, &y, &w, &h);
        double ratio = (((double) origw) / w) * zoom;
        origrelx = ratio * (double) (zx - x);
        origrely = ratio * (double) (zy - y);

        /* Offset of the cursor from the first visible pixel of the
         * content. */
        offx = (zx - x) - sx;
        offy = (zy - y) - sy;
     }

   Evas_Coord imw, imh;
   imw = origw * zoom;
   imh = origh * zoom;
   evas_object_size_hint_min_set(img, imw, imh);
   evas_object_size_hint_max_set(img, imw, imh);

   elm_scroller_region_show(scr, origrelx - offx, origrely - offy, sw, sh);
}

static Evas_Event_Flags
zoom_start(void *data , void *event_info)
{
   bmp_info_st *st = data;
   Elm_Gesture_Zoom_Info *p = (Elm_Gesture_Zoom_Info *) event_info;
   clouseau_lines_free(st);
   _update_zoom(st->o, st->scr, p->x, p->y, st->zoom_val, st->w, st->h);

   return EVAS_EVENT_FLAG_ON_HOLD;
}

static Evas_Event_Flags
zoom_move(void *data , void *event_info)
{
   bmp_info_st *st = data;
   Elm_Gesture_Zoom_Info *p = (Elm_Gesture_Zoom_Info *) event_info;
   _update_zoom(st->o, st->scr, p->x, p->y,
         st->zoom_val * p->zoom, st->w, st->h);

   return EVAS_EVENT_FLAG_ON_HOLD;
}

static Evas_Event_Flags
zoom_end(void *data , void *event_info)
{
   Elm_Gesture_Zoom_Info *p = (Elm_Gesture_Zoom_Info *) event_info;
   bmp_info_st *st = data;
   st->zoom_val *= p->zoom;

   return EVAS_EVENT_FLAG_ON_HOLD;
}
/* END   - Callbacks to handle zoom on app window (screenshot) */

static void
_open_app_window(bmp_info_st *st, Evas_Object *bt, Clouseau_Tree_Item *treeit)
{
#define SHOT_HEADER " - Screenshot"
#define SBAR_PAD_X 4
#define SBAR_PAD_Y 2

   Evas_Object *tb, *bg, *lb_size, *hbx, *glayer;

   char s_bar[128];
   char *win_name = malloc(strlen(treeit->name) + strlen(SHOT_HEADER) + 1);
   st->bt = bt;
   st->win = elm_win_add(NULL, "win", ELM_WIN_BASIC);
   sprintf(win_name, "%s%s", treeit->name, SHOT_HEADER);
   elm_win_title_set(st->win, win_name);
   free(win_name);

   bg = elm_bg_add(st->win);
   elm_win_resize_object_add(st->win, bg);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bg);

   Evas_Object *bx = elm_box_add(st->win);
   evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(bx, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(bx);

   /* Table to holds bg and scr on top of it */
   tb = elm_table_add(bx);
   elm_box_pack_end(bx, tb);
   evas_object_size_hint_weight_set(tb, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(tb, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(tb);

   /* Set background to scr in table cell */
   bg = elm_bg_add(tb);
   snprintf(s_bar, sizeof(s_bar), "%s/images/background.png",
         PACKAGE_DATA_DIR);
   elm_bg_file_set(bg, s_bar, NULL);
   elm_bg_option_set(bg, ELM_BG_OPTION_TILE);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(bg, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(bg);
   elm_table_pack(tb, bg, 0, 0, 1, 1);

   /* Then add the scroller in same cell */
   st->scr = elm_scroller_add(tb);
   elm_table_pack(tb, st->scr, 0, 0, 1, 1);
   evas_object_size_hint_weight_set(st->scr,
         EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

   evas_object_size_hint_align_set(st->scr, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(st->scr);

   st->o = evas_object_image_filled_add(
         evas_object_evas_get(bx));

   evas_object_size_hint_min_set(st->o, st->w, st->h);
   elm_object_content_set(st->scr, st->o);
   elm_object_cursor_set(st->o, ELM_CURSOR_TARGET);

   elm_object_disabled_set(bt, EINA_TRUE);
   evas_object_image_colorspace_set(st->o, EVAS_COLORSPACE_ARGB8888);
   evas_object_image_alpha_set(st->o, EINA_FALSE);
   evas_object_image_size_set(st->o, st->w, st->h);
   evas_object_image_data_copy_set(st->o, st->bmp);
   evas_object_image_data_update_add(st->o, 0, 0, st->w, st->h);
   evas_object_show(st->o);
   evas_object_smart_callback_add(st->win,
         "delete,request", _app_win_del, st);

   /* Build status bar */
   hbx = elm_box_add(bx);
   elm_box_horizontal_set(hbx, EINA_TRUE);
   evas_object_show(hbx);
   elm_box_padding_set(hbx, SBAR_PAD_X, SBAR_PAD_Y);
   evas_object_size_hint_align_set(hbx, 0.0, EVAS_HINT_FILL);
   elm_box_pack_end(bx, hbx);
   lb_size = elm_label_add(hbx);
   sprintf(s_bar, "%llux%llu", st->w, st->h);
   elm_object_text_set(lb_size, s_bar);
   evas_object_show(lb_size);
   elm_box_pack_end(hbx, lb_size);

   st->lb_mouse = elm_label_add(hbx);
   elm_object_text_set(st->lb_mouse, s_bar);
   evas_object_show(st->lb_mouse);
   elm_box_pack_end(hbx, st->lb_mouse);

   st->lb_rgba = elm_label_add(hbx);
   elm_object_text_set(st->lb_rgba, s_bar);
   evas_object_show(st->lb_rgba);
   elm_box_pack_end(hbx, st->lb_rgba);

   evas_object_event_callback_add(st->o, EVAS_CALLBACK_MOUSE_MOVE,
         _mouse_move, st);

   evas_object_event_callback_add(st->o, EVAS_CALLBACK_MOUSE_OUT,
         _mouse_out, st);

   evas_object_event_callback_add(st->o, EVAS_CALLBACK_MOUSE_DOWN,
         clouseau_lines_cb, st);

   evas_object_resize(st->scr, st->w, st->h);
   elm_win_resize_object_add(st->win, bx);
   evas_object_resize(st->win, st->w, st->h);

   elm_win_autodel_set(st->win, EINA_TRUE);
   evas_object_show(st->win);

   /* Attach a gesture layer object to support ZOOM gesture */
   glayer = elm_gesture_layer_add(st->scr);
   elm_gesture_layer_attach(glayer, st->scr);

   /* Reset zoom and remove lines on double click */
   elm_gesture_layer_cb_set(glayer, ELM_GESTURE_N_DOUBLE_TAPS,
         ELM_GESTURE_STATE_END, reset_view, st);

   elm_gesture_layer_cb_set(glayer, ELM_GESTURE_ZOOM,
         ELM_GESTURE_STATE_START, zoom_start, st);
   elm_gesture_layer_cb_set(glayer, ELM_GESTURE_ZOOM,
         ELM_GESTURE_STATE_MOVE, zoom_move, st);
   elm_gesture_layer_cb_set(glayer, ELM_GESTURE_ZOOM,
         ELM_GESTURE_STATE_END, zoom_end, st);
   elm_gesture_layer_cb_set(glayer, ELM_GESTURE_ZOOM,
         ELM_GESTURE_STATE_ABORT, zoom_end, st);
}

static void
_show_app_window(void *data, Evas_Object *obj, void *event_info EINA_UNUSED)
{  /* Open window with currnent bmp, or download it if missing   */
   app_info_st *st = gui->sel_app->app->data;
   Clouseau_Tree_Item *treeit = data;

   /* First search app->view list if already have the window bmp */
   Variant_st *v = (Variant_st *)
      eina_list_search_unsorted(st->view, _bmp_object_ptr_cmp,
            (void *) (uintptr_t) treeit->ptr);
   if (v)
     return _open_app_window(v->data, obj, data);

   /* Need to issue BMP_REQ */
   if (svr)
     {
        int size = 0;
        bmp_req_st t = { (unsigned long long) (uintptr_t) NULL,
             (unsigned long long) (uintptr_t) st->ptr,
             (unsigned long long) (uintptr_t) treeit->ptr, st->refresh_ctr };

        void *p = clouseau_data_packet_compose(CLOUSEAU_BMP_REQ,
              &t, sizeof(t), &size, NULL, 0);

        if (p)
          {
             ecore_ipc_server_send(svr,
                   0,0,0,0,EINA_FALSE, p, size);
             ecore_ipc_server_flush(svr);
             free(p);

             elm_object_disabled_set(obj, EINA_TRUE);
             elm_progressbar_pulse(gui->pb, EINA_TRUE);
             evas_object_show(gui->pb);

             bmp_node *b_node = malloc(sizeof(*b_node));
             b_node->ctr = st->refresh_ctr;
             b_node->object = (unsigned long long) (uintptr_t) treeit->ptr;
             b_node->bt = obj;       /* Button of BMP_REQ */
             bmp_req = eina_list_append(bmp_req, b_node);
          }
     }
   else  /* Disable button if we lost server */
     _set_button(gui->win, obj,
           SCREENSHOT_MISSING,
           "Screenshot not available", EINA_TRUE);
}

Eina_Bool
_data(void *data, int type EINA_UNUSED, void *event)
{
   Ecore_Ipc_Event_Server_Data *ev = event;
   Variant_st *v;

   v = clouseau_data_packet_info_get(ev->data, ev->size);
   if (!v) return ECORE_CALLBACK_RENEW;

   switch (clouseau_data_packet_mapping_type_get(v->type))
     {
      case CLOUSEAU_APP_ADD:            /* Add info to list of APPs  */
         _add_app(data, v);    /* v->data is (app_info_st *) */
         break;

      case CLOUSEAU_APP_CLOSED:         /* Remove and free APP info */
         _remove_app(data, v); /* v->data is (app_closed_st *) */
         break;

      case CLOUSEAU_TREE_DATA:           /* Update genlist with APP TREE info */
         _update_tree(data, v); /* data is the gui pointer */
         break;                 /* v->data is (tree_data_st *) */

      case CLOUSEAU_BMP_DATA:         /* Contains a snapshot of canvas window */
        {                 /* v->data is (bmp_info_st *) */
           _add_bmp(data, v);  /* data is the gui pointer */
        }
        break;

      default:
         break;
     }

   /* variant_free(v) - freed when removed from app list */

   return ECORE_CALLBACK_RENEW;
}

static void
gl_exp(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info)
{
   Elm_Object_Item *glit = event_info;
   Evas_Object *gl = elm_object_item_widget_get(glit);
   Clouseau_Tree_Item *parent = elm_object_item_data_get(glit);
   Clouseau_Tree_Item *treeit;
   Eina_List *itr;

   EINA_LIST_FOREACH(parent->children, itr, treeit)
     {
        if ((!list_show_hidden && !treeit->is_visible) ||
              (!list_show_clippers && treeit->is_clipper))
           continue;

        Elm_Genlist_Item_Type iflag = (treeit->children) ?
           ELM_GENLIST_ITEM_TREE : ELM_GENLIST_ITEM_NONE;
        elm_genlist_item_append(gl, &itc, treeit, glit, iflag,
              NULL, NULL);
     }
}

static void
gl_con(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info)
{
   Elm_Object_Item *glit = event_info;
   elm_genlist_item_subitems_clear(glit);
}

static void
gl_exp_req(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info)
{
   Elm_Object_Item *glit = event_info;
   elm_genlist_item_expanded_set(glit, EINA_TRUE);
}

static void
gl_con_req(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info)
{
   Elm_Object_Item *glit = event_info;
   elm_genlist_item_expanded_set(glit, EINA_FALSE);
}



static Evas_Object *
item_icon_get(void *data, Evas_Object *parent, const char *part)
{
   Clouseau_Tree_Item *treeit = data;
   char buf[PATH_MAX];

   if (!treeit->is_obj)
     {  /* Add "Download" button for evas objects */
        if (!strcmp(part, "elm.swallow.end"))
          {
             Evas_Object *bt = elm_button_add(parent);
             app_info_st *app = NULL;
             if (gui->sel_app)
               app = gui->sel_app->app->data;

             if (app)
               {  /* match ptr with bmp->object ptr to find view */
                  Variant_st *v = (Variant_st *)
                     eina_list_search_unsorted(app->view, _bmp_object_ptr_cmp,
                           (void *) (uintptr_t) treeit->ptr);

                  if (v)
                    {  /* Set to "show view" if view exists */
                       _set_button(parent, bt,
                             SHOW_SCREENSHOT,
                             "Show App Screenshot", EINA_FALSE);
                    }
                  else
                    {  /* Set to Download or not available if offline */
                       if (svr)
                         {
                            _set_button(parent, bt,
                                  TAKE_SCREENSHOT,
                                  "Download Screenshot", EINA_FALSE);
                         }
                       else
                         { /* Make button display: screenshot NOT available */
                            _set_button(parent, bt,
                                  SCREENSHOT_MISSING,
                                  "Screenshot not available", EINA_TRUE);
                         }
                    }
               }

             evas_object_smart_callback_add(bt, "clicked",
                   _show_app_window, treeit);

             evas_object_show(bt);
             return bt;
          }

        return NULL;
     }

   if (!strcmp(part, "elm.swallow.icon"))
     {
        if (treeit->is_clipper && !treeit->is_visible)
          {
             Evas_Object *ic;
             Evas_Object *bx = elm_box_add(parent);
             evas_object_size_hint_aspect_set(bx, EVAS_ASPECT_CONTROL_VERTICAL,
                   1, 1);

             ic = elm_icon_add(bx);
             snprintf(buf, sizeof(buf), "%s/images/clipper.png",
                   PACKAGE_DATA_DIR);
             elm_image_file_set(ic, buf, NULL);
             evas_object_size_hint_aspect_set(ic, EVAS_ASPECT_CONTROL_VERTICAL,
                   1, 1);
             evas_object_size_hint_weight_set(ic, EVAS_HINT_EXPAND,
                   EVAS_HINT_EXPAND);
             evas_object_size_hint_align_set(ic, EVAS_HINT_FILL,
                   EVAS_HINT_FILL);
             elm_box_pack_end(bx, ic);

             ic = elm_icon_add(bx);
             snprintf(buf, sizeof(buf), "%s/images/hidden.png",
                   PACKAGE_DATA_DIR);
             elm_image_file_set(ic, buf, NULL);
             evas_object_size_hint_aspect_set(ic, EVAS_ASPECT_CONTROL_VERTICAL,
                   1, 1);
             evas_object_size_hint_weight_set(ic, EVAS_HINT_EXPAND,
                   EVAS_HINT_EXPAND);
             evas_object_size_hint_align_set(ic, EVAS_HINT_FILL,
                   EVAS_HINT_FILL);
             elm_box_pack_end(bx, ic);

             return bx;

          }
        else if (treeit->is_clipper)
          {
             Evas_Object *ic;
             ic = elm_icon_add(parent);
             snprintf(buf, sizeof(buf), "%s/images/clipper.png",
                   PACKAGE_DATA_DIR);
             elm_image_file_set(ic, buf, NULL);
             evas_object_size_hint_aspect_set(ic, EVAS_ASPECT_CONTROL_VERTICAL,
                   1, 1);
             return ic;
          }
        else if (!treeit->is_visible)
          {
             Evas_Object *ic;
             ic = elm_icon_add(parent);
             snprintf(buf, sizeof(buf), "%s/images/hidden.png",
                   PACKAGE_DATA_DIR);
             elm_image_file_set(ic, buf, NULL);
             evas_object_size_hint_aspect_set(ic, EVAS_ASPECT_CONTROL_VERTICAL,
                   1, 1);
             return ic;
          }
     }

   return NULL;
}

static char *
item_text_get(void *data, Evas_Object *obj EINA_UNUSED,
      const char *part EINA_UNUSED)
{
   Clouseau_Tree_Item *treeit = data;
   char buf[256];
   snprintf(buf, sizeof(buf), "%llx %s", treeit->ptr, treeit->name);
   return strdup(buf);
}

static void
client_win_del(void *data EINA_UNUSED,
      Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{  /* called when client window is deleted */
   elm_exit(); /* exit the program's main loop that runs in elm_run() */
}

static Ecore_Ipc_Server *
_connect_to_daemon(gui_elements *g)
{
   if (svr && ecore_ipc_server_connected_get(svr))
     return svr;  /* Already connected */

   int port = PORT;
   char *address = LOCALHOST;
   char *p_colon = NULL;

   if (g->address && strlen(g->address))
     {
        address = g->address;
        p_colon = strchr(g->address, ':');
     }

   if (p_colon)
     {
        *p_colon = '\0';
        if (isdigit(*(p_colon+1)))
          port = atoi(p_colon+1);
     }

   svr = ecore_ipc_server_connect(ECORE_IPC_REMOTE_SYSTEM,
         address, port, NULL);

   if (p_colon)
     *p_colon = ':';

   if (!svr)
     {
        printf("could not connect to the server: %s\n", g->address);
        return NULL;
     }

   ecore_ipc_server_data_size_max_set(svr, -1);

   /* set event handler for server connect */
   ecore_event_handler_add(ECORE_IPC_EVENT_SERVER_ADD, _add, g);
   /* set event handler for server disconnect */
   ecore_event_handler_add(ECORE_IPC_EVENT_SERVER_DEL, _del, g);
   /* set event handler for receiving server data */
   ecore_event_handler_add(ECORE_IPC_EVENT_SERVER_DATA, _data, g);

   return svr;
}

static void
_gl_selected(void *data EINA_UNUSED, Evas_Object *pobj EINA_UNUSED,
      void *event_info)
{
   clouseau_object_information_list_clear();
   gui_elements *g = data;
   Clouseau_Tree_Item *treeit = elm_object_item_data_get(event_info);
   const Elm_Object_Item *parent;
   const Elm_Object_Item *prt = elm_genlist_item_parent_get(event_info);
   if (!prt)
     return;

   /* Populate object information, then do highlight */
   clouseau_object_information_list_populate(treeit, g->lb);

   if (!do_highlight)
     return;

   /* START - replacing libclouseau_highlight(obj); */
   int size;
   app_info_st *app = g->sel_app->app->data;
   highlight_st st = { (unsigned long long) (uintptr_t) app->ptr,
                       treeit->ptr };

   if (svr)
     {
        void *p = clouseau_data_packet_compose(CLOUSEAU_HIGHLIGHT,
              &st, sizeof(st), &size, NULL, 0);

        if (p)
          {
             ecore_ipc_server_send(svr,
                   0,0,0,0,EINA_FALSE, p, size);
             ecore_ipc_server_flush(svr);
             free(p);
          }
     }

   /* We also like to HIGHLIGHT on any app views that open (for offline) */
   do
     {
        parent = prt;
        prt = elm_genlist_item_parent_get(prt);
     }
   while (prt);

   Clouseau_Tree_Item *t = elm_object_item_data_get(parent);
   Variant_st *v = eina_list_search_unsorted(app->view,
                                             _bmp_object_ptr_cmp,
                                             (void*) (uintptr_t) t->ptr);

   if (v)
     {  /* Third param gives evas surface when running offline */
        clouseau_data_object_highlight((void*) (uintptr_t) treeit->ptr,
                                  &treeit->info->evas_props, v->data);
     }
   /* END   - replacing clouseau_object_highlight(obj); */
}

static int
_load_list(gui_elements *g)
{
   elm_progressbar_pulse(g->pb, EINA_FALSE);
   evas_object_hide(g->pb);

   if (g->sel_app)
     {
        elm_genlist_clear(g->gl);
        elm_genlist_clear(g->prop_list);
        app_info_st *st = g->sel_app->app->data;
        tree_data_st *td = (g->sel_app->td) ? g->sel_app->td->data: NULL;

        if (td)
          {  /* Just show currnet tree we got */
             _load_gui_with_list(g, td->tree);
          }
        else
          {  /* Ask for app info only if was not fetched */
             if (!svr)
               {
                  _update_tree_offline(g, g->sel_app->td);
                  return 0;
               }

             if (eina_list_search_unsorted(apps, _app_ptr_cmp,
                      (void *) (uintptr_t) st->ptr))
               {  /* do it only if app selected AND found in apps list */
                  int size;
                  data_req_st t = { (unsigned long long) (uintptr_t) NULL,
                       (unsigned long long) (uintptr_t) st->ptr };

                  void *p = clouseau_data_packet_compose(CLOUSEAU_DATA_REQ,
                        &t, sizeof(t), &size, NULL, 0);

                  if (p)
                    {
                       elm_progressbar_pulse(g->pb, EINA_TRUE);
                       evas_object_show(g->pb);
                       ecore_ipc_server_send(svr,
                             0,0,0,0,EINA_FALSE, p, size);
                       ecore_ipc_server_flush(svr);
                       free(p);
                    }
               }
          }
     }

   return 0;
}

static void
_show_clippers_check_changed(void *data, Evas_Object *obj,
      void *event_info EINA_UNUSED)
{
   list_show_clippers = elm_check_state_get(obj);
   _load_list(data);
}

static void
_highlight_check_check_changed(EINA_UNUSED void *data, Evas_Object *obj,
      void *event_info EINA_UNUSED)
{
   do_highlight = elm_check_state_get(obj);
}

static void
_show_hidden_check_changed(void *data, Evas_Object *obj,
      void *event_info EINA_UNUSED)
{
   list_show_hidden = elm_check_state_get(obj);
   _load_list(data);
}

static void
_bt_clicked(void *data, Evas_Object *obj, void *event_info EINA_UNUSED)
{
   gui_elements *g = data;

   /* Close all app-bmp-view windows here and clear mem */
   if (g->sel_app)
     {
        app_info_st *st = g->sel_app->app->data;
        _close_app_views(st, EINA_TRUE);
        st->refresh_ctr++;
     }

   elm_object_text_set(obj, "Reload");
   _free_app_tree_data(g->sel_app->td);
   g->sel_app->td = NULL;
   _load_list(data);
}

static void
_bt_load_file(void *data, Evas_Object *obj EINA_UNUSED, void *event_info)
{
   gui_elements *g = data;
   Variant_st *app = calloc(1, sizeof(Variant_st));
   Variant_st *td =  calloc(1, sizeof(Variant_st));
   /* app_info_st *app = NULL; */
   if (event_info)
     {
        Eina_Bool s = clouseau_data_eet_info_read(event_info,
              (app_info_st **) &app->data,
              (tree_data_st **) &td->data);

        if (s)
          {  /* Add the app to list of apps, then set this as selected app */
              ((app_info_st *) app->data)->file = strdup(event_info);
             app_data_st *st = _add_app(g, app);
             st->td = td;  /* This is the same as we got TREE_DATA message */
             _set_selected_app(st, g->dd_list, NULL);
          }
     }
}

static void
_dismiss_save_dialog(void *data,
      Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{  /* Just close save file save_inwin, do nothing */
   gui_elements *g = data;
   evas_object_del(g->save_inwin);
   g->save_inwin = NULL;
}

static void
_bt_save_file(void *data, Evas_Object *obj EINA_UNUSED, void *event_info)
{
   app_info_st *app = gui->sel_app->app->data;
   tree_data_st *ftd = gui->sel_app->td->data;
   if (event_info)
     {
        /* FIXME: Handle failure. */
        clouseau_data_eet_info_save(event_info, app, ftd, data);
     }

   eina_list_free(data);

   if (event_info)  /* Dismiss save dialog after saving */
     _dismiss_save_dialog(gui, NULL, NULL);
}

static void
_dismiss_inwin(gui_elements *g)
{
   g->address = (g->en) ? strdup(elm_entry_entry_get(g->en)) : NULL;
   evas_object_del(g->connect_inwin);
   g->en = NULL;
   g->connect_inwin = NULL;
}

static void
_save_all(void *data, Evas_Object *obj, void *event_info EINA_UNUSED)
{
   Evas_Object *ck_bx = data;
   Evas_Object *ck;
   Eina_List *l;
   Eina_List *ck_list = elm_box_children_get(ck_bx);
   Eina_Bool val = elm_check_state_get(obj);

   EINA_LIST_FOREACH(ck_list, l, ck)
     {  /* Run through checkoxes, set / unset marks for all */
        if (!elm_object_disabled_get(ck))
          elm_check_state_set(ck, val);
     }

   eina_list_free(ck_list);
}

static void
_save_file_dialog(void *data,
      Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{  /* START - Popup to save eet file */
   gui_elements *g = data;
   Evas_Object *scr, *bt_bx, *bx, *ck_bx,
               *lb, *ck, *bt_cancel, *bt_save;
   g->save_inwin = elm_win_inwin_add(g->win);
   evas_object_show(g->save_inwin);


   bx = elm_box_add(g->save_inwin);
   evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bx);

   lb = elm_label_add(bx);
   evas_object_size_hint_weight_set(lb, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(lb, EVAS_HINT_FILL, 0.0);
   elm_object_text_set(lb, "Select Screeenshots to save:");
   elm_box_pack_end(bx, lb);
   evas_object_show(lb);

   /* Add checkboxes to select screenshots to save */
   ck_bx = elm_box_add(g->save_inwin);

   Eina_List *l;
   app_info_st *a = g->sel_app->app->data;
   tree_data_st *td = g->sel_app->td->data;
   bmp_info_st *bmp;
   Clouseau_Tree_Item *treeit;
   char buf[256];
   EINA_LIST_FOREACH(td->tree, l, treeit)
     {  /* First search app->view list if already have the window bmp */
        Variant_st *v = (Variant_st *)
           eina_list_search_unsorted(a->view, _bmp_object_ptr_cmp,
                 (void *) (uintptr_t) treeit->ptr);

        bmp = (v) ? v->data : NULL;

        ck = elm_check_add(ck_bx);
        evas_object_size_hint_weight_set(ck, EVAS_HINT_EXPAND, 1.0);
        evas_object_size_hint_align_set(ck, EVAS_HINT_FILL, 0.0);
        elm_box_pack_end(ck_bx, ck);
        elm_object_disabled_set(ck, !(bmp && bmp->bmp));
        evas_object_data_set(ck, BMP_FIELD, bmp); /* Associate ck with bmp */
        snprintf(buf, sizeof(buf), "%llx %s", treeit->ptr, treeit->name);
        elm_object_text_set(ck, buf);

        evas_object_show(ck);
     }

   evas_object_show(ck_bx);
   scr = elm_scroller_add(bx);
   elm_object_content_set(scr, ck_bx);
   evas_object_size_hint_align_set(scr, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_weight_set(scr, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(scr);
   elm_box_pack_end(bx, scr);

   /* Add the save all checkbox */
   ck = elm_check_add(bx);
   elm_object_text_set(ck, "Save All");
   evas_object_smart_callback_add(ck, "changed", _save_all, ck_bx);
   evas_object_show(ck);
   elm_box_pack_end(bx, ck);

   bt_bx = elm_box_add(bx);
   elm_box_horizontal_set(bt_bx, EINA_TRUE);
   elm_box_homogeneous_set(bt_bx, EINA_TRUE);
   evas_object_size_hint_align_set(bt_bx, 0.5, 1.0);
   evas_object_size_hint_weight_set(bt_bx, EVAS_HINT_EXPAND, 0.0);
   evas_object_show(bt_bx);
   elm_box_pack_end(bx, bt_bx);

   /* Add the cancel button */
   bt_cancel = elm_button_add(bt_bx);
   elm_object_text_set(bt_cancel, "Cancel");
   evas_object_smart_callback_add(bt_cancel, "clicked",
         _dismiss_save_dialog, g);

   elm_box_pack_end(bt_bx, bt_cancel);
   evas_object_show(bt_cancel);

   /* Add the Save fileselector button */
   bt_save = elm_fileselector_button_add(bt_bx);
   elm_fileselector_button_is_save_set(bt_save, EINA_TRUE);
   elm_object_text_set(bt_save, "Save File");
   elm_fileselector_button_path_set(bt_save, getenv("HOME"));
   evas_object_smart_callback_add(bt_save, "file,chosen",
         _bt_save_file, elm_box_children_get(ck_bx));

   elm_box_pack_end(bt_bx, bt_save);
   evas_object_show(bt_save);

   elm_win_inwin_content_set(g->save_inwin, bx);
   /* END   - Popup to save eet file */
}

static void
_remove_apps_with_no_tree_data(gui_elements *g)
{  /* We need to remove apps with no tree data when losing commection
    * with daemon. We may have apps in our list that were added but
    * tree-data was NOT loaded.
    * In this case, we want to remove them if connection was lost.    */

   Eina_List *l, *l_next;
   app_data_st *st;
   app_closed_st t;
   Variant_st *v;
   EINA_LIST_FOREACH_SAFE(apps, l, l_next, st)
     {
        if (!st->td)
          {  /* We actually fake APP_CLOSED message, for app NO tree */
             t.ptr = (unsigned long long) (uintptr_t)
                (((app_info_st *) st->app->data)->ptr);

             /* v is freed by _remove_app */
             v = clouseau_data_variant_alloc(CLOUSEAU_APP_CLOSED,
                   sizeof(t), &t);

             _remove_app(g, v); /* v->data is (app_closed_st *) */
          }
     }
}

static void
_show_gui(gui_elements *g, Eina_Bool work_offline)
{
   if (work_offline)
     {  /* Replace bt_load with fileselector button */
        _titlebar_string_set(g, EINA_FALSE);
        elm_box_unpack(g->hbx, g->bt_load);
        evas_object_del(g->bt_load);

        /* We need this in case conneciton closed and no tree data */
        _remove_apps_with_no_tree_data(g);

        g->bt_load = elm_fileselector_button_add(g->hbx);
        elm_box_pack_start(g->hbx, g->bt_load);
        elm_object_text_set(g->bt_load, "Load File");
        elm_fileselector_button_path_set(g->bt_load, getenv("HOME"));
        evas_object_smart_callback_add(g->bt_load, "file,chosen",
              _bt_load_file, g);

        evas_object_show(g->bt_load);
     }
   else
     {
        elm_object_text_set(g->bt_load, "Load Tree");
        evas_object_smart_callback_add(g->bt_load, "clicked", _bt_clicked, g);

        /* Add the Save button to open save dialog */
        if (g->bt_save)
          evas_object_del(g->bt_save);

        g->bt_save = elm_button_add(g->hbx);
        elm_object_text_set(g->bt_save, "SAVE");
        evas_object_smart_callback_add(g->bt_save, "clicked",
              _save_file_dialog, (void *) gui);

        elm_box_pack_end(g->hbx, g->bt_save);
        evas_object_show(g->bt_save);

        elm_object_disabled_set(g->bt_load, (g->sel_app == NULL));
        elm_object_disabled_set(g->bt_save, (g->sel_app == NULL));
        evas_object_show(g->bt_save);

        svr = _connect_to_daemon(g);
        if(!svr)
          {
             printf("Failed to connect to server.\n");
             elm_exit(); /* exit the program's main loop,runs in elm_run() */
          }
     }

   evas_object_show(g->bx);
   evas_object_show(g->panel);
}

static void
_cancel_bt_clicked(void *data,
      Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
   _dismiss_inwin(data);
   elm_exit(); /* exit the program's main loop that runs in elm_run() */
}

static void
_ok_bt_clicked(void *data,
      Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{  /* Set the IP, PORT, then connect to server */
   _dismiss_inwin(data);
   _show_gui(data, EINA_FALSE);
}

static void
_ofl_bt_clicked(void *data,
      Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{  /* Disbale entry when working offline */
   _dismiss_inwin(data);
   _show_gui(data, EINA_TRUE);
}

#ifndef ELM_LIB_QUICKLAUNCH
EAPI int
elm_main(int argc EINA_UNUSED, char **argv EINA_UNUSED)
{  /* Create Client Window */
   Evas_Object *win, *bg, *panes,
               *show_hidden_check, *show_clippers_check, *highlight_check;


   /* For inwin popup */
   Evas_Object *lb, *bxx, *bt_bx, *bt_ok, *bt_cancel;
   Evas_Object *bt_ofl; /* work_offline button  */
   void *st;

   gui = calloc(1, sizeof(gui_elements));

   gui->win = win = elm_win_add(NULL, "client", ELM_WIN_BASIC);
   elm_win_autodel_set(win, EINA_TRUE);

   _titlebar_string_set(gui, EINA_FALSE);

   bg = elm_bg_add(win);
   elm_win_resize_object_add(win, bg);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bg);

   gui->bx = elm_box_add(win);
   evas_object_size_hint_weight_set(gui->bx,
         EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(gui->bx, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_win_resize_object_add(win, gui->bx);

   /* Control buttons */
     {
        gui->hbx = elm_box_add(gui->bx);
        evas_object_size_hint_align_set(gui->hbx, 0.0, 0.5);
        elm_box_horizontal_set(gui->hbx, EINA_TRUE);
        elm_box_pack_end(gui->bx, gui->hbx);
        elm_box_padding_set(gui->hbx, 10, 0);
        evas_object_show(gui->hbx);

        gui->bt_load = elm_button_add(gui->hbx);
        evas_object_size_hint_align_set(gui->bt_load, 0.0, 0.3);
        elm_box_pack_end(gui->hbx, gui->bt_load);
        evas_object_show(gui->bt_load);

        gui->dd_list = elm_hoversel_add(gui->hbx);
        elm_hoversel_hover_parent_set(gui->dd_list, win);
        elm_object_text_set(gui->dd_list, "SELECT APP");

        evas_object_size_hint_align_set(gui->dd_list, 0.0, 0.3);
        elm_box_pack_end(gui->hbx, gui->dd_list);
        evas_object_show(gui->dd_list);

        show_hidden_check = elm_check_add(gui->hbx);
        elm_object_text_set(show_hidden_check, "Show Hidden");
        elm_check_state_set(show_hidden_check, list_show_hidden);
        elm_box_pack_end(gui->hbx, show_hidden_check);
        evas_object_show(show_hidden_check);

        show_clippers_check = elm_check_add(gui->hbx);
        elm_object_text_set(show_clippers_check, "Show Clippers");
        elm_check_state_set(show_clippers_check, list_show_clippers);
        elm_box_pack_end(gui->hbx, show_clippers_check);
        evas_object_show(show_clippers_check);

        highlight_check = elm_check_add(gui->hbx);
        elm_object_text_set(highlight_check , "Highlight");
        elm_check_state_set(highlight_check , do_highlight);
        elm_box_pack_end(gui->hbx, highlight_check);
        evas_object_show(highlight_check);
     }

   panes = elm_panes_add(gui->bx);
   evas_object_size_hint_weight_set(panes, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(panes, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(gui->bx, panes);
   evas_object_show(panes);

   /* The main list */
     {
        gui->gl = elm_genlist_add(panes);
        elm_genlist_select_mode_set(gui->gl, ELM_OBJECT_SELECT_MODE_ALWAYS);
        evas_object_size_hint_align_set(gui->gl,
              EVAS_HINT_FILL, EVAS_HINT_FILL);
        evas_object_size_hint_weight_set(gui->gl,
              EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
        elm_object_part_content_set(panes, "left", gui->gl);
        evas_object_show(gui->gl);

        evas_object_smart_callback_add(show_hidden_check, "changed",
              _show_hidden_check_changed, gui);
        evas_object_smart_callback_add(show_clippers_check, "changed",
              _show_clippers_check_changed, gui);
        evas_object_smart_callback_add(highlight_check, "changed",
              _highlight_check_check_changed, gui);

        itc.item_style = "default";
        itc.func.text_get = item_text_get;
        itc.func.content_get = item_icon_get;
        itc.func.state_get = NULL;
        itc.func.del = NULL;

        evas_object_smart_callback_add(gui->gl,
              "expand,request", gl_exp_req, gui->gl);
        evas_object_smart_callback_add(gui->gl,
              "contract,request", gl_con_req, gui->gl);
        evas_object_smart_callback_add(gui->gl,
              "expanded", gl_exp, gui->gl);
        evas_object_smart_callback_add(gui->gl,
              "contracted", gl_con, gui->gl);
        evas_object_smart_callback_add(gui->gl,
              "selected", _gl_selected, gui);
     }

   /* Properties list */
     {
        Evas_Object *prop_list = NULL;
        prop_list = clouseau_object_information_list_add(panes);
        gui->prop_list = prop_list;
        evas_object_size_hint_align_set(prop_list,
              EVAS_HINT_FILL, EVAS_HINT_FILL);
        evas_object_size_hint_weight_set(prop_list,
              EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

        elm_object_part_content_set(panes, "right", prop_list);
        evas_object_show(prop_list);
     }

   /* START Add buttom panel */
   gui->panel = elm_panel_add(win);
   elm_panel_orient_set(gui->panel, ELM_PANEL_ORIENT_BOTTOM);
   evas_object_size_hint_weight_set(gui->panel,
         EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(gui->panel, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_panel_hidden_set(gui->panel, EINA_TRUE);
   elm_win_resize_object_add(win, gui->panel);

   gui->lb = elm_label_add(win);
   evas_object_size_hint_weight_set(gui->lb, EVAS_HINT_EXPAND, 0);
   evas_object_size_hint_align_set(gui->lb, EVAS_HINT_FILL, 0);
   evas_object_show(gui->lb);

   elm_object_content_set(gui->panel, gui->lb);
   /* END   Add buttom panel */

   /* Add progress wheel */
   gui->pb = elm_progressbar_add(win);
   elm_object_style_set(gui->pb, "wheel");
   elm_object_text_set(gui->pb, "Style: wheel");
   elm_progressbar_pulse(gui->pb, EINA_FALSE);
   evas_object_size_hint_align_set(gui->pb, 0.5, 0.0);
   evas_object_size_hint_weight_set(gui->pb,
         EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, gui->pb);

   /* Resize and show main window */
   evas_object_resize(win, 500, 500);
   evas_object_show(win);

   evas_object_smart_callback_add(win, "delete,request", client_win_del, NULL);

   eina_init();
   ecore_init();
   ecore_ipc_init();
   clouseau_data_init();

   /* START - Popup to get IP, PORT from user */
   gui->connect_inwin = elm_win_inwin_add(win);
   evas_object_show(gui->connect_inwin);

   bxx = elm_box_add(gui->connect_inwin);
   evas_object_size_hint_weight_set(bxx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bxx);

   lb = elm_label_add(bxx);
   evas_object_size_hint_weight_set(lb, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(lb, EVAS_HINT_FILL, 0.0);
   elm_object_text_set(lb, "Enter remote address[:port]");
   elm_box_pack_end(bxx, lb);
   evas_object_show(lb);

   /* Single line selected entry */
   gui->en = elm_entry_add(bxx);
   elm_entry_scrollable_set(gui->en, EINA_TRUE);
   evas_object_size_hint_weight_set(gui->en,
         EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(gui->en, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_object_style_set(gui->connect_inwin, "minimal_vertical");
   elm_entry_scrollbar_policy_set(gui->en, ELM_SCROLLER_POLICY_OFF,
         ELM_SCROLLER_POLICY_OFF);
   elm_object_text_set(gui->en, LOCALHOST);
   elm_entry_single_line_set(gui->en, EINA_TRUE);
   elm_entry_select_all(gui->en);
   elm_box_pack_end(bxx, gui->en);
   evas_object_show(gui->en);

   bt_bx = elm_box_add(bxx);
   elm_box_horizontal_set(bt_bx, EINA_TRUE);
   elm_box_homogeneous_set(bt_bx, EINA_TRUE);
   evas_object_size_hint_align_set(bt_bx, 0.5, 0.5);
   evas_object_size_hint_weight_set(bt_bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bt_bx);
   elm_box_pack_end(bxx, bt_bx);

   /* Add the cancel button */
   bt_cancel = elm_button_add(bt_bx);
   elm_object_text_set(bt_cancel, "Cancel");
   evas_object_smart_callback_add(bt_cancel, "clicked",
         _cancel_bt_clicked, (void *) gui);

   elm_box_pack_end(bt_bx, bt_cancel);
   evas_object_show(bt_cancel);

   /* Add the OK button */
   bt_ok = elm_button_add(bt_bx);
   elm_object_text_set(bt_ok, "OK");
   evas_object_smart_callback_add(bt_ok, "clicked",
         _ok_bt_clicked, (void *) gui);

   elm_box_pack_end(bt_bx, bt_ok);
   evas_object_show(bt_ok);

   bt_ofl = elm_button_add(bt_bx);
   elm_object_text_set(bt_ofl, "Work Offline");
   evas_object_smart_callback_add(bt_ofl, "clicked",
         _ofl_bt_clicked, (void *) gui);

   elm_box_pack_end(bt_bx, bt_ofl);
   evas_object_show(bt_ofl);

   elm_win_inwin_content_set(gui->connect_inwin, bxx);
   /* END   - Popup to get IP, PORT from user */

   elm_run();

   /* cleanup - free apps data */
   EINA_LIST_FREE(apps, st)
      _free_app(st);

   EINA_LIST_FREE(bmp_req, st)
      free(st);

   clouseau_data_shutdown();
   if (gui->address)
     free(gui->address);

   free(gui);

   elm_shutdown();

   return 0;
}
ELM_MAIN()
#endif
