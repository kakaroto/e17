#include <stdio.h>
#include <Ecore.h>
#include <Ecore_Ipc.h>
#include <Eina.h>
#include <Edje.h>
#include <Evas.h>
#include <Elementary.h>
#include <stdint.h>

#include "libclouseau.h"
#include "eet_data.h"

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

struct _app_data_st
{
   Variant_st *app;  /* app->data is (app_info_st *)   */
   Variant_st *td;   /* tree->data is (tree_data_st *) */
};
typedef struct _app_data_st app_data_st;

struct _gui_elements
{
   Evas_Object *dd_list;
   Evas_Object *gl;
   Evas_Object *lb;  /* Label showing backtrace */
   Evas_Object *prop_list;
   Evas_Object *inwin;
   Evas_Object *en;
   Evas_Object *pb; /* Progress wheel shown when waiting for TREE_DATA */
   char *address;
   app_data_st *sel_app; /* Currently selected app data */
};
typedef struct _gui_elements gui_elements;

static int _load_list(gui_elements *gui);

/* Globals */
static gui_elements *gui = NULL;
static Eina_List *apps= NULL;  /* List of (app_data_st *) */
static Elm_Genlist_Item_Class itc;
static Eina_Bool list_show_clippers = EINA_TRUE, list_show_hidden = EINA_TRUE;

Eina_Bool
_add(void *data EINA_UNUSED, int type EINA_UNUSED, Ecore_Ipc_Event_Server_Add *ev)
{
   void *p;
   int size = 0;

   ecore_ipc_server_data_size_max_set(ev->server, -1);

   connect_st t = { getpid(), __FILE__ };
   p = packet_compose(GUI_CLIENT_CONNECT, &t, sizeof(t), &size);
   if (p)
     {
        ecore_ipc_server_send(ev->server, 0,0,0,0,EINA_FALSE, p, size);
        ecore_ipc_server_flush(ev->server);
        free(p);
     }

   return ECORE_CALLBACK_RENEW;
}


Eina_Bool
_del(void *data EINA_UNUSED, int type EINA_UNUSED, Ecore_Ipc_Event_Server_Del *ev)
{
   if (!ev->server)
     {
        printf("Failed to establish connection to the server.\nExiting.\n");
        ecore_main_loop_quit();
        return ECORE_CALLBACK_RENEW;
     }

   printf("Lost server with ip %s!\n", ecore_ipc_server_ip_get(ev->server));

   ecore_ipc_server_del(ev->server);

   ecore_main_loop_quit();
   return ECORE_CALLBACK_RENEW;
}


static Eina_Bool
_load_gui_with_list(gui_elements *g, Eina_List *trees)
{
   Eina_List *l;
   Tree_Item *treeit;

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

static void
_set_selected_app(void *data, Evas_Object *pobj,
      void *event_info EINA_UNUSED)
{  /* Set hovel label */
   app_data_st *st = data;
   app_info_st *app = st->app->data;
   if (gui->sel_app != st)
     {  /* Reload only of selected some other app */
        gui->sel_app = st;

        char *str = malloc(strlen(app->name)+32);
        sprintf(str, "%s [%d]", app->name, app->pid);
        elm_object_text_set(pobj, str);
        free(str);

        elm_progressbar_pulse(gui->pb, EINA_FALSE);
        evas_object_hide(gui->pb);
        _load_list(gui);
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
   app_info_st *app = st->app->data;

   char *str = malloc(strlen(app->name)+32);
   sprintf(str, "%s [%d]", app->name, app->pid);
   elm_hoversel_item_add(dd_list, str, NULL, ELM_ICON_NONE,
         _set_selected_app, st);

   free(str);
}

static void
_add_app(gui_elements *g, Variant_st *v)
{
   app_data_st *st = malloc(sizeof(app_data_st));
   st->app = v;
   st->td = NULL; /* Will get this on TREE_DATA message */
   apps = eina_list_append(apps, st);

   _add_app_to_dd_list(g->dd_list, st);
}

static void
_free_app_tree_data(Variant_st *td)
{
   if (td)
     {
        tree_data_st *ftd = td->data;
        item_tree_free(ftd->tree);
        variant_free(td);
     }
}

static void
_free_app(app_data_st *st)
{
   variant_free(st->app);
   _free_app_tree_data(st->td);
   free(st);
}

static void
_remove_app(gui_elements *g, Variant_st *v)
{
   app_closed_st *app = v->data;
   app_info_st *sel_app = g->sel_app->app->data;
   app_data_st *st = (app_data_st *)
      eina_list_search_unsorted(apps, _app_ptr_cmp,
            (void *) (uintptr_t) app->ptr);

   if (app->ptr == sel_app->ptr)
     {
        elm_object_text_set(g->dd_list, "SELECT APP");
        elm_genlist_clear(g->gl);
        elm_genlist_clear(g->prop_list);
        g->sel_app = NULL;
     }

   if (st)
     {  /* Remove from list and free all variants */
        apps = eina_list_remove(apps, st);
        _free_app(st);

        if (!elm_hoversel_expanded_get(g->dd_list))
          {
             Eina_List *l;
             elm_hoversel_clear(g->dd_list);
             EINA_LIST_FOREACH(apps, l , st)
                _add_app_to_dd_list(g->dd_list, st);
          }
     }

   variant_free(v);
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

Eina_Bool
_data(void *data, int type EINA_UNUSED, Ecore_Ipc_Event_Server_Data *ev)
{
   Variant_st *v = packet_info_get(ev->data, ev->size);

   if (v)
     {
        switch(packet_mapping_type_get(v->t.type))
          {
           case APP_ADD:            /* Add info to list of APPs  */
              _add_app(data, v);    /* v->data is (app_info_st *) */
              break;

           case APP_CLOSED:         /* Remove and free APP info */
              _remove_app(data, v); /* v->data is (app_closed_st *) */
              break;

           case TREE_DATA:           /* Update genlist with APP TREE info */
              _update_tree(data, v); /* data is the gui pointer */
              break;                 /* v->data is (tree_data_st *) */

           default:
              break;
          }

        /* variant_free(v) - freed when removed from app list */
     }

   return ECORE_CALLBACK_RENEW;
}

static void
gl_exp(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info)
{
   Elm_Object_Item *glit = event_info;
   Evas_Object *gl = elm_object_item_widget_get(glit);
   Tree_Item *parent = elm_object_item_data_get(glit);
   Tree_Item *treeit;
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
   Tree_Item *treeit = data;

   if (!treeit->is_obj)
      return NULL;

   if (!strcmp(part, "elm.swallow.icon"))
     {
        char buf[PATH_MAX];

        if (treeit->is_clipper && !treeit->is_visible)
          {
             Evas_Object *ic;
             Evas_Object *bx = elm_box_add(parent);
             evas_object_size_hint_aspect_set(bx, EVAS_ASPECT_CONTROL_VERTICAL,
                   1, 1);

             ic = elm_icon_add(bx);
             snprintf(buf, sizeof(buf), "%s/images/clipper.png",
                   PACKAGE_DATA_DIR);
             elm_icon_file_set(ic, buf, NULL);
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
             elm_icon_file_set(ic, buf, NULL);
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
             elm_icon_file_set(ic, buf, NULL);
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
             elm_icon_file_set(ic, buf, NULL);
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
   Tree_Item *treeit = data;
   char buf[256];
   snprintf(buf, sizeof(buf), "%p %s", treeit->ptr, treeit->name);
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
   static Ecore_Ipc_Server *svr = NULL;
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

   if ((!svr) || (!ecore_ipc_server_connected_get(svr)))
     {
        printf("could not connect to the server: %s\n", g->address);
        return NULL;
     }


   ecore_ipc_server_data_size_max_set(svr, -1);

   /* set event handler for server connect */
   ecore_event_handler_add(ECORE_IPC_EVENT_SERVER_ADD, (Ecore_Event_Handler_Cb)_add, g);
   /* set event handler for server disconnect */
   ecore_event_handler_add(ECORE_IPC_EVENT_SERVER_DEL, (Ecore_Event_Handler_Cb)_del, g);
   /* set event handler for receiving server data */
   ecore_event_handler_add(ECORE_IPC_EVENT_SERVER_DATA, (Ecore_Event_Handler_Cb)_data, g);

   return svr;
}

static void
_gl_selected(void *data EINA_UNUSED, Evas_Object *pobj EINA_UNUSED,
      void *event_info)
{
   /* If not an object, return. */
   if (!elm_genlist_item_parent_get(event_info))
      return;

   Tree_Item *treeit = elm_object_item_data_get(event_info);

   /* START - replacing libclouseau_highlight(obj); */
   int size;
   gui_elements *g = data;
   app_info_st *app = g->sel_app->app->data;
   highlight_st st = { (unsigned long long) (uintptr_t) app->ptr,
        (unsigned long long) (uintptr_t)  treeit->ptr };

   Ecore_Ipc_Server *svr = _connect_to_daemon(g);
   void *p = packet_compose(HIGHLIGHT, &st, sizeof(st), &size);
   if (p)
     {
        ecore_ipc_server_send(svr,
              0,0,0,0,EINA_FALSE, p, size);
        ecore_ipc_server_flush(svr);
        free(p);
     }
   /* END   - replacing libclouseau_highlight(obj); */

   clouseau_obj_information_list_populate(treeit, gui->lb);
}

static int
_load_list(gui_elements *g)
{
   if (g->sel_app)
     {
        elm_genlist_clear(g->gl);
        elm_genlist_clear(g->prop_list);
        app_info_st *st = g->sel_app->app->data;

        if (eina_list_search_unsorted(apps, _app_ptr_cmp,
                 (void *) (uintptr_t) st->ptr))
          {  /* do it only if app selected AND found in apps list */
             int size;
             Ecore_Ipc_Server *svr = _connect_to_daemon(g);
             data_req_st t = { (unsigned long long) (uintptr_t) NULL,
                  (unsigned long long) (uintptr_t) st->ptr };

             void *p = packet_compose(DATA_REQ, &t, sizeof(t), &size);
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
_show_hidden_check_changed(void *data, Evas_Object *obj,
      void *event_info EINA_UNUSED)
{
   list_show_hidden = elm_check_state_get(obj);
   _load_list(data);
}

static void
_bt_clicked(void *data, Evas_Object *obj, void *event_info EINA_UNUSED)
{
   elm_object_text_set(obj, "Reload");
   _load_list(data);
}

static void
_dismiss_inwin(gui_elements *g)
{
   g->address = strdup(elm_entry_entry_get(g->en));
   evas_object_del(g->inwin);
   g->en = NULL;
   g->inwin = NULL;
}

static void
_cancel_bt_clicked(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
   _dismiss_inwin(data);
   elm_exit(); /* exit the program's main loop that runs in elm_run() */
}

static void
_ok_bt_clicked(void *data, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{  /* Set the IP, PORT, then connect to server */
   _dismiss_inwin(data);

   if(!_connect_to_daemon(data))
     {
        printf("Failed to connect to server.\n");
        elm_exit(); /* exit the program's main loop that runs in elm_run() */
     }
}

#ifndef ELM_LIB_QUICKLAUNCH
EAPI int
elm_main(int argc EINA_UNUSED, char **argv EINA_UNUSED)
{  /* Create Client Window */
   Evas_Object *win, *bg, *panes, *bx, *bt,
               *show_hidden_check, *show_clippers_check;

   /* For inwin popup */
   Evas_Object *lb, *bxx, *bt_bx, *bt_ok, *bt_cancel;

   gui = calloc(1, sizeof(gui_elements));

   win = elm_win_add(NULL, "client", ELM_WIN_BASIC);
   elm_win_autodel_set(win, EINA_TRUE);
   elm_win_title_set(win, "Clouseau Client");

   bg = elm_bg_add(win);
   elm_win_resize_object_add(win, bg);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bg);

   bx = elm_box_add(win);
   evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(bx, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_win_resize_object_add(win, bx);
   evas_object_show(bx);

   /* Control buttons */
     {
        Evas_Object *hbx;

        hbx = elm_box_add(bx);
        evas_object_size_hint_align_set(hbx, 0.0, 0.5);
        elm_box_horizontal_set(hbx, EINA_TRUE);
        elm_box_pack_end(bx, hbx);
        elm_box_padding_set(hbx, 10, 0);
        evas_object_show(hbx);

        bt = elm_button_add(hbx);
        evas_object_size_hint_align_set(bt, 0.0, 0.3);
        elm_object_text_set(bt, "Load");
        elm_box_pack_end(hbx, bt);
        evas_object_show(bt);

        gui->dd_list = elm_hoversel_add(win);
        elm_hoversel_hover_parent_set(gui->dd_list, win);
        elm_object_text_set(gui->dd_list, "SELECT APP");

        evas_object_size_hint_align_set(gui->dd_list, 0.0, 0.3);
        elm_box_pack_end(hbx, gui->dd_list);
        evas_object_show(gui->dd_list);

        show_hidden_check = elm_check_add(hbx);
        elm_object_text_set(show_hidden_check, "Show Hidden");
        elm_check_state_set(show_hidden_check, list_show_hidden);
        elm_box_pack_end(hbx, show_hidden_check);
        evas_object_show(show_hidden_check);

        show_clippers_check = elm_check_add(hbx);
        elm_object_text_set(show_clippers_check, "Show Clippers");
        elm_check_state_set(show_clippers_check, list_show_clippers);
        elm_box_pack_end(hbx, show_clippers_check);
        evas_object_show(show_clippers_check);
     }

   panes = elm_panes_add(win);
   evas_object_size_hint_weight_set(panes, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(panes, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(bx, panes);
   evas_object_show(panes);

   /* The main list */
     {
        gui->gl = elm_genlist_add(panes);
        evas_object_size_hint_align_set(gui->gl,
              EVAS_HINT_FILL, EVAS_HINT_FILL);
        evas_object_size_hint_weight_set(gui->gl,
              EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
        elm_object_part_content_set(panes, "left", gui->gl);
        evas_object_show(gui->gl);

        evas_object_smart_callback_add(bt, "clicked", _bt_clicked, gui);
        evas_object_smart_callback_add(show_hidden_check, "changed",
              _show_hidden_check_changed, gui);
        evas_object_smart_callback_add(show_clippers_check, "changed",
              _show_clippers_check_changed, gui);

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
        prop_list = clouseau_obj_information_list_add(panes);
        gui->prop_list = prop_list;
        evas_object_size_hint_align_set(prop_list,
              EVAS_HINT_FILL, EVAS_HINT_FILL);
        evas_object_size_hint_weight_set(prop_list,
              EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

        elm_object_part_content_set(panes, "right", prop_list);
        evas_object_show(prop_list);
     }

   /* START Add buttom panel */
   Evas_Object *panel;
   panel = elm_panel_add(win);
   elm_panel_orient_set(panel, ELM_PANEL_ORIENT_BOTTOM);
   evas_object_size_hint_weight_set(panel, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(panel, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_panel_hidden_set(panel, EINA_TRUE);
   elm_win_resize_object_add(win, panel);

   gui->lb = elm_label_add(win);
   evas_object_size_hint_weight_set(gui->lb, EVAS_HINT_EXPAND, 0);
   evas_object_size_hint_align_set(gui->lb, EVAS_HINT_FILL, 0);
   evas_object_show(gui->lb);

   elm_object_content_set(panel, gui->lb);
   evas_object_show(panel);
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

   /* START - Popup to get IP, PORT from user */
   gui->inwin = elm_win_inwin_add(win);
   evas_object_show(gui->inwin);

   bxx = elm_box_add(gui->inwin);
   evas_object_size_hint_weight_set(bxx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bxx);

   lb = elm_label_add(gui->inwin);
   evas_object_size_hint_weight_set(lb, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(lb, EVAS_HINT_FILL, 0.0);
   elm_object_text_set(lb, "Enter host IP:PORT");
   elm_box_pack_end(bxx, lb);
   evas_object_show(lb);

   /* Single line selected entry */
   char buf[32];
   gui->en = elm_entry_add(gui->inwin);
   elm_entry_scrollable_set(gui->en, EINA_TRUE);
   evas_object_size_hint_weight_set(gui->en,
         EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(gui->en, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_object_style_set(gui->inwin, "minimal_vertical");
   elm_entry_scrollbar_policy_set(gui->en, ELM_SCROLLER_POLICY_OFF,
         ELM_SCROLLER_POLICY_OFF);
   sprintf(buf, "%s:%d", LOCALHOST, PORT);
   elm_object_text_set(gui->en, buf);
   elm_entry_single_line_set(gui->en, EINA_TRUE);
   elm_entry_select_all(gui->en);
   elm_box_pack_end(bxx, gui->en);
   evas_object_show(gui->en);

   bt_bx = elm_box_add(gui->inwin);
   elm_box_horizontal_set(bt_bx, EINA_TRUE);
   elm_box_homogeneous_set(bt_bx, EINA_TRUE);
   evas_object_size_hint_align_set(bt_bx, 0.5, 0.5);
   evas_object_size_hint_weight_set(bt_bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bt_bx);
   elm_box_pack_end(bxx, bt_bx);

   /* Add the cancel button */
   bt_cancel = elm_button_add(gui->inwin);
   elm_object_text_set(bt_cancel, "Cancel");
   evas_object_smart_callback_add(bt_cancel, "clicked",
         _cancel_bt_clicked, (void *) gui);

   elm_box_pack_end(bt_bx, bt_cancel);
   evas_object_show(bt_cancel);

   /* Add the OK button */
   bt_ok = elm_button_add(gui->inwin);
   elm_object_text_set(bt_ok, "OK");
   evas_object_smart_callback_add(bt_ok, "clicked",
         _ok_bt_clicked, (void *) gui);

   elm_box_pack_end(bt_bx, bt_ok);
   evas_object_show(bt_ok);

   elm_win_inwin_content_set(gui->inwin, bxx);
   /* END   - Popup to get IP, PORT from user */

   elm_run();
   elm_shutdown();

   /* cleanup - free apps data */
   void *st;
   EINA_LIST_FREE(apps, st)
      _free_app(st);

   data_descriptors_shutdown();
   if (gui->address)
     free(gui->address);

   free(gui);
   return 0;
}
ELM_MAIN()
#endif
