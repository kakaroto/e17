#include "Evoak.h"
#include "evoak_private.h"


#define EVOAK_MAGIC                  0x6834bca1
#define EVOAK_OBJECT_MAGIC           0x715682fe
#define EVOAK_OBJECT_RECTANGLE_MAGIC 0x28a8bcf4
#define EVOAK_OBJECT_IMAGE_MAGIC     0x9874fedc
#define EVOAK_OBJECT_TEXT_MAGIC      0xab53cd82
#define EVOAK_OBJECT_LINE_MAGIC      0x924f8e9c
#define EVOAK_OBJECT_POLYGON_MAGIC   0xa73bc84a
#define EVOAK_OBJECT_GRADIENT_MAGIC  0xf73de985
#define EVOAK_OBJECT_EDJE_MAGIC      0x152efc93

int EVOAK_EVENT_CANVAS_INFO = 0;
int EVOAK_EVENT_SYNC_REPLY = 0;
int EVOAK_EVENT_DISCONNECT = 0;

/****************************************************************************/

static void  _evoak_ipc_send(Evoak *ev, int proto, int v, int err, void *data, int data_size);

static int   _evoak_ipc_cb_server_add(void *data, int type, void *event);
static int   _evoak_ipc_cb_server_del(void *data, int type, void *event);
static int   _evoak_ipc_cb_server_data(void *data, int type, void *event);

static void  _evoak_event_generic_free(void *data, void *ev);

static void  _evoak_callback_call(Evoak_Object *o, Evoak_Callback_Type type, void *event_info);

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
//   printf("REQ: %i, bytes %i, total sent %i\n", ev->request_id, b, ev->bytecount);
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
      case EVOAK_PR_OBJECT_IMAGE_SMOOTH_SCALE_SET: /* client doesnt handle this */
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
			    o->layer_data->objects = evas_list_remove_list(o->layer_data->objects, o->link);
			    o->layer_data->objects = evas_list_append(o->layer_data->objects, o);
			    o->link = evas_list_last(o->layer_data->objects);
			 }
		       else
			 {
			    o->layer_data->objects = evas_list_remove_list(o->layer_data->objects, o->link);
			    o->layer_data->objects = evas_list_append(o->layer_data->objects, o);
			    o->link = o->layer_data->objects;
			 }
		    }
		  else
		    {
		       Evoak_Object *o_rel;
		  
		       o_rel = _evoak_hash_find(&(ev->object_hash), p.relative_id);
		       if (o_rel)
			 {
			    if (p.abovebelow)
			      {
				 o->layer_data->objects = evas_list_remove_list(o->layer_data->objects, o->link);
				 o->layer_data->objects = evas_list_append_relative(o->layer_data->objects, o, o_rel);
				 o->link = o_rel->link->next;
			      }
			    else
			      {
				 o->layer_data->objects = evas_list_remove_list(o->layer_data->objects, o->link);
				 o->layer_data->objects = evas_list_prepend_relative(o->layer_data->objects, o, o_rel);
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

static void
_evoak_event_generic_free(void *data, void *ev)
{
   free(ev);
}

static void
_evoak_callback_call(Evoak_Object *o, Evoak_Callback_Type type, void *event_info)
{
   Evas_List *l;
   
   _evoak_object_ref(o);
   o->lock++;
   for (l = o->callbacks; l; l = l->next)
     {
	Evoak_Callback *cb;
	
	cb = l->data;
	if (cb->just_added) cb->just_added = 0;
	else if ((!cb->delete_me) && (cb->type == type))
	  cb->func(cb->data, o->evoak, o, event_info);
	if (o->delete_me) break;
     }
   o->lock--;
   if ((o->lock == 0) && (o->delete_cb))
     {
	for (l = o->callbacks; l;)
	  {
	     Evoak_Callback *cb;
	     
	     cb = l->data;
	     if (cb->delete_me)
	       {
		  Evas_List *nl;
		  
		  nl = l->next;
		  o->callbacks = evas_list_remove_list(o->callbacks, nl);
		  free(cb);
	       }
	     else
	       l = l->next;
	  }
	o->delete_cb = 0;
     }
   _evoak_object_unref(o);
}

static int
_evoak_hash_keygen(Evoak_Hash *hash, int key)
{
   return (key & (64 - 1));
}

static int
_evoak_hash_store(Evoak_Hash *hash, int id, void *data)
{
   Evoak_Hash_Node *hn;
   int h;
   
   hn = malloc(sizeof(Evoak_Hash_Node));
   if (!hn) return 0;
   h = _evoak_hash_keygen(hash, id);
   hn->id = id;
   hn->data = data;
   hash->buckets[h] = evas_list_prepend(hash->buckets[h], hn);
   return 1;
}

static void *
_evoak_hash_unstore(Evoak_Hash *hash, int id)
{
   Evoak_Hash_Node *hn;
   Evas_List *l;
   int h;
   
   h = _evoak_hash_keygen(hash, id);
   for (l = hash->buckets[h]; l; l = l->next)
     {
	hn = l->data;       
	if (hn->id == id)
	  {                 
	     void *data;
	     
	     hash->buckets[h] = evas_list_remove_list(hash->buckets[h], l);
	     data = hn->data;
	     free(hn);
	     return data;        
	  }
     }
   return NULL;
}

static void *
_evoak_hash_find(Evoak_Hash *hash, int id)
{
   Evoak_Hash_Node *hn;
   Evas_List *l;
   int h;
   
   h = _evoak_hash_keygen(hash, id);
   for (l = hash->buckets[h]; l; l = l->next)
     {
	hn = l->data;       
	if (hn->id == id)
	  {                 
	     hash->buckets[h] = evas_list_remove_list(hash->buckets[h], l);
	     hash->buckets[h] = evas_list_prepend(hash->buckets[h], hn);
	     return hn->data;
	  }
     }
   return NULL;
}

static void
_evoak_ref(Evoak *ev)
{
   ev->ref++;
}

static void
_evoak_unref(Evoak *ev)
{
   ev->ref--;
   if (ev->ref <= 0)
     {
	if (ev->client_name) free(ev->client_name);
	if (ev->client_class) free(ev->client_class);
	ecore_ipc_server_del(ev->server);
	while (ev->font_path)
	  {
	     free(ev->font_path->data);
	     ev->font_path = evas_list_remove(ev->font_path, ev->font_path->data);
	  }
	while (ev->layers)
	  {
	     Evoak_Layer *lay;
	     
	     lay = ev->layers->data;
	     while (lay)
	       {
		  Evoak_Object *eo;
		  
		  eo = lay->objects->data;
		  _evoak_object_unref(eo);
		  if (!ev->layers) lay = NULL;
		  else lay = ev->layers->data;
	       }
	  }
	free(ev);
     }
}

static void
_evoak_request_id_inc(Evoak *ev)
{
   ev->request_id++;
}

static void
_evoak_request_id_unuse(Evoak *ev, int id)
{
}

static void
_evoak_object_layer_del(Evoak_Object *o)
{
   o->layer_data->objects = evas_list_remove(o->layer_data->objects, o);
   if (!o->layer_data->objects)
     {
	o->evoak->layers = evas_list_remove_list(o->evoak->layers, o->layer_data->link);
	free(o->layer_data);
     }
   o->layer_data = NULL;
}

static void
_evoak_object_layer_add(Evoak_Object *o)
{
   Evas_List *l, *ll;
   Evoak_Layer *lay;
           
   for (l = o->evoak->layers; l; l = l->next)
     {
	lay = l->data;
	if (lay->layer == o->layer)
	  {
	     lay->objects = evas_list_append(lay->objects, o);
	     o->link = evas_list_last(lay->objects);
	     o->layer_data = lay;
	     return;
	  }
	else if (lay->layer > o->layer)
	  {
	     /* insert new layer before */
	     lay = calloc(1, sizeof(Evoak_Layer));
	     lay->evoak = o->evoak;
	     lay->layer = o->layer;
	     o->evoak->layers = evas_list_prepend_relative(o->evoak->layers, lay, l->data);
	     lay->link = l->prev;
	     lay->objects = evas_list_append(lay->objects, o);
	     o->link = evas_list_last(lay->objects);
	     o->layer_data = lay;
	     return;
	  }
     }
   lay = calloc(1, sizeof(Evoak_Layer));
   lay->evoak = o->evoak;
   lay->layer = o->layer;
   o->evoak->layers = evas_list_append(o->evoak->layers, lay);
   lay->link = evas_list_last(o->evoak->layers);
   lay->objects = evas_list_append(lay->objects, o);
   o->link = evas_list_last(lay->objects);
   o->layer_data = lay;
}

static void
_evoak_object_id_inc(Evoak *ev)
{
   ev->object_id++;
}

static void
_evoak_object_ref(Evoak_Object *o)
{
   o->ref++;
   _evoak_ref(o->evoak);
}

static void
_evoak_object_unref(Evoak_Object *o)
{
   Evoak *ev;
   
   o->ref--;
   ev = o->evoak;
   if (o->ref <= 0)
     {
	_evoak_hash_unstore(&(ev->object_hash), o->id);
	_evoak_object_layer_del(o);
	if (o->clip)
	  o->clip->clipees = evas_list_remove(o->clip->clipees, o);
	while (o->clipees)
	  {
	     Evoak_Object *o2;
	     
	     o2 = o->clipees->data;
	     o2->clip = NULL;
	     o->clipees = evas_list_remove(o->clipees, o2);
	  }
	if (o->name)
	  {
	     o->evoak->object_names = evas_hash_del(o->evoak->object_names, o->name, o);
	     free(o->name);
	  }
	while (o->data)
	  {
	     Evoak_Data *ed;
	     
	     ed = o->data->data;
	     o->data = evas_list_remove(o->data, ed);
	     free(ed->key);
	     free(ed);
	  }
	while (o->callbacks)
	  {
	     Evoak_Callback *cb;
	     
	     cb = o->callbacks->data;
	     o->callbacks = evas_list_remove(o->callbacks, cb);
	     free(cb);
	  }
	_evoak_request_id_unuse(ev, o->id);
	if ((o->meta) && (o->meta_free)) o->meta_free(o->meta);
	o->magic = 0xf1f2fffe;
	free(o);
     }
   _evoak_unref(ev);
}

/****************************************************************************/

/*
 * Init any systems needed to run evoak
 * @return Greater than 0 if successful, 0 if not
 * 
 * This will intialize any data, variables, structs, files etc. needed for
 * an application to use evoak. It should be called first before any other
 * evoak calls are used.
 */
int
evoak_init(void)
{
   _evoak_init_count++;
   if (_evoak_init_count > 1) return _evoak_init_count;
   ecore_init();
   ecore_ipc_init();
   _evoak_ipc_cb_server_add_handle = ecore_event_handler_add(ECORE_IPC_EVENT_SERVER_ADD, _evoak_ipc_cb_server_add, NULL);
   _evoak_ipc_cb_server_del_handle = ecore_event_handler_add(ECORE_IPC_EVENT_SERVER_DEL, _evoak_ipc_cb_server_del, NULL);
   _evoak_ipc_cb_server_data_handle = ecore_event_handler_add(ECORE_IPC_EVENT_SERVER_DATA, _evoak_ipc_cb_server_data, NULL);
   _evoak_protocol_init();
   if (!EVOAK_EVENT_CANVAS_INFO)
     {
	EVOAK_EVENT_CANVAS_INFO = ecore_event_type_new();
	EVOAK_EVENT_SYNC_REPLY = ecore_event_type_new();
	EVOAK_EVENT_DISCONNECT = ecore_event_type_new();
     }
   evas_imaging_image_cache_set(0); /* flush cache */
   evas_imaging_image_cache_set(21 * 1024); /* 32kb image cache for image metadata */
   evas_imaging_font_cache_set(0); /* flush cache */
   evas_imaging_font_cache_set(256 * 1024); /* 256kb font cache for image metadata */
   edje_init();
   return _evoak_init_count;
}

/*
 * Shut down the evoak system
 * @return 0 if the system completely shut down successfully
 * 
 * This will shut down Evoak. No evoak calls should be called after this, until
 * another evoak_init() is called.
 */
int
evoak_shutdown(void)
{
   _evoak_init_count--;
   if (_evoak_init_count > 0) return _evoak_init_count;
   edje_shutdown();
   _evoak_protocol_shutdown();
   if (_evoak_ipc_cb_server_add_handle)
     {
	ecore_event_handler_del(_evoak_ipc_cb_server_add_handle);
	_evoak_ipc_cb_server_add_handle = NULL;
     }
   if (_evoak_ipc_cb_server_del_handle)
     {
	ecore_event_handler_del(_evoak_ipc_cb_server_del_handle);
	_evoak_ipc_cb_server_del_handle = NULL;
     }
   if (_evoak_ipc_cb_server_data_handle)
     {
	ecore_event_handler_del(_evoak_ipc_cb_server_data_handle);
	_evoak_ipc_cb_server_data_handle = NULL;
     }
   ecore_ipc_shutdown();
   ecore_shutdown();
   return _evoak_init_count;
}

/*
 * Connect to an evoak shared canvas
 * @param server The string ID of the server to connect to
 * @param client_name The string name of the client
 * @param client_class The string class of the client
 * @return A pointer to an Evoak connection, or NULL on failure
 * 
 * This connects to an evoak canvas shared server. 1 server shares 1 canvas
 * only. If @p server is NULL then the default server will be used. The
 * default is determined by the EVOAK_SERVER environment variable or if not
 * set then the local users "evoak" named server running over unix sockets
 * under instance 0 will be connected to. This parameter is currently unused.
 * 
 * NULL will be returned if connection setup fails or memory cannot be
 * allocated for the connection. Otherwise a pointer to the connection will
 * be returned and should be used to addess this particular shared canvas from
 * now until evoak_disconnect() is called on the handle.
 * 
 * This does NOT mean the connection is completely successful. Authentication
 * may fail or the server may be full, and so a callback will be called later
 * indicating this. The client will recieve a series of events after connecting
 * giving information about the shared canvas. It is suggested the client waits
 * until these events have arrived before doing anything.
 * 
 * The @p client_name is a string identification to help identify this client
 * and tell it apart from others. It is suggested the application name be used
 * as this is normally unique. This should really never shange for a client
 * so it can always be readily identified as the same client name in future.
 * 
 * The @p client_class is a string indicating what type of client it is. For
 * example, it may be a "background" client or a "panel" client, a
 * "application" client or a "custom" client. The fill list of class names
 * is not covered here and is not yet defined.
 */
Evoak *
evoak_connect(char *server, char *client_name, char *client_class)
{
   Evoak *ev;
   
   ev = calloc(1, sizeof(Evoak));
   if (!ev) return NULL;
   ev->magic = EVOAK_MAGIC;
   /* FIXME: parse "server" string to determine local, remote, SSL and */
   /* instance number */
   ev->server = ecore_ipc_server_connect(ECORE_IPC_LOCAL_USER,
					 "evoak",
					 0,
					 ev);
   if (!ev->server)
     {
	free(ev);
	return NULL;
     }
   ev->client_name = strdup(client_name);
   ev->client_class = strdup(client_class);
   ev->request_id = 1;
   ev->object_id = 1;
   _evoak_ref(ev);
   return ev;
}

/*
 * Disconnect from an evoak shared canvas
 * @param ev The shared canvas handle to disconnect from
 * 
 * This disconnects from a shared canvas @p ev, flushing any buffers of unsent
 * requests and terminating the connection and freeing all memory and resources
 * on the client side, associated with this shared canvas. No more requests
 * should be sent to this shared canvas handle as it is now invalid.
 */
void
evoak_disconnect(Evoak *ev)
{
   Evas_List *l, *ll, *tmpl = NULL;
   
   if ((!ev) || (ev->magic != EVOAK_MAGIC)) return;
   for (l = ev->layers; l; l = l->next)
     {
	Evoak_Layer *lay;
	
	lay = l->data;
	for (ll = lay->objects; ll; ll = ll->next)
	  {
	     Evoak_Object *eo;
	     
	     eo = ll->data;
	     tmpl = evas_list_append(tmpl, eo);
	  }
     }
   while (tmpl)
     {
	Evoak_Object *eo;
	
	eo = tmpl->data;
	tmpl = evas_list_remove_list(tmpl, tmpl);
	_evoak_object_unref(eo);
     }
   _evoak_unref(ev);
}

void
evoak_output_size_get(Evoak *ev, Evoak_Coord *w, Evoak_Coord *h)
{
   if ((!ev) || (ev->magic != EVOAK_MAGIC)) return;
   if (w) *w = ev->w;
   if (h) *h = ev->h;
}

/*
 * Send a sync request with a given numeric Id to the server
 * @param ev The evoak server connection
 * @param id The numeric integer ID to send
 * 
 * This sends a sync request to the evoak server that will elicit a reply at
 * some point in the future of an event of type EVOAK_EVENT_SYNC_REPLY, so the
 * client should have an event handler callback for this type of event if it
 * wants the reply. When the reply is recieved the the id memeber of the
 * Evoak_Event_Sync_Reply event structure will have the same numeric ID sent
 * to the server by this call. This will allow the client to know that all
 * calls called before the sync call that produced this reply have been
 * completed. It is suggested that the *p id is chosen in a unique way so that
 * no other outstanding sync requests (that have not recieved replied) are
 * using the same ID value, so the sync can be identified uniquely.
 */
void
evoak_sync(Evoak *ev, int id)
{
   if ((!ev) || (ev->magic != EVOAK_MAGIC)) return;
   _evoak_ipc_send(ev, EVOAK_PR_SYNC, 0, id, NULL, 0);
}

void
evoak_freeze(Evoak *ev)
{
   ev->freeze++;
   if (ev->freeze == 1)
     {
	_evoak_ipc_send(ev, EVOAK_PR_CLIENT_FREEZE, 0, 0, NULL, 0);
     }
}

void
evoak_thaw(Evoak *ev)
{
   ev->freeze--;
   if (ev->freeze == 0)
     {
	_evoak_ipc_send(ev, EVOAK_PR_CLIENT_THAW, 0, 0, NULL, 0);
     }
}

/*
 * Delete an object from the canvas
 * @param o The object to delete
 * 
 * This function will delete the specified object @p o from the evoak canvas
 * it belongs to. The client will recieve no further events for this object
 * after this call is complete.
 */
void
evoak_object_del(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->delete_me) return;
   _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_DEL, o->id, 0, NULL, 0);
   _evoak_object_ref(o);
   _evoak_callback_call(o, EVOAK_CALLBACK_FREE, NULL);
   _evoak_object_unref(o);
   o->delete_me = 1;
   if (o->swallow)
     {
	o->swallow->swallowees = evas_list_remove(o->swallow->swallowees, o);
	o->swallow = NULL;
     }
   while (o->swallowees)
     {
	Evoak_Object *o2;
	
	o2 = o->swallowees->data;
	o2->swallow = NULL;
	o->swallowees = evas_list_remove_list(o->swallowees, o->swallowees);
     }
   _evoak_object_unref(o);
}

const char *
evoak_object_type_get(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return "";
   if (o->delete_me) return "";
   return o->type;
}

void
evoak_object_move(Evoak_Object *o, Evoak_Coord x, Evoak_Coord y)
{
   unsigned char *d;
   int s;
   int dx, dy;

   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->delete_me) return;
   if ((x == o->x) && (y == o->y)) return;
   /* FIXME: adjust coords for line & poly objs */

   if (((-128 <= x) && (x < 128)) && ((-128 <= y) && (y < 128)))
     {
	Evoak_PR_Object_Move8 p;
	
	p.x = x;
	p.y = y;
	d = _evoak_proto[EVOAK_PR_OBJECT_MOVE8].enc(&p, &s);
	if (d)
	  {
	     _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_MOVE8, o->id, 0, d, s);
	     free(d);
	  }
	o->x = x;
	o->y = y;
	return;
     }
   dx = x - o->x;
   dy = y - o->y;
   if (((-128 <= dx) && (dx < 128)) && ((-128 <= dy) && (dy < 128)))
     {	
	Evoak_PR_Object_Move_Rel8 p;
	
	p.x = dx;
	p.y = dy;
	d = _evoak_proto[EVOAK_PR_OBJECT_MOVE_REL8].enc(&p, &s);
	if (d)
	  {
	     _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_MOVE_REL8, o->id, 0, d, s);
	     free(d);
	  }
	o->x = x;
	o->y = y;
	return;
     }
   if (((-32768 <= x) && (x < 32768)) && ((-32768 <= y) && (y < 32768)))
     {
	Evoak_PR_Object_Move16 p;
	
	p.x = x;
	p.y = y;
	d = _evoak_proto[EVOAK_PR_OBJECT_MOVE16].enc(&p, &s);
	if (d)
	  {
	     _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_MOVE16, o->id, 0, d, s);
	     free(d);
	  }
	o->x = x;
	o->y = y;
	return;
     }
   if (((-32768 <= dx) && (dx < 32768)) && ((-32768 <= dy) && (dy < 32768)))
     {	
	Evoak_PR_Object_Move_Rel16 p;
	
	p.x = dx;
	p.y = dy;
	d = _evoak_proto[EVOAK_PR_OBJECT_MOVE_REL16].enc(&p, &s);
	if (d)
	  {
	     _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_MOVE_REL16, o->id, 0, d, s);
	     free(d);
	  }
	o->x = x;
	o->y = y;
	return;
     }

     {
	Evoak_PR_Object_Move p;
	
	p.x = x;
	p.y = y;
	d = _evoak_proto[EVOAK_PR_OBJECT_MOVE].enc(&p, &s);
	if (d)
	  {
	     _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_MOVE, o->id, 0, d, s);
	     free(d);
	  }
	o->x = x;
	o->y = y;
     }
}

void
evoak_object_resize(Evoak_Object *o, Evoak_Coord w, Evoak_Coord h)
{
   unsigned char *d;
   int s;
   int dh, dw;

   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->delete_me) return;
   if (w < 0) w = 0;
   if (h < 0) h = 0;
   if ((w == o->w) && (h == o->h)) return;
   /* FIXME: disallow for text objects */
   /* FIXME: adjust coords for line & poly objs */
   
   if (((0 <= w) && (w < 256)) && ((0 <= h) && (h < 256)))
     {
	Evoak_PR_Object_Resize8 p;
	
	p.w = w;
	p.h = h;
	d = _evoak_proto[EVOAK_PR_OBJECT_RESIZE8].enc(&p, &s);
	if (d)
	  {
	     _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_RESIZE8, o->id, 0, d, s);
	     free(d);
	  }
	o->w = w;
	o->h = h;
	return;
     }
   dw = w - o->w;
   dh = h - o->h;
   if (((-128 <= dw) && (dw < 128)) && ((-128 <= dh) && (dh < 128)))
     {	
	Evoak_PR_Object_Resize_Rel8 p;
	
	p.w = dw;
	p.h = dh;
	d = _evoak_proto[EVOAK_PR_OBJECT_RESIZE_REL8].enc(&p, &s);
	if (d)
	  {
	     _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_RESIZE_REL8, o->id, 0, d, s);
	     free(d);
	  }
	o->w = w;
	o->h = h;
	return;
     }
   if (((-32768 <= w) && (w < 32768)) && ((-32768 <= h) && (h < 32768)))
     {
	Evoak_PR_Object_Resize16 p;
	
	p.w = w;
	p.h = h;
	d = _evoak_proto[EVOAK_PR_OBJECT_RESIZE16].enc(&p, &s);
	if (d)
	  {
	     _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_RESIZE16, o->id, 0, d, s);
	     free(d);
	  }
	o->w = w;
	o->h = h;
	return;
     }
   if (((-32768 <= dw) && (dw < 32768)) && ((-32768 <= dh) && (dh < 32768)))
     {	
	Evoak_PR_Object_Resize_Rel16 p;
	
	p.w = dw;
	p.h = dh;
	d = _evoak_proto[EVOAK_PR_OBJECT_RESIZE_REL16].enc(&p, &s);
	if (d)
	  {
	     _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_RESIZE_REL16, o->id, 0, d, s);
	     free(d);
	  }
	o->w = w;
	o->h = h;
	return;
     }
   
     {
	Evoak_PR_Object_Resize p;
	
	p.w = w;
	p.h = h;
	d = _evoak_proto[EVOAK_PR_OBJECT_RESIZE].enc(&p, &s);
	if (d)
	  {
	     _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_RESIZE, o->id, 0, d, s);
	     free(d);
	  }
	o->w = w;
	o->h = h;
     }
}

void
evoak_object_geometry_get(Evoak_Object *o, Evoak_Coord *x, Evoak_Coord *y, Evoak_Coord *w, Evoak_Coord *h)
{
   if (o->delete_me) return;
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (x) *x = o->x;
   if (y) *y = o->y;
   if (w) *w = o->w;
   if (h) *h = o->h;
}

void
evoak_object_show(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->delete_me) return;
   if (o->visible) return;
   o->visible = 1;
   _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_SHOW, o->id, 0, NULL, 0);
}

void
evoak_object_hide(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->delete_me) return;
   if (!o->visible) return;
   o->visible = 0;
   _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_HIDE, o->id, 0, NULL, 0);
}

Evoak_Bool
evoak_object_visible_get(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return 0;
   if (o->delete_me) return 0;
   return o->visible;
}

void
evoak_object_clip_set(Evoak_Object *o, Evoak_Object *oc)
{
   unsigned char *d;
   int s;
   Evoak_PR_Object_Clip_Set p;

   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->delete_me) return;
   if (oc)
     {
	if (oc->magic != EVOAK_OBJECT_MAGIC) return;
	if (oc->delete_me) return;
     }
   if (o->clip == oc) return;
   if (o->clip)
     o->clip->clipees = evas_list_remove(o->clip->clipees, o);
   o->clip = oc;
   if (oc)
     oc->clipees = evas_list_append(oc->clipees, o);
   if (!oc) p.clipper_id = 0;
   else p.clipper_id = oc->id;
   d = _evoak_proto[EVOAK_PR_OBJECT_CLIP_SET].enc(&p, &s);
   if (d)
     {
	_evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_CLIP_SET, o->id, 0, d, s);
	free(d);
     }
}

Evoak_Object *
evoak_object_clip_get(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return NULL;   
   if (o->delete_me) return NULL;
   return o->clip;
}

void
evoak_object_clip_unset(Evoak_Object *o, Evoak_Object *oc)
{
   evoak_object_clip_set(o, NULL);
}

const Evoak_List *
evoak_object_clipees_get(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return NULL;
   if (o->delete_me) return NULL;
   return (Evoak_List *)o->clipees;
}


void
evoak_object_color_set(Evoak_Object *o, int r, int g, int b, int a)
{
   unsigned char *d;
   int s;
   Evoak_PR_Object_Color_Set p;

   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->delete_me) return;
   if (r < 0) r = 0; if (r > 255) r = 255;
   if (g < 0) g = 0; if (g > 255) r = 255;
   if (b < 0) b = 0; if (b > 255) r = 255;
   if (a < 0) a = 0; if (a > 255) r = 255;
   if ((o->r == r) && (o->g == g) && (o->b == b) && (o->a == a)) return;
   o->r = r;
   o->g = g;
   o->b = b;
   o->a = a;
   p.r = r;
   p.g = g;
   p.b = b;
   p.a = a;
   d = _evoak_proto[EVOAK_PR_OBJECT_COLOR_SET].enc(&p, &s);
   if (d)
     {
	_evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_COLOR_SET, o->id, 0, d, s);
	free(d);
     }
}

void
evoak_object_color_get(Evoak_Object *o, int *r, int *g, int *b, int *a)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->delete_me) return;
   if (r) *r = o->r;
   if (g) *g = o->g;
   if (b) *b = o->b;
   if (a) *a = o->a;
}

void
evoak_object_layer_set(Evoak_Object *o, int l)
{
   unsigned char *d;
   int s;
   Evoak_PR_Object_Layer_Set p;

   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->delete_me) return;
   if (o->swallow) return;
   if (o->layer == l)
     {
	evoak_object_raise(o);
	return;
     }
   _evoak_object_layer_del(o);
   o->layer = l;
   _evoak_object_layer_add(o);
   p.layer = l;
   d = _evoak_proto[EVOAK_PR_OBJECT_LAYER_SET].enc(&p, &s);
   if (d)
     {
	_evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_LAYER_SET, o->id, 0, d, s);
	free(d);
     }
}

int
evoak_object_layer_get(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return 0;
   if (o->delete_me) return 0;
   return o->layer;
}

void
evoak_object_raise(Evoak_Object *o)
{
   unsigned char *d;
   int s;
   Evoak_PR_Object_Stack p;

   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->delete_me) return;
   if (o->swallow) return;
//   if (!o->link->next) return;
   o->layer_data->objects = evas_list_remove_list(o->layer_data->objects, o->link);
   o->layer_data->objects = evas_list_append(o->layer_data->objects, o);
   o->link = evas_list_last(o->layer_data->objects);
   p.relative_id = 0;
   p.relative = EVOAK_RELATIVE_ABOVE;
   d = _evoak_proto[EVOAK_PR_OBJECT_STACK].enc(&p, &s);
   if (d)
     {
	_evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_STACK, o->id, 0, d, s);
	free(d);
     }
}

void
evoak_object_lower(Evoak_Object *o)
{
   unsigned char *d;
   int s;
   Evoak_PR_Object_Stack p;

   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->delete_me) return;
   if (o->swallow) return;
//   if (!o->link->prev) return;
   o->layer_data->objects = evas_list_remove_list(o->layer_data->objects, o->link);
   o->layer_data->objects = evas_list_append(o->layer_data->objects, o);
   o->link = o->layer_data->objects;
   p.relative_id = 0;
   p.relative = EVOAK_RELATIVE_BELOW;
   d = _evoak_proto[EVOAK_PR_OBJECT_STACK].enc(&p, &s);
   if (d)
     {
	_evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_STACK, o->id, 0, d, s);
	free(d);
     }
}

void
evoak_object_stack_above(Evoak_Object *o, Evoak_Object *above)
{
   unsigned char *d;
   int s;
   Evoak_PR_Object_Stack p;

   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->delete_me) return;
   if (o->swallow) return;
   if (!above)
     {
	evoak_object_raise(o);
	return;
     }
   if (above->magic != EVOAK_OBJECT_MAGIC) return;
   if (above->delete_me) return;
//   if ((o->link->prev) && (o->link->prev->data == above)) return;
   o->layer_data->objects = evas_list_remove_list(o->layer_data->objects, o->link);
   o->layer_data->objects = evas_list_append_relative(o->layer_data->objects, o, above);
   o->link = above->link->next;
   p.relative_id = above->id;
   p.relative = EVOAK_RELATIVE_ABOVE;
   d = _evoak_proto[EVOAK_PR_OBJECT_STACK].enc(&p, &s);
   if (d)
     {
	_evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_STACK, o->id, 0, d, s);
	free(d);
     }
}

void
evoak_object_stack_below(Evoak_Object *o, Evoak_Object *below)
{
   unsigned char *d;
   int s;
   Evoak_PR_Object_Stack p;

   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->delete_me) return;
   if (o->swallow) return;
   if (!below)
     {
	evoak_object_lower(o);
	return;
     }
   if (below->magic != EVOAK_OBJECT_MAGIC) return;
   if (below->delete_me) return;
//   if ((o->link->next) && (o->link->next->data == below)) return;
   o->layer_data->objects = evas_list_remove_list(o->layer_data->objects, o->link);
   o->layer_data->objects = evas_list_prepend_relative(o->layer_data->objects, o, below);
   o->link = below->link->prev;
   p.relative_id = below->id;
   p.relative = EVOAK_RELATIVE_BELOW;
   d = _evoak_proto[EVOAK_PR_OBJECT_STACK].enc(&p, &s);
   if (d)
     {
	_evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_STACK, o->id, 0, d, s);
	free(d);
     }
}

Evoak_Object *
evoak_object_above_get(Evoak_Object *o)
{
   Evas_List *l, *ll;
   int first = 1;
   
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return NULL;
   if (o->delete_me) return NULL;
   for (l = o->layer_data->link; l; l = l->next)
     {
	Evoak_Layer *lay;
	
	lay = l->data;
	if (first) ll = o->link->next;
	else ll = lay->objects;
	first = 0;
	for (; ll; ll = ll->next)
	  {
	     Evoak_Object *eo;
	     
	     eo = ll->data;
	     if (!eo->delete_me) return eo;
	  }
     }
   return NULL;
}

Evoak_Object *
evoak_object_below_get(Evoak_Object *o)
{
   Evas_List *l, *ll;
   int first = 1;
   
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return NULL;
   if (o->delete_me) return NULL;
   for (l = o->layer_data->link; l; l = l->prev)
     {
	Evoak_Layer *lay;
	
	lay = l->data;
	if (first) ll = o->link->prev;
	else ll = lay->objects;
	first = 0;
	for (; ll; ll = ll->prev)
	  {
	     Evoak_Object *eo;
	     
	     eo = ll->data;
	     if (!eo->delete_me) return eo;
	  }
     }
}

Evoak_Object *
evoak_object_bottom_get(Evoak *ev)
{
   Evas_List *l, *ll;
   
   if ((!ev) || (ev->magic != EVOAK_MAGIC)) return;
   for (l = ev->layers; l; l = l->next)
     {
	Evoak_Layer *lay;
	
	lay = l->data;
	for (ll = lay->objects; ll; ll = ll->next)
	  {
	     Evoak_Object *eo;
	     
	     eo = ll->data;
	     if (!eo->delete_me) return eo;
	  }
     }
   return NULL;
}

Evoak_Object *
evoak_object_top_get(Evoak *ev)
{
   Evas_List *l, *ll;
   
   if ((!ev) || (ev->magic != EVOAK_MAGIC)) return;
   for (l = evas_list_last(ev->layers); l; l = l->prev)
     {
	Evoak_Layer *lay;
	
	lay = l->data;
	for (ll = evas_list_last(lay->objects); ll; ll = ll->prev)
	  {
	     Evoak_Object *eo;
	     
	     eo = ll->data;
	     if (!eo->delete_me) return eo;
	  }
     }
   return NULL;
}

void
evoak_object_data_set(Evoak_Object *o, const char *key, const void *data)
{
   Evoak_Data *ed;
   
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->delete_me) return;
   if ((!key) || (!data)) return;
   evoak_object_data_del(o, key);
   ed = malloc(sizeof(Evoak_Data));
   if (!ed) return;
   ed->key = strdup(key);
   ed->data = (void *)data;
   o->data = evas_list_prepend(o->data, ed);
}

void *
evoak_object_data_get(Evoak_Object *o, const char *key)
{
   Evas_List *l;
   
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return NULL;
   if (o->delete_me) return NULL;
   if (!key) return NULL;
   for (l = o->data; l; l = l->next)
     {
	Evoak_Data *ed;
	
	ed = l->data;
	if (!strcmp(ed->key, key))
	  {
	     if (l->prev)
	       {
		  o->data = evas_list_remove_list(o->data, l);
		  o->data = evas_list_prepend(o->data, ed);
	       }
	     return ed->data;
	  }
     }
   return NULL;
}

void *
evoak_object_data_del(Evoak_Object *o, const char *key)
{
   Evas_List *l;
   
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return NULL;
   if (o->delete_me) return NULL;
   if (!key) return NULL;
   for (l = o->data; l; l = l->next)
     {
	Evoak_Data *ed;
	
	ed = l->data;
	if (!strcmp(ed->key, key))
	  {
	     void *data;
	     
	     o->data = evas_list_remove_list(o->data, l);
	     data = ed->data;
	     free(ed->key);
	     free(ed);
	     return data;
	  }
     }
   return NULL;
}

void
evoak_object_name_set(Evoak_Object *o, const char *name)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->delete_me) return;
   if (o->name)
     {
	o->evoak->object_names = evas_hash_del(o->evoak->object_names, o->name, o);
	free(o->name);
	o->name = NULL;
     }
   if (name)
     {
	o->name = strdup(name);
	o->evoak->object_names = evas_hash_add(o->evoak->object_names, o->name, o);
     }
}

const char *
evoak_object_name_get(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return NULL;
   if (o->delete_me) return NULL;
   return o->name;
}

Evoak_Object *
evoak_object_name_find(Evoak *ev, const char *name)
{
   if ((!ev) || (ev->magic != EVOAK_MAGIC)) return NULL;
   if (!name) return NULL;
   return evas_hash_find(ev->object_names, name);
}

Evoak *
evoak_object_evoak_get(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return NULL;
   if (o->delete_me) return NULL;
   return o->evoak;
}

void
evoak_object_pass_events_set(Evoak_Object *o, Evoak_Bool onoff)
{
   unsigned char *d;
   int s; 
   Evoak_PR_Object_Event_Prop_Set p;

   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->delete_me) return;
   if (((o->pass_events) && (onoff)) ||
       ((!o->pass_events) && (!onoff))) return;
   o->pass_events = onoff;
   p.property = EVOAK_PROPERTY_PASS;
   p.value = onoff;
   d = _evoak_proto[EVOAK_PR_OBJECT_EVENT_PROP_SET].enc(&p, &s);
   if (d)
     {
	_evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_EVENT_PROP_SET, o->id, 0, d, s);
	free(d);
     }
}

Evoak_Bool
evoak_object_pass_events_get(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return 0;
   if (o->delete_me) return 0;
   return o->pass_events;
}

void
evoak_object_repeat_events_set(Evoak_Object *o, Evoak_Bool onoff)
{
   unsigned char *d;
   int s;
   Evoak_PR_Object_Event_Prop_Set p;

   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->delete_me) return;
   if (((o->repeat_events) && (onoff)) ||
       ((!o->repeat_events) && (!onoff))) return;
   o->repeat_events = onoff;
   p.property = EVOAK_PROPERTY_REPEAT;
   p.value = onoff;
   d = _evoak_proto[EVOAK_PR_OBJECT_EVENT_PROP_SET].enc(&p, &s);
   if (d)
     {
	_evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_EVENT_PROP_SET, o->id, 0, d, s);
	free(d);
     }
}

Evoak_Bool
evoak_object_repeat_events_get(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return 0;
   if (o->delete_me) return 0;
   return o->repeat_events;
}

void
evoak_object_event_callback_add(Evoak_Object *o, Evoak_Callback_Type type, void (*func) (void *data, Evoak *e, Evoak_Object *obj, void *event_info), const void *data)
{
   Evoak_Callback *cb;
   
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->delete_me) return;
   cb = calloc(1, sizeof(Evoak_Callback));
   if (cb)
     {
	int changed = 0;
	
	cb->type = type;
	cb->func = func;
	cb->data = (void *)data;
	if (o->lock > 1) cb->just_added = 1;
	o->callbacks = evas_list_append(o->callbacks, cb);
	switch (type)
	  {
	   case EVOAK_CALLBACK_MOUSE_IN:
	     if (!(o->mask1 & EVOAK_EVENT_MASK1_MOUSE_IN)) changed = 1;
	     o->mask1 |= EVOAK_EVENT_MASK1_MOUSE_IN;
	     break;
	   case EVOAK_CALLBACK_MOUSE_OUT:
	     if (!(o->mask1 & EVOAK_EVENT_MASK1_MOUSE_OUT)) changed = 1;
	     o->mask1 |= EVOAK_EVENT_MASK1_MOUSE_OUT;
	     break;
	   case EVOAK_CALLBACK_MOUSE_DOWN:
	     if (!(o->mask1 & EVOAK_EVENT_MASK1_MOUSE_DOWN)) changed = 1;
	     o->mask1 |= EVOAK_EVENT_MASK1_MOUSE_DOWN;
	     break;
	   case EVOAK_CALLBACK_MOUSE_UP:
	     if (!(o->mask1 & EVOAK_EVENT_MASK1_MOUSE_UP)) changed = 1;
	     o->mask1 |= EVOAK_EVENT_MASK1_MOUSE_UP;
	     break;
	   case EVOAK_CALLBACK_MOUSE_MOVE:
	     if (!(o->mask1 & EVOAK_EVENT_MASK1_MOUSE_MOVE)) changed = 1;
	     o->mask1 |= EVOAK_EVENT_MASK1_MOUSE_MOVE;
	     break;
	   case EVOAK_CALLBACK_MOUSE_WHEEL:
	     if (!(o->mask1 & EVOAK_EVENT_MASK1_MOUSE_WHEEL)) changed = 1;
	     o->mask1 |= EVOAK_EVENT_MASK1_MOUSE_WHEEL;
	     break;
	   case EVOAK_CALLBACK_FREE:
	     if (!(o->mask1 & EVOAK_EVENT_MASK1_OBJ_FREED)) changed = 1;
	     o->mask1 |= EVOAK_EVENT_MASK1_OBJ_FREED;
	     break;
	   case EVOAK_CALLBACK_KEY_DOWN:
	     if (!(o->mask1 & EVOAK_EVENT_MASK1_KEY_DOWN)) changed = 1;
	     o->mask1 |= EVOAK_EVENT_MASK1_KEY_DOWN;
	     break;
	   case EVOAK_CALLBACK_KEY_UP:
	     if (!(o->mask1 & EVOAK_EVENT_MASK1_KEY_UP)) changed = 1;
	     o->mask1 |= EVOAK_EVENT_MASK1_KEY_UP;
	     break;
	   case EVOAK_CALLBACK_FOCUS_IN:
	     if (!(o->mask1 & EVOAK_EVENT_MASK1_OBJ_FOCUS_IN)) changed = 1;
	     o->mask1 |= EVOAK_EVENT_MASK1_OBJ_FOCUS_IN;
	     break;
	   case EVOAK_CALLBACK_FOCUS_OUT:
	     if (!(o->mask1 & EVOAK_EVENT_MASK1_OBJ_FOCUS_OUT)) changed = 1;
	     o->mask1 |= EVOAK_EVENT_MASK1_OBJ_FOCUS_OUT;
	     break;
	   case EVOAK_CALLBACK_SHOW:
	     if (!(o->mask1 & EVOAK_EVENT_MASK1_OBJ_VISIBLE_CHANGE)) changed = 1;
	     o->mask1 |= EVOAK_EVENT_MASK1_OBJ_VISIBLE_CHANGE;
	     break;
	   case EVOAK_CALLBACK_HIDE:
	     if (!(o->mask1 & EVOAK_EVENT_MASK1_OBJ_VISIBLE_CHANGE)) changed = 1;
	     o->mask1 |= EVOAK_EVENT_MASK1_OBJ_VISIBLE_CHANGE;
	     break;
	   case EVOAK_CALLBACK_MOVE:
	     if (!(o->mask1 & EVOAK_EVENT_MASK1_OBJ_MOVED)) changed = 1;
	     o->mask1 |= EVOAK_EVENT_MASK1_OBJ_MOVED;
	     break;
	   case EVOAK_CALLBACK_RESIZE:
	     if (!(o->mask1 & EVOAK_EVENT_MASK1_OBJ_RESIZED)) changed = 1;
	     o->mask1 |= EVOAK_EVENT_MASK1_OBJ_RESIZED;
	     break;
	   case EVOAK_CALLBACK_RESTACK:
	     if (!(o->mask1 & EVOAK_EVENT_MASK1_OBJ_RESTACKED)) changed = 1;
	     o->mask1 |= EVOAK_EVENT_MASK1_OBJ_RESTACKED;
	     break;
	   default:
	     break;
	  }
	if (changed)
	  {
	     unsigned char *d;
	     int s;
	     Evoak_PR_Object_Event_Mask_Set p;
	     
	     p.mask1 = o->mask1;
	     p.mask2 = o->mask2;
	     d = _evoak_proto[EVOAK_PR_OBJECT_EVENT_MASK_SET].enc(&p, &s);
	     if (d)
	       {
		  _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_EVENT_MASK_SET, o->id, 0, d, s);
		  free(d);
	       }
	  }
     }
}

void *
evoak_object_event_callback_del(Evoak_Object *o, Evoak_Callback_Type type, void (*func) (void *data, Evoak *e, Evoak_Object *obj, void *event_info))
{
   Evas_List *l;
   
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return NULL;
   if (o->delete_me) return NULL;
   for (l = o->callbacks; l; l = l->next)
     {
	Evoak_Callback *cb;
	
	cb = l->data;
	if ((cb->type == type) && (cb->func == func))
	  {
	     void *data;
	     int changed = 0;
	     
	     if (o->lock > 0)
	       {
		  o->delete_cb = 1;
		  cb->delete_me = 1;
	       }
	     else
	       {
		  o->callbacks = evas_list_remove_list(o->callbacks, l);
		  data = cb->data;
		  free(cb);
	       }
	     switch (type)
	       {
		case EVOAK_CALLBACK_MOUSE_IN:
		  if ((o->mask1 & EVOAK_EVENT_MASK1_MOUSE_IN)) changed = 1;
		  o->mask1 &= ~EVOAK_EVENT_MASK1_MOUSE_IN;
		  break;
		case EVOAK_CALLBACK_MOUSE_OUT:
		  if ((o->mask1 & EVOAK_EVENT_MASK1_MOUSE_OUT)) changed = 1;
		  o->mask1 &= ~EVOAK_EVENT_MASK1_MOUSE_OUT;
		  break;
		case EVOAK_CALLBACK_MOUSE_DOWN:
		  if ((o->mask1 & EVOAK_EVENT_MASK1_MOUSE_DOWN)) changed = 1;
		  o->mask1 &= ~EVOAK_EVENT_MASK1_MOUSE_DOWN;
		  break;
		case EVOAK_CALLBACK_MOUSE_UP:
		  if ((o->mask1 & EVOAK_EVENT_MASK1_MOUSE_UP)) changed = 1;
		  o->mask1 &= ~EVOAK_EVENT_MASK1_MOUSE_UP;
		  break;
		case EVOAK_CALLBACK_MOUSE_MOVE:
		  if ((o->mask1 & EVOAK_EVENT_MASK1_MOUSE_MOVE)) changed = 1;
		  o->mask1 &= ~EVOAK_EVENT_MASK1_MOUSE_MOVE;
		  break;
		case EVOAK_CALLBACK_MOUSE_WHEEL:
		  if ((o->mask1 & EVOAK_EVENT_MASK1_MOUSE_WHEEL)) changed = 1;
		  o->mask1 &= ~EVOAK_EVENT_MASK1_MOUSE_WHEEL;
		  break;
		case EVOAK_CALLBACK_FREE:
		  if ((o->mask1 & EVOAK_EVENT_MASK1_OBJ_FREED)) changed = 1;
		  o->mask1 &= ~EVOAK_EVENT_MASK1_OBJ_FREED;
		  break;
		case EVOAK_CALLBACK_KEY_DOWN:
		  if ((o->mask1 & EVOAK_EVENT_MASK1_KEY_DOWN)) changed = 1;
		  o->mask1 &= ~EVOAK_EVENT_MASK1_KEY_DOWN;
		  break;
		case EVOAK_CALLBACK_KEY_UP:
		  if ((o->mask1 & EVOAK_EVENT_MASK1_KEY_UP)) changed = 1;
		  o->mask1 &= ~EVOAK_EVENT_MASK1_KEY_UP;
		  break;
		case EVOAK_CALLBACK_FOCUS_IN:
		  if ((o->mask1 & EVOAK_EVENT_MASK1_OBJ_FOCUS_IN)) changed = 1;
		  o->mask1 &= ~EVOAK_EVENT_MASK1_OBJ_FOCUS_IN;
		  break;
		case EVOAK_CALLBACK_FOCUS_OUT:
		  if ((o->mask1 & EVOAK_EVENT_MASK1_OBJ_FOCUS_OUT)) changed = 1;
		  o->mask1 &= ~EVOAK_EVENT_MASK1_OBJ_FOCUS_OUT;
		  break;
		case EVOAK_CALLBACK_SHOW:
		  if ((o->mask1 & EVOAK_EVENT_MASK1_OBJ_VISIBLE_CHANGE)) changed = 1;
		  o->mask1 &= ~EVOAK_EVENT_MASK1_OBJ_VISIBLE_CHANGE;
		  break;
		case EVOAK_CALLBACK_HIDE:
		  if ((o->mask1 & EVOAK_EVENT_MASK1_OBJ_VISIBLE_CHANGE)) changed = 1;
		  o->mask1 &= ~EVOAK_EVENT_MASK1_OBJ_VISIBLE_CHANGE;
		  break;
		case EVOAK_CALLBACK_MOVE:
		  if ((o->mask1 & EVOAK_EVENT_MASK1_OBJ_MOVED)) changed = 1;
		  o->mask1 &= ~EVOAK_EVENT_MASK1_OBJ_MOVED;
		  break;
		case EVOAK_CALLBACK_RESIZE:
		  if ((o->mask1 & EVOAK_EVENT_MASK1_OBJ_RESIZED)) changed = 1;
		  o->mask1 &= ~EVOAK_EVENT_MASK1_OBJ_RESIZED;
		  break;
		case EVOAK_CALLBACK_RESTACK:
		  if ((o->mask1 & EVOAK_EVENT_MASK1_OBJ_RESTACKED)) changed = 1;
		  o->mask1 &= ~EVOAK_EVENT_MASK1_OBJ_RESTACKED;
		  break;
		default:
		  break;
	       }
	     if (changed)
	       {
		  unsigned char *d;
		  int s;
		  Evoak_PR_Object_Event_Mask_Set p;
		  
		  p.mask1 = o->mask1;
		  p.mask2 = o->mask2;
		  d = _evoak_proto[EVOAK_PR_OBJECT_EVENT_MASK_SET].enc(&p, &s);
		  if (d)
		    {
		       _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_EVENT_MASK_SET, o->id, 0, d, s);
		       free(d);
		    }
	       }
	     return data;
	  }
     }
   return NULL;
}

void
evoak_object_focus_set(Evoak_Object *o, Evoak_Bool onoff)
{
   unsigned char *d;
   int s; 
   Evoak_PR_Object_Focus_Set p;

   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->delete_me) return;
   if (((o->focused) && (onoff)) ||
       ((!o->focused) && (!onoff))) return;
   o->focused = onoff;
   p.onoff = onoff;
   d = _evoak_proto[EVOAK_PR_OBJECT_FOCUS_SET].enc(&p, &s);
   if (d)
     {
	_evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_FOCUS_SET, o->id, 0, d, s);
	free(d);
     }
}

Evoak_Bool
evoak_object_focus_get(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return 0;
   if (o->delete_me) return 0;
   return o->focused;
}

/*
 * Create a new rectangle object
 * @param ev The evoak server connection on whhich to create the object
 * @return A new object handle
 * 
 * This function will create a new rectangle object on the server and return a
 * pointer to a local object structure that will be used to address the
 * remotely created object. On failure a NULL pointer is returned.
 */
Evoak_Object *
evoak_object_rectangle_add(Evoak *ev)
{
   unsigned char *d;
   int s;
   Evoak_PR_Object_Add p;
   Evoak_Object *o;

   if ((!ev) || (ev->magic != EVOAK_MAGIC)) return NULL;
   o = calloc(1, sizeof(Evoak_Object));
   if (!o) return NULL;
   o->evoak = ev;
   o->magic = EVOAK_OBJECT_MAGIC;
   o->magic2 = EVOAK_OBJECT_RECTANGLE_MAGIC;
   o->type = "rectangle";
   o->r = 255; o->g = 255; o->b = 255; o->a = 255;
   p.type = EVOAK_OBJECT_TYPE_RECTANGLE;
   o->id = ev->object_id;
   _evoak_object_id_inc(ev);
   d = _evoak_proto[EVOAK_PR_OBJECT_ADD].enc(&p, &s);
   if (d)
     {
	_evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_ADD, o->id, 0, d, s);
	free(d);
     }
   _evoak_hash_store(&(ev->object_hash), o->id, o);
   _evoak_object_layer_add(o);
   _evoak_object_ref(o);
   return o;
}

static void *
_evoak_object_image_meta_new(Evoak_Object *o)
{
   Evoak_Meta_Image *m;
   
   m = calloc(1, sizeof(Evoak_Meta_Image));
   if (!m) return NULL;
   m->smooth_scale = 1;
   m->has_alpha = 1;
   return m;
}

static void
_evoak_object_image_meta_free(void *meta)
{
   Evoak_Meta_Image *m;
   
   m = meta;
   if (m->file) free(m->file);
   if (m->key) free(m->key);
   free(m);
}

/*
 * Create a new image object
 * @param ev The evoak server connection on whhich to create the object
 * @return A new object handle
 * 
 * This function will create a new image object on the server and return a
 * pointer to a local object structure that will be used to address the
 * remotely created object. On failure a NULL pointer is returned.
 */
Evoak_Object *
evoak_object_image_add(Evoak *ev)
{
   unsigned char *d;
   int s;
   Evoak_PR_Object_Add p;
   Evoak_Object *o;

   if ((!ev) || (ev->magic != EVOAK_MAGIC)) return NULL;
   o = calloc(1, sizeof(Evoak_Object));
   if (!o) return NULL;
   o->evoak = ev;
   o->magic = EVOAK_OBJECT_MAGIC;
   o->magic2 = EVOAK_OBJECT_IMAGE_MAGIC;
   o->type = "image";
   o->r = 255; o->g = 255; o->b = 255; o->a = 255;
   p.type = EVOAK_OBJECT_TYPE_IMAGE;
   o->id = ev->object_id;
   _evoak_object_id_inc(ev);
   d = _evoak_proto[EVOAK_PR_OBJECT_ADD].enc(&p, &s);
   if (d)
     {
	_evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_ADD, o->id, 0, d, s);
	free(d);
     }
   _evoak_hash_store(&(ev->object_hash), o->id, o);
   _evoak_object_layer_add(o);
   _evoak_object_ref(o);
   o->meta = _evoak_object_image_meta_new(o);
   o->meta_free = _evoak_object_image_meta_free;
   return o;
}

void
evoak_object_image_file_set(Evoak_Object *o, const char *file, const char *key)
{
   unsigned char *d;
   int s;
   Evoak_PR_Object_Image_File_Set p;
   char buf[4096];

   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_IMAGE_MAGIC) return;
   if (o->delete_me) return;
   if (!file) file = "";
   if (!key) key = "";
   if (file[0] != '/')
     {
	static char wd[4096] ="";
	
	if (wd[0] == 0) getcwd(wd, 4096);
	snprintf(buf, sizeof(buf), "%s/%s", wd, file);
	file = buf;
     }
     {
	Evoak_Meta_Image *m;
	Evas_Imaging_Image *im;
	
	m = o->meta;
	if ((m->file) && (!strcmp(m->file, file)))
	  {
	     if ((m->key) && (!strcmp(m->key, key))) return;
	  }
	if (m->file) free(m->file);
	if (m->key) free(m->key);
	m->file = strdup(file);
	m->key = strdup(key);
	im = evas_imaging_image_load((char *)file, (char *)key);
	if (im)
	  {
	     int w, h;
	     
	     evas_imaging_image_size_get(im, &w, &h);
	     m->image.w = w;
	     m->image.h = h;
	     m->has_alpha = evas_imaging_image_alpha_get(im);
	     evas_imaging_image_free(im);
	  }
     }
   p.file = (char *)file;
   p.key = (char *)key;
   d = _evoak_proto[EVOAK_PR_OBJECT_IMAGE_FILE_SET].enc(&p, &s);
   if (d)
     {
	_evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_IMAGE_FILE_SET, o->id, 0, d, s);
	free(d);
     }
}

void
evoak_object_image_file_get(Evoak_Object *o, const char **file, const char **key)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_IMAGE_MAGIC) return;
   if (o->delete_me) return;
     {
	Evoak_Meta_Image *m;
	
	m = o->meta;
	if (file) *file = m->file;
	if (key) *key = m->key;
     }
}

void
evoak_object_image_fill_set(Evoak_Object *o, Evoak_Coord x, Evoak_Coord y, Evoak_Coord w, Evoak_Coord h)
{
   unsigned char *d;
   int s;

   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_IMAGE_MAGIC) return;
   if (o->delete_me) return;
   if (w < 1) w = 1;
   if (h < 1) h = 1;
     {
	Evoak_Meta_Image *m;
	
	m = o->meta;
	
	if ((m->fill.x == x) && (m->fill.y == y) &&
	    (m->fill.w == w) && (m->fill.h == h)) return;
	m->fill.x = x;
	m->fill.y = y;
	m->fill.w = w;
	m->fill.h = h;
     }
   if ((x == 0) && (y == 0) && (w == o->w) && (h == o->h))
     {
	_evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_IMAGE_FILL_ALL_SET, o->id, 0, NULL, 0);
	return;
     }
   if ((x == 0) && (y == 0) &&
       ((0 <= w) && (w < 256)) && ((0 <= h) && (h < 256)))
     {
	Evoak_PR_Object_Image_Fill_Size8_Set p;
	
	p.w = w;
	p.h = h;
	d = _evoak_proto[EVOAK_PR_OBJECT_IMAGE_FILL_SIZE8_SET].enc(&p, &s);
	if (d)
	  {
	     _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_IMAGE_FILL_SIZE8_SET, o->id, 0, d, s);
	     free(d);
	  }
	return;
     }
   if ((x == 0) && (y == 0) &&
       ((0 <= w) && (w < 32768)) && ((0 <= h) && (h < 32768)))
     {
	Evoak_PR_Object_Image_Fill_Size16_Set p;
	
	p.w = w;
	p.h = h;
	d = _evoak_proto[EVOAK_PR_OBJECT_IMAGE_FILL_SIZE16_SET].enc(&p, &s);
	if (d)
	  {
	     _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_IMAGE_FILL_SIZE16_SET, o->id, 0, d, s);
	     free(d);
	  }
	return;
     }
   if (((-128 <= x) && (x < 128)) && ((-128 <= y) && (y < 128)) &&
       ((0 <= w) && (w < 256)) && ((0 <= h) && (h < 256)))
     {
	Evoak_PR_Object_Image_Fill8_Set p;
	
	p.x = x;
	p.y = y;
	p.w = w;
	p.h = h;
	d = _evoak_proto[EVOAK_PR_OBJECT_IMAGE_FILL8_SET].enc(&p, &s);
	if (d)
	  {
	     _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_IMAGE_FILL8_SET, o->id, 0, d, s);
	     free(d);
	  }
	return;
     }
   if (((-32768 <= x) && (x < 32768)) && ((-32768 <= y) && (y < 32768)) &&
       ((0 <= w) && (w < 32768)) && ((0 <= h) && (h < 32768)))
     {
	Evoak_PR_Object_Image_Fill16_Set p;
	
	p.x = x;
	p.y = y;
	p.w = w;
	p.h = h;
	d = _evoak_proto[EVOAK_PR_OBJECT_IMAGE_FILL16_SET].enc(&p, &s);
	if (d)
	  {
	     _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_IMAGE_FILL16_SET, o->id, 0, d, s);
	     free(d);
	  }
	return;
     }
     {
	Evoak_PR_Object_Image_Fill_Set p;
	
	p.x = x;
	p.y = y;
	p.w = w;
	p.h = h;
	d = _evoak_proto[EVOAK_PR_OBJECT_IMAGE_FILL_SET].enc(&p, &s);
	if (d)
	  {
	     _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_IMAGE_FILL_SET, o->id, 0, d, s);
	     free(d);
	  }
     }
}
void
evoak_object_image_fill_get(Evoak_Object *o, Evoak_Coord *x, Evoak_Coord *y, Evoak_Coord *w, Evoak_Coord *h)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_IMAGE_MAGIC) return;
   if (o->delete_me) return;
     {
	Evoak_Meta_Image *m;
	
	m = o->meta;
	if (x) *x = m->fill.x;
	if (y) *y = m->fill.y;
	if (w) *w = m->fill.w;
	if (h) *h = m->fill.h;
     }
}

void
evoak_object_image_border_set(Evoak_Object *o, int l, int r, int t, int b)
{
   unsigned char *d;
   int s;
   Evoak_PR_Object_Image_Border_Set p;
   
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_IMAGE_MAGIC) return;
   if (o->delete_me) return;
   if (l < 0) l = 0;
   if (r < 0) r = 0;
   if (t < 0) t = 0;
   if (b < 0) b = 0;
     {
	Evoak_Meta_Image *m;
	
	m = o->meta;
	
	if ((m->border.l == l) && (m->border.r == r) &&
	    (m->border.t == t) && (m->border.b == b)) return;
	m->border.l = l;
	m->border.r = r;
	m->border.t = t;
	m->border.b = b;
     }
   p.l = l;
   p.r = r;
   p.t = t;
   p.b = b;
   d = _evoak_proto[EVOAK_PR_OBJECT_IMAGE_BORDER_SET].enc(&p, &s);
   if (d)
     {
	_evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_IMAGE_BORDER_SET, o->id, 0, d, s);
	free(d);
     }
}

void
evoak_object_image_border_get(Evoak_Object *o, int *l, int *r, int *t, int *b)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_IMAGE_MAGIC) return;
   if (o->delete_me) return;
     {
	Evoak_Meta_Image *m;
	
	m = o->meta;
	if (l) *l = m->border.l;
	if (r) *r = m->border.r;
	if (t) *t = m->border.t;
	if (b) *b = m->border.b;
     }
}

void
evoak_object_image_smooth_scale_set(Evoak_Object *o, Evoak_Bool onoff)
{
   unsigned char *d;
   int s;
   Evoak_PR_Object_Image_Smooth_Scale_Set p;
   
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_IMAGE_MAGIC) return;
   if (o->delete_me) return;
     {
	Evoak_Meta_Image *m;
	
	m = o->meta;
	
	if (((m->smooth_scale) && (onoff)) ||
	    ((!m->smooth_scale) && (!onoff))) return;
	m->smooth_scale = onoff;
     }
   p.onoff = onoff;
   d = _evoak_proto[EVOAK_PR_OBJECT_IMAGE_SMOOTH_SCALE_SET].enc(&p, &s);
   if (d)
     {
	_evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_IMAGE_SMOOTH_SCALE_SET, o->id, 0, d, s);
	free(d);
     }
}

Evoak_Bool
evoak_object_image_smooth_scale_get(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return 0;
   if (o->magic2 != EVOAK_OBJECT_IMAGE_MAGIC) return 0;
   if (o->delete_me) return 0;
     {
	Evoak_Meta_Image *m;
	
	m = o->meta;
	return m->smooth_scale;
     }
   return 0;
}

void
evoak_object_image_size_set(Evoak_Object *o, int w, int h)
{
   unsigned char *d;
   int s;
   Evoak_PR_Object_Image_Size_Set p;
   
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_IMAGE_MAGIC) return;
   if (w < 0) w = 0;
   if (h < 0) h = 0;
   if (o->delete_me) return;
     {
	Evoak_Meta_Image *m;
	
	m = o->meta;
	
	if ((m->image.w == w) && (m->image.h == h)) return;
	m->image.w = w;
	m->image.h = h;
     }
   p.w = w;
   p.h = h;
   d = _evoak_proto[EVOAK_PR_OBJECT_IMAGE_SIZE_SET].enc(&p, &s);
   if (d)
     {
	_evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_IMAGE_SIZE_SET, o->id, 0, d, s);
	free(d);
     }
}

void
evoak_object_image_size_get(Evoak_Object *o, int *w, int *h)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_IMAGE_MAGIC) return;
   if (o->delete_me) return;
     {
	Evoak_Meta_Image *m;
	
	m = o->meta;
	if (w) *w = m->image.w;
	if (h) *h = m->image.h;
     }
}

void
evoak_object_image_alpha_set(Evoak_Object *o, Evoak_Bool onoff)
{
   unsigned char *d;
   int s;
   Evoak_PR_Object_Image_Alpha_Set p;
   
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_IMAGE_MAGIC) return;
   if (o->delete_me) return;
     {
	Evoak_Meta_Image *m;
	
	m = o->meta;
	
	if (((m->has_alpha) && (onoff)) ||
	    ((!m->has_alpha) && (!onoff))) return;
	m->has_alpha = onoff;
     }
   p.onoff = onoff;
   d = _evoak_proto[EVOAK_PR_OBJECT_IMAGE_ALPHA_SET].enc(&p, &s);
   if (d)
     {
	_evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_IMAGE_ALPHA_SET, o->id, 0, d, s);
	free(d);
     }
}

Evoak_Bool
evoak_object_image_alpha_get(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return 0;
   if (o->magic2 != EVOAK_OBJECT_IMAGE_MAGIC) return 0;
   if (o->delete_me) return 0;
     {
	Evoak_Meta_Image *m;
	
	m = o->meta;
	return m->has_alpha;
     }
}

static void *
_evoak_object_line_meta_new(Evoak_Object *o)
{
   Evoak_Meta_Line *m;
   
   m = calloc(1, sizeof(Evoak_Meta_Line));
   if (!m) return NULL;
   return m;
}

static void
_evoak_object_line_meta_free(void *meta)
{
   Evoak_Meta_Line *m;
   
   m = meta;
   free(m);
}

/*
 * Create a new line object
 * @param ev The evoak server connection on whhich to create the object
 * @return A new object handle
 * 
 * This function will create a new line object on the server and return a
 * pointer to a local object structure that will be used to address the
 * remotely created object. On failure a NULL pointer is returned.
 */
Evoak_Object *
evoak_object_line_add(Evoak *ev)
{
   unsigned char *d;
   int s;
   Evoak_PR_Object_Add p;
   Evoak_Object *o;

   if ((!ev) || (ev->magic != EVOAK_MAGIC)) return NULL;
   o = calloc(1, sizeof(Evoak_Object));
   if (!o) return NULL;
   o->evoak = ev;
   o->magic = EVOAK_OBJECT_MAGIC;
   o->magic2 = EVOAK_OBJECT_LINE_MAGIC;
   o->type = "line";
   o->r = 255; o->g = 255; o->b = 255; o->a = 255;
   p.type = EVOAK_OBJECT_TYPE_LINE;
   o->id = ev->object_id;
   _evoak_object_id_inc(ev);
   d = _evoak_proto[EVOAK_PR_OBJECT_ADD].enc(&p, &s);
   if (d)
     {
	_evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_ADD, o->id, 0, d, s);
	free(d);
     }
   _evoak_hash_store(&(ev->object_hash), o->id, o);
   _evoak_object_layer_add(o);
   _evoak_object_ref(o);
   o->meta = _evoak_object_line_meta_new(o);
   o->meta_free = _evoak_object_line_meta_free;
   return o;
}

void
evoak_object_line_xy_set(Evoak_Object *o, Evoak_Coord x1, Evoak_Coord y1, Evoak_Coord x2, Evoak_Coord y2)
{
   unsigned char *d;
   int s;
   Evoak_PR_Object_Line_Set p;
   
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_LINE_MAGIC) return;
   if (o->delete_me) return;
     {
	Evoak_Meta_Line *m;
	
	m = o->meta;
	if ((x1 == m->x1) && (y1 == m->y1) &&
	    (x2 == m->x2) && (y2 == m->y2)) return;
     }
   p.x1 = x1;
   p.y1 = y1;
   p.x2 = x2;
   p.y2 = y2;
   if (x1 <= x2)
     {
	o->x = x1;
	o->w = (x2 - x1) + 1;
     }
   else
     {
	o->x = x2;
	o->w = (x1 - x2) + 1;
     }
   if (y1 <= y2)
     {
	o->y = y1;
	o->h = (y2 - y1) + 1;
     }
   else
     {
	o->y = y2;
	o->h = (y1 - y2) + 1;
     }
   d = _evoak_proto[EVOAK_PR_OBJECT_LINE_SET].enc(&p, &s);
   if (d)
     {
	_evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_LINE_SET, o->id, 0, d, s);
	free(d);
     }
}

void
evoak_object_line_xy_get(Evoak_Object *o, Evoak_Coord *x1, Evoak_Coord *y1, Evoak_Coord *x2, Evoak_Coord *y2)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_LINE_MAGIC) return;
   if (o->delete_me) return;
     {
	Evoak_Meta_Line *m;
	
	m = o->meta;
	if (x1) *x1 = m->x1;
	if (y1) *y1 = m->y1;
	if (x2) *x2 = m->x2;
	if (y2) *y2 = m->y2;
     }
}

static void *
_evoak_object_polygon_meta_new(Evoak_Object *o)
{
   Evoak_Meta_Polygon *m;
   
   m = calloc(1, sizeof(Evoak_Meta_Polygon));
   if (!m) return NULL;
   return m;
}

static void
_evoak_object_polygon_meta_free(void *meta)
{
   Evoak_Meta_Polygon *m;
   
   m = meta;
   while (m->points)
     {
	free(m->points->data);
	m->points = evas_list_remove(m->points, m->points->data);
     }
   free(m);
}

/*
 * Create a new polygon object
 * @param ev The evoak server connection on whhich to create the object
 * @return A new object handle
 * 
 * This function will create a new polygon object on the server and return a
 * pointer to a local object structure that will be used to address the
 * remotely created object. On failure a NULL pointer is returned.
 */
Evoak_Object *
evoak_object_polygon_add(Evoak *ev)
{
   unsigned char *d;
   int s;
   Evoak_PR_Object_Add p;
   Evoak_Object *o;

   if ((!ev) || (ev->magic != EVOAK_MAGIC)) return NULL;
   o = calloc(1, sizeof(Evoak_Object));
   if (!o) return NULL;
   o->evoak = ev;
   o->magic = EVOAK_OBJECT_MAGIC;
   o->magic2 = EVOAK_OBJECT_POLYGON_MAGIC;
   o->type = "polygon";
   o->r = 255; o->g = 255; o->b = 255; o->a = 255;
   p.type = EVOAK_OBJECT_TYPE_POLYGON;
   o->id = ev->object_id;
   _evoak_object_id_inc(ev);
   d = _evoak_proto[EVOAK_PR_OBJECT_ADD].enc(&p, &s);
   if (d)
     {
	_evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_ADD, o->id, 0, d, s);
	free(d);
     }
   _evoak_hash_store(&(ev->object_hash), o->id, o);
   _evoak_object_layer_add(o);
   _evoak_object_ref(o);
   o->meta = _evoak_object_polygon_meta_new(o);
   o->meta_free = _evoak_object_polygon_meta_free;
   return o;
}

void
evoak_object_polygon_point_add(Evoak_Object *o, Evoak_Coord x, Evoak_Coord y)
{
   unsigned char *d;
   int s;
   Evoak_PR_Object_Poly_Point_Add p;
   
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_POLYGON_MAGIC) return;
   if (o->delete_me) return;
     {
	Evoak_Meta_Polygon *m;
	
	m = o->meta;
	if (!m->points)
	  {
	     o->x = x;
	     o->y = y;
	     o->w = 1;
	     o->h = 1;
	  }
	  {
	     Evoak_Meta_Polygon_Point *pt;
	     
	     pt = calloc(1, sizeof(Evoak_Meta_Polygon_Point));
	     if (!pt) return;
	     pt->x = x;
	     pt->y = y;
	     m->points = evas_list_append(m->points, pt);
	     if (x < o->x)
	       {
		  o->w += (o->x - x);
		  o->x = x;
	       }
	     else if (x >= (o->x + o->w))
	       {
		  o->w = x - o->x + 1;
	       }
	     if (y < o->y)
	       {
		  o->h += (o->y - y);
		  o->y = y;
	       }
	     else if (y >= (o->y + o->h))
	       {
		  o->h = y - o->y + 1;
	       }
	  }
	
     }
   p.x = x;
   p.y = y;
   d = _evoak_proto[EVOAK_PR_OBJECT_POLY_POINT_ADD].enc(&p, &s);
   if (d)
     {
	_evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_POLY_POINT_ADD, o->id, 0, d, s);
	free(d);
     }
}

void
evoak_object_polygon_points_clear(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_POLYGON_MAGIC) return;
   if (o->delete_me) return;
     {
	Evoak_Meta_Polygon *m;
	
	m = o->meta;
	if (!m->points) return;
	while (m->points)
	  {
	     free(m->points->data);
	     m->points = evas_list_remove(m->points, m->points->data);
	  }
	o->x = 0;
	o->y = 0;
	o->w = 0;
	o->h = 0;
     }
   _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_POLY_POINT_CLEAR, o->id, 0, NULL, 0);
}

static void *
_evoak_object_gradient_meta_new(Evoak_Object *o)
{
   Evoak_Meta_Gradient *m;
   
   m = calloc(1, sizeof(Evoak_Meta_Gradient));
   if (!m) return NULL;
   return m;
}

static void
_evoak_object_gradient_meta_free(void *meta)
{
   Evoak_Meta_Gradient *m;
   
   m = meta;
   free(m);
}

/*
 * Create a new gradient object
 * @param ev The evoak server connection on whhich to create the object
 * @return A new object handle
 * 
 * This function will create a new gradient object on the server and return a
 * pointer to a local object structure that will be used to address the
 * remotely created object. On failure a NULL pointer is returned.
 */
Evoak_Object *
evoak_object_gradient_add(Evoak *ev)
{
   unsigned char *d;
   int s;
   Evoak_PR_Object_Add p;
   Evoak_Object *o;

   if ((!ev) || (ev->magic != EVOAK_MAGIC)) return NULL;
   o = calloc(1, sizeof(Evoak_Object));
   if (!o) return NULL;
   o->evoak = ev;
   o->magic = EVOAK_OBJECT_MAGIC;
   o->magic2 = EVOAK_OBJECT_GRADIENT_MAGIC;
   o->type = "gradient";
   o->r = 255; o->g = 255; o->b = 255; o->a = 255;
   p.type = EVOAK_OBJECT_TYPE_GRADIENT;
   o->id = ev->object_id;
   _evoak_object_id_inc(ev);
   d = _evoak_proto[EVOAK_PR_OBJECT_ADD].enc(&p, &s);
   if (d)
     {
	_evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_ADD, o->id, 0, d, s);
	free(d);
     }
   _evoak_hash_store(&(ev->object_hash), o->id, o);
   _evoak_object_layer_add(o);
   _evoak_object_ref(o);
   o->meta = _evoak_object_gradient_meta_new(o);
   o->meta_free = _evoak_object_gradient_meta_free;
   return o;
}

void
evoak_object_gradient_color_add(Evoak_Object *o, int r, int g, int b, int a, int distance)
{
   unsigned char *d;
   int s;
   Evoak_PR_Object_Grad_Color_Add p;
   
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_GRADIENT_MAGIC) return;
   if (o->delete_me) return;
   p.r = r;
   p.g = g;
   p.b = b;
   p.a = a;
   p.dist = distance;
   d = _evoak_proto[EVOAK_PR_OBJECT_GRAD_COLOR_ADD].enc(&p, &s);
   if (d)
     {
	_evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_GRAD_COLOR_ADD, o->id, 0, d, s);
	free(d);
    }
}

void
evoak_object_gradient_colors_clear(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_GRADIENT_MAGIC) return;
   if (o->delete_me) return;
   _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_GRAD_COLOR_CLEAR, o->id, 0, NULL, 0);
}

void
evoak_object_gradient_angle_set(Evoak_Object *o, Evoak_Angle angle)
{
   unsigned char *d;
   int s;
   Evoak_PR_Object_Grad_Angle_Set p;
   
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_GRADIENT_MAGIC) return;
   if (o->delete_me) return;
     {
	Evoak_Meta_Gradient *m;
	
	m = o->meta;
	if (m->angle == angle) return;
	m->angle = angle;
     }
   p.ang100 = angle * 100;
   d = _evoak_proto[EVOAK_PR_OBJECT_GRAD_ANGLE_SET].enc(&p, &s);
   if (d)
     {
	_evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_GRAD_ANGLE_SET, o->id, 0, d, s);
	free(d);
    }
}
  
Evoak_Angle
evoak_object_gradient_angle_get(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return 0;
   if (o->magic2 != EVOAK_OBJECT_GRADIENT_MAGIC) return 0;
   if (o->delete_me) return 0;
     {
	Evoak_Meta_Gradient *m;
	
	m = o->meta;
	return m->angle;
     }
   return 0;
}





typedef struct _Evoak_Font_Dir   Evoak_Font_Dir;
typedef struct _Evoak_Font       Evoak_Font;
typedef struct _Evoak_Font_Alias Evoak_Font_Alias;
   
struct _Evoak_Font_Dir
{
   Evas_Hash          *lookup;
   Evas_List          *fonts;
   Evas_List          *aliases;
   unsigned long long  dir_mod_time;
   unsigned long long  fonts_dir_mod_time;
   unsigned long long  fonts_alias_mod_time;
};
        
struct _Evoak_Font
{
   char     type;
   struct {
      char *prop[14];
   } x;
   struct {
      char *name;
   } simple;
   char    *path;
};

struct _Evoak_Font_Alias
{
   char      *alias;
   Evoak_Font *fn;
};
                            
/* font dir cache */
static Evas_Hash *_evoak_font_dirs = NULL;

static char               *_evoak_file_path_join (char *p1, char *p2);
static Evas_List          *_evoak_file_path_list (char *path, char *match, int match_case);
static unsigned long long  _evoak_file_modified_time(char *file);

static char *
_evoak_file_path_join(char *p1, char *p2)
{
   char *s;
   int l1, l2;
   
   l1 = strlen(p1);
   l2 = strlen(p2);
   s = malloc(l1 + 1 + l2 + 1);
   if (!s) return NULL;
   strcpy(s, p1);
   s[l1] = '/';
   strcpy(&(s[l1 + 1]), p2);
   return s;
}

static Evas_List *
_evoak_file_path_list(char *path, char *match, int match_case)
{
   Evas_List *files = NULL;
   DIR *dir;
   
   dir = opendir(path);
   if (!dir) return NULL;
     {
	struct dirent      *dp;
	int flags;
	
	flags = FNM_PATHNAME;
#ifdef FNM_CASEFOLD
	if (!match_case) flags |= FNM_CASEFOLD;
#else
# warning "Your libc does not provide case-insensitive matching!"
#endif
	while ((dp = readdir(dir)))
	  {
	     if ((!strcmp(dp->d_name, ".")) || (!strcmp(dp->d_name, "..")))
	       continue;
	     if (match)
	       {
		  if (fnmatch(match, dp->d_name, flags) == 0)
		    files = evas_list_append(files, strdup(dp->d_name));
	       }
	     else files = evas_list_append(files, strdup(dp->d_name));
	  }
	closedir(dir);
     }
   return files;   
}

static unsigned long long
_evoak_file_modified_time(char *file)
{
   struct stat st;
   
   if (stat(file, &st) < 0) return 0;
   if (st.st_ctime > st.st_mtime) return (unsigned long long)st.st_ctime;
   else return (unsigned long long)st.st_mtime;
   return 0;
}

/* private methods for font dir cache */
static char           *_evoak_font_cache_find(char *dir, char *font);
static Evoak_Font_Dir *_evoak_font_cache_dir_update(char *dir, Evoak_Font_Dir *fd);
static Evoak_Font     *_evoak_font_cache_font_find_x(Evoak_Font_Dir *fd, char *font);
static Evoak_Font     *_evoak_font_cache_font_find_file(Evoak_Font_Dir *fd, char *font);
static Evoak_Font     *_evoak_font_cache_font_find_alias(Evoak_Font_Dir *fd, char *font);
static Evoak_Font     *_evoak_font_cache_font_find(Evoak_Font_Dir *fd, char *font);
static Evoak_Font_Dir *_evoak_font_cache_dir_add(char *dir);
static void            _evoak_font_cache_dir_del(char *dir, Evoak_Font_Dir *fd);
static int             _evoak_font_string_parse(char *buffer, char dest[14][256]);

static Evoak_Bool _evoak_font_cache_dir_free_cb(Evas_Hash *hash, const char *key, void *data, void *fdata);
static Evoak_Bool
_evoak_font_cache_dir_free_cb(Evas_Hash *hash, const char *key, void *data, void *fdata)
{
   _evoak_font_cache_dir_del((char *) key, data);
   return 1;
}

/* FIXME: this needs to be accessed on evoak_shutdown */
static void
_evoak_font_dir_cache_free(void)
{
   if (!_evoak_font_dirs) return;
   evas_hash_foreach(_evoak_font_dirs, _evoak_font_cache_dir_free_cb, NULL);
   evas_hash_free(_evoak_font_dirs);
   _evoak_font_dirs = NULL;
}
          
static char *
_evoak_font_cache_find(char *dir, char *font)
{
   Evoak_Font_Dir *fd;
   
   fd = evas_hash_find(_evoak_font_dirs, dir);
   fd = _evoak_font_cache_dir_update(dir, fd);
   if (fd)
     {
	Evoak_Font *fn;
	
	fn = _evoak_font_cache_font_find(fd, font);
	if (fn) return fn->path;
     }
   return NULL;
}

static Evoak_Font_Dir *
_evoak_font_cache_dir_update(char *dir, Evoak_Font_Dir *fd)
{
   unsigned long long mt;
   char *tmp;
   
   if (fd)
     {
	mt = _evoak_file_modified_time(dir);
	if (mt != fd->dir_mod_time)
	  {
	     _evoak_font_cache_dir_del(dir, fd);
	     _evoak_font_dirs = evas_hash_del(_evoak_font_dirs, dir, fd);
	  }
	else
	  {
	     tmp = _evoak_file_path_join(dir, "fonts.dir");
	     if (tmp)
	       {
		  mt = _evoak_file_modified_time(tmp);
		  free(tmp);
		  if (mt != fd->fonts_dir_mod_time)
		    {
		       _evoak_font_cache_dir_del(dir, fd);
		       _evoak_font_dirs = evas_hash_del(_evoak_font_dirs, dir, fd);
		    }
		  else
		    {
		       tmp = _evoak_file_path_join(dir, "fonts.alias");
		       if (tmp)
			 {
			    mt = _evoak_file_modified_time(tmp);
			    free(tmp);
			 }
		       if (mt != fd->fonts_alias_mod_time)
			 {
			    _evoak_font_cache_dir_del(dir, fd);
			    _evoak_font_dirs = evas_hash_del(_evoak_font_dirs, dir, fd);
			 }
		       else
			 return fd;
		    }
	       }
	  }
     }
   return _evoak_font_cache_dir_add(dir);
}

static Evoak_Font *
_evoak_font_cache_font_find_x(Evoak_Font_Dir *fd, char *font)
{
   Evas_List *l;
   char font_prop[14][256];
   int num;
   
   num = _evoak_font_string_parse(font, font_prop);
   if (num != 14) return NULL;
   for (l = fd->fonts; l; l = l->next)
     {
	Evoak_Font *fn;
	
	fn = l->data;
	if (fn->type == 1)
	  {
	     int i;
	     int match = 0;
	     
	     for (i = 0; i < 14; i++)
	       {
		  if ((font_prop[i][0] == '*') && (font_prop[i][1] == 0))
		    match++;
		  else
		    {
		       if (!strcasecmp(font_prop[i], fn->x.prop[i])) match++;
		       else break;
		    }
	       }
	     if (match == 14) return fn;
	  }
     }
   return NULL;
}

static Evoak_Font *
_evoak_font_cache_font_find_file(Evoak_Font_Dir *fd, char *font)
{
   Evas_List *l;
   
   for (l = fd->fonts; l; l = l->next)
     {
	Evoak_Font *fn;
	
	fn = l->data;
	if (fn->type == 0)
	  {
	     if (!strcasecmp(font, fn->simple.name)) return fn;
	  }
     }
   return NULL;
}

static Evoak_Font *
_evoak_font_cache_font_find_alias(Evoak_Font_Dir *fd, char *font)
{
   Evas_List *l;
   
   for (l = fd->aliases; l; l = l->next)
     {
	Evoak_Font_Alias *fa;
	
	fa = l->data;
	if (!strcasecmp(fa->alias, font)) return fa->fn;
     }
   return NULL;
}

static Evoak_Font *
_evoak_font_cache_font_find(Evoak_Font_Dir *fd, char *font)
{
   Evoak_Font *fn;
   
   fn = evas_hash_find(fd->lookup, font);
   if (fn) return fn;
   fn = _evoak_font_cache_font_find_alias(fd, font);
   if (!fn) fn = _evoak_font_cache_font_find_x(fd, font);
   if (!fn) fn = _evoak_font_cache_font_find_file(fd, font);
   if (!fn) return NULL;
   fd->lookup = evas_hash_add(fd->lookup, font, fn);
   return fn;
}

static Evoak_Font_Dir *
_evoak_font_cache_dir_add(char *dir)
{
   Evoak_Font_Dir *fd;
   char *tmp;
   Evas_List *fdir;
   
   fd = calloc(1, sizeof(Evoak_Font_Dir));
   if (!fd) return NULL;
   _evoak_font_dirs = evas_hash_add(_evoak_font_dirs, dir, fd);
   
   /* READ fonts.alias, fonts.dir and directory listing */
   
   /* fonts.dir */
   tmp = _evoak_file_path_join(dir, "fonts.dir");
   if (tmp)
     {
	FILE *f;
	
	f = fopen(tmp, "r");
	if (f)
	  {
	     int num;
	     char fname[4096], fdef[4096];
	     
	     if (fscanf(f, "%i\n", &num) != 1) goto cant_read;
	     /* read font lines */
	     while (fscanf(f, "%4090s %[^\n]\n", fname, fdef) == 2)
	       {
		  char font_prop[14][256];
		  int i;
		  
		  /* skip comments */
		  if ((fdef[0] == '!') || (fdef[0] == '#')) continue;
		  /* parse font def */
		  num = _evoak_font_string_parse((char *)fdef, font_prop);
		  if (num == 14)
		    {
		       Evoak_Font *fn;
		       
		       fn = calloc(1, sizeof(Evoak_Font));
		       if (fn)
			 {
			    fn->type = 1;
			    for (i = 0; i < 14; i++)
			      {
				 fn->x.prop[i] = strdup(font_prop[i]);
				 /* FIXME: what if strdup fails! */
			      }
			    fn->path = _evoak_file_path_join(dir, fname);
			    /* FIXME; what is evas_file_path_join fails! */
			    fd->fonts = evas_list_append(fd->fonts, fn);
			 }
		    }
	       }
	     cant_read: ;
	     fclose(f);
	  }
	free(tmp);
     }
   
   /* directoy listing */
   fdir = _evoak_file_path_list(dir, "*.ttf", 0);
   while (fdir)
     {
	tmp = _evoak_file_path_join(dir, fdir->data);
	if (tmp)
	  {
	     Evoak_Font *fn;
	     
	     fn = calloc(1, sizeof(Evoak_Font));
	     if (fn)
	       {
		  fn->type = 0;
		  fn->simple.name = strdup(fdir->data);
		  if (fn->simple.name)
		    {
		       char *p;
		       
		       p = strrchr(fn->simple.name, '.');
		       if (p) *p = 0;
		    }
		  fn->path = _evoak_file_path_join(dir, fdir->data);
		  fd->fonts = evas_list_append(fd->fonts, fn);
	       }
	     free(tmp);
	  }
	free(fdir->data);
	fdir = evas_list_remove(fdir, fdir->data);
     }
   
   /* fonts.alias */
   tmp = _evoak_file_path_join(dir, "fonts.alias");
      if (tmp)
     {
	FILE *f;
	
	f = fopen(tmp, "r");
	if (f)
	  {
	     char fname[4096], fdef[4096];
	     
	     /* read font alias lines */
	     while (fscanf(f, "%4090s %[^\n]\n", fname, fdef) == 2)
	       {
		  Evoak_Font_Alias *fa;
		  
		  /* skip comments */
		  if ((fdef[0] == '!') || (fdef[0] == '#')) continue;
		  fa = calloc(1, sizeof(Evoak_Font_Alias));
		  if (fa)
		    {
		       fa->alias = strdup(fname);
		       fa->fn = _evoak_font_cache_font_find_x(fd, fdef);
		       if ((!fa->alias) || (!fa->fn))
			 {
			    if (fa->alias) free(fa->alias);
			    free(fa);
			 }
		       else
			 fd->aliases = evas_list_append(fd->aliases, fa);
		    }
	       }
	     fclose(f);
	  }
	free(tmp);
     }
   
   fd->dir_mod_time = _evoak_file_modified_time(dir);
   tmp = _evoak_file_path_join(dir, "fonts.dir");
   if (tmp)
     {
	fd->fonts_dir_mod_time = _evoak_file_modified_time(tmp);
	free(tmp);
     }
   tmp = _evoak_file_path_join(dir, "fonts.alias");
   if (tmp)
     {
	fd->fonts_alias_mod_time = _evoak_file_modified_time(tmp);
	free(tmp);
     }
   
   return fd;
}

static void
_evoak_font_cache_dir_del(char *dir, Evoak_Font_Dir *fd)
{
   if (fd->lookup) evas_hash_free(fd->lookup);
   while (fd->fonts)
     {
	Evoak_Font *fn;
	int i;
	
	fn = fd->fonts->data;
	fd->fonts = evas_list_remove(fd->fonts, fn);
	for (i = 0; i < 14; i++)
	  {
	     if (fn->x.prop[i]) free(fn->x.prop[i]);
	  }
	if (fn->simple.name) free(fn->simple.name);
	if (fn->path) free(fn->path);
	free(fn);
     }
   while (fd->aliases)
     {
	Evoak_Font_Alias *fa;
	
	fa = fd->aliases->data;
	fd->aliases = evas_list_remove(fd->aliases, fa);
	if (fa->alias) free(fa->alias);
	free(fa);
     }
   free(fd);
}

static int
_evoak_font_string_parse(char *buffer, char dest[14][256])
{
   char *p;
   int n, m, i;
   
   n = 0;
   m = 0;
   p = buffer;
   if (p[0] != '-') return 0;
   i = 1;
   while (p[i])
     {
	dest[n][m] = p[i];
	if ((p[i] == '-') || (m == 256))
	  {
	     dest[n][m] = 0;
	     n++;
	     m = -1;
	  }
	i++;
	m++;
	if (n == 14) return n;
     }
   dest[n][m] = 0;
   n++;
   return n;
}




static void *
_evoak_object_text_meta_new(Evoak_Object *o)
{
   Evoak_Meta_Text *m;
   
   m = calloc(1, sizeof(Evoak_Meta_Text));
   if (!m) return NULL;
   m->text = strdup("");
   m->name = strdup("");
   return m;
}

static void
_evoak_object_text_meta_free(void *meta)
{
   Evoak_Meta_Text *m;
   
   m = meta;
   if (m->name) free(m->name);
   if (m->source) free(m->source);
   if (m->ffile) free(m->ffile);
   if (m->text) free(m->text);
   if (m->font) evas_imaging_font_free(m->font);
   free(m);
}

/*
 * Create a new text object
 * @param ev The evoak server connection on whhich to create the object
 * @return A new object handle
 * 
 * This function will create a new text object on the server and return a
 * pointer to a local object structure that will be used to address the
 * remotely created object. On failure a NULL pointer is returned.
 */
Evoak_Object *
evoak_object_text_add(Evoak *ev)
{
   unsigned char *d;
   int s;
   Evoak_PR_Object_Add p;
   Evoak_Object *o;

   if ((!ev) || (ev->magic != EVOAK_MAGIC)) return NULL;
   o = calloc(1, sizeof(Evoak_Object));
   if (!o) return NULL;
   o->evoak = ev;
   o->magic = EVOAK_OBJECT_MAGIC;
   o->magic2 = EVOAK_OBJECT_TEXT_MAGIC;
   o->type = "text";
   o->r = 255; o->g = 255; o->b = 255; o->a = 255;
   p.type = EVOAK_OBJECT_TYPE_TEXT;
   o->id = ev->object_id;
   _evoak_object_id_inc(ev);
   d = _evoak_proto[EVOAK_PR_OBJECT_ADD].enc(&p, &s);
   if (d)
     {
	_evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_ADD, o->id, 0, d, s);
	free(d);
     }
   _evoak_hash_store(&(ev->object_hash), o->id, o);
   _evoak_object_layer_add(o);
   _evoak_object_ref(o);
   o->meta = _evoak_object_text_meta_new(o);
   o->meta_free = _evoak_object_text_meta_free;
   return o;
}

void
evoak_object_text_font_source_set(Evoak_Object *o, const char *source)
{
   unsigned char *d;
   int s;
   Evoak_PR_Object_Text_Source_Set p;
   
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_TEXT_MAGIC) return;
   if (o->delete_me) return;
     {
	Evoak_Meta_Text *m;
	
	m = o->meta;
	if ((m->source) && (source) && (!strcmp(source, m->source))) return;
	if ((!source) && (!m->source)) return;
	if (m->source)
	  {
	     free(m->source);
	     m->source = NULL;
	  }
	if (source) m->source = strdup(source);
     }
   p.source = (char *)source;
   d = _evoak_proto[EVOAK_PR_OBJECT_TEXT_SOURCE_SET].enc(&p, &s);
   if (d)
     {
	_evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_TEXT_SOURCE_SET, o->id, 0, d, s);
	free(d);
    }
}

const char *
evoak_object_text_font_source_get(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return NULL;
   if (o->magic2 != EVOAK_OBJECT_TEXT_MAGIC) return NULL;
   if (o->delete_me) return NULL;
     {
	Evoak_Meta_Text *m;
	
	m = o->meta;
	return m->source;
     }
   return NULL;
}

void
evoak_object_text_font_set(Evoak_Object *o, const char *font, Evoak_Font_Size size)
{
   unsigned char *d;
   int s;
   Evoak_PR_Object_Text_Font_Set p;
   
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_TEXT_MAGIC) return;
   if (o->delete_me) return;
   if (size < 0) size = 0;
     {
	Evoak_Meta_Text *m;
	
	m = o->meta;
	if ((m->name) && (font) && (!strcmp(m->name, font)))
	  {
	     if (m->size == size) return;
	  }
	if ((!font) && (!m->name)) return;
	if (m->font)
	  {
	     evas_imaging_font_free(m->font);
	     m->font = NULL;
	  }
	if (font != m->name)
	  {
	     if (m->name)
	       {
		  free(m->name);
		  m->name = NULL;
	       }
	     if (font) m->name = strdup(font);
	  }
	if (m->ffile)
	  {
	     free(m->ffile);
	     m->ffile = NULL;
	  }
	m->size = size;
	if (m->name)
	  {
	     if (m->source)
	       {
		  m->font = evas_imaging_font_load(m->source, m->name, m->size);
	       }
	     if (!m->font)
	       {
		  if (m->name[0] == '/') /* FIXME: portable */
		    m->font = evas_imaging_font_load(m->name, NULL, m->size);
		  else
		    {
		       Evas_List *l;
		       
		       for (l = o->evoak->font_path; l; l = l->next)
			 {
			    char *f_file;
			    
			    f_file = _evoak_font_cache_find(l->data, m->name);
			    if (f_file)
			      {
				 m->font = evas_imaging_font_load(f_file, NULL, m->size);
				 m->ffile = strdup(f_file);
				 if (m->font) break;
			      }
			 }
		    }
	       }
	  }
	if (m->font)
	  {
	     if (m->text)
	       {
		  int w, h;
		  
		  evas_imaging_font_string_size_query(m->font, m->text, &w, &h);
		  o->w = w;
		  o->h = h;
	       }
	     else
	       {
		  int asc, desc;
		  
		  asc = evas_imaging_font_max_ascent_get(m->font);
		  desc = evas_imaging_font_max_descent_get(m->font);
		  o->w = 0;
		  o->h = asc + desc;
	       }	     
	  }
	else
	  {
	     o->w = 0;
	     o->h = 0;
	  }
	p.size = size;
	if (m->ffile) p.font = m->ffile;
	else if (font) p.font = (char *)font;
	else p.font = "";
	d = _evoak_proto[EVOAK_PR_OBJECT_TEXT_FONT_SET].enc(&p, &s);
	if (d)
	  {
	     _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_TEXT_FONT_SET, o->id, 0, d, s);
	     free(d);
	  }
     }
}

void
evoak_object_text_font_get(Evoak_Object *o, const char **font, Evoak_Font_Size *size)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_TEXT_MAGIC) return;
   if (o->delete_me) return;
     {
	Evoak_Meta_Text *m;
	
	m = o->meta;
	if (font) *font = m->name;
	if (size) *size = m->size;
     }
}

void
evoak_object_text_text_set(Evoak_Object *o, const char *text)
{
   unsigned char *d;
   int s;
   Evoak_PR_Object_Text_Text_Set p;
   
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_TEXT_MAGIC) return;
   if (o->delete_me) return;
     {
	Evoak_Meta_Text *m;
	
	m = o->meta;
	if (text == m->text) return;
	if ((text) && (m->text) && (!strcmp(m->text, text))) return;
	if ((!text) && (!m->text)) return;
	if (m->text)
	  {
	     free(m->text);
	     m->text = NULL;
	  }
	if (text) m->text = strdup(text);
	if (m->font)
	  {
	     if (m->text)
	       {
		  int w, h;
		  
		  evas_imaging_font_string_size_query(m->font, m->text, &w, &h);
		  o->w = w;
		  o->h = h;
	       }
	     else
	       {
		  int asc, desc;
		  
		  asc = evas_imaging_font_max_ascent_get(m->font);
		  desc = evas_imaging_font_max_descent_get(m->font);
		  o->w = 0;
		  o->h = asc + desc;
	       }	     
	  }
	else
	  {
	     o->w = 0;
	     o->h = 0;
	  }
	if (text) p.text = (char *)text;
	else p.text = "";
	d = _evoak_proto[EVOAK_PR_OBJECT_TEXT_TEXT_SET].enc(&p, &s);
	if (d)
	  {
	     _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_TEXT_TEXT_SET, o->id, 0, d, s);
	     free(d);
	  }
     }
}

const char *
evoak_object_text_text_get(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return NULL;
   if (o->magic2 != EVOAK_OBJECT_TEXT_MAGIC) return NULL;
   if (o->delete_me) return NULL;
     {
	Evoak_Meta_Text *m;
	
	m = o->meta;
	return m->text;
     }
   return NULL;
}

Evoak_Coord
evoak_object_text_ascent_get(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return 0;
   if (o->magic2 != EVOAK_OBJECT_TEXT_MAGIC) return 0;
   if (o->delete_me) return 0;
     {
	Evoak_Meta_Text *m;
	
	m = o->meta;
	if (m->font) return evas_imaging_font_ascent_get(m->font);
     }
   return 0;
}

Evoak_Coord
evoak_object_text_descent_get(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return 0;
   if (o->magic2 != EVOAK_OBJECT_TEXT_MAGIC) return 0;
   if (o->delete_me) return 0;
     {
	Evoak_Meta_Text *m;
	
	m = o->meta;
	if (m->font) return evas_imaging_font_descent_get(m->font);
     }
   return 0;
}

Evoak_Coord
evoak_object_text_max_ascent_get(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return 0;
   if (o->magic2 != EVOAK_OBJECT_TEXT_MAGIC) return 0;
   if (o->delete_me) return 0;
     {
	Evoak_Meta_Text *m;
	
	m = o->meta;
	if (m->font) return evas_imaging_font_max_ascent_get(m->font);
     }
   return 0;
}

Evoak_Coord
evoak_object_text_max_descent_get(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return 0;
   if (o->magic2 != EVOAK_OBJECT_TEXT_MAGIC) return 0;
   if (o->delete_me) return 0;
     {
	Evoak_Meta_Text *m;
	
	m = o->meta;
	if (m->font) return evas_imaging_font_max_descent_get(m->font);
     }
   return 0;
}

Evoak_Coord
evoak_object_text_horiz_advance_get(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return 0;
   if (o->magic2 != EVOAK_OBJECT_TEXT_MAGIC) return 0;
   if (o->delete_me) return 0;
     {
	Evoak_Meta_Text *m;
	
	m = o->meta;
	if ((m->font) && (m->text))
	  {
	     int x, y;
	     
	     evas_imaging_font_string_advance_get(m->font, m->text, &x, &y);
	     return x;
	  }
     }
   return 0;
}

Evoak_Coord
evoak_object_text_vert_advance_get(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return 0;
   if (o->magic2 != EVOAK_OBJECT_TEXT_MAGIC) return 0;
   if (o->delete_me) return 0;
     {
	Evoak_Meta_Text *m;
	
	m = o->meta;
	if ((m->font) && (m->text))
	  {
	     int x, y;
	     
	     evas_imaging_font_string_advance_get(m->font, m->text, &x, &y);
	     return y;
	  }
     }
   return 0;
}

Evoak_Coord
evoak_object_text_inset_get(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return 0;
   if (o->magic2 != EVOAK_OBJECT_TEXT_MAGIC) return 0;
   if (o->delete_me) return 0;
     {
	Evoak_Meta_Text *m;
	
	m = o->meta;
	if ((m->font) && (m->text))
	  return evas_imaging_font_string_inset_get(m->font, m->text);
     }
   return 0;
}

int
evoak_object_text_char_pos_get(Evoak_Object *o, int pos, Evoak_Coord *cx, Evoak_Coord *cy, Evoak_Coord *cw, Evoak_Coord *ch)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return 0;
   if (o->magic2 != EVOAK_OBJECT_TEXT_MAGIC) return 0;
   if (o->delete_me) return 0;
     {
	Evoak_Meta_Text *m;
	
	m = o->meta;
	if ((m->font) && (m->text))
	  {
	     int v;
	     int inset, max_ascent;
	     Evoak_Coord ccx, ccy, ccw, cch;
	     
	     inset = evas_imaging_font_string_inset_get(m->font, m->text);
	     max_ascent = evas_imaging_font_max_ascent_get(m->font);
	     v = evas_imaging_font_string_char_coords_get(m->font, m->text, pos, &ccx, &ccy, &ccw, &cch);
	     ccx -= inset;
	     ccy += max_ascent;
	     if (ccx < 0)
	       {
		  ccw += ccx;
		  ccx = 0;
	       }
	     if (ccx + ccw > o->w) ccw = o->w - ccx;
	     if (ccw < 0) ccw = 0;
	     if (ccy < 0)
	       {
		  cch += ccy;
		  ccy = 0;
	       }
	     if (ccy + cch > o->h) cch = o->h - ccy;
	     if (cch < 0) cch = 0;
	     if (cx) *cx = ccx;
	     if (cy) *cy = ccy;
	     if (cw) *cw = ccw;
	     if (ch) *ch = cch;
	     return v;
	  }
     }
   return 0;
}

int
evoak_object_text_char_coords_get(Evoak_Object *o, Evoak_Coord x, Evoak_Coord y, Evoak_Coord *cx, Evoak_Coord *cy, Evoak_Coord *cw, Evoak_Coord *ch)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return -1;
   if (o->magic2 != EVOAK_OBJECT_TEXT_MAGIC) return -1;
   if (o->delete_me) return -1;
     {
	Evoak_Meta_Text *m;
	
	m = o->meta;
	if ((m->font) && (m->text))
	  {
	     int v;
	     int inset, max_ascent;
	     Evoak_Coord ccx, ccy, ccw, cch;
	     
	     inset = evas_imaging_font_string_inset_get(m->font, m->text);
	     max_ascent = evas_imaging_font_max_ascent_get(m->font);
	     v = evas_imaging_font_string_char_at_coords_get(m->font, m->text, x + inset, y - max_ascent, &ccx, &ccy, &ccw, &cch);
	     ccx -= inset;
	     ccy += max_ascent;
	     if (ccx < 0)
	       {
		  ccw += ccx;
		  ccx = 0;
	       }
	     if (ccx + ccw > o->w) ccw = o->w - ccx;
	     if (ccw < 0) ccw = 0;
	     if (ccy < 0)
	       {
		  cch += ccy;
		  ccy = 0;
	       }
	     if (ccy + cch > o->h) cch = o->h - ccy;
	     if (cch < 0) cch = 0;
	     if (cx) *cx = ccx;
	     if (cy) *cy = ccy;
	     if (cw) *cw = ccw;
	     if (ch) *ch = cch;
	     return v;
	  }
     }
   return -1;
}

void
evoak_font_path_clear(Evoak *ev)
{
   if ((!ev) || (ev->magic != EVOAK_MAGIC)) return;
   while (ev->font_path)
     {
	free(ev->font_path->data);
	ev->font_path = evas_list_remove(ev->font_path, ev->font_path->data);
     }
}

void
evoak_font_path_append(Evoak *ev, const char *path)
{
   if ((!ev) || (ev->magic != EVOAK_MAGIC)) return;
   ev->font_path = evas_list_append(ev->font_path, strdup(path));
}

void
evoak_font_path_prepend(Evoak *ev, const char *path)
{
   if ((!ev) || (ev->magic != EVOAK_MAGIC)) return;
   ev->font_path = evas_list_prepend(ev->font_path, strdup(path));
}

const Evoak_List *
evoak_font_path_list(Evoak *ev)
{
   if ((!ev) || (ev->magic != EVOAK_MAGIC)) return NULL;
   return (Evoak_List *)ev->font_path;
}

int
evoak_string_char_next_get(const char *str, int pos, int *decoded)
{
   return evas_string_char_next_get(str, pos, decoded);
}

int
evoak_string_char_prev_get(const char *str, int pos, int *decoded)
{
   return evas_string_char_prev_get(str, pos, decoded);
}




static void _evoak_object_edje_file_load(Evoak_Meta_Edje *m);
static void _evoak_object_edje_file_unload(Evoak_Meta_Edje *m);
static void _evoak_object_edje_file_free(Edje_File *edf);
static void _evoak_object_edje_collection_free(Edje_Part_Collection *ec);
static void _evoak_object_edje_collection_free_part_description_free(Edje_Part_Description *desc);

extern Eet_Data_Descriptor *_edje_edd_edje_file;
extern Eet_Data_Descriptor *_edje_edd_edje_data;
extern Eet_Data_Descriptor *_edje_edd_edje_image_directory;
extern Eet_Data_Descriptor *_edje_edd_edje_image_directory_entry;
extern Eet_Data_Descriptor *_edje_edd_edje_program;
extern Eet_Data_Descriptor *_edje_edd_edje_program_target;
extern Eet_Data_Descriptor *_edje_edd_edje_program_after;
extern Eet_Data_Descriptor *_edje_edd_edje_part_collection_directory;
extern Eet_Data_Descriptor *_edje_edd_edje_part_collection_directory_entry;
extern Eet_Data_Descriptor *_edje_edd_edje_part_collection;
extern Eet_Data_Descriptor *_edje_edd_edje_part;
extern Eet_Data_Descriptor *_edje_edd_edje_part_description;
extern Eet_Data_Descriptor *_edje_edd_edje_part_image_id;

static Evas_Hash *_evoak_edje_file_hash = NULL;

static void
_evoak_object_edje_file_load(Evoak_Meta_Edje *m)
{
   Eet_File *ef = NULL;
   Evas_List *l;
   int id = -1;
   
   m->load_error = 0;
   ef = eet_open(m->file, EET_FILE_MODE_READ);
   if (!ef)
     {
	m->load_error = 1;
	return;
     }
   
   m->edje.file = eet_data_read(ef, _edje_edd_edje_file, "edje_file");
   if (!m->edje.file)
     {
	m->load_error = 1;
	goto out;
     }
   
   m->edje.file->references = 1;
   m->edje.file->path = strdup(m->file);
   if (!m->edje.file->collection_dir)
     {
	_evoak_object_edje_file_free(m->edje.file);
	m->edje.file = NULL;
	m->load_error = 1;
	goto out;
     }
   _evoak_edje_file_hash = 
     evas_hash_add(_evoak_edje_file_hash, 
		   m->edje.file->path, 
		   m->edje.file);
   m->edje.collection = 
     evas_hash_find(m->edje.file->collection_hash, m->group);
   if (m->edje.collection)
     m->edje.collection->references++;
   else
     {
	for (l = m->edje.file->collection_dir->entries; l; l = l->next)
	  {
	     Edje_Part_Collection_Directory_Entry *ce;
	     
	     ce = l->data;
	     if ((ce->entry) && (!strcmp(ce->entry, m->group)))
	       {
		  id = ce->id;
		  break;
	       }
	  }
	if (id >= 0)
	  {
	     char buf[256];
	     int  size;
	     void *data;
	     
	     snprintf(buf, sizeof(buf), "collections/%i", id);
	     if (!ef) ef = eet_open(m->file, EET_FILE_MODE_READ);
	     if (!ef)
	       {
		  m->edje.file->references--;
		  if (m->edje.file->references <= 0)
		    {
		       _evoak_edje_file_hash = 
			 evas_hash_del(_evoak_edje_file_hash, m->file, m->edje.file);
		       _evoak_object_edje_file_free(m->edje.file);
		    }
		  m->edje.file = NULL;
		  m->load_error = 1;
		  goto out;
	       }
	     m->edje.collection = 
	       eet_data_read(ef,
			     _edje_edd_edje_part_collection,
			     buf);
	     if (!m->edje.collection)
	       {
		  m->edje.file->references--;
		  if (m->edje.file->references <= 0)
		    {
		       _evoak_edje_file_hash = 
			 evas_hash_del(_evoak_edje_file_hash, m->file, m->edje.file);
		       _evoak_object_edje_file_free(m->edje.file);
		    }
		  m->edje.file = NULL;
		  m->load_error = 1;
		  goto out;
	       }
	     m->edje.collection->references = 1;
	     m->edje.file->collection_hash = 
	       evas_hash_add(m->edje.file->collection_hash, 
			     m->group, 
			     m->edje.collection);
	  }
	else
	  {
	     m->edje.file->references--;
	     if (m->edje.file->references <= 0)
	       {
		  _evoak_edje_file_hash = 
		    evas_hash_del(_evoak_edje_file_hash, m->file, m->edje.file);
		  _evoak_object_edje_file_free(m->edje.file);
	       }
	     m->edje.file = NULL;
	     m->load_error = 1;
	  }
     }
   out:
   if (ef) eet_close(ef);
   /* build part list shadow */
   if (m->edje.collection)
     {
	for (l = m->edje.collection->parts; l; l = l->next)
	  {
	     Edje_Part *ep;
	     Evoak_Meta_Edje_Part *evp;
	     
	     ep = l->data;
	     evp = calloc(1, sizeof(Evoak_Meta_Edje_Part));
	     evp->part = ep;
	     m->parts = evas_list_append(m->parts, evp);
	  }
     }
}

static void
_evoak_object_edje_file_unload(Evoak_Meta_Edje *m)
{
   while (m->parts)
     {
	Evoak_Meta_Edje_Part *evp;
	
	evp = m->parts->data;
	if (evp->swallow)
	  evoak_object_edje_part_unswallow(m->obj, evp->swallow);
	if (evp->text) free(evp->text);
	if (evp->state_name) free(evp->state_name);
	free(evp);
	m->parts = evas_list_remove_list(m->parts, m->parts);
     }
   if (m->edje.collection)
     {       
	m->edje.collection->references--;
	if (m->edje.collection->references <= 0)
	  {
	     m->edje.file->collection_hash = 
	       evas_hash_del(m->edje.file->collection_hash, 
			     m->group, 
			     m->edje.collection);
	     m->edje.collection->references--;
	     _evoak_object_edje_collection_free(m->edje.collection);
	  }
	m->edje.collection = NULL;
     }
   if (m->edje.file)
     {
	m->edje.file->references--;
	if (m->edje.file->references <= 0)
	  {
	     _evoak_edje_file_hash = 
	       evas_hash_del(_evoak_edje_file_hash, m->file, m->edje.file);
	     _evoak_object_edje_file_free(m->edje.file);
	  }
	m->edje.file = NULL;
     }
}

static void
_evoak_object_edje_file_free(Edje_File *edf)
{
   if (edf->path) free(edf->path);
   if (edf->image_dir)
     {
	while (edf->image_dir->entries)
	  {
	     Edje_Image_Directory_Entry *ie;
	     
	     ie = edf->image_dir->entries->data;
	     edf->image_dir->entries =
	       evas_list_remove(edf->image_dir->entries, ie);
	     if (ie->entry) free(ie->entry);
	     free(ie);
	  }
	free(edf->image_dir);
     }
   if (edf->collection_dir)
     {
	while (edf->collection_dir->entries)
	  {
	     Edje_Part_Collection_Directory_Entry *ce;
	     
	     ce = edf->collection_dir->entries->data;
	     edf->collection_dir->entries =
	       evas_list_remove(edf->collection_dir->entries, ce);
	     if (ce->entry) free(ce->entry);
	     free(ce);
	  }
	free(edf->collection_dir);
     }
   if (edf->collection_hash) evas_hash_free(edf->collection_hash);
   free(edf);
}

static void
_evoak_object_edje_collection_free(Edje_Part_Collection *ec)
{
   while (ec->programs)
     {
	Edje_Program *pr;
	
	pr = ec->programs->data;
	ec->programs = evas_list_remove(ec->programs, pr);
	if (pr->name) free(pr->name);
	if (pr->signal) free(pr->signal);
	if (pr->source) free(pr->source);
	if (pr->state) free(pr->state);
	if (pr->state2) free(pr->state2);
	while (pr->targets)
	  {
	     Edje_Program_Target *prt;
	     
	     prt = pr->targets->data;
	     pr->targets = evas_list_remove(pr->targets, prt);
	     free(prt);
	  }
	while (pr->after)
	  {
	     Edje_Program_After *pa;
	     
	     pa = pr->after->data;
	     pr->after = evas_list_remove(pr->after, pa);
	     free(pa);
	  }
	free(pr);
     }
   while (ec->parts)
     {
	Edje_Part *ep;
	
	ep = ec->parts->data;
	ec->parts = evas_list_remove(ec->parts, ep);
	if (ep->name) free(ep->name);
	if (ep->default_desc)
	  {
	     _evoak_object_edje_collection_free_part_description_free(ep->default_desc);
	     ep->default_desc = NULL;
	  }
	while (ep->other_desc)
	  {
	     Edje_Part_Description *desc;
	     
	     desc = ep->other_desc->data;
	     ep->other_desc = evas_list_remove(ep->other_desc, desc);
	     _evoak_object_edje_collection_free_part_description_free(desc);
	  }
	free(ep);
     }
#ifdef EDJE_PROGRAM_CACHE
   if (ec->prog_cache.no_matches) evas_hash_free(ec->prog_cache.no_matches);
   if (ec->prog_cache.matches)
     {
	evas_hash_foreach(ec->prog_cache.matches,
			  _edje_collection_free_prog_cache_matches_free_cb,
			  NULL);
	evas_hash_free(ec->prog_cache.matches);
     }
#endif
   free(ec);
}

static void
_evoak_object_edje_collection_free_part_description_free(Edje_Part_Description *desc)
{
   if (desc->state.name) free(desc->state.name);
   while (desc->image.tween_list)
     {
	Edje_Part_Image_Id *pi;
	
	pi = desc->image.tween_list->data;
	desc->image.tween_list = evas_list_remove(desc->image.tween_list, pi);
	free(pi);
     }
   if (desc->color_class)     free(desc->color_class);
   if (desc->text.text)       free(desc->text.text);
   if (desc->text.text_class) free(desc->text.text_class);
   if (desc->text.font)       free(desc->text.font);
   free(desc);
}

static void *
_evoak_object_edje_meta_new(Evoak_Object *o)
{
   Evoak_Meta_Edje *m;
   
   m = calloc(1, sizeof(Evoak_Meta_Edje));
   if (!m) return NULL;
   m->obj = o;
   return m;
}

static void
_evoak_object_edje_meta_free(void *meta)
{
   Evoak_Meta_Edje *m;
   
   m = meta;
   _evoak_object_edje_file_unload(m);
   if (m->file) free(m->file);
   if (m->group) free(m->group);
   while (m->callbacks)
     {
	Evoak_Meta_Edje_Callback *cb;
	
	cb = m->callbacks->data;
	free(cb->emission);
	free(cb->source);
	free(cb);
	m->callbacks = evas_list_remove(m->callbacks, m->callbacks->data);
     }   
   free(m);
}

/*
 * Create a new edje object
 * @param ev The evoak server connection on whhich to create the object
 * @return A new object handle
 * 
 * This function will create a new edje object on the server and return a
 * pointer to a local object structure that will be used to address the
 * remotely created object. On failure a NULL pointer is returned.
 */
Evoak_Object *
evoak_object_edje_add(Evoak *ev)
{
   unsigned char *d;
   int s;
   Evoak_PR_Object_Add p;
   Evoak_Object *o;

   if ((!ev) || (ev->magic != EVOAK_MAGIC)) return NULL;
   o = calloc(1, sizeof(Evoak_Object));
   if (!o) return NULL;
   o->evoak = ev;
   o->magic = EVOAK_OBJECT_MAGIC;
   o->magic2 = EVOAK_OBJECT_EDJE_MAGIC;
   o->type = "edje";
   o->r = 255; o->g = 255; o->b = 255; o->a = 255;
   p.type = EVOAK_OBJECT_TYPE_EDJE;
   o->id = ev->object_id;
   _evoak_object_id_inc(ev);
   d = _evoak_proto[EVOAK_PR_OBJECT_ADD].enc(&p, &s);
   if (d)
     {
	_evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_ADD, o->id, 0, d, s);
	free(d);
     }
   _evoak_hash_store(&(ev->object_hash), o->id, o);
   _evoak_object_layer_add(o);
   _evoak_object_ref(o);
   o->meta = _evoak_object_edje_meta_new(o);
   o->meta_free = _evoak_object_edje_meta_free;
   return o;
}

int
evoak_object_edje_file_set(Evoak_Object *o, const char *file, const char *part)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return 0;
   if (o->magic2 != EVOAK_OBJECT_EDJE_MAGIC) return 0;
   if (o->delete_me) return 0;
   if (!file) file = "";
   if (!part) part = "";
     {
	Evoak_Meta_Edje *m;
	
	m = o->meta;
	if ((file) && (m->file) && (!strcmp(m->file, file)))
	  {
	     if ((m->group) && (part) && (!strcmp(m->group, part))) return 1;
	  }
	if (m->file)
	  {
	     free(m->file);
	     m->file = NULL;
	  }
	if (m->group)
	  {
	     free(m->group);
	     m->group = NULL;
	  }
	if (file) m->file = strdup(file);
	if (part) m->group = strdup(part);
	_evoak_object_edje_file_unload(m);
	_evoak_object_edje_file_load(m);
	  {
	     unsigned char *d;
	     int s;
	     Evoak_PR_Object_Edje_File_Set p;
	     
	     p.file = (char *)file;
	     p.group = (char *)part;
	     d = _evoak_proto[EVOAK_PR_OBJECT_EDJE_FILE_SET].enc(&p, &s);
	     if (d)
	       {
		  _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_EDJE_FILE_SET, o->id, 0, d, s);
		  free(d);
	       }
	  }
	if (m->load_error) return 0;
	return 1;
     }
   return 0;
}

void
evoak_object_edje_file_get(Evoak_Object *o, const char **file, const char **part)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_EDJE_MAGIC) return;
   if (o->delete_me) return;
     {
	Evoak_Meta_Edje *m;
	
	m = o->meta;
	if (file) *file = m->file;
	if (part) *part = m->group;
     }
}

const char *
evoak_object_edje_data_get(Evoak_Object *o, const char *key)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return NULL;
   if (o->magic2 != EVOAK_OBJECT_EDJE_MAGIC) return NULL;
   if (o->delete_me) return NULL;
     {
	Evoak_Meta_Edje *m;
	Evas_List *l;
	
	m = o->meta;
	for (l = m->edje.collection->data; l; l = l->next)
	  {
	     Edje_Data *di;
	     
	     di = l->data;
	     if (!strcmp(di->key, key)) return (const char *)di->value;
	  }
     }
   return NULL;
}

void
evoak_object_edje_signal_callback_add(Evoak_Object *o, const char *emission, const char *source, void (*func) (void *data, Evoak_Object *o, const char *emission, const char *source), void *data)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_EDJE_MAGIC) return;
   if (o->delete_me) return;
   if ((!emission) || (!source)) return;
     {
	Evoak_Meta_Edje *m;
	Evoak_Meta_Edje_Callback *cb;
	
	m = o->meta;
	m->callback_id++;
	cb = calloc(1, sizeof(Evoak_Meta_Edje_Callback));
	if (!cb) return;
	cb->id = m->callback_id;
	cb->emission = strdup(emission);
	cb->source = strdup(source);
	cb->func = func;
	cb->data = data;
	m->callbacks = evas_list_append(m->callbacks, cb);
	  {
	     unsigned char *d;
	     int s;
	     Evoak_PR_Object_Edje_Signal_Listen p;
	     
	     p.callback_id = cb->id;
	     p.emission = (char *)emission;
	     p.source = (char *)source;
	     d = _evoak_proto[EVOAK_PR_OBJECT_EDJE_SIGNAL_LISTEN].enc(&p, &s);
	     if (d)
	       {
		  _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_EDJE_SIGNAL_LISTEN, o->id, 0, d, s);
		  free(d);
	       }
	  }
     }
}

void *
evoak_object_edje_signal_callback_del(Evoak_Object *o, const char *emission, const char *source, void (*func) (void *data, Evoak_Object *o, const char *emission, const char *source))
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return NULL;
   if (o->magic2 != EVOAK_OBJECT_EDJE_MAGIC) return NULL;
   if (o->delete_me) return NULL;
     {
	Evoak_Meta_Edje *m;
	Evas_List *l;
	
	m = o->meta;
	for (l = m->callbacks; l; l = l->next)
	  {
	     Evoak_Meta_Edje_Callback *cb;
	     
	     cb = l->data;
	     if (((((emission) && (!strcmp(cb->emission, emission))) || (!emission)) &&
		  (((source) && (!strcmp(cb->source, source))) || (!source))) &&
		 ((cb->func == func) || (!func)))
	       {
		  void *data;
		  
		    {
		       unsigned char *d;
		       int s;
		       Evoak_PR_Object_Edje_Signal_Unlisten p;
		       
		       p.callback_id = cb->id;
		       p.emission = (char *)emission;
		       p.source = (char *)source;
		       d = _evoak_proto[EVOAK_PR_OBJECT_EDJE_SIGNAL_UNLISTEN].enc(&p, &s);
		       if (d)
			 {
			    _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_EDJE_SIGNAL_UNLISTEN, o->id, 0, d, s);
			    free(d);
			 }
		    }
		  data = cb->data;
		  free(cb->emission);
		  free(cb->source);
		  free(cb);
		  m->callbacks = evas_list_remove_list(m->callbacks, l);
		  return data;
	       }
	  }
     }
   return NULL;
}

void
evoak_object_edje_signal_emit(Evoak_Object *o, const char *emission, const char *source)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_EDJE_MAGIC) return;
   if (o->delete_me) return;
   if ((!emission) || (!source)) return;
     {
	unsigned char *d;
	int s;
	Evoak_PR_Object_Edje_Signal_Emit p;
	
	p.emission = (char *)emission;
	p.source = (char *)source;
	d = _evoak_proto[EVOAK_PR_OBJECT_EDJE_SIGNAL_EMIT].enc(&p, &s);
	if (d)
	  {
	     _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_EDJE_SIGNAL_EMIT, o->id, 0, d, s);
	     free(d);
	  }
     }
}

int
evoak_object_edje_part_exists(Evoak_Object *o, const char *part)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return 0;
   if (o->magic2 != EVOAK_OBJECT_EDJE_MAGIC) return 0;
   if (o->delete_me) return 0;
   if (!part) return 0;
     {
	Evoak_Meta_Edje *m;
	Evas_List *l;
	
	m = o->meta;
	for (l = m->edje.collection->parts; l; l = l->next)
	  {
	     Edje_Part *ep;
	     
	     ep = l->data;
	     if (!strcmp(ep->name, part)) return 1;
	  }
     }
   return 0;
}

void
evoak_object_edje_part_swallow(Evoak_Object *o, const char *part, Evoak_Object *o_swallow)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_EDJE_MAGIC) return;
   if (o->delete_me) return;
   if ((!o_swallow) || (o_swallow->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o_swallow->delete_me) return;
     {
	Evas_List *l;
	Evoak_Meta_Edje *m;
	
	m = o->meta;
	for (l = m->parts; l; l = l->next)
	  {
	     Evoak_Meta_Edje_Part *evp;
	     
	     evp = l->data;
	     if (!strcmp(evp->part->name, part))
	       {
		  if (evp->swallow)
		    evoak_object_edje_part_unswallow(o, evp->swallow);
		  evp->swallow = o_swallow;
		    {
		       unsigned char *d;
		       int s;
		       Evoak_PR_Object_Edje_Swallow p;
		       
		       p.swallow_id = o_swallow->id;
		       p.part = (char *)part;
		       d = _evoak_proto[EVOAK_PR_OBJECT_EDJE_SWALLOW].enc(&p, &s);
		       if (d)
			 {
			    _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_EDJE_SWALLOW, o->id, 0, d, s);
			    free(d);
			 }
		    }
		  return;
	       }
	  }
     }
}

void
evoak_object_edje_part_unswallow(Evoak_Object *o, Evoak_Object *o_swallow)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_EDJE_MAGIC) return;
   if (o->delete_me) return;
   if ((!o_swallow) || (o_swallow->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o_swallow->delete_me) return;
     {
	Evas_List *l;
	Evoak_Meta_Edje *m;
	
	m = o->meta;
	for (l = m->parts; l; l = l->next)
	  {
	     Evoak_Meta_Edje_Part *evp;
	     
	     evp = l->data;
	     if (evp->swallow == o_swallow)
	       {
		  evp->swallow = NULL;
		    {
		       unsigned char *d;
		       int s;
		       Evoak_PR_Object_Edje_Unswallow p;
		       
		       p.swallow_id = o_swallow->id;
		       d = _evoak_proto[EVOAK_PR_OBJECT_EDJE_UNSWALLOW].enc(&p, &s);
		       if (d)
			 {
			    _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_EDJE_UNSWALLOW, o->id, 0, d, s);
			    free(d);
			 }
		    }
		  o->swallowees = evas_list_remove(o->swallowees, o_swallow);
		  o_swallow->swallow = NULL;
		  return;
	       }
	  }
     }
}

Evoak_Object *
evoak_object_edje_part_swallow_get(Evoak_Object *o, const char *part)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return NULL;
   if (o->magic2 != EVOAK_OBJECT_EDJE_MAGIC) return NULL;
   if (o->delete_me) return NULL;
     {
	Evas_List *l;
	Evoak_Meta_Edje *m;
	
	m = o->meta;
	for (l = m->parts; l; l = l->next)
	  {
	     Evoak_Meta_Edje_Part *evp;
	     
	     evp = l->data;
	     if (!strcmp(evp->part->name, part))
	       return evp->swallow;
	  }
     }
   return NULL;
}

void
evoak_object_edje_part_text_set(Evoak_Object *o, const char *part, const char *text)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_EDJE_MAGIC) return;
   if (o->delete_me) return;
   if ((!part) || (!text)) return;
     {
	Evas_List *l;
	Evoak_Meta_Edje *m;
	
	m = o->meta;
	for (l = m->parts; l; l = l->next)
	  {
	     Evoak_Meta_Edje_Part *evp;
	     
	     evp = l->data;
	     if (!strcmp(evp->part->name, part))
	       {
		    {
		       unsigned char *d;
		       int s;
		       Evoak_PR_Object_Edje_Text_Set p;
		       
		       p.part = (char *)part;
		       p.text = (char *)text;
		       d = _evoak_proto[EVOAK_PR_OBJECT_EDJE_TEXT_SET].enc(&p, &s);
		       if (d)
			 {
			    _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_EDJE_TEXT_SET, o->id, 0, d, s);
			    free(d);
			 }
		    }
		  if (evp->text) free(evp->text);
		  evp->text = strdup(text);
		  return;
	       }
	  }
     }
}

const char *
evoak_object_edje_part_text_get(Evoak_Object *o, const char *part)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return NULL;
   if (o->magic2 != EVOAK_OBJECT_EDJE_MAGIC) return NULL;
   if (o->delete_me) return NULL;
     {
	Evas_List *l;
	Evoak_Meta_Edje *m;
	
	m = o->meta;
	for (l = m->parts; l; l = l->next)
	  {
	     Evoak_Meta_Edje_Part *evp;
	     
	     evp = l->data;
	     if (!strcmp(evp->part->name, part))
	       return evp->text;
	  }
     }
   return NULL;
}
