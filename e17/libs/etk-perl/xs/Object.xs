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

static Etk_Bool
callback_VOID(Etk_Object *object, void *data)
{
   dSP;
   Callback_Signal_Data *cbd = NULL;
   int count;
   Etk_Bool ret = ETK_TRUE;
   SV * sv_ret;

   cbd = data;
   
   PUSHMARK(SP);
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_object)));
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_data)));
   PUTBACK ;
      
   count = call_sv(cbd->perl_callback, G_SCALAR);

   SPAGAIN;

   if (count == 1) {
   	sv_ret = POPs;
	if (SvIOK(sv_ret))
		ret = SvIV(sv_ret);
	/* Assume true by default */
	if (ret != ETK_FALSE) ret = ETK_TRUE;
   }

   PUTBACK;
   FREETMPS;
   LEAVE;

   return ret;
   
}

static Etk_Bool
callback_INT(Etk_Object *object, int value, void *data)
{
   dSP;
   Callback_Signal_Data *cbd = NULL;
   int count;
   Etk_Bool ret = ETK_TRUE;
   SV * sv_ret;

   cbd = data;

   PUSHMARK(SP) ;
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_object)));
   XPUSHs(sv_2mortal(newSViv(value)));
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_data)));   
   PUTBACK ;
      
   count = call_sv(cbd->perl_callback, G_SCALAR);

   SPAGAIN;

   if (count == 1) {
   	sv_ret = POPs;
	if (SvIOK(sv_ret))
		ret = SvIV(sv_ret);
	
	if (ret != ETK_FALSE) ret = ETK_TRUE;
   }

   PUTBACK;
   FREETMPS;
   LEAVE;

   return ret;


}

static Etk_Bool
callback_DOUBLE(Etk_Object *object, double value, void *data)
{
   dSP;
   Callback_Signal_Data *cbd = NULL;
   int count;
   Etk_Bool ret = ETK_TRUE;
   SV * sv_ret;

   cbd = data;

   PUSHMARK(SP) ;
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_object)));
   XPUSHs(sv_2mortal(newSVnv(value)));
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_data)));   
   PUTBACK ;

   count = call_sv(cbd->perl_callback, G_SCALAR);

   SPAGAIN;

   if (count == 1) {
   	sv_ret = POPs;
	if (SvIOK(sv_ret))
		ret = SvIV(sv_ret);
	
	if (ret != ETK_FALSE) ret = ETK_TRUE;
   }

   PUTBACK;
   FREETMPS;
   LEAVE;

   return ret;

}

static Etk_Bool
callback_POINTER(Etk_Object *object, void *value, void *data)
{
   dSP;
   Callback_Signal_Data *cbd = NULL;
   int count;
   Etk_Bool ret = ETK_TRUE;
   SV *event_rv;
   SV * sv_ret;
   cbd = data;   

   event_rv = GetSignalEvent_POINTER(object, value, cbd);
   
   PUSHMARK(SP) ;
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_object)));
   XPUSHs(sv_2mortal(event_rv));
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_data)));   
   PUTBACK ;
      
   count = call_sv(cbd->perl_callback, G_SCALAR);

   SPAGAIN;

   if (count == 1) {
   	sv_ret = POPs;
	if (SvIOK(sv_ret))
		ret = SvIV(sv_ret);
	
	if (ret != ETK_FALSE) ret = ETK_TRUE;
   }

   PUTBACK;
   FREETMPS;
   LEAVE;

   return ret;

}

static Etk_Bool
callback_POINTER_POINTER(Etk_Object *object, void *val1, void *val2, void *data)
{
   dSP;
   Callback_Signal_Data *cbd = NULL;
   int count;
   Etk_Bool ret = ETK_TRUE;
   SV * sv_ret;

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
      
 
   count = call_sv(cbd->perl_callback, G_SCALAR);

   SPAGAIN;

   if (count == 1) {
   	sv_ret = POPs;
	if (SvIOK(sv_ret))
		ret = SvIV(sv_ret);
	
	if (ret != ETK_FALSE) ret = ETK_TRUE;
   }

   PUTBACK;
   FREETMPS;
   LEAVE;

   return ret;

}

static Etk_Bool
callback_INT_INT(Etk_Object *object, int val1, int val2, void *data)
{
   dSP;
   Callback_Signal_Data *cbd = NULL;
   int count;
   Etk_Bool ret = ETK_TRUE;
   SV * sv_ret;

   cbd = data;

   PUSHMARK(SP) ;
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_object)));
   XPUSHs(sv_2mortal(newSViv(val1)));
   XPUSHs(sv_2mortal(newSViv(val2)));
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_data)));   
   PUTBACK ;
      
   count = call_sv(cbd->perl_callback, G_SCALAR);

   SPAGAIN;

   if (count == 1) {
   	sv_ret = POPs;
	if (SvIOK(sv_ret))
		ret = SvIV(sv_ret);
	
	if (ret != ETK_FALSE) ret = ETK_TRUE;
   }

   PUTBACK;
   FREETMPS;
   LEAVE;

   return ret;


}


static Etk_Bool
callback_OBJECT(Etk_Object *object, Etk_Object *obj2, void *data)
{
   dSP;
   Callback_Signal_Data *cbd = NULL;
   int count;
   Etk_Bool ret = ETK_TRUE;
   SV * sv_ret;

   cbd = data;

   PUSHMARK(SP) ;
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_object)));
// XXX add the object
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_data)));   
   PUTBACK ;
      
   count = call_sv(cbd->perl_callback, G_SCALAR);

   SPAGAIN;

   if (count == 1) {
   	sv_ret = POPs;
	if (SvIOK(sv_ret))
		ret = SvIV(sv_ret);
	
	if (ret != ETK_FALSE) ret = ETK_TRUE;
   }

   PUTBACK;
   FREETMPS;
   LEAVE;

   return ret;


}


const Etk_Signal_Callback *
__etk_signal_connect_full(SV *signal, SV *object, SV *callback, SV *data, Etk_Bool swapped, Etk_Bool after)
{
	dSP;

	Callback_Signal_Data *cbd = NULL;
	Etk_Object * obj;
	const Etk_Signal_Callback * ret = NULL;
	int signal_code;
	char * signal_name = NULL;
	Etk_Type *type = NULL;
	Etk_Signal *sig = NULL;
	Etk_Marshaller marsh;

	ENTER;
	SAVETMPS;

	obj = (Etk_Object *)SvObj(object, "Etk::Object");
	type = etk_object_object_type_get(obj);

	cbd = calloc(1, sizeof(Callback_Signal_Data));
	cbd->object = obj;
	cbd->perl_object = newSVsv(object);
	cbd->perl_data = newSVsv(data);
	cbd->perl_callback = newSVsv(callback);	

#define S(A) \
	if ( signal_name == NULL ) \
		ret = etk_signal_connect_full_by_code(signal_code,obj,ETK_CALLBACK(A),cbd,swapped,after); \
	else \
		ret = etk_signal_connect_full_by_name(signal_name,obj,ETK_CALLBACK(A),cbd,swapped,after); 


	if (SvPOK(signal)) {
		signal_name = SvPV_nolen(signal);
		cbd->signal_name = strdup(signal_name);
		sig = etk_type_signal_get_by_name(type, signal_name);
	} else {
		signal_code = SvIV(signal);
		cbd->signal_code = signal_code;
		sig = etk_type_signal_get(type, signal_code);
	}

	marsh = etk_signal_marshaller_get(sig);
	if (marsh == etk_marshaller_VOID) {
		S(callback_VOID);
	} else if (marsh == etk_marshaller_INT) {
		S(callback_INT);
	} else if (marsh == etk_marshaller_DOUBLE) {
		S(callback_DOUBLE);
	} else if (marsh == etk_marshaller_POINTER) {
		S(callback_POINTER);
	} else if (marsh == etk_marshaller_POINTER_POINTER) {
		S(callback_POINTER_POINTER);
	} else if (marsh == etk_marshaller_OBJECT) {
		S(callback_OBJECT);
	} else {
		S(callback_VOID);
	}

	PUTBACK;
	FREETMPS;
	LEAVE;

	return ret;
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


const Etk_Signal_Callback *
signal_connect(object, signal, callback, data=NULL)
	SV *		object
	SV *		signal
	SV *	        callback
	SV *            data
      ALIAS:
	SignalConnect=1
       
	CODE:	
	RETVAL = __etk_signal_connect_full(signal, newSVsv(object),
		newSVsv(callback), newSVsv(data), 
		ETK_FALSE, ETK_FALSE);
	OUTPUT:
	RETVAL

const Etk_Signal_Callback *
signal_connect_after(object, signal, callback, data=NULL)
	SV *		object
	SV *	        signal
	SV *	        callback
	SV *            data
      ALIAS:
	SignalConnectAfter=1
	
	CODE:	
	RETVAL = __etk_signal_connect_full(signal, newSVsv(object), newSVsv(callback), 
		newSVsv(data), ETK_FALSE, ETK_TRUE);
	OUTPUT:
	RETVAL

const Etk_Signal_Callback * 
signal_connect_full(object, signal, callback, data, swapped, after)
	SV *		object
	SV *	        signal
	SV *	        callback
	SV *            data
	Etk_Bool	swapped
	Etk_Bool	after
      ALIAS:
	SignalConnectFull=1
	CODE:
	RETVAL = __etk_signal_connect_full(signal, newSVsv(object), newSVsv(callback),
		newSVsv(data), swapped, after);
	OUTPUT:
	RETVAL

	
const Etk_Signal_Callback *
signal_connect_swapped(object, signal, callback, data=NULL)
	SV *		object
	SV *	        signal
	SV *	        callback
	SV *            data
      ALIAS:
	SignalConnectSwapped=1
	
	CODE:	
	RETVAL = __etk_signal_connect_full(signal, newSVsv(object), newSVsv(callback),
		newSVsv(data), ETK_TRUE, ETK_FALSE);
	OUTPUT:
	RETVAL

void
signal_disconnect(object, signal_name, callback)
	Etk_Object *	object
	char *	        signal_name
	Etk_Signal_Callback *callback
      ALIAS:
	SignalDisconnect=1
	
	CODE:	
	// TODO

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


