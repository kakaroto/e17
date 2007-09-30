#ifndef _ETK__SIGNALS_H___
#define _ETK__SIGNALS_H___

#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#define NEED_newRV_noinc
#include "../ppport.h"

#ifdef _
#undef _
#endif

#include <Etk.h>

typedef struct _Callback_Signal_Data Callback_Signal_Data;
typedef struct _Notification_Callback_Data Notification_Callback_Data;
typedef struct _Callback_Tree_Compare_Data Callback_Tree_Compare_Data;
typedef struct _Callback_Timer_Data Callback_Timer_Data;

/* Carries info for normal signals */
struct _Callback_Signal_Data
{
   char       *signal_name;   /* etk signal name */
   int        signal_code;   /* etk signal code */
   Etk_Object *object;        /* object signal is connected to */
   SV         *perl_object;   /* reference to the perl object */
   SV         *perl_callback; /* perl callback to be called */
   void       *perl_data;     /* perl data to pass to the perl callback */
};

/* Carries info for the notification callback */
struct _Notification_Callback_Data
{
   Etk_Object	*object;
   SV		*perl_callback;
   void		*perl_data;
};

/* Carries info for tree (column) compare callback */
struct _Callback_Tree_Compare_Data
{
   Etk_Object *object;          /* the column */
   SV         *perl_callback;   /* perl function that does the comparison */
   SV         *perl_data;       /* data to be passed to the above function */
};

/* Carries info when we have a timer callback */
struct _Callback_Timer_Data
{
   SV *perl_callback; /* the perl callback to call per timer tick */
   SV *perl_data;     /* data to pass to the perl callback */
};



SV * GetSignalEvent_POINTER(Etk_Object * object, void * value, Callback_Signal_Data * cbd);
void GetSignalEvent_POINTER_POINTER(Etk_Object * object, void * val1, void * val2, Callback_Signal_Data * cbd, SV ** obj, SV ** event);
	 

#endif

