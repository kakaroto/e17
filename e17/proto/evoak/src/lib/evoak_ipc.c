#include "Evoak.h"
#include "evoak_private.h"

int EVOAK_EVENT_CANVAS_INFO = 0;
int EVOAK_EVENT_SYNC_REPLY = 0;
int EVOAK_EVENT_DISCONNECT = 0;

/****************************************************************************/

static void  _evoak_ipc_send(Evoak *ev, int proto, int v, int err, void *data, i
nt data_size);

static int   _evoak_ipc_cb_server_add(void *data, int type, void *event);
static int   _evoak_ipc_cb_server_del(void *data, int type, void *event);
static int   _evoak_ipc_cb_server_data(void *data, int type, void *event);

static void  _evoak_event_generic_free(void *data, void *ev);

static void  _evoak_callback_call(Evoak_Object *o, Evoak_Callback_Type type, voi
d *event_info);

static int   _evoak_hash_keygen (Evoak_Hash *hash, int key);
static int   _evoak_hash_store  (Evoak_Hash *hash, int id, void *data);
static void *_evoak_hash_unstore(Evoak_Hash *hash, int id);
static void *_evoak_hash_find   (Evoak_Hash *hash, int id);
static void  _evoak_ref(Evoak *ev);
static void  _evoak_unref(Evoak *ev);
static void  _evoak_request_id_inc(Evoak *ev);
static void  _evoak_request_id_unuse(Evoak *ev, int id);
static void  _evoak_object_layer_del(Evoak_Object *o);
static void  _evoak_object_layer_add(Evoak_Object *o);
static void  _evoak_object_id_inc(Evoak *ev);
static void  _evoak_object_ref(Evoak_Object *o);
static void  _evoak_object_unref(Evoak_Object *o);

/****************************************************************************/

static int _evoak_init_count = 0;
static Ecore_Event_Handler *_evoak_ipc_cb_server_add_handle = NULL;
static Ecore_Event_Handler *_evoak_ipc_cb_server_del_handle = NULL;
static Ecore_Event_Handler *_evoak_ipc_cb_server_data_handle = NULL;

/*#define BANDWIDTH*/

/****************************************************************************/

static void
_evoak_ipc_send(Evoak *ev, int proto, int v, int err, void *data, int data_size)
{
   int b;
#ifdef BANDWIDTH
   static int bc_last = 0;
   static int calls = 0;
   static double t_last = 0.0;

   if (calls == 0)
     {
        bc_last = 0;
        t_last = ecore_time_get();
     }
   calls++;
#endif
   b = ecore_ipc_server_send(ev->server, proto,
                             EVOAK_PROTOCOL_VERSION,
                             ev->request_id, v,
                             err,
                             data, data_size);
   _evoak_request_id_inc(ev);
   ev->bytecount += b;
#ifdef BANDWIDTH
   bc_last += b;
//   printf("REQ: %i, bytes %i, total sent %i\n", ev->request_id, b, ev->bytecou
nt);
   if (calls == 500)
     {
        double t;
        double bps;

        t = ecore_time_get() - t_last;
        bps = (double)bc_last / t;
        printf("%i bytes in %3.3f sec\n", bc_last, t);
        printf("DATARATE: %3.3fKb/sec (%3.3fKbps), total %3.3fKb\n",
               bps / 1024.0,
               (bps * 8.0) / 1024.0,
               (double)ev->bytecount / 1024.0
               );
        calls = 0;
     }
#endif
}

static int
_evoak_ipc_cb_server_add(void *data, int type, void *event)
{
   Ecore_Ipc_Event_Server_Del *e;
   Evoak *ev;

   e = event;
   ev = ecore_ipc_server_data_get(e->server);
   ev->connected = 1;
     {
        Evoak_PR_Connect p;
        void *d;
        int s;

        p.proto_min = 1;
        p.proto_max = 1;
        p.client_name = ev->client_name;
        p.client_class = ev->client_class;
        p.auth_key = "";
        d = _evoak_proto[EVOAK_PR_CONNECT].enc(&p, &s);
        if (d)
          {
             _evoak_ipc_send(ev, EVOAK_PR_CONNECT, 0, 0, d, s);
             free(d);
          }
     }
   return 1;
}

static int
_evoak_ipc_cb_server_del(void *data, int type, void *event)
{
   Ecore_Ipc_Event_Server_Del *e;
   Evoak *ev;

   e = event;
   ev = ecore_ipc_server_data_get(e->server);
   ev->connected = 0;
   printf("!!! server %p disconnected us!\n", e->server);
     {
        Evoak_Event_Disconnect *e2;

        e2 = calloc(1, sizeof(Evoak_Event_Disconnect));
        if (e2)
          ecore_event_add(EVOAK_EVENT_DISCONNECT, e2,
                          _evoak_event_generic_free, NULL);
     }
   return 1;
}

static int
_evoak_ipc_cb_server_data(void *data, int type, void *event)
{
   Ecore_Ipc_Event_Server_Data *e;
   Evoak *ev;

   e = event;
   ev = ecore_ipc_server_data_get(e->server);
//   printf("!!! server %p sent data!\n", e->server);
   if ((e->major <= EVOAK_PR_NONE) || (e->major >= EVOAK_PR_LAST) ||
       (e->minor != EVOAK_PROTOCOL_VERSION)) return 1;
   switch (e->major)
     {
      case EVOAK_PR_CONNECT: /* client doesnt handle this */
        break;
      case EVOAK_PR_CANVAS_INFO:
          {
             Evoak_PR_Canvas_Info p;
             Evoak_Event_Canvas_Info *e2;

             ev->w = p.w;
             ev->h = p.h;
             if (!_evoak_proto[e->major].dec(e->data, e->size, &p)) return 1;
             e2 = calloc(1, sizeof(Evoak_Event_Canvas_Info));
             if (e2)
               {
                  e2->evoak = ev;
                  e2->w = p.w;
                  e2->h = p.h;
                  e2->rotation = p.rot;
                  ecore_event_add(EVOAK_EVENT_CANVAS_INFO, e2,
                                  _evoak_event_generic_free, NULL);
               }
          }
        break;
      case EVOAK_PR_SYNC: /* client doesnt handle this */
        break;
      case EVOAK_PR_SYNC_REPLY:
          {
             Evoak_Event_Sync_Reply *e2;

             e2 = calloc(1, sizeof(Evoak_Event_Sync_Reply));
             if (e2)
               {
                  e2->evoak = ev;
                  e2->id = e->response;
                  ecore_event_add(EVOAK_EVENT_SYNC_REPLY, e2,
                                  _evoak_event_generic_free, NULL);
               }
          }
        break;
      case EVOAK_PR_OBJECT_ADD: /* client doesnt handle this */
        break;
      case EVOAK_PR_OBJECT_DEL: /* client doesnt handle this */
        break;
      case EVOAK_PR_OBJECT_MOVE: /* client doesnt handle this */
        break;
      case EVOAK_PR_OBJECT_RESIZE: /* client doesnt handle this */
        break;
      case EVOAK_PR_OBJECT_SHOW: /* client doesnt handle this */
        break;
      case EVOAK_PR_OBJECT_HIDE: /* client doesnt handle this */
        break;
      case EVOAK_PR_OBJECT_CLIP_SET: /* client doesnt handle this */
        break;
      case EVOAK_PR_OBJECT_COLOR_SET: /* client doesnt handle this */
        break;
      case EVOAK_PR_OBJECT_LAYER_SET: /* client doesnt handle this */
        break;
      case EVOAK_PR_OBJECT_STACK: /* client doesnt handle this */
        break;
      case EVOAK_PR_OBJECT_EVENT_PROP_SET: /* client doesnt handle this */
        break;
      case EVOAK_PR_OBJECT_EVENT_MASK_SET: /* client doesnt handle this */
        break;
      case EVOAK_PR_OBJECT_FOCUS_SET: /* client doesnt handle this */
        break;
      case EVOAK_PR_OBJECT_IMAGE_FILE_SET: /* client doesnt handle this */
        break;
      case EVOAK_PR_OBJECT_IMAGE_FILL_SET: /* client doesnt handle this */
        break;
      case EVOAK_PR_CLIENT_FREEZE: /* client doesnt handle this */
        break;
      case EVOAK_PR_CLIENT_THAW: /* client doesnt handle this */
        break;
      case EVOAK_PR_OBJECT_IMAGE_BORDER_SET: /* client doesnt handle this */
        break;
      case EVOAK_PR_OBJECT_IMAGE_SMOOTH_SCALE_SET: /* client doesnt handle this 
*/
        break;
      case EVOAK_PR_OBJECT_IMAGE_SIZE_SET: /* client doesnt handle this */
        break;
      case EVOAK_PR_OBJECT_IMAGE_ALPHA_SET: /* client doesnt handle this */
        break;
      case EVOAK_PR_OBJECT_EVENT_MOUSE_MOVE:
          {
             Evoak_PR_Object_Event_Mouse_Move p;
             Evoak_Object *o;

             if (!_evoak_proto[e->major].dec(e->data, e->size, &p)) return 1;
             o = _evoak_hash_find(&(ev->object_hash), e->ref_to);
             if (o)
               {
                  Evoak_Event_Mouse_Move evi;

                  evi.cur.x = p.x;
                  evi.cur.y = p.y;
                  evi.prev.x = p.px;
                  evi.prev.y = p.py;
                  evi.buttons = p.bmask;
                  evi.modmask = p.modmask;
                  evi.lockmask = p.lockmask;
                  _evoak_callback_call(o, EVOAK_CALLBACK_MOUSE_MOVE, &evi);
               }
          }
        break;
      case EVOAK_PR_OBJECT_EVENT_MOUSE_DOWNUP:
          {
             Evoak_PR_Object_Event_Mouse_Downup p;
             Evoak_Object *o;

             if (!_evoak_proto[e->major].dec(e->data, e->size, &p)) return 1;
             o = _evoak_hash_find(&(ev->object_hash), e->ref_to);
             if (o)
               {
                  if (p.downup)
                    {
                       Evoak_Event_Mouse_Down evi;

                       evi.x = p.x;
                       evi.y = p.y;
                       evi.button = p.b;
                       evi.modmask = p.modmask;
                       evi.lockmask = p.lockmask;
                       _evoak_callback_call(o, EVOAK_CALLBACK_MOUSE_DOWN, &evi);
                    }
                  else
                    {
                       Evoak_Event_Mouse_Up evi;

                       evi.x = p.x;
                       evi.y = p.y;
                       evi.button = p.b;
                       evi.modmask = p.modmask;
                       evi.lockmask = p.lockmask;
                       _evoak_callback_call(o, EVOAK_CALLBACK_MOUSE_UP, &evi);
                    }
               }
          }
        break;
      case EVOAK_PR_OBJECT_EVENT_MOUSE_INOUT:
          {
             Evoak_PR_Object_Event_Mouse_Inout p;
             Evoak_Object *o;

             if (!_evoak_proto[e->major].dec(e->data, e->size, &p)) return 1;
             o = _evoak_hash_find(&(ev->object_hash), e->ref_to);
             if (o)
               {
                  if (p.inout)
                    {
                       Evoak_Event_Mouse_In evi;

                       evi.x = p.x;
                       evi.y = p.y;
                       evi.buttons = p.bmask;
                       evi.modmask = p.modmask;
                       evi.lockmask = p.lockmask;
                       _evoak_callback_call(o, EVOAK_CALLBACK_MOUSE_IN, &evi);
                    }
                  else
                    {
                       Evoak_Event_Mouse_Out evi;

                       evi.x = p.x;
                       evi.y = p.y;
                       evi.buttons = p.bmask;
                       evi.modmask = p.modmask;
                       evi.lockmask = p.lockmask;
                       _evoak_callback_call(o, EVOAK_CALLBACK_MOUSE_OUT, &evi);
                    }
               }
          }
        break;
        break;
      case EVOAK_PR_OBJECT_EVENT_MOUSE_WHEEL:
          {
             Evoak_PR_Object_Event_Mouse_Wheel p;
             Evoak_Object *o;

             if (!_evoak_proto[e->major].dec(e->data, e->size, &p)) return 1;
             o = _evoak_hash_find(&(ev->object_hash), e->ref_to);
             if (o)
               {
                  Evoak_Event_Mouse_Wheel evi;

                  evi.dir = p.dir;
                  evi.z = p.z;
                  evi.x = p.x;
                  evi.y = p.y;
                  evi.modmask = p.modmask;
                  evi.lockmask = p.lockmask;
                  _evoak_callback_call(o, EVOAK_CALLBACK_MOUSE_WHEEL, &evi);
               }
          }
        break;
      case EVOAK_PR_OBJECT_EVENT_KEY_DOWNUP:
          {
             Evoak_PR_Object_Event_Key_Downup p;
             Evoak_Object *o;

             if (!_evoak_proto[e->major].dec(e->data, e->size, &p)) return 1;
             o = _evoak_hash_find(&(ev->object_hash), e->ref_to);
             if (o)
               {
                  if (p.downup)
                    {
                       Evoak_Event_Key_Down evi;

                       evi.keyname = p.keyname;
                       evi.string = p.string;
                       evi.modmask = p.modmask;
                       evi.lockmask = p.lockmask;
                       _evoak_callback_call(o, EVOAK_CALLBACK_KEY_DOWN, &evi);
                    }
                  else
                    {
                       Evoak_Event_Key_Up evi;

                       evi.keyname = p.keyname;
                       evi.string = p.string;
                       evi.modmask = p.modmask;
                       evi.lockmask = p.lockmask;
                       _evoak_callback_call(o, EVOAK_CALLBACK_KEY_UP, &evi);
                    }
               }
          }
        break;
      case EVOAK_PR_OBJECT_LINE_SET:
        break;
      case EVOAK_PR_OBJECT_GRAD_ANGLE_SET:
        break;
      case EVOAK_PR_OBJECT_GRAD_COLOR_ADD:
        break;
      case EVOAK_PR_OBJECT_GRAD_COLOR_CLEAR:
        break;
      case EVOAK_PR_OBJECT_POLY_POINT_ADD:
        break;
      case EVOAK_PR_OBJECT_POLY_POINT_CLEAR:
        break;
      case EVOAK_PR_OBJECT_TEXT_SOURCE_SET:
        break;
      case EVOAK_PR_OBJECT_TEXT_FONT_SET:
        break;
      case EVOAK_PR_OBJECT_TEXT_TEXT_SET:
        break;
      case EVOAK_PR_OBJECT_MOVE8:
        break;
      case EVOAK_PR_OBJECT_MOVE16:
        break;
      case EVOAK_PR_OBJECT_MOVE_REL8:
        break;
      case EVOAK_PR_OBJECT_MOVE_REL16:
        break;
      case EVOAK_PR_OBJECT_RESIZE8:
        break;
      case EVOAK_PR_OBJECT_RESIZE16:
        break;
      case EVOAK_PR_OBJECT_RESIZE_REL8:
        break;
      case EVOAK_PR_OBJECT_RESIZE_REL16:
        break;
      case EVOAK_PR_OBJECT_IMAGE_FILL8_SET:
        break;
      case EVOAK_PR_OBJECT_IMAGE_FILL16_SET:
        break;
      case EVOAK_PR_OBJECT_IMAGE_FILL_SIZE8_SET:
        break;
      case EVOAK_PR_OBJECT_IMAGE_FILL_SIZE16_SET:
        break;
      case EVOAK_PR_OBJECT_IMAGE_FILL_ALL_SET:
        break;
      case EVOAK_PR_OBJECT_EVENT_OBJECT_MOVE:
          {
             Evoak_Object *o;
             Evoak_PR_Object_Event_Object_Move p;

             if (!_evoak_proto[e->major].dec(e->data, e->size, &p)) return 1;
             o = _evoak_hash_find(&(ev->object_hash), e->ref_to);
             if (o)
               {
                  o->x = p.x;
                  o->y = p.y;
               }
          }
        break;
      case EVOAK_PR_OBJECT_EVENT_OBJECT_RESIZE:
          {
             Evoak_Object *o;
             Evoak_PR_Object_Event_Object_Resize p;

             if (!_evoak_proto[e->major].dec(e->data, e->size, &p)) return 1;
             o = _evoak_hash_find(&(ev->object_hash), e->ref_to);
             if (o)
               {
                  o->w = p.w;
                  o->h = p.h;
               }
          }
        break;
      case EVOAK_PR_OBJECT_EVENT_OBJECT_RESTACK:
          {
             Evoak_Object *o;
             Evoak_PR_Object_Event_Object_Restack p;

             if (!_evoak_proto[e->major].dec(e->data, e->size, &p)) return 1;
             o = _evoak_hash_find(&(ev->object_hash), e->ref_to);
             if (o)
               {
                  if (p.relative_id == 0)
                    {
                       if (p.abovebelow)
                         {
                            o->layer_data->objects = evas_list_remove_list(o->la
yer_data->objects, o->link);
                            o->layer_data->objects = evas_list_append(o->layer_d
ata->objects, o);
                            o->link = evas_list_last(o->layer_data->objects);
                         }
                       else
                         {
                            o->layer_data->objects = evas_list_remove_list(o->la
yer_data->objects, o->link);
                            o->layer_data->objects = evas_list_append(o->layer_d
ata->objects, o);
                            o->link = o->layer_data->objects;
                         }
                    }
                  else
                    {
                       Evoak_Object *o_rel;

                       o_rel = _evoak_hash_find(&(ev->object_hash), p.relative_i
d);
                       if (o_rel)
                         {
                            if (p.abovebelow)
                              {
                                 o->layer_data->objects = evas_list_remove_list(
o->layer_data->objects, o->link);
                                 o->layer_data->objects = evas_list_append_relat
ive(o->layer_data->objects, o, o_rel);
                                 o->link = o_rel->link->next;
                              }
                            else
                              {
                                 o->layer_data->objects = evas_list_remove_list(
o->layer_data->objects, o->link);
                                 o->layer_data->objects = evas_list_prepend_rela
tive(o->layer_data->objects, o, o_rel);
                                 o->link = o_rel->link->prev;
                              }
                         }
                    }
               }
          }
        break;
      case EVOAK_PR_OBJECT_EVENT_OBJECT_LAYER_SET:
          {
             Evoak_Object *o;
             Evoak_PR_Object_Event_Object_Layer_Set p;

             if (!_evoak_proto[e->major].dec(e->data, e->size, &p)) return 1;
             o = _evoak_hash_find(&(ev->object_hash), e->ref_to);
             if (o)
               {
                  if (o->layer != p.l)
                    {
                       _evoak_object_layer_del(o);
                       o->layer = p.l;
                       _evoak_object_layer_add(o);
                    }
               }
          }
        break;
      case EVOAK_PR_OBJECT_EVENT_OBJECT_SHOW:
          {
             Evoak_Object *o;

             o = _evoak_hash_find(&(ev->object_hash), e->ref_to);
             if (o)
               {
                  o->visible = 1;
               }
          }
        break;
      case EVOAK_PR_OBJECT_EVENT_OBJECT_HIDE:
          {
             Evoak_Object *o;

             o = _evoak_hash_find(&(ev->object_hash), e->ref_to);
             if (o)
               {
                  o->visible = 0;
               }
          }
        break;
      case EVOAK_PR_OBJECT_EDJE_FILE_SET:
        break;
      case EVOAK_PR_OBJECT_EDJE_SWALLOW:
        break;
      case EVOAK_PR_OBJECT_EDJE_UNSWALLOW:
        break;
      case EVOAK_PR_OBJECT_EDJE_TEXT_SET:
        break;
      case EVOAK_PR_OBJECT_EDJE_TEXT_CHANGED:
          {
             Evoak_Object *o;
             Evoak_PR_Object_Edje_Text_Changed p;

             if (!_evoak_proto[e->major].dec(e->data, e->size, &p)) return 1;
             o = _evoak_hash_find(&(ev->object_hash), e->ref_to);
             if (o)
               {
                  if (o->magic2 == EVOAK_OBJECT_EDJE_MAGIC)
                    {
                       Evas_List *l;
                       Evoak_Meta_Edje *m;

                       m = o->meta;
                       for (l = m->parts; l; l = l->next)
                         {
                            Evoak_Meta_Edje_Part *evp;

                            evp = l->data;
                            if (!strcmp(evp->part->name, p.part))
                              {
                                 if (evp->text) free(evp->text);
                                 evp->text = strdup(p.text);
                              }
                         }
                    }
               }
          }
        break;
      case EVOAK_PR_OBJECT_EDJE_SIGNAL_LISTEN:
        break;
      case EVOAK_PR_OBJECT_EDJE_SIGNAL_UNLISTEN:
        break;
      case EVOAK_PR_OBJECT_EDJE_SIGNAL_EMIT:
        break;
      case EVOAK_PR_OBJECT_EDJE_SIGNAL:
          {
             Evoak_Object *o;
             Evoak_PR_Object_Edje_Signal p;

             if (!_evoak_proto[e->major].dec(e->data, e->size, &p)) return 1;
             o = _evoak_hash_find(&(ev->object_hash), e->ref_to);
             if (o)
               {
                  if (o->magic2 == EVOAK_OBJECT_EDJE_MAGIC)
                    {
                       Evoak_Meta_Edje *m;
                       Evas_List *l;

                       m = o->meta;
                       for (l = m->callbacks; l; l = l->next)
                         {
                            Evoak_Meta_Edje_Callback *cb;

                            cb = l->data;
                            if ((cb->id == p.callback_id) && (cb->func))
                              {
                                 cb->func(cb->data, o, p.emission, p.source);
                                 break;
                              }
                         }
                    }
               }
          }
        break;
      default:
        break;
     }
   return 1;
}


