#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "explicit_client.h"

static Eina_Bool
_explicit_call(Explicit *context, Evas_Object *obj,
	       Explicit_Callback_Type type, void *event)
{
   Explicit_Callback_Object *call;
   Eina_List *l;
   Eina_Bool result = EINA_TRUE;

   EINA_LIST_FOREACH(context->callbacks, l, call)
     if (call->type == type)
       result &= call->cb(context, obj, type, event, (void*) call->data);

   return result;
}

Eina_Bool
_explicit_call_event_cancel(Explicit *context, Evas_Object *obj,
			    const char *url, const char *key, int status)
{
   Explicit_Event_Cancel cancel = { { url, key }, status };

   return _explicit_call(context, obj, EXPLICIT_CALLBACK_CANCEL, &cancel);
}

Eina_Bool
_explicit_call_event_done(Explicit *context, Evas_Object *obj,
			  const char *file, const char *key,
			  size_t downloaded, size_t size, int status)
{
   Explicit_Event_Done done = { { file, key },
				downloaded, size, status };

   return _explicit_call(context, obj, EXPLICIT_CALLBACK_DONE, &done);
}

Eina_Bool
_explicit_call_event_progress(Explicit *context, Evas_Object *obj,
			      const char *url, const char *key,
			      size_t downloaded, size_t size)
{
   Explicit_Event_Progress progress = { { url, key },
					downloaded, size };

   return _explicit_call(context, obj, EXPLICIT_CALLBACK_PROGRESS, &progress);
}

#define STRINGSHARE_FREED(Str) \
  eina_stringshare_del(Str);   \
  Str = NULL;

Eina_Bool
_explicit_call_event_approval(Explicit *context, Evas_Object *obj,
			      const char *url, const char *key, Explicit_Target *destination)
{
   Explicit_Callback_Object *call;
   Eina_List *l;
   Explicit_Event_Approval approval;

   approval.source.url = eina_stringshare_add(url);
   approval.source.key = eina_stringshare_add(key);
   approval.destination.url = eina_stringshare_ref(approval.source.url);
   approval.destination.key = eina_stringshare_ref(approval.source.key);

   EINA_LIST_FOREACH(context->callbacks, l, call)
     if (call->type == EXPLICIT_CALLBACK_APPROVAL)
       {
	  Eina_Bool result;

	  result = call->cb(context, obj,
			    EXPLICIT_CALLBACK_APPROVAL, &approval, (void*) call->data);

	  STRINGSHARE_FREED(approval.source.url);
	  STRINGSHARE_FREED(approval.source.key);

	  if (!result)
	    {
	       STRINGSHARE_FREED(approval.destination.url);
	       STRINGSHARE_FREED(approval.destination.key);

	       return EINA_FALSE;
	    }

	  approval.source.url = approval.destination.url;
	  approval.source.key = approval.destination.key;
	  approval.destination.url = eina_stringshare_ref(approval.source.url);
	  approval.destination.key = eina_stringshare_ref(approval.source.key);
       }

   STRINGSHARE_FREED(approval.source.url);
   STRINGSHARE_FREED(approval.source.key);

   *destination = approval.destination;

   return EINA_TRUE;
}

