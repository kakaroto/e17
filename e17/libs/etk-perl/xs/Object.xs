#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "../ppport.h"

#ifdef _
#undef _
#endif

#include <Etk.h>
#include <Ecore.h>
#include <Ecore_Data.h>

#include "EtkTypes.h"
#include "EtkSignals.h"

static void
notification_callback(Etk_Object * object, const char * property_name, void * data)
{
   dSP;
   Notification_Callback_Data * ncb = NULL;

   ncb = data;

   PUSHMARK(SP);
   XPUSHs(sv_2mortal(newSVObject(object)));
   XPUSHs(sv_2mortal(newSVpv(property_name, strlen(property_name))));
   XPUSHs(sv_2mortal(newSVsv(ncb->perl_data)));
   PUTBACK;

   call_sv(ncb->perl_callback, G_DISCARD);
}

static void
callback_VOID__VOID(Etk_Object *object, void *data)
{
   dSP;
   Callback_Signal_Data *cbd = NULL;

   cbd = data;
   
   PUSHMARK(SP);
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_object)));
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_data)));
   PUTBACK ;
      
   call_sv(cbd->perl_callback, G_DISCARD);
}

static void
callback_VOID__INT(Etk_Object *object, int value, void *data)
{
   dSP;
   Callback_Signal_Data *cbd = NULL;

   cbd = data;

   PUSHMARK(SP) ;
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_object)));
   XPUSHs(sv_2mortal(newSViv(value)));
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_data)));   
   PUTBACK ;
      
   call_sv(cbd->perl_callback, G_DISCARD);
}

static void
callback_VOID__DOUBLE(Etk_Object *object, double value, void *data)
{
   dSP;
   Callback_Signal_Data *cbd = NULL;

   cbd = data;

   PUSHMARK(SP) ;
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_object)));
   XPUSHs(sv_2mortal(newSVnv(value)));
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_data)));   
   PUTBACK ;

   call_sv(cbd->perl_callback, G_DISCARD);
}

static void
callback_VOID__POINTER(Etk_Object *object, void *value, void *data)
{
   dSP;
   Callback_Signal_Data *cbd = NULL;
   SV *event_rv;
   cbd = data;   

   event_rv = GetSignalEvent_POINTER(object, value, cbd);
   
   PUSHMARK(SP) ;
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_object)));
   XPUSHs(sv_2mortal(event_rv));
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_data)));   
   PUTBACK ;
      
   call_sv(cbd->perl_callback, G_DISCARD);
}

static void
callback_VOID__POINTER_POINTER(Etk_Object *object, void *val1, void *val2, void *data)
{
   dSP;
   Callback_Signal_Data *cbd = NULL;
   cbd = data;   

   SV * obj1_rv, *event_rv;
   GetSignalEvent_POINTER_POINTER(object, val1, val2, cbd, &obj1_rv, &event_rv);

   cbd = data;

   PUSHMARK(SP) ;
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_object)));
   XPUSHs(sv_2mortal(obj1_rv));
   XPUSHs(sv_2mortal(event_rv));
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_data)));   
   PUTBACK ;
      
   call_sv(cbd->perl_callback, G_DISCARD);
}

static void
callback_VOID__INT_POINTER(Etk_Object *object, int val1, void *val2, void *data)
{
   dSP;
   Callback_Signal_Data *cbd = NULL;

   cbd = data;

   PUSHMARK(SP) ;
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_object)));
   XPUSHs(sv_2mortal(newSViv(val1)));
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_data)));   
   PUTBACK ;
      
   call_sv(cbd->perl_callback, G_DISCARD);
}

static void
callback_BOOL__VOID(Etk_Object *object, void *data)
{
   dSP;
   Callback_Signal_Data *cbd = NULL;

   cbd = data;

   PUSHMARK(SP) ;
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_object)));
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_data)));   
   PUTBACK ;
      
   call_sv(cbd->perl_callback, G_DISCARD);
}

static void
callback_BOOL__DOUBLE(Etk_Object *object, double value, void *data)
{
   dSP;
   Callback_Signal_Data *cbd = NULL;

   cbd = data;

   PUSHMARK(SP) ;
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_object)));
   XPUSHs(sv_2mortal(newSVnv(value)));
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_data)));   
   PUTBACK ;
      
   call_sv(cbd->perl_callback, G_DISCARD);
}

static void
callback_BOOL__POINTER_POINTER(Etk_Object *object, void *val1, void *val2, void *data)
{
   dSP;
   Callback_Signal_Data *cbd = NULL;

   cbd = data;

   PUSHMARK(SP) ;
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_object)));
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_data)));   
   PUTBACK ;
      
   call_sv(cbd->perl_callback, G_DISCARD);
}

static void
__etk_signal_connect_full(char *signal_name, SV *object, SV *callback, SV *data, Etk_Bool swapped, Etk_Bool after)
{
	dSP;

	Callback_Signal_Data *cbd = NULL;
	Etk_Signal *sig = NULL;
	Etk_Marshaller marsh;
	Etk_Object * obj;

	ENTER;
	SAVETMPS;

	obj = (Etk_Object *)SvObj(object, "Etk::Object");

	cbd = calloc(1, sizeof(Callback_Signal_Data));
	cbd->signal_name = strdup(signal_name);
	cbd->object = obj;
	cbd->perl_object = newSVsv(object);
	cbd->perl_data = newSVsv(data);
	cbd->perl_callback = newSVsv(callback);	
	
	sig = etk_signal_lookup(signal_name, obj->type);
	if(!sig) printf("CANT GET SIG!\n");
	marsh = etk_signal_marshaller_get(sig);

	if(marsh == etk_marshaller_VOID__VOID)
	  etk_signal_connect_full(sig, obj, ETK_CALLBACK(callback_VOID__VOID), cbd, swapped, after);
	else if(marsh == etk_marshaller_VOID__INT)
	  etk_signal_connect_full(sig, obj, ETK_CALLBACK(callback_VOID__INT), cbd, swapped, after);
	else if(marsh == etk_marshaller_VOID__DOUBLE)
	  etk_signal_connect_full(sig, obj, ETK_CALLBACK(callback_VOID__DOUBLE), cbd, swapped, after);
	else if(marsh == etk_marshaller_VOID__POINTER)
	  etk_signal_connect_full(sig, obj, ETK_CALLBACK(callback_VOID__POINTER), cbd, swapped, after);
	else if(marsh == etk_marshaller_VOID__INT_POINTER)
	  etk_signal_connect_full(sig, obj, ETK_CALLBACK(callback_VOID__INT_POINTER), cbd, swapped, after);
	else if(marsh == etk_marshaller_BOOL__VOID)
	  etk_signal_connect_full(sig, obj, ETK_CALLBACK(callback_BOOL__VOID), cbd, swapped, after);
	else if(marsh == etk_marshaller_BOOL__DOUBLE)
	  etk_signal_connect_full(sig, obj, ETK_CALLBACK(callback_BOOL__DOUBLE), cbd, swapped, after);
	else if(marsh == etk_marshaller_BOOL__POINTER_POINTER)
	  etk_signal_connect_full(sig, obj, ETK_CALLBACK(callback_BOOL__POINTER_POINTER), cbd, swapped, after);
	else if(marsh == etk_marshaller_VOID__POINTER_POINTER)
	  etk_signal_connect_full(sig, obj, ETK_CALLBACK(callback_VOID__POINTER_POINTER), cbd, swapped, after);
	else
	etk_signal_connect_full(sig, obj, ETK_CALLBACK(callback_VOID__VOID), cbd, swapped, after);

	PUTBACK;
	FREETMPS;
	LEAVE;
}



MODULE = Etk::Object	PACKAGE = Etk::Object	PREFIX = etk_object_

SV *
etk_object_data_get(object, key)
	Etk_Object *	object
	char *	key
      ALIAS:
	DataGet=1
	CODE:
	RETVAL = newSVsv((SV*)etk_object_data_get(object, key));
	OUTPUT:
	RETVAL

void
etk_object_data_set(object, key, value)
	Etk_Object *	object
	char *	key
	SV *	value
      ALIAS:
	DataSet=1
	CODE:
	etk_object_data_set(object, key, newSVsv(value));

void
etk_object_notification_callback_add(object, property_name, callback, data)
	Etk_Object *	object
	char *	property_name
	SV *	callback
	SV *	data
      ALIAS:
	NotificationCallbackAdd=1

	CODE:
	Notification_Callback_Data *ncb = NULL;

	ncb = calloc(1, sizeof(Notification_Callback_Data));
	ncb->object = object;
	ncb->perl_data = newSVsv(data);
	ncb->perl_callback = newSVsv(callback);

	etk_object_notification_callback_add(object, property_name, notification_callback, ncb);

void
etk_object_notification_callback_remove(object, property_name, callback)
	Etk_Object *	object
	char *	property_name
      ALIAS:
	NotificationCallbackRemove=1
	CODE:
	etk_object_notification_callback_remove(object, property_name, notification_callback);

void
etk_object_notify(object, property_name)
	Etk_Object *	object
	char *	property_name
      ALIAS:
	Notify=1


void
signal_connect(object, signal_name, callback, data=NULL)
	SV *		object
	char *	        signal_name
	SV *	        callback
	SV *            data
      ALIAS:
	SignalConnect=1
       
	CODE:	
	__etk_signal_connect_full(signal_name, newSVsv(object), newSVsv(callback), newSVsv(data), 
			ETK_FALSE, ETK_FALSE);

void
signal_connect_after(object, signal_name, callback, data=NULL)
	SV *		object
	char *	        signal_name
	SV *	        callback
	SV *            data
      ALIAS:
	SignalConnectAfter=1
	
	CODE:	
	__etk_signal_connect_full(signal_name, object, callback, data, ETK_FALSE, ETK_TRUE);

void 
signal_connect_full(object, signal_name, callback, data, swapped, after)
	SV *		object
	char *	        signal_name
	SV *	        callback
	SV *            data
	Etk_Bool	swapped
	Etk_Bool	after
      ALIAS:
	SignalConnectFull=1
	CODE:
	__etk_signal_connect_full(signal_name, object, callback, data, swapped, after);

	
void
signal_connect_swapped(object, signal_name, callback, data=NULL)
	SV *		object
	char *	        signal_name
	SV *	        callback
	SV *            data
      ALIAS:
	SignalConnectSwapped=1
	
	CODE:	
	__etk_signal_connect_full(signal_name, object, callback, data, ETK_TRUE, ETK_FALSE);

void
signal_disconnect(object, signal_name, callback)
	SV *		object
	char *	        signal_name
	SV *	        callback
      ALIAS:
	SignalDisconnect=1
	
	CODE:	
	Etk_Signal *sig = NULL;
	Etk_Marshaller marsh;
	Etk_Object * obj;

	obj = (Etk_Object *) SvObj(object, "Etk::Object");
	
	sig = etk_signal_lookup(signal_name, obj->type);
	if(!sig) printf("CANT GET SIG!\n");
	marsh = etk_signal_marshaller_get(sig);
	
	if(marsh == etk_marshaller_VOID__VOID)
	  etk_signal_disconnect(signal_name, obj, ETK_CALLBACK(callback_VOID__VOID));
	else if(marsh == etk_marshaller_VOID__INT)
	  etk_signal_disconnect(signal_name, obj, ETK_CALLBACK(callback_VOID__INT));
	else if(marsh == etk_marshaller_VOID__DOUBLE)
	  etk_signal_disconnect(signal_name, obj, ETK_CALLBACK(callback_VOID__DOUBLE));
	else if(marsh == etk_marshaller_VOID__POINTER)
	  etk_signal_disconnect(signal_name, obj, ETK_CALLBACK(callback_VOID__POINTER));
	else if(marsh == etk_marshaller_VOID__INT_POINTER)
	  etk_signal_disconnect(signal_name, obj, ETK_CALLBACK(callback_VOID__INT_POINTER));
	else if(marsh == etk_marshaller_BOOL__VOID)
	  etk_signal_disconnect(signal_name, obj, ETK_CALLBACK(callback_BOOL__VOID));
	else if(marsh == etk_marshaller_BOOL__DOUBLE)
	  etk_signal_disconnect(signal_name, obj, ETK_CALLBACK(callback_BOOL__DOUBLE));
	else if(marsh == etk_marshaller_BOOL__POINTER_POINTER)
	  etk_signal_disconnect(signal_name, obj, ETK_CALLBACK(callback_BOOL__POINTER_POINTER));
	else
 	  etk_signal_disconnect(signal_name, obj, ETK_CALLBACK(callback_VOID__VOID));

void
signal_disconnect_all(object, signal_name)
	SV *		object
	char *	        signal_name
      ALIAS:
	SignalDisconnectAll=1
	
	CODE:	
	Etk_Object * obj;
	obj = (Etk_Object *) SvObj(object, "Etk::Object");
	etk_signal_disconnect_all(signal_name, obj);

const char *
etk_object_name_get(object)
	Etk_Object *	object
      ALIAS:
	NameGet=1

Etk_Object *
etk_object_name_find(name)
	const char * name
	ALIAS:
	NameFind=1

void
etk_object_name_set(object, name)
	Etk_Object *	object
	char *	name
      ALIAS:
	NameSet=1

void
etk_object_destroy(object)
	Etk_Object *    object
	ALIAS:
	Destroy=1

void
etk_object_shutdown()
	ALIAS:
	Shutdown=1

void
etk_object_purge()
	ALIAS:
	Purge=1


