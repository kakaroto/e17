#include "e_connman_private.h"
#include <string.h>
#include <errno.h>

static Eina_Hash *elements = NULL;

typedef struct _E_Connman_Array E_Connman_Array;
typedef struct _E_Connman_Element_Pending E_Connman_Element_Pending;
typedef struct _E_Connman_Element_Call_Data E_Connman_Element_Call_Data;
typedef struct _E_Connman_Element_Property E_Connman_Element_Property;
typedef struct _E_Connman_Element_Listener E_Connman_Element_Listener;

struct _E_Connman_Array
{
   int type;
   Eina_Array *array;
};

struct _E_Connman_Element_Pending
{
   EINA_INLIST;
   DBusPendingCall *pending;
   void *data;
   E_DBus_Method_Return_Cb user_cb;
   void *user_data;
};

struct _E_Connman_Element_Call_Data
{
   E_Connman_Element *element;
   E_DBus_Method_Return_Cb cb;
   E_Connman_Element_Pending *pending;
   Eina_Inlist **p_list;
};

struct _E_Connman_Element_Property
{
   EINA_INLIST;
   const char *name;
   int type;
   union {
      bool boolean;
      const char *str;
      unsigned short u16;
      unsigned int u32;
      unsigned char byte;
      const char *path;
      void *variant;
      E_Connman_Array *array;
   } value;
};

struct _E_Connman_Element_Listener
{
   EINA_INLIST;
   void (*cb)(void *data, const E_Connman_Element *element);
   void *data;
   void (*free_data)(void *data);
};


static void
_e_connman_element_event_no_free(void *data, void *ev)
{
   E_Connman_Element *element = ev;
   e_connman_element_unref(element);
}

static void
e_connman_element_event_add(int event_type, E_Connman_Element *element)
{
   e_connman_element_ref(element);
   ecore_event_add
     (event_type, element, _e_connman_element_event_no_free, element);
}

static void
e_connman_element_call_dispatch_and_free(void *d, DBusMessage *msg, DBusError *err)
{
   E_Connman_Element_Call_Data *data = d;
   E_Connman_Element_Pending *pending;

   pending = data->pending;
   pending->pending = NULL;

   if (data->cb)
     data->cb(data->element, msg, err);

   if (pending->user_cb)
     pending->user_cb(pending->user_data, msg, err);

   pending->data = NULL;
   *data->p_list = eina_inlist_remove(*data->p_list, EINA_INLIST_GET(pending));
   free(pending);
   free(data);
}

static void
e_connman_element_pending_cancel_and_free(Eina_Inlist **pending)
{
   while (*pending)
     {
	E_Connman_Element_Pending *p = (E_Connman_Element_Pending *)*pending;
	DBusError err;

	dbus_pending_call_cancel(p->pending);

	dbus_error_init(&err);
	dbus_set_error(&err, "Canceled", "Pending method call was canceled.");
	e_connman_element_call_dispatch_and_free(p->data, NULL, &err);
	dbus_error_free(&err);
     }
}

void
e_connman_element_listener_add(E_Connman_Element *element, void (*cb)(void *data, const E_Connman_Element *element), const void *data, void (*free_data)(void *data))
{
   E_Connman_Element_Listener *l;

   if (!element)
     {
	ERR("safety check failed: element == NULL\n");
	goto error;
     }
   if (!cb)
     {
	ERR("safety check failed: cb == NULL\n");
	goto error;
     }

   l = malloc(sizeof(*l));
   if (!l)
     {
	ERR("could not allocate E_Connman_Element_Listener\n");
	goto error;
     }

   l->cb = cb;
   l->data = (void *)data;
   l->free_data = free_data;

   element->_listeners = eina_inlist_append
     (element->_listeners, EINA_INLIST_GET(l));

   return;

 error:
   if (free_data)
     free_data((void *)data);
}

void
e_connman_element_listener_del(E_Connman_Element *element, void (*cb)(void *data, const E_Connman_Element *element), const void *data)
{
   E_Connman_Element_Listener *l;

   EINA_SAFETY_ON_NULL_RETURN(element);
   EINA_SAFETY_ON_NULL_RETURN(cb);

   EINA_INLIST_FOREACH(element->_listeners, l)
     if ((l->cb == cb) && (l->data == data))
       {
	  element->_listeners = eina_inlist_remove
	    (element->_listeners, EINA_INLIST_GET(l));
	  if (l->free_data) l->free_data(l->data);
	  free(l);
	  return;
       }
}

static void
_e_connman_element_listeners_call_do(E_Connman_Element *element)
{
   E_Connman_Element_Listener *l, **shadow;
   unsigned int i, count;

   /* NB: iterate on a copy in order to allow listeners to be deleted
    * from callbacks.  number of listeners should be small, so the
    * following should do fine.
    */
   count = eina_inlist_count(element->_listeners);
   if (count < 1)
     goto end;

   shadow = alloca(sizeof(*shadow) * count);
   if (!shadow)
     goto end;

   i = 0;
   EINA_INLIST_FOREACH(element->_listeners, l)
     shadow[i++] = l;

   for (i = 0; i < count; i++)
     shadow[i]->cb(shadow[i]->data, element);

 end:
   e_connman_element_event_add(E_CONNMAN_EVENT_ELEMENT_UPDATED, element);
}

static int
_e_connman_element_listeners_call_idler(void *data)
{
   E_Connman_Element *element = data;
   _e_connman_element_listeners_call_do(element);
   element->_idler.changed = NULL;
   return 0;
}

static void
_e_connman_element_listeners_call(E_Connman_Element *element)
{
   if (element->_idler.changed)
     return;
   element->_idler.changed = ecore_idler_add
     (_e_connman_element_listeners_call_idler, element);
}

/***********************************************************************
 * Property
 ***********************************************************************/

static void
_e_connman_element_array_free(E_Connman_Array *array, E_Connman_Array *new)
{
   Eina_Array_Iterator iterator;
   unsigned int i;
   void *item;

   if (!array)
     return;

   switch (array->type)
     {
      case DBUS_TYPE_OBJECT_PATH:
	 EINA_ARRAY_ITER_NEXT(array->array, i, item, iterator)
	   eina_stringshare_del(item);
	 break;
      case DBUS_TYPE_STRING:
	 EINA_ARRAY_ITER_NEXT(array->array, i, item, iterator)
	   eina_stringshare_del(item);
	 break;
      default:
	 break;
     }
   eina_array_free(array->array);
   free(array);
}

static void
_e_connman_element_property_value_free(E_Connman_Element_Property *property)
{
   switch (property->type)
     {
      case 0:
	 return;
      case DBUS_TYPE_BOOLEAN:
      case DBUS_TYPE_BYTE:
      case DBUS_TYPE_UINT16:
      case DBUS_TYPE_UINT32:
	 break;
      case DBUS_TYPE_STRING:
	 eina_stringshare_del(property->value.str);
	 break;
      case DBUS_TYPE_OBJECT_PATH:
	 eina_stringshare_del(property->value.path);
	 break;
      case DBUS_TYPE_ARRAY:
	 _e_connman_element_array_free(property->value.array, NULL);
	 break;
      default:
	 ERR("don't know how to free value of property type %c (%d)\n",
	     property->type, property->type);
     }
}

static const char *
_e_connman_element_get_interface(const char *key)
{
   const char *interface = NULL, *tail;
   char head;

   head = key[0];
   tail = key + 1;

   switch (head)
     {
      case 'P':
	 if (strcmp(tail, "rofiles") == 0)
	   interface = e_connman_iface_profile;
	 break;
      case 'D':
	 if (strcmp(tail, "evices") == 0)
	   interface = e_connman_iface_device;
	 break;
      case 'N':
	 if (strcmp(tail, "etworks") == 0)
	   interface = e_connman_iface_network;
	 break;
      case 'C':
	 if (strcmp(tail, "onnections") == 0)
	   interface = e_connman_iface_connection;
	 break;
      default:
	 break;
     }

   if (!interface)
     ERR("connman reported unknown interface: %s\n", key);

   return interface;
}

static void
_e_connman_element_item_register(const char *key, const char *item)
{
   E_Connman_Element *element;
   const char *interface;

   interface = _e_connman_element_get_interface(key);
   if (!interface)
     return;
   element = e_connman_element_register(item, interface);
   if ((element) && (!e_connman_element_properties_sync(element)))
     WRN("could not get properties of %s\n", element->path);
}

static void
_e_connman_element_objects_array_register(E_Connman_Array *array, const char *name)
{
   Eina_Array_Iterator iterator;
   unsigned int i;
   void *item;

   if (array->type != DBUS_TYPE_OBJECT_PATH)
     return;
   EINA_ARRAY_ITER_NEXT(array->array, i, item, iterator)
     _e_connman_element_item_register(name, item);
}

static bool
_e_connman_element_property_update(E_Connman_Element_Property *property, int type, void *data)
{
   int changed = 0;

   if ((type == DBUS_TYPE_STRING || type == DBUS_TYPE_OBJECT_PATH) && data)
     data = (char *)eina_stringshare_add(data);

   if (property->type != type)
     {
	if (property->type)
	  DBG("property type changed from '%c' to '%c'\n",
	      property->type, type);
	_e_connman_element_property_value_free(property);
	memset(&property->value, 0, sizeof(property->value));
	property->type = type;
	changed = 1;
     }

   switch (type)
     {
      case DBUS_TYPE_BOOLEAN:
	 if (changed || property->value.boolean != (bool)(long)data)
	   {
	      property->value.boolean = (bool)(long)data;
	      changed = 1;
	   }
	 break;
      case DBUS_TYPE_BYTE:
	 if (changed || property->value.byte != (unsigned char)(long)data)
	   {
	      property->value.byte = (unsigned char)(long)data;
	      changed = 1;
	   }
      case DBUS_TYPE_UINT16:
	 if (changed || property->value.u16 != (unsigned short)(long)data)
	   {
	      property->value.u16 = (unsigned short)(long)data;
	      changed = 1;
	   }
	 break;
      case DBUS_TYPE_UINT32:
	 if (changed || property->value.u32 != (unsigned int)(long)data)
	   {
	      property->value.u32 = (unsigned int)(long)data;
	      changed = 1;
	   }
	 break;
      case DBUS_TYPE_STRING:
	 if (changed)
	   property->value.str = data;
	 else
	   {
	      if (property->value.str)
		eina_stringshare_del(property->value.str);
	      if (property->value.str != data)
		{
		   property->value.str = data;
		   changed = 1;
		}
	   }
	 break;
      case DBUS_TYPE_OBJECT_PATH:
	 if (changed)
	   property->value.path = data;
	 else
	   {
	      if (property->value.path)
		eina_stringshare_del(property->value.path);
	      if (property->value.path != data)
		{
		   property->value.path = data;
		   changed = 1;
		}
	   }
	 break;
      case DBUS_TYPE_ARRAY:
	 if (!changed)
	   if (property->value.array)
	     {
		_e_connman_element_array_free(property->value.array, data);
		_e_connman_element_objects_array_register(data, property->name);
	     }
	 property->value.array = data;
	 changed = 1;
	 break;
      default:
	 ERR("don't know how to update property type %c (%d)\n", type, type);
     }

   return changed;
}

static E_Connman_Element_Property *
_e_connman_element_property_new(const char *name, int type, void *data)
{
   E_Connman_Element_Property *property;

   property = calloc(1, sizeof(*property));
   if (!property)
     {
	eina_stringshare_del(name);
	ERR("could not allocate property: %s\n", strerror(errno));
	return NULL;
     }

   property->name = name;
   _e_connman_element_property_update(property, type, data);
   return property;
}

static void
_e_connman_element_property_free(E_Connman_Element_Property *property)
{
   _e_connman_element_property_value_free(property);
   eina_stringshare_del(property->name);
   free(property);
}

/***********************************************************************
 * Element
 ***********************************************************************/
unsigned char *
e_connman_element_bytes_array_get_stringshared(const E_Connman_Element *element, const char *property, unsigned int *count)
{
   Eina_Array_Iterator iterator;
   E_Connman_Array *array;
   unsigned char *ret, *p;
   unsigned int i;
   void *item;

   EINA_SAFETY_ON_NULL_RETURN_VAL(element, NULL);
   EINA_SAFETY_ON_NULL_RETURN_VAL(property, NULL);
   EINA_SAFETY_ON_NULL_RETURN_VAL(count, NULL);

   *count = 0;

   if (!e_connman_element_property_get_stringshared
       (element, property, NULL, &array))
     return NULL;

   if ((!array) || (!(array->array)))
     return NULL;

   *count = eina_array_count_get(array->array);
   ret = malloc(*count * sizeof(unsigned char));
   if (!ret)
     {
	ERR("could not allocate return array of %d bytes: %s\n",
	    *count, strerror(errno));
	return NULL;
     }
   p = ret;

   EINA_ARRAY_ITER_NEXT(array->array, i, item, iterator)
     {
	*p = (unsigned char)(long)item;
	p++;
     }
   return ret;
}

bool
e_connman_element_objects_array_get_stringshared(const E_Connman_Element *element, const char *property, unsigned int *count, E_Connman_Element ***elements)
{
   E_Connman_Element **ret, **p;
   Eina_Array_Iterator iterator;
   E_Connman_Array *array;
   unsigned int i;
   int type;
   void *item;

   EINA_SAFETY_ON_NULL_RETURN_VAL(element, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(property, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(count, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(elements, 0);

   *count = 0;
   *elements = NULL;

   if (!e_connman_element_property_get_stringshared
       (element, property, &type, &array))
     return 0;

   if (type != DBUS_TYPE_ARRAY)
     {
	ERR("property %s is not an array!\n", property);
	return 0;
     }

   if ((!array) || (!array->array) || (array->type == DBUS_TYPE_INVALID))
     return 0;

   if (array->type != DBUS_TYPE_OBJECT_PATH)
     {
	ERR("property %s is not an array of object paths!\n", property);
	return 0;
     }

   *count = eina_array_count_get(array->array);
   ret = malloc(*count * sizeof(E_Connman_Element *));
   if (!ret)
     {
	ERR("could not allocate return array of %d elements: %s\n",
	    *count, strerror(errno));
	*count = 0;
	return 0;
     }
   p = ret;

   EINA_ARRAY_ITER_NEXT(array->array, i, item, iterator)
     {
	E_Connman_Element *e = e_connman_element_get(item);
	if (!e)
	  continue;
	*p = e;
	p++;
     }
   *count = p - ret;
   *elements = ret;
   return 1;
}

void
_e_connman_element_array_print(FILE *fp, E_Connman_Array *array)
{
   Eina_Array_Iterator iterator;
   unsigned int i;
   void *item;

   if (!array)
     return;

   switch (array->type)
     {
      case DBUS_TYPE_OBJECT_PATH:
	 EINA_ARRAY_ITER_NEXT(array->array, i, item, iterator)
	   fprintf(fp, "\"%s\", ", (const char *)item);
	 break;
      case DBUS_TYPE_BYTE:
	 EINA_ARRAY_ITER_NEXT(array->array, i, item, iterator)
	   fprintf(fp, "%#02hhx (\"%c\"), ", (unsigned char)(long)item,
		   (unsigned char)(long)item);
	 break;
      case DBUS_TYPE_INVALID:
	 break;
      default:
	 fprintf(fp, "<UNKNOWN ARRAY TYPE '%c'>", array->type);
     }
}

/**
 * Print element to file descriptor.
 */
void
e_connman_element_print(FILE *fp, const E_Connman_Element *element)
{
   const E_Connman_Element_Property *p;

   EINA_SAFETY_ON_NULL_RETURN(fp);
   if (!element)
     {
	fputs("ERROR: no element to print\n", fp);
	return;
     }

   fprintf(fp,
	   "Element %p: %s [%s]\n"
	   "\tProperties:\n",
	   element, element->path, element->interface);

   EINA_INLIST_FOREACH(element->props, p)
     {
	fprintf(fp, "\t\t%s (%c) = ", p->name, p->type);

	switch (p->type)
	  {
	   case DBUS_TYPE_STRING:
	      fprintf(fp, "\"%s\"", p->value.str);
	      break;
	   case DBUS_TYPE_OBJECT_PATH:
	      fprintf(fp, "\"%s\"", p->value.path);
	      break;
	   case DBUS_TYPE_BOOLEAN:
	      fprintf(fp, "%hhu", p->value.boolean);
	      break;
	   case DBUS_TYPE_BYTE:
	      fprintf(fp, "%#02hhx (%d), ", p->value.byte, p->value.byte);
	      break;
	   case DBUS_TYPE_UINT16:
	      fprintf(fp, "%hu", p->value.u16);
	      break;
	   case DBUS_TYPE_UINT32:
	      fprintf(fp, "%u", p->value.u32);
	      break;
	   case DBUS_TYPE_ARRAY:
	      _e_connman_element_array_print(fp, p->value.array);
	      break;
	   default:
	      fputs("don't know how to print type", fp);
	  }

	fputc('\n', fp);
     }
}

static E_Connman_Element *
e_connman_element_new(const char *path, const char *interface)
{
   E_Connman_Element *element;

   element = calloc(1, sizeof(*element));
   if (!element)
     {
	ERR("could not allocate element: %s\n",	strerror(errno));
	return NULL;
     }

   element->path = eina_stringshare_add(path);
   element->interface = eina_stringshare_ref(interface);
   element->_references = 1;

   return element;
}

static void
e_connman_element_extra_properties_free(E_Connman_Element *element)
{
   while (element->props)
     {
	E_Connman_Element_Property *prop;
	prop = (E_Connman_Element_Property *)element->props;
	element->props = element->props->next;
	_e_connman_element_property_free(prop);
     }
}

static void
e_connman_element_free(E_Connman_Element *element)
{
   if (element->_idler.changed)
     ecore_idler_del(element->_idler.changed);

   while (element->_listeners)
     {
	E_Connman_Element_Listener *l = (void *)element->_listeners;
	element->_listeners = eina_inlist_remove
	  (element->_listeners, element->_listeners);

	if (l->free_data) l->free_data(l->data);
	free(l);
     }

   e_connman_element_pending_cancel_and_free(&element->_pending.properties_get);
   e_connman_element_pending_cancel_and_free(&element->_pending.property_set);
   e_connman_element_pending_cancel_and_free(&element->_pending.network_create);
   e_connman_element_pending_cancel_and_free(&element->_pending.network_remove);
   e_connman_element_pending_cancel_and_free(&element->_pending.network_connect);
   e_connman_element_pending_cancel_and_free(&element->_pending.network_disconnect);
   e_connman_element_pending_cancel_and_free(&element->_pending.agent_register);
   e_connman_element_pending_cancel_and_free(&element->_pending.agent_unregister);
   e_connman_element_pending_cancel_and_free(&element->_pending.device_propose_scan);

   e_connman_element_extra_properties_free(element);
   eina_stringshare_del(element->interface);
   eina_stringshare_del(element->path);
   free(element);
}

/**
 * Add reference to element.
 */
int
e_connman_element_ref(E_Connman_Element *element)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(element, 0);
   return ++element->_references;
}

/**
 * Remove reference from element.
 *
 * If reference count drops to 0 element will be freed.
 */
int
e_connman_element_unref(E_Connman_Element *element)
{
   int i;
   EINA_SAFETY_ON_NULL_RETURN_VAL(element, 0);

   i = --element->_references;
   if (i == 0)
     e_connman_element_free(element);
   else if (i < 0)
     ERR("element %p references %d < 0\n", element, i);
   return i;
}

/**
 * Send message with callbacks set to work with connman elements.
 *
 * If this call fails (returns 0), pending callbacks will not be called,
 * not even with error messages.
 *
 * @return 1 on success, 0 on failure.
 */
bool
e_connman_element_message_send(E_Connman_Element *element, const char *method_name, E_DBus_Method_Return_Cb cb, DBusMessage *msg, Eina_Inlist **pending, E_DBus_Method_Return_Cb user_cb, const void *user_data)
{
   E_Connman_Element_Call_Data *data;
   E_Connman_Element_Pending *p;

   EINA_SAFETY_ON_NULL_RETURN_VAL(element, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(method_name, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(pending, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(msg, 0);

   data = malloc(sizeof(*data));
   if (!data)
     {
	ERR("could not alloc e_connman_element_call_data: %s\n",
	    strerror(errno));
	dbus_message_unref(msg);
	return 0;
     }

   p = malloc(sizeof(*p));
   if (!p)
     {
	ERR("could not alloc E_Connman_Element_Pending: %s\n",
	    strerror(errno));
	free(data);
	dbus_message_unref(msg);
	return 0;
     }

   data->element = element;
   data->cb = cb;
   data->pending = p;
   data->p_list = pending;
   p->user_cb = user_cb;
   p->user_data = (void *)user_data;
   p->data = data;
   p->pending = e_dbus_message_send
     (e_connman_conn, msg, e_connman_element_call_dispatch_and_free, -1, data);
   dbus_message_unref(msg);

   if (p->pending)
     {
	*pending = eina_inlist_append(*pending, EINA_INLIST_GET(p));
	return 1;
     }
   else
     {
	ERR("failed to call %s (obj=%s, path=%s, iface=%s)\n",
	    method_name, e_connman_system_bus_name_get(),
	    element->path, element->interface);
	free(data);
	free(p);
	return 0;
     }
}

bool
e_connman_element_call_full(E_Connman_Element *element, const char *method_name, E_DBus_Method_Return_Cb cb, Eina_Inlist **pending, E_DBus_Method_Return_Cb user_cb, const void *user_data)
{
   DBusMessage *msg;

   EINA_SAFETY_ON_NULL_RETURN_VAL(element, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(method_name, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(pending, 0);

   msg = dbus_message_new_method_call
     (e_connman_system_bus_name_get(), element->path, element->interface,
      method_name);

   return e_connman_element_message_send
     (element, method_name, cb, msg, pending, user_cb, user_data);
}

static bool
_e_connman_element_property_value_add(E_Connman_Element *element, const char *name, int type, void *value)
{
   E_Connman_Element_Property *p;

   name = eina_stringshare_add(name);
   EINA_INLIST_FOREACH(element->props, p)
     {
	if (p->name == name)
	  {
	     eina_stringshare_del(name);
	     return _e_connman_element_property_update(p, type, value);
	  }
     }

   p = _e_connman_element_property_new(name, type, value);
   if (!p)
     {
	ERR("could not create property %s (%c)\n", name, type);
	return 0;
     }

   element->props = eina_inlist_append(element->props, EINA_INLIST_GET(p));
   return 1;
}

static E_Connman_Array *
_e_connman_element_iter_get_array(DBusMessageIter *itr, const char *key)
{
   E_Connman_Array *array;
   DBusMessageIter e_itr;

   array = malloc(sizeof(E_Connman_Array));
   if (!array)
     {
	ERR("could not create new e_connman array.\n");
	return NULL;
     }
   array->array = eina_array_new(16);
   if (!(array->array))
     {
	ERR("could not create new eina array.\n");
	free(array);
	return NULL;
     }

   dbus_message_iter_recurse(itr, &e_itr);
   array->type = dbus_message_iter_get_arg_type(&e_itr);
   do
     {
	switch (array->type)
	  {
	   case DBUS_TYPE_INVALID:
	      break;
	   case DBUS_TYPE_OBJECT_PATH:
	     {
		const char *path;

		dbus_message_iter_get_basic(&e_itr, &path);
		path = eina_stringshare_add(path);
		eina_array_push(array->array, path);
		_e_connman_element_item_register(key, path);
	     }
	     break;
	   case DBUS_TYPE_BYTE:
	     {
		unsigned char byte;
		dbus_message_iter_get_basic(&e_itr, &byte);
		eina_array_push(array->array, (void *)(long)byte);
	     }
	     break;
	   default:
	      ERR("don't know how to get property type %c (%d)\n",
		  array->type, array->type);
	  }
     }
   while (dbus_message_iter_next(&e_itr));
   return array;
}

static void
_e_connman_element_get_properties_callback(void *user_data, DBusMessage *msg, DBusError *err)
{
   E_Connman_Element *element = user_data;
   DBusMessageIter itr, s_itr;
   int t, changed;

   DBG("get_properties msg=%p\n", msg);

   if (!_dbus_callback_check_and_init(msg, &itr, err))
     return;

   t = dbus_message_iter_get_arg_type(&itr);
   if (!_dbus_iter_type_check(t, DBUS_TYPE_ARRAY))
     return;

   changed = 0;
   dbus_message_iter_recurse(&itr, &s_itr);
   do
     {
	DBusMessageIter e_itr, v_itr;
	const char *key;
	void *value;
	int r;

	t = dbus_message_iter_get_arg_type(&s_itr);
	if (!_dbus_iter_type_check(t, DBUS_TYPE_DICT_ENTRY))
	  continue;

	dbus_message_iter_recurse(&s_itr, &e_itr);

	t = dbus_message_iter_get_arg_type(&e_itr);
	if (!_dbus_iter_type_check(t, DBUS_TYPE_STRING))
	  continue;

	dbus_message_iter_get_basic(&e_itr, &key);
	dbus_message_iter_next(&e_itr);
	t = dbus_message_iter_get_arg_type(&e_itr);
	if (!_dbus_iter_type_check(t, DBUS_TYPE_VARIANT))
	  continue;

	dbus_message_iter_recurse(&e_itr, &v_itr);
	t = dbus_message_iter_get_arg_type(&v_itr);
	if (t == DBUS_TYPE_ARRAY)
	  value = _e_connman_element_iter_get_array(&v_itr, key);
	else if (t != DBUS_TYPE_INVALID)
	  dbus_message_iter_get_basic(&v_itr, &value);
	r = _e_connman_element_property_value_add(element, key, t, value);
	if (r < 0)
	  ERR("failed to add property value %s (%c)\n", key, t);
	else if (r == 1)
	  {
	     INF("property value changed %s (%c)\n", key, t);
	     changed = 1;
	  }
     }
   while (dbus_message_iter_next(&s_itr));

   if (changed)
     _e_connman_element_listeners_call(element);
}

/**
 * Sync element properties with server.
 *
 * Call method GetProperties() at the given element on server in order to sync
 * them.
 *
 * @param element to call method on server.
 * @param cb function to call when server replies or some error happens.
 * @param data data to give to cb when it is called.
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_connman_element_sync_properties_full(E_Connman_Element *element, E_DBus_Method_Return_Cb cb, const void *data)
{
   const char name[] = "GetProperties";

   EINA_SAFETY_ON_NULL_RETURN_VAL(element, 0);
   return e_connman_element_call_full
     (element, name, _e_connman_element_get_properties_callback,
      &element->_pending.properties_get, cb, data);
}

/**
 * Sync element properties with server, simple version.
 *
 * Call method GetProperties() at the given element on server in order to sync
 * them. This is the simple version and there is no check of server reply
 * for errors.
 *
 * @param element to call method on server.
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_connman_element_properties_sync(E_Connman_Element *element)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(element, 0);
   return e_connman_element_sync_properties_full(element, NULL, NULL);
}

/**
 * Call method SetProperty(prop, value) at the given element on server.
 *
 * This is a server call, not local, so it may fail and in that case
 * no property is updated locally. If the value was set the event
 * E_CONNMAN_EVENT_ELEMENT_UPDATED will be added to main loop.
 *
 * @param element to call method on server.
 * @param prop property name.
 * @param type DBus type to use for value.
 * @param value pointer to value, just like regular DBus, see
 *        dbus_message_iter_append_basic().
 * @param cb function to call when server replies or some error happens.
 * @param data data to give to cb when it is called.
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_connman_element_property_set_full(E_Connman_Element *element, const char *prop, int type, const void *value, E_DBus_Method_Return_Cb cb, const void *data)
{
   const char name[] = "SetProperty";
   char typestr[2];
   DBusMessage *msg;

   EINA_SAFETY_ON_NULL_RETURN_VAL(element, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(prop, 0);

   msg = dbus_message_new_method_call
     (e_connman_system_bus_name_get(), element->path, element->interface, name);

   if (!msg)
     return 0;

   DBusMessageIter itr, v;
   dbus_message_iter_init_append(msg, &itr);
   dbus_message_iter_append_basic(&itr, DBUS_TYPE_STRING, &prop);

   typestr[0] = type;
   typestr[1] = '\0';
   dbus_message_iter_open_container(&itr, DBUS_TYPE_VARIANT, typestr, &v);
   if ((type == DBUS_TYPE_STRING) || (type == DBUS_TYPE_OBJECT_PATH))
     dbus_message_iter_append_basic(&v, type, &value);
   else
     dbus_message_iter_append_basic(&v, type, value);
   dbus_message_iter_close_container(&itr, &v);

   return e_connman_element_message_send
     (element, name, NULL, msg, &element->_pending.property_set, cb, data);
}

/**
 * Call method SetProperty(prop, value) at the given element on server.
 *
 * This is the simple version and there is no check of server reply
 * for errors.
 *
 * @param element to call method on server.
 * @param prop property name.
 * @param type DBus type to use for value.
 * @param value pointer to value, just like regular DBus, see
 *        dbus_message_iter_append_basic().
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_connman_element_property_set(E_Connman_Element *element, const char *prop, int type, const void *value)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(element, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(prop, 0);
   return e_connman_element_property_set_full
     (element, prop, type, value, NULL, NULL);
}

bool
e_connman_element_call_with_path(E_Connman_Element *element, const char *method_name, const char *string, E_DBus_Method_Return_Cb cb, Eina_Inlist **pending, E_DBus_Method_Return_Cb user_cb, const void *user_data)
{
   DBusMessageIter itr;
   DBusMessage *msg;

   EINA_SAFETY_ON_NULL_RETURN_VAL(element, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(method_name, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(string, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(pending, 0);

   msg = dbus_message_new_method_call
     (e_connman_system_bus_name_get(), element->path, element->interface,
      method_name);

   if (!msg)
     return 0;

   dbus_message_iter_init_append(msg, &itr);
   dbus_message_iter_append_basic(&itr, DBUS_TYPE_OBJECT_PATH, &string);

   return e_connman_element_message_send
     (element, method_name, cb, msg, pending, user_cb, user_data);
}

/**
 * Get property type.
 *
 * If zero is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * @param element which element to get the property
 * @param name property name, must be previously stringshared
 * @param type will contain the value type.
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_connman_element_property_type_get_stringshared(const E_Connman_Element *element, const char *name, int *type)
{
   const E_Connman_Element_Property *p;

   EINA_SAFETY_ON_NULL_RETURN_VAL(element, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(name, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(type, 0);

   EINA_INLIST_FOREACH(element->props, p)
     {
	if (p->name == name)
	  {
	     *type = p->type;
	     return 1;
	  }
     }

   WRN("element %s (%p) has no property with name \"%s\".\n",
       element->path, element, name);
   return 0;
}

/**
 * Get property type.
 *
 * If zero is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * @param element which element to get the property
 * @param name property name
 * @param type will contain the value type.
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_connman_element_property_type_get(const E_Connman_Element *element, const char *name, int *type)
{
   bool ret;
   name = eina_stringshare_add(name);
   ret = e_connman_element_property_type_get_stringshared(element, name, type);
   eina_stringshare_del(name);
   return ret;
}

void
e_connman_element_list_properties(const E_Connman_Element *element, bool (*cb)(void *data, const E_Connman_Element *element, const char *name, int type, const void *value), const void *data)
{
   const E_Connman_Element_Property *p;

   EINA_SAFETY_ON_NULL_RETURN(element);
   EINA_SAFETY_ON_NULL_RETURN(cb);

   EINA_INLIST_FOREACH(element->props, p)
     {
	const void *value = NULL;

	switch (p->type)
	  {
	   case DBUS_TYPE_STRING:
	      value = &p->value.str;
	      break;
	   case DBUS_TYPE_OBJECT_PATH:
	      value = &p->value.path;
	      break;
	   case DBUS_TYPE_BOOLEAN:
	      value = (void *)p->value.boolean;
	      break;
	   case DBUS_TYPE_UINT16:
	      value = &p->value.u16;
	      break;
	   case DBUS_TYPE_UINT32:
	      value = &p->value.u32;
	      break;
	   default:
	      ERR("unsupported type %c\n", p->type);
	  }

	if (!cb((void *)data, element, p->name, p->type, value))
	  return;
     }
}

/**
 * Get property value given its name.
 *
 * This will look into properties, to find the property.
 * If no property is found then 0 is returned.
 *
 * If zero is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * @param element which element to get the property
 * @param name property name, must be previously stringshared
 * @param type if provided it will contain the value type.
 * @param value where to store the property value, must be a pointer to the
 *        exact type, (bool *) for booleans, (char **) for strings, and so on.
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_connman_element_property_get_stringshared(const E_Connman_Element *element, const char *name, int *type, void *value)
{
   const E_Connman_Element_Property *p;

   EINA_SAFETY_ON_NULL_RETURN_VAL(element, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(name, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(value, 0);

   EINA_INLIST_FOREACH(element->props, p)
     {
	if (p->name != name)
	  continue;

	if (type) *type = p->type;

	switch (p->type)
	  {
	   case DBUS_TYPE_BOOLEAN:
	      *(bool *)value = p->value.boolean;
	      return 1;
	   case DBUS_TYPE_BYTE:
	      *(unsigned char *)value = p->value.byte;
	      return 1;
	   case DBUS_TYPE_UINT16:
	      *(unsigned short *)value = p->value.u16;
	      return 1;
	   case DBUS_TYPE_UINT32:
	      *(unsigned int *)value = p->value.u32;
	      return 1;
	   case DBUS_TYPE_STRING:
	      *(const char **)value = p->value.str;
	      return 1;
	   case DBUS_TYPE_OBJECT_PATH:
	      *(const char **)value = p->value.path;
	      return 1;
	   case DBUS_TYPE_ARRAY:
	      *(E_Connman_Array **)value = p->value.array;
	      return 1;
	   default:
	      ERR("don't know how to get property type %c (%d)\n",
		  p->type, p->type);
	      return 0;
	  }
     }

   WRN("element %s (%p) has no property with name \"%s\".\n",
       element->path, element, name);
   return 0;
}

/**
 * Get property value given its name.
 *
 * This will look into properties, to find the property.
 * If no property is found then 0 is returned.
 *
 * If zero is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * @param element which element to get the property
 * @param name property name
 * @param type if provided it will contain the value type.
 * @param value where to store the property value, must be a pointer to the
 *        exact type, (bool *) for booleans, (char **) for strings, and so on.
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_connman_element_property_get(const E_Connman_Element *element, const char *name, int *type, void *value)
{
   bool ret;
   name = eina_stringshare_add(name);
   ret = e_connman_element_property_get_stringshared
     (element, name, type, value);
   eina_stringshare_del(name);
   return ret;
}


struct e_connman_elements_for_each_data
{
   Eina_Hash_Foreach cb;
   void *data;
};

Eina_Bool
_e_connman_elements_for_each(Eina_Hash *hash, const char *key, void *data, void *fdata)
{
   struct e_connman_elements_for_each_data *each_data = fdata;

   each_data->cb(elements, key, data, each_data->data);
   return 1;
}

/**
 * Call the given function for each existing element.
 *
 * @param cb function to call for each element. It will get as parameters,
 *        in order: the element pointer and the given @a user_data.
 * @param user_data data to give to @a cb for each element.
 */
void
e_connman_elements_for_each(Eina_Hash_Foreach cb, const void *user_data)
{
   struct e_connman_elements_for_each_data data = {cb, (void *)user_data};

   EINA_SAFETY_ON_NULL_RETURN(cb);

   eina_hash_foreach(elements, (Eina_Hash_Foreach) _e_connman_elements_for_each,
		     &data);
}

static bool
_e_connman_elements_get_allocate(unsigned int *count, E_Connman_Element ***p_elements)
{
   *count = eina_hash_population(elements);
   if (*count == 0)
     {
	*p_elements = NULL;
	return 1;
     }

   *p_elements = malloc(*count * sizeof(E_Connman_Element *));
   if (!*p_elements)
     {
	ERR("could not allocate return array of %d elements: %s\n",
	    *count, strerror(errno));
	*count = 0;
	return 0;
     }
   return 1;
}

Eina_Bool
_e_connman_elements_get_all(Eina_Hash *hash, const char *key, void *data, void *fdata)
{
   E_Connman_Element *element = data;
   E_Connman_Element ***p_ret = fdata;

   **p_ret = element;
   (*p_ret)++;
   return 1;
}

/**
 * Get all known elements.
 *
 * No reference is added to these elements, since there are no threads
 * in the system, you are free to add references yourself right after
 * the return of this call without race condition, elements by the
 * system (ie: elementRemoved signal)could only be touched on the next
 * main loop iteration.
 *
 * @param count return the number of elements in array.
 * @param p_elements array with all elements, these are not referenced
 *        and in no particular order, just set if return is 1.
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_connman_elements_get_all(unsigned int *count, E_Connman_Element ***p_elements)
{
   E_Connman_Element **p;

   EINA_SAFETY_ON_NULL_RETURN_VAL(count, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(p_elements, 0);

   if (!_e_connman_elements_get_allocate(count, p_elements))
     return 0;
   p = *p_elements;
   eina_hash_foreach(elements, (Eina_Hash_Foreach) _e_connman_elements_get_all,
		     &p);
   return 1;
}

struct e_connman_elements_get_all_str_data
{
   E_Connman_Element **elements;
   int count;
   const char *str;
};

Eina_Bool
_e_connman_elements_get_all_type(Eina_Hash *hash, const char *key, void *e, void *user_data)
{
   struct e_connman_elements_get_all_str_data *data = user_data;
   E_Connman_Element *element = e;

   if ((data->str) && (element->interface != data->str))
     return 1;

   data->elements[data->count] = element;
   data->count++;
   return 1;
}

/**
 * Get all known elements of type.
 *
 * No reference is added to these elements, since there are no threads
 * in the system, you are free to add references yourself right after
 * the return of this call without race condition, elements by the
 * system (ie: ElementRemoved signal) could only be touched on the next
 * main loop iteration.
 *
 * @param type type to filter, or NULL to get all.
 * @param count return the number of elements in array.
 * @param p_elements array with all elements, these are not referenced
 *        and in no particular order, just set if return is 1.
 *
 * @return 1 on success, 0 otherwise.
 *
 * @see e_connman_elements_get_all()
 */
bool
e_connman_elements_get_all_type(const char *type, unsigned int *count, E_Connman_Element ***p_elements)
{
   struct e_connman_elements_get_all_str_data data;

   EINA_SAFETY_ON_NULL_RETURN_VAL(count, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(p_elements, 0);

   if (!_e_connman_elements_get_allocate(count, p_elements))
     return 0;

   data.elements = *p_elements;
   data.count = 0;
   data.str = eina_stringshare_add(type);
   eina_hash_foreach(elements,
		     (Eina_Hash_Foreach) _e_connman_elements_get_all_type,
		     &data);

   eina_stringshare_del(data.str);
   *count = data.count;
   return 1;
}

/**
 * Get the element registered at given path.
 *
 * @param path the path to query for registered object.
 *
 * @return element pointer if found, NULL otherwise. No references are added.
 */
E_Connman_Element *
e_connman_element_get(const char *path)
{
   E_Connman_Element *element;

   EINA_SAFETY_ON_NULL_RETURN_VAL(path, NULL);
   element = eina_hash_find(elements, path);

   return element;
}

static void
_e_connman_element_property_changed_callback(void *data, DBusMessage *msg)
{
   E_Connman_Element *element = (E_Connman_Element *)data;
   DBusMessageIter itr, v_itr;
   int t, r, changed = 0;
   const char *name;
   void *value;

   DBG("Property changed in element %s\n", element->path);

   if (!_dbus_callback_check_and_init(msg, &itr, NULL))
     return;

   t = dbus_message_iter_get_arg_type(&itr);
   if (!_dbus_iter_type_check(t, DBUS_TYPE_STRING))
     ERR("missing name in property changed signal\n");
   dbus_message_iter_get_basic(&itr, &name);

   dbus_message_iter_next(&itr);
   t = dbus_message_iter_get_arg_type(&itr);
   if (!_dbus_iter_type_check(t, DBUS_TYPE_VARIANT))
     ERR("missing value in property changed signal\n");
   dbus_message_iter_recurse(&itr, &v_itr);
   t = dbus_message_iter_get_arg_type(&v_itr);

   if (t == DBUS_TYPE_ARRAY)
     value = _e_connman_element_iter_get_array(&v_itr, name);
   else if (t != DBUS_TYPE_INVALID)
     dbus_message_iter_get_basic(&v_itr, &value);
   r = _e_connman_element_property_value_add(element, name, t, value);
   if (r < 0)
     ERR("failed to add property value %s (%c)\n", name, t);
   else if (r == 1)
     {
	INF("property value changed %s (%c)\n", name, t);
	changed = 1;
     }
   if (changed)
     _e_connman_element_listeners_call(element);
}

/**
 * Register the given path, possible creating and element and return it.
 *
 * This will check if path is already registered, in that case the
 * exiting element is returned. If it was not registered yet, a new
 * element is created, registered and returned.
 *
 * This call will not add extra references to the object.
 *
 * @param path the path to register the element
 *
 * @return the registered object, no references are added.
 */
E_Connman_Element *
e_connman_element_register(const char *path, const char *interface)
{
   E_Connman_Element *element;

   EINA_SAFETY_ON_NULL_RETURN_VAL(path, NULL);
   EINA_SAFETY_ON_NULL_RETURN_VAL(interface, NULL);

   element = eina_hash_find(elements, path);
   if (element)
     return element;

   element = e_connman_element_new(path, interface);
   if (!element)
     return NULL;

   if (!eina_hash_add(elements, element->path, element))
     {
	ERR("could not add element %s to hash, delete it.\n", path);
	e_connman_element_free(element);
	return NULL;
     }

   element->signal_handler =
     e_dbus_signal_handler_add
     (e_connman_conn, e_connman_system_bus_name_get(),
      element->path, element->interface, "PropertyChanged",
      _e_connman_element_property_changed_callback, element);

   e_connman_element_event_add(E_CONNMAN_EVENT_ELEMENT_ADD, element);

   return element;
}

static void
_e_connman_element_event_unregister_and_free(void *data, void *ev)
{
   E_Connman_Element *element = ev;
   e_connman_element_unref(element);
}

static void
_e_connman_element_unregister_internal(E_Connman_Element *element)
{
   if (element->signal_handler)
     {
	e_dbus_signal_handler_del(e_connman_conn, element->signal_handler);
	element->signal_handler = NULL;
     }

   ecore_event_add(E_CONNMAN_EVENT_ELEMENT_DEL, element,
		   _e_connman_element_event_unregister_and_free, NULL);
}

/**
 * Forget about the given element.
 *
 * This will remove the element from the pool of known objects, then
 * add an E_CONNMAN_EVENT_ELEMENT_DEL and after that will unreference it,
 * possible freeing it.
 *
 * @param element element to forget about. Its reference will be removed.
 */
void
e_connman_element_unregister(E_Connman_Element *element)
{
   if (!element)
     return;

   if (elements)
     eina_hash_del_by_key(elements, element->path);

   _e_connman_element_unregister_internal(element);
}

/**
 * Remove all known elements.
 *
 * This will remove all known elements but will NOT add any
 * E_CONNMAN_EVENT_ELEMENT_DEL to main loop.
 *
 * This is just useful to make sure next e_connman_manager_sync_elements()
 * will not leave any stale elements. This is unlikely to happen, as
 * E_Connman is supposed to catch all required events to avoid stale elements.
 */
void
e_connman_manager_clear_elements(void)
{
   e_connman_elements_shutdown();
   e_connman_elements_init();
}

/**
 * Creates elements hash.
 *
 * This has no init counter since its already guarded by other code.
 * @internal
 */
void
e_connman_elements_init(void)
{
   EINA_SAFETY_ON_FALSE_RETURN(elements == NULL);
   elements =
     eina_hash_string_superfast_new(EINA_FREE_CB
				    (_e_connman_element_unregister_internal));
}

void
e_connman_elements_shutdown(void)
{
   EINA_SAFETY_ON_FALSE_RETURN(elements != NULL);
   eina_hash_free(elements);
   elements = NULL;
}

static inline bool
_e_connman_element_is(const E_Connman_Element *element, const char *interface)
{
   return element->interface == interface;
}

bool
e_connman_element_is_manager(const E_Connman_Element *element)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(element, 0);
   return _e_connman_element_is(element, e_connman_iface_manager);
}

bool
e_connman_element_is_device(const E_Connman_Element *element)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(element, 0);
   return _e_connman_element_is(element, e_connman_iface_device);
}

bool
e_connman_element_is_profile(const E_Connman_Element *element)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(element, 0);
   return _e_connman_element_is(element, e_connman_iface_profile);
}

bool
e_connman_element_is_connection(const E_Connman_Element *element)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(element, 0);
   return _e_connman_element_is(element, e_connman_iface_connection);
}

bool
e_connman_element_is_network(const E_Connman_Element *element)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(element, 0);
   return _e_connman_element_is(element, e_connman_iface_network);
}
