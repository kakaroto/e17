#include "Clouseau.h"
#include <dlfcn.h>
#include <execinfo.h>

#include <Ecore_Ipc.h>
#include <Edje.h>
#include <Evas.h>
#include <Elementary.h>
#include <Ecore_X.h>

#include "clouseau_private.h"

static Eina_Bool _elm_is_init = EINA_FALSE;
static const char *_my_app_name = NULL;

static void
libclouseau_item_add(Evas_Object *o, Clouseau_Tree_Item *parent)
{
   Clouseau_Tree_Item *treeit;
   Eina_List *children;
   Evas_Object *child;

   treeit = calloc(1, sizeof(Clouseau_Tree_Item));
   if (!treeit) return ;

   treeit->ptr = (uintptr_t) o;
   treeit->is_obj = EINA_TRUE;

   treeit->name = eina_stringshare_add(evas_object_type_get(o));
   treeit->is_clipper = !!evas_object_clipees_get(o);
   treeit->is_visible = evas_object_visible_get(o);
   treeit->info = obj_information_get(treeit);

   parent->children = eina_list_append(parent->children, treeit);

   /* if (!evas_object_smart_data_get(o)) return ; */

   /* Do this only for smart object */
   children = evas_object_smart_members_get(o);
   EINA_LIST_FREE(children, child)
     libclouseau_item_add(child, treeit);
}

static void *
_canvas_bmp_get(Ecore_Evas *ee, Evas_Coord *w_out, Evas_Coord *h_out)
{
   Ecore_X_Image *img;
   Ecore_X_Window_Attributes att;
   unsigned char *src;
   unsigned int *dst;
   int bpl = 0, rows = 0, bpp = 0;
   Evas_Coord w, h;

   /* Check that this window still exists */
   Eina_List *eeitr, *ees = ecore_evas_ecore_evas_list_get();
   Ecore_Evas *eel;
   Eina_Bool found_evas = EINA_FALSE;

   EINA_LIST_FOREACH(ees, eeitr, eel)
      if (eel == ee)
        {
           found_evas = EINA_TRUE;
           break;
        }

   Ecore_X_Window xwin = (found_evas) ?
      (Ecore_X_Window) ecore_evas_window_get(ee) : 0;

   if (!xwin)
     {
        printf("Can't grab X window.\n");
        *w_out = *h_out = 0;
        return NULL;
     }

   Evas *e = ecore_evas_get(ee);
   evas_output_size_get(e, &w, &h);
   memset(&att, 0, sizeof(Ecore_X_Window_Attributes));
   ecore_x_window_attributes_get(xwin, &att);
   img = ecore_x_image_new(w, h, att.visual, att.depth);
   ecore_x_image_get(img, xwin, 0, 0, 0, 0, w, h);
   src = ecore_x_image_data_get(img, &bpl, &rows, &bpp);
   dst = malloc(w * h * sizeof(int));  /* Will be freed by the user */
   if (!ecore_x_image_is_argb32_get(img))
     {  /* Fill dst buffer with image convert */
        ecore_x_image_to_argb_convert(src, bpp, bpl,
              att.colormap, att.visual,
              0, 0, w, h,
              dst, (w * sizeof(int)), 0, 0);
     }
   else
     {  /* Fill dst buffer by copy */
        memcpy(dst, src, (w * h * sizeof(int)));
     }

   /* dst now holds window bitmap */
   ecore_x_image_free(img);
   *w_out = w;
   *h_out = h;
   return (void *) dst;
}

static Eina_List *
_load_list(void)
{
   Eina_List *tree = NULL;
   Eina_List *ees;
   Ecore_Evas *ee;

   ees = ecore_evas_ecore_evas_list_get();

   EINA_LIST_FREE(ees, ee)
     {
        Eina_List *objs;
        Evas_Object *obj;
        Clouseau_Tree_Item *treeit;

        Evas *e;
        int w, h;

        e = ecore_evas_get(ee);
        evas_output_size_get(e, &w, &h);

        treeit = calloc(1, sizeof(Clouseau_Tree_Item));
        if (!treeit) continue ;

        treeit->name = eina_stringshare_add(ecore_evas_title_get(ee));
        treeit->ptr = (uintptr_t) ee;

        tree = eina_list_append(tree, treeit);

        objs = evas_objects_in_rectangle_get(e, SHRT_MIN, SHRT_MIN,
              USHRT_MAX, USHRT_MAX, EINA_TRUE, EINA_TRUE);

        EINA_LIST_FREE(objs, obj)
          libclouseau_item_add(obj, treeit);
    }

   return tree;  /* User has to call clouseau_tree_free() */
}

Eina_Bool
_add(void *data EINA_UNUSED, int type EINA_UNUSED, Ecore_Ipc_Event_Server_Add *ev)
{
   void *p;
   int size = 0;

   ecore_ipc_server_data_size_max_set(ev->server, -1);

   connect_st t = { getpid(), _my_app_name };
   p = packet_compose(CLOUSEAU_APP_CLIENT_CONNECT, &t, sizeof(t), &size, NULL, 0);
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
   Variant_st *v;

   v = packet_info_get(ev->data, ev->size);
   switch (clouseau_packet_mapping_type_get(v->t.type))
     {
      case CLOUSEAU_DATA_REQ:
        {  /* data req includes ptr to GUI, to tell which client asking */
           int size = 0;
           data_req_st *req = v->data;
           tree_data_st t;
           t.gui = req->gui;  /* GUI client requesting data from daemon */
           t.app = req->app;  /* APP client sending data to daemon */
           t.tree = _load_list();

           if (t.tree)
             {  /* Reply with tree data to data request */
                void *p = packet_compose(CLOUSEAU_TREE_DATA,
                                         &t, sizeof(t), &size,
                                         NULL, 0);
                if (p)
                  {
                     ecore_ipc_server_send(ev->server, 0,0,0,0,
                                           EINA_FALSE, p, size);
                     ecore_ipc_server_flush(ev->server);
                     free(p);
                  }

                clouseau_tree_free(t.tree);
             }
        }
        break;

      case CLOUSEAU_HIGHLIGHT:
           {  /* Highlight msg contains PTR of object to highlight */
              highlight_st *ht = v->data;
              Evas_Object *obj = (Evas_Object *) (uintptr_t) ht->object;
              clouseau_object_highlight(obj, NULL, NULL);
           }
         break;

      case CLOUSEAU_BMP_REQ:
           {  /* Bitmap req msg contains PTR of Ecore Evas */
              bmp_req_st *req = v->data;
              Evas_Coord w, h;
              int size = 0;
              void *bmp = _canvas_bmp_get((Ecore_Evas *) (uintptr_t)
                    req->object, &w, &h);

              bmp_info_st t = { req->gui,
                   req->app, req->object , req->ctr, w, h,
                   NULL,NULL, NULL, 1.0,
                   NULL, NULL, NULL, NULL, NULL, NULL };

              void *p = packet_compose(CLOUSEAU_BMP_DATA, &t, sizeof(t), &size,
                    bmp, (w * h * sizeof(int)));

              if (p)
                {
                   ecore_ipc_server_send(ev->server, 0,0,0,0,
                         EINA_FALSE, p, size);
                   ecore_ipc_server_flush(ev->server);
                   free(p);
                }

              if (bmp)
                free(bmp);
           }
         break;

      default:
         break;
     }

   clouseau_variant_free(v);
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

   svr = ecore_ipc_server_connect(ECORE_IPC_REMOTE_SYSTEM,
         LOCALHOST, PORT, NULL);

   if (!svr)
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

   clouseau_init();

   _ecore_main_loop_begin();

   clouseau_shutdown();

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
