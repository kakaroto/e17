#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef HAVE_EDJE
# include <Edje.h>
#endif

#include "explicit_client.h"

static void
_explicit_object_request_cleanup(Explicit_Object_Request *request)
{
   request->obj->object_requests = eina_list_remove(request->obj->object_requests, request);

   /* If no more download time to cleanup memory */
   if (!request->obj->object_requests)
     /* status is useless, as their is nobody to warn. */
     explicit_object_url_cancel(request->obj->object, 0);

   eina_stringshare_del(request->key);
   free(request);
}

Eina_Bool
_explicit_object_cancel(Explicit_Object_Request *request, int status)
{
   /* Time to call cancel callback. */
   _explicit_call_event_cancel(request->obj->context,
			       request->obj->object,
			       request->client->file,
			       request->key,
			       status);

   /* FIXME: possibly delete recursion !!! */
   _explicit_request_cancel(request);

   _explicit_object_request_cleanup(request);

   return EINA_TRUE;
}

Eina_Bool
_explicit_object_done(Explicit_Object_Request *request, size_t downloaded, size_t size, int status)
{
   /* Time to call done callback. */
   _explicit_call_event_done(request->obj->context,
			     request->obj->object,
			     request->client->file,
			     request->key,
			     downloaded,
			     size,
			     status);

   _explicit_object_request_cleanup(request);

   return EINA_TRUE;
}


static void
_explicit_object_del(__UNUSED__ void *data,
		     __UNUSED__ Evas *e,
		     Evas_Object *obj,
		     __UNUSED__ void *event_info)
{
   /* Cancel all download from this object when deleting this object. */
   explicit_object_url_cancel(obj, EXPLICIT_CANCEL_OBJECT_DEL);
}

/* Explicit support Evas_Object_Image and Edje_Object */
EAPI Eina_Bool
explicit_object_url_get(Explicit *context, Evas_Object *obj, const char *url, const char *key)
{
   Explicit_Object_Request *request = NULL;
   Explicit_Object *ox = NULL;
   Explicit_Target destination = { NULL, NULL };

   if (!obj)
     return EINA_FALSE;

   explicit_object_url_cancel(obj, EXPLICIT_CANCEL_OVERWRITE);

   if (!url)
     return EINA_FALSE;

   if (!EINA_MAGIC_CHECK(context, EINA_MAGIC_EXPLICIT))
     {
	EINA_MAGIC_FAIL(ox, EINA_MAGIC_EXPLICIT);
	return EINA_FALSE;
     }

   /* Time to call approval callback. */
   if (!_explicit_call_event_approval(context, obj, url, key, &destination))
     return EINA_FALSE;

   ox = calloc(1, sizeof (Explicit_Object));
   if (!ox)
     return EINA_FALSE;

   ox->context = context;
   ox->object = obj;
   ox->single_request = EINA_TRUE;

   if (strcmp(evas_object_type_get(obj), "image") == 0)
     ox->type = EVAS_OBJECT_IMAGE;
   else if (strcmp(evas_object_type_get(obj), "edje") == 0)
     {
	ox->type = EDJE_OBJECT;
	if (!destination.key)
	  goto on_error;
     }
   else goto on_error;

   request = calloc(1, sizeof (Explicit_Object_Request));
   if (!request) goto on_error;

   request->obj = ox;
   request->key = destination.key;

   if (!_explicit_request_file(context, destination.url, request))
     goto on_error;

   eina_stringshare_del(destination.url);

   EINA_MAGIC_SET(ox, EINA_MAGIC_EXPLICIT);
   ox->object_requests = eina_list_append(ox->object_requests, request);
   evas_object_data_set(obj, "explicit", ox);
   evas_object_event_callback_add(obj, EVAS_CALLBACK_DEL, _explicit_object_del, ox);

   return EINA_TRUE;

 on_error:
   eina_stringshare_del(destination.url);
   free(request);
   free(ox);

   return EINA_FALSE;
}

/* Download multiple files without testing them, but report progress to obj */
EAPI Eina_Bool
explicit_object_list_url_get(Explicit *context, Evas_Object *obj, Eina_List *urls)
{
   Explicit_Object_Request *request;
   Explicit_Object *ox;
   Eina_List *l;
   const char *url;

   if (!obj)
     return EINA_FALSE;

   explicit_object_url_cancel(obj, EXPLICIT_CANCEL_OVERWRITE);

   if (!urls)
     return EINA_FALSE;

   if (!EINA_MAGIC_CHECK(context, EINA_MAGIC_EXPLICIT))
     {
	EINA_MAGIC_FAIL(context, EINA_MAGIC_EXPLICIT);
	return EINA_FALSE;
     }

   ox = calloc(1, sizeof (Explicit_Object));
   if (!ox)
     return EINA_FALSE;

   ox->context = context;
   ox->object = obj;
   ox->single_request = EINA_FALSE;

   if (strcmp(evas_object_type_get(obj), "image") == 0)
     ox->type = EVAS_OBJECT_IMAGE;
   else if (strcmp(evas_object_type_get(obj), "edje") == 0)
     ox->type = EDJE_OBJECT;
   else
     ox->type = ANY;

   EINA_LIST_FOREACH(urls, l, url)
     {
	Explicit_Target destination;

	/* Time to call approval callback. */
	if (!_explicit_call_event_approval(context, obj, url, NULL, &destination))
	  continue;

	request = calloc(1, sizeof (Explicit_Object_Request));
	if (!request)
	  {
	     eina_stringshare_del(destination.url);
	     continue ;
	  }

	request->obj = ox;

	if (_explicit_request_file(context, destination.url, request))
	  {
	     ox->object_requests = eina_list_append(ox->object_requests, request);
	  }
	else
	  {
	     _explicit_call_event_cancel(context, obj, destination.url, NULL, EXPLICIT_CANCEL_NOT_STARTED);
	     free(request);
	  }

	eina_stringshare_del(destination.url);
     }

   if (!ox->object_requests)
     {
	free(ox);

	return EINA_FALSE;
     }

   EINA_MAGIC_SET(ox, EINA_MAGIC_EXPLICIT);
   evas_object_data_set(obj, "explicit", ox);
   evas_object_event_callback_add(obj, EVAS_CALLBACK_DEL, _explicit_object_del, ox);

   return EINA_TRUE;
}

EAPI Eina_Bool
explicit_object_url_cancel(Evas_Object *obj, int status)
{
   Explicit_Object_Request *request;
   Explicit_Object *ox;

   if (!obj) return EINA_FALSE;

   ox = evas_object_data_get(obj, "explicit");
   if (!ox) return EINA_FALSE;

   if (!EINA_MAGIC_CHECK(ox, EINA_MAGIC_EXPLICIT))
     {
	EINA_MAGIC_FAIL(ox, EINA_MAGIC_EXPLICIT);
	return EINA_FALSE;
     }

   /* No need to track this object anymore. */
   evas_object_event_callback_del_full(obj, EVAS_CALLBACK_DEL, _explicit_object_del, ox);
   evas_object_data_set(obj, "explicit", NULL);

   /* Stop all download. */
   while (ox->object_requests)
     {
	request = eina_list_data_get(ox->object_requests);
	_explicit_object_cancel(request, status);
     }

   /* Finally destroy the link. */
   EINA_MAGIC_SET(ox, 0);
   free(ox);

   return EINA_TRUE;
}

Eina_Bool
_explicit_object_downloaded(Explicit *context, int id, const char *file, size_t downloaded, size_t size, int status)
{
   Explicit_Client_Request *client;
   Explicit_Object_Request *request;
   Eina_Bool ret = EINA_FALSE;

   client = eina_hash_find(context->requests_lookup, &id);
   if (!client)
     return EINA_FALSE;

   EINA_LIST_FREE(client->object_requests, request)
     {
	if (request->obj->single_request)
	  {
	     /* One download for this object, so we need to set directly
		the right value. */
	     switch (request->obj->type)
	       {
		case EVAS_OBJECT_IMAGE:
		   evas_object_image_file_set(request->obj->object, file, request->key);
		   if (evas_object_image_load_error_get(request->obj->object) != EVAS_LOAD_ERROR_NONE)
		     {
			evas_object_image_file_set(request->obj->object, NULL, NULL);
			_explicit_object_cancel(request, EXPLICIT_CANCEL_UNRECOGNIZED_FILE);
		     }
		   else
		     {
			_explicit_object_done(request, downloaded, size, status);
			ret = EINA_TRUE;
		     }
		   break;
		case EDJE_OBJECT:
#ifdef HAVE_EDJE
 		   edje_object_file_set(request->obj->object, file, request->key);
		   if (edje_object_load_error_get(request->obj->object) != EDJE_LOAD_ERROR_NONE)
		     {
			edje_object_file_set(request->obj->object, NULL, NULL);
			_explicit_object_cancel(request, EXPLICIT_CANCEL_UNRECOGNIZED_FILE);
		     }
		   else
		     {
			_explicit_object_done(request, downloaded, size, status);
			ret = EINA_TRUE;
		     }
		   break;
#endif
		case ANY:
		   /* Something is wrong here. Cancel for this object. */
		   _explicit_object_cancel(request, EXPLICIT_CANCEL_UNRECOGNIZED_FILE);
		   break;
	       }
	  }
	else
	  {
	     _explicit_object_done(request, downloaded, size, status);
	     ret = EINA_TRUE;
	  }
     }

   /* Now that the request has been handle, we can destroy it. */
   eina_hash_del(context->requests_lookup, &id, client);
   eina_stringshare_del(client->file);
   free(client);

   return ret;
}

