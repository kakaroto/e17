#define _GNU_SOURCE 1
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <sys/socket.h>

#include <Eina.h>
#include <Ecore.h>
#include <Ecore_Ipc.h>
#include <Edje.h>
#include <Evas.h>
#include <Elementary.h>

#include "helper.h"
#include "libclouseau.h"

static Eina_Bool _elm_is_init = EINA_FALSE;
static const char *_my_app_name = NULL;

static void libclouseau_highlight(Evas_Object *obj);

static void
libclouseau_item_add(Evas_Object *o, Tree_Item *parent)
{
   Eina_List *children, *tmp;
   Evas_Object *child;
   Tree_Item *treeit;
   char buf[1024];

   treeit = calloc(1, sizeof(*treeit));
   treeit->ptr = o;
   treeit->is_obj = EINA_TRUE;
   snprintf(buf, sizeof(buf), "%s", evas_object_type_get(o));

   treeit->name = eina_stringshare_add(buf);
   treeit->is_clipper = !!evas_object_clipees_get(o);
   treeit->is_visible = evas_object_visible_get(o);
   treeit->info = obj_information_get(treeit);

   parent->children = eina_list_append(parent->children, treeit);

   children = evas_object_smart_members_get(o);
   EINA_LIST_FOREACH(children, tmp, child){
        libclouseau_item_add(child, treeit);
   }
}

static Eina_List *
_load_list(void)
{
   Eina_List *tree = NULL;
   Eina_List *ees, *eeitr;
   Ecore_Evas *ee;


   ees = ecore_evas_ecore_evas_list_get();

   EINA_LIST_FOREACH(ees, eeitr, ee)
     {
        Eina_List *objs, *objitr;
        Evas_Object *obj;
        Tree_Item *treeit;

        Evas *e;
        int w, h;

        e = ecore_evas_get(ee);
        evas_output_size_get(e, &w, &h);

        treeit = calloc(1, sizeof(*treeit));
        treeit->name = eina_stringshare_add(ecore_evas_title_get(ee));
        treeit->ptr = ee;

        tree = eina_list_append(tree, treeit);

        objs = evas_objects_in_rectangle_get(e, SHRT_MIN, SHRT_MIN,
              USHRT_MAX, USHRT_MAX, EINA_TRUE, EINA_TRUE);
        EINA_LIST_FOREACH(objs, objitr, obj)
          {
             libclouseau_item_add(obj, treeit);
          }
    }

   return tree;  /* User has to call item_tree_free() */
}

Eina_Bool
_add(void *data EINA_UNUSED, int type EINA_UNUSED, Ecore_Ipc_Event_Server_Add *ev)
{
   void *p;
   int size = 0;

   ecore_ipc_server_data_size_max_set(ev->server, -1);

   connect_st t = { getpid(), _my_app_name };
   p = packet_compose(APP_CLIENT_CONNECT, &t, sizeof(t), &size);
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

Eina_Bool
_data(void *data EINA_UNUSED, int type EINA_UNUSED, Ecore_Ipc_Event_Server_Data *ev)
{
   Variant_st *v = packet_info_get(ev->data, ev->size);

   switch(packet_mapping_type_get(v->t.type))
     {
      case DATA_REQ:
           {
              int size = 0;
              data_req_st *req = v->data;
              tree_data_st t;
              t.gui = req->gui;
              t.app = req->app;
              t.tree = _load_list();
              if (t.tree)
                {  /* Reply with tree data to data request */
                   void *p = packet_compose(TREE_DATA, &t, sizeof(t), &size);
                   if (p)
                     {
                        ecore_ipc_server_send(ev->server, 0,0,0,0,
                              EINA_FALSE, p, size);
                        ecore_ipc_server_flush(ev->server);
                        free(p);
                     }

                   item_tree_free(t.tree);
                }
           }
         break;

      case HIGHLIGHT:
           {
              highlight_st *ht = v->data;
              libclouseau_highlight((Evas_Object *) (uintptr_t) ht->object);
           }
         break;

      default:
         break;
     }

   return ECORE_CALLBACK_RENEW;
}

static int
_connect_to_daemon(void)
{
   Ecore_Ipc_Server *svr;
   const char *address = LOCALHOST;

   eina_init();
   ecore_init();
   ecore_ipc_init();

   if (!(svr = ecore_ipc_server_connect(ECORE_IPC_REMOTE_SYSTEM, LOCALHOST, PORT, NULL)))
     {
        printf("could not connect to the server: %s, port %d.\n",
              address, PORT);
        return EINA_FALSE;
     }

   ecore_ipc_server_data_size_max_set(svr, -1);

   /* set event handler for server connect */
   ecore_event_handler_add(ECORE_IPC_EVENT_SERVER_ADD,
         (Ecore_Event_Handler_Cb)_add, NULL);
   /* set event handler for server disconnect */
   ecore_event_handler_add(ECORE_IPC_EVENT_SERVER_DEL,
         (Ecore_Event_Handler_Cb)_del, NULL);
   /* set event handler for receiving server data */
   ecore_event_handler_add(ECORE_IPC_EVENT_SERVER_DATA,
         (Ecore_Event_Handler_Cb)_data, NULL);

   return EINA_TRUE;
}

/* Hook on the elm_init
 * We only do something here if we didn't already go into elm_init,
 * which probably means we are not using elm. */
int
elm_init(int argc, char **argv)
{
   int (*_elm_init)(int, char **) = dlsym(RTLD_NEXT, "elm_init");

   if (!_elm_is_init)
     {
        _my_app_name = argv[0];
        _elm_is_init = EINA_TRUE;
     }

   return _elm_init(argc, argv);
}

/* Hook on the main loop
 * We only do something here if we didn't already go into elm_init,
 * which probably means we are not using elm. */
void
ecore_main_loop_begin(void)
{
   void (*_ecore_main_loop_begin)(void) =
      dlsym(RTLD_NEXT, "ecore_main_loop_begin");

   if (!_elm_is_init)
     {
        char *margv[] = { "clouseau" };
        /* Make sure we init elementary, wouldn't be needed once we
         * take away the ui to another proc. */
        elm_init(1, margv);
     }

   if(!_connect_to_daemon())
     {
        printf("Failed to connect to server.\n");
        return;
     }

   _ecore_main_loop_begin();

   data_descriptors_shutdown();
   return;
}

#define EINA_LOCK_DEBUG_BT_NUM 64
typedef void (*Eina_Lock_Bt_Func) ();

Evas_Object *
evas_object_new(Evas *e)
{
   Eina_Lock_Bt_Func lock_bt[EINA_LOCK_DEBUG_BT_NUM];
   int lock_bt_num;
   Evas_Object *(*_evas_object_new)(Evas *e) = dlsym(RTLD_NEXT, "evas_object_new");
   Eina_Strbuf *str;
   Evas_Object *r;
   char **strings;
   int i;

   r = _evas_object_new(e);
   if (!r) return NULL;

   lock_bt_num = backtrace((void **)lock_bt, EINA_LOCK_DEBUG_BT_NUM);
   strings = backtrace_symbols((void **)lock_bt, lock_bt_num);

   str = eina_strbuf_new();

   for (i = 1; i < lock_bt_num; ++i)
     eina_strbuf_append_printf(str, "%s\n", strings[i]);

   evas_object_data_set(r, ".clouseau.bt", eina_stringshare_add(eina_strbuf_string_get(str)));

   free(strings);
   eina_strbuf_free(str);

   return r;
}

void
evas_object_free(Evas_Object *obj, int clean_layer)
{
   void (*_evas_object_free)(Evas_Object *obj, int clean_layer) = dlsym(RTLD_NEXT, "evas_object_free");
   const char *tmp;

   tmp = evas_object_data_get(obj, ".clouseau.bt");
   eina_stringshare_del(tmp);

   _evas_object_free(obj, clean_layer);
}

/* HIGHLIGHT code. */
/* The color of the highlight */
enum {
	HIGHLIGHT_R = 255,
	HIGHLIGHT_G = 128,
	HIGHLIGHT_B = 128,
	HIGHLIGHT_A = 255,

	/* How much padding around the highlight box.
         * Currently we don't want any. */
	PADDING = 0,
};

static Eina_Bool
libclouseau_highlight_fade(void *_rect)
{
   Evas_Object *rect = _rect;
   int r, g, b, a;
   double na;

   evas_object_color_get(rect, &r, &g, &b, &a);
   if (a < 20)
     {
        evas_object_del(rect);
        return EINA_FALSE;
     }

   na = a - 20;
   r = na / a * r;
   g = na / a * g;
   b = na / a * b;
   evas_object_color_set(rect, r, g, b, na);

   return EINA_TRUE;
}

static void
libclouseau_highlight(Evas_Object *obj)
{
   Evas *e;
   Evas_Object *r;
   int x, y, w, h;
   const char *tmp;

   e = evas_object_evas_get(obj);
   if (!e) return;

   evas_object_geometry_get(obj, &x, &y, &w, &h);

   r = evas_object_rectangle_add(e);
   evas_object_move(r, x - PADDING, y - PADDING);
   evas_object_resize(r, w + (2 * PADDING), h + (2 * PADDING));
   evas_object_color_set(r, HIGHLIGHT_R, HIGHLIGHT_G, HIGHLIGHT_B,
         HIGHLIGHT_A);
   evas_object_show(r);
   ecore_timer_add(0.1, libclouseau_highlight_fade, r);

   tmp = evas_object_data_get(obj, ".clouseau.bt");
   fprintf(stderr, "Creation backtrace :\n%s*******\n", tmp);
}
