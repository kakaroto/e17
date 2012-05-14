#include <stdio.h>
#include <Ecore.h>
#include <Ecore_Ipc.h>
#include <Eina.h>
#include <Edje.h>
#include <Evas.h>
#include <Elementary.h>
#include <stdint.h>

#include "libclouseau.h"
#include "helper.h"           /*  Our own helper functions  */
#include "eet_dump.h"

#ifdef HAVE_CONFIG_H
# include "config.h"
#else
#define __UNUSED__
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
   Evas_Object *prop_list;
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
_add(void *data __UNUSED__, int type __UNUSED__, Ecore_Ipc_Event_Server_Add *ev)
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
_del(void *data __UNUSED__, int type __UNUSED__, Ecore_Ipc_Event_Server_Del *ev)
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
_load_gui_with_list(Evas_Object *gl, Eina_List *trees)
{
   Eina_List *l;
   Tree_Item *treeit;
   if (!trees)
     return EINA_TRUE;

   EINA_LIST_FOREACH(trees, l, treeit)
     {  /* Insert the base ee items */
        Elm_Genlist_Item_Type glflag = (treeit->children) ?
           ELM_GENLIST_ITEM_TREE : ELM_GENLIST_ITEM_NONE;
        elm_genlist_item_append(gl, &itc, treeit, NULL,
              glflag, NULL, NULL);
     }

   return EINA_TRUE;
}

static void
_set_selected_app(void *data, Evas_Object *pobj,
      void *event_info)
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
_add_app(gui_elements *gui, Variant_st *v)
{
   app_data_st *st = malloc(sizeof(app_data_st));
   st->app = v;
   st->td = NULL; /* Will get this on TREE_DATA message */
   apps = eina_list_append(apps, st);

   _add_app_to_dd_list(gui->dd_list, st);
}

static void
_remove_app(gui_elements *gui, Variant_st *v)
{
   app_closed_st *app = v->data;
   app_info_st *sel_app = gui->sel_app->app->data;
   app_data_st *st = (app_data_st *)
      eina_list_search_unsorted(apps, _app_ptr_cmp,
            (void *) (uintptr_t) app->ptr);

   if (app->ptr == sel_app->ptr)
     {
        elm_object_text_set(gui->dd_list, "SELECT APP");
        elm_genlist_clear(gui->gl);
        elm_genlist_clear(gui->prop_list);
        gui->sel_app = NULL;
     }

   if (st)
     {  /* Remove from list and free all variants */
        /* TODO: Remove app from Drop Down List */
        apps = eina_list_remove(apps, st);
        free(st->app);
        if (st->td)
          free(st->td);

        free(st);

        if (!elm_hoversel_expanded_get(gui->dd_list))
          {
             Eina_List *l;
             elm_hoversel_clear(gui->dd_list);
             EINA_LIST_FOREACH(apps, l , st)
                _add_app_to_dd_list(gui->dd_list, st);
          }
     }

   free(v);
}

static void
_update_tree(gui_elements *gui, Variant_st *v)
{  /* Update Tree for app, then update GUI if its displayed */
   tree_data_st *td = v->data;
   app_data_st *st = (app_data_st *)
      eina_list_search_unsorted(apps, _app_ptr_cmp,
            (void *) (uintptr_t) td->app);

   if (st)
     {
        if (st->td)
          free(st->td);

        st->td = v;

        elm_genlist_clear(gui->gl);
        _load_gui_with_list(gui->gl, td->tree);
     }
}

Eina_Bool
_data(void *data, int type __UNUSED__, Ecore_Ipc_Event_Server_Data *ev)
{
   static Eina_Bool got_tree = EINA_FALSE;

   printf("Received %i bytes from server:\n"
         ">>>>>\n"
         "%%.%is\n"
         ">>>>>\n",
         ev->size, ev->size);

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
          }

       /* free(v) - freed when removed from app list */
     }

   return ECORE_CALLBACK_RENEW;
}

static void
gl_exp(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
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
gl_con(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
   Elm_Object_Item *glit = event_info;
   elm_genlist_item_subitems_clear(glit);
}

static void
gl_exp_req(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
   Elm_Object_Item *glit = event_info;
   elm_genlist_item_expanded_set(glit, EINA_TRUE);
}

static void
gl_con_req(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
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
                   elm_app_data_dir_get());
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
                   elm_app_data_dir_get());
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
                   elm_app_data_dir_get());
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
                   elm_app_data_dir_get());
             elm_icon_file_set(ic, buf, NULL);
             evas_object_size_hint_aspect_set(ic, EVAS_ASPECT_CONTROL_VERTICAL,
                   1, 1);
             return ic;
          }
     }

   return NULL;
}

static char *
item_text_get(void *data, Evas_Object *obj __UNUSED__,
      const char *part __UNUSED__)
{
   Tree_Item *treeit = data;
   char buf[256];
   snprintf(buf, sizeof(buf), "%p %s", treeit->ptr, treeit->name);
   return strdup(buf);
}

static void
client_win_del(void *data, Evas_Object *obj, void *event_info)
{  /* called when client window is deleted, do cleanup here */
   /* TODO: client cleanup */
   elm_exit(); /* exit the program's main loop that runs in elm_run() */
}

static Ecore_Ipc_Server *
_connect_to_daemon(gui_elements *gui)
{
   static Ecore_Ipc_Server *svr = NULL;
   const char *address = LOCALHOST;

   if (svr && ecore_ipc_server_connected_get(svr))
     return svr;  /* Already connected */

   if (!(svr = ecore_ipc_server_connect(ECORE_IPC_REMOTE_SYSTEM, LOCALHOST, PORT, NULL)))
     {
        printf("could not connect to the server: %s, port %d.\n",
              address, PORT);
        return NULL;
     }

   ecore_ipc_server_data_size_max_set(svr, -1);

   /* set event handler for server connect */
   ecore_event_handler_add(ECORE_IPC_EVENT_SERVER_ADD, (Ecore_Event_Handler_Cb)_add, gui);
   /* set event handler for server disconnect */
   ecore_event_handler_add(ECORE_IPC_EVENT_SERVER_DEL, (Ecore_Event_Handler_Cb)_del, gui);
   /* set event handler for receiving server data */
   ecore_event_handler_add(ECORE_IPC_EVENT_SERVER_DATA, (Ecore_Event_Handler_Cb)_data, gui);

   return svr;
}

static void
_gl_selected(void *data __UNUSED__, Evas_Object *pobj __UNUSED__,
      void *event_info)
{
//   clouseau_obj_information_list_clear();
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

   clouseau_obj_information_list_populate(treeit);
}

void
_item_tree_print_string(Tree_Item *parent)
{
   Tree_Item *treeit;
   Eina_List *l;

   EINA_LIST_FOREACH(parent->children, l, treeit)
     {
         _item_tree_print_string(treeit);
     }

   if (parent->name)
     printf("From EET: <%s>\n", parent->name);
}

static int
_load_list(gui_elements *gui)
{
   if (gui->sel_app)
     {
        elm_genlist_clear(gui->gl);
        elm_genlist_clear(gui->prop_list);
        app_info_st *st = gui->sel_app->app->data;

        if (eina_list_search_unsorted(apps, _app_ptr_cmp,
                 (void *) (uintptr_t) st->ptr))
          {  /* do it only if app selected AND found in apps list */
             int size;
             Ecore_Ipc_Server *svr = _connect_to_daemon(gui);
             data_req_st t = { (unsigned long long) (uintptr_t) NULL,
                  (unsigned long long) (uintptr_t) st->ptr };

             void *p = packet_compose(DATA_REQ, &t, sizeof(t), &size);
             if (p)
               {
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
      void *event_info __UNUSED__)
{
   list_show_clippers = elm_check_state_get(obj);
   _load_list(data);
}

static void
_show_hidden_check_changed(void *data, Evas_Object *obj,
      void *event_info __UNUSED__)
{
   list_show_hidden = elm_check_state_get(obj);
   _load_list(data);
}

static void
_bt_clicked(void *data, Evas_Object *obj, void *event_info __UNUSED__)
{
   elm_object_text_set(obj, "Reload");
   _load_list(data);
}

#ifndef ELM_LIB_QUICKLAUNCH
EAPI int
elm_main(int argc, char **argv)
{  /* Create Client Window */
   Evas_Object *win, *bg, *panes, *bx, *bt, *show_hidden_check,
               *show_clippers_check;

   gui = malloc(sizeof(gui_elements));
   gui->sel_app = NULL;

   win = elm_win_add(NULL, "client", ELM_WIN_BASIC);
   elm_win_autodel_set(win, EINA_TRUE);
   elm_win_title_set(win, "client");

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

   evas_object_resize(win, 500, 500);
   evas_object_show(win);

   evas_object_smart_callback_add(win, "delete,request", client_win_del, NULL);

   eina_init();
   ecore_init();
   ecore_ipc_init();

   if(!_connect_to_daemon(gui))
     {
        printf("Failed to connect to server.\n");
        return 1;
     }

   elm_run();
   elm_shutdown();

   data_descriptors_shutdown();
   free(gui);
   printf("Client cleanup.\n");
   return 0;
}
ELM_MAIN()
#endif
