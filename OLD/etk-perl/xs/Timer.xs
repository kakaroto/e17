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

int
callback_timer(void *data)
{
   dSP;
   SV* cb;
   Callback_Timer_Data *cbd;
   int count;
   int ret = 0;
   
   cbd = data;   
   PUSHMARK(SP) ;
   if(cbd->perl_data)
     XPUSHs(sv_2mortal(newSVsv(cbd->perl_data)));      
   PUTBACK ;  
   
   count = call_sv(cbd->perl_callback, G_SCALAR);

   SPAGAIN;

   /* if the return value is incorrect, return 0 to end timer */
   if(count != 1)
      croak("Improper return value from timer callback!\n");
   
   ret = POPi;
   
   PUTBACK;
      
   return ret;
}



MODULE = Etk::Timer	PACKAGE = Etk::Timer 

Ecore_Timer *
new(class, interval, callback, data=&PL_sv_undef)
	SV * class
        double interval
	SV *    callback
        SV *    data
      CODE:        
        Callback_Timer_Data *cbd;
        
        cbd = calloc(1, sizeof(Callback_Timer_Data));
        cbd->perl_data = newSVsv(data);
        cbd->perl_callback = newSVsv(callback);
        RETVAL = ecore_timer_add(interval, callback_timer, cbd);
      OUTPUT:
        RETVAL
	
void
Delete(timer)
      Ecore_Timer * timer
    CODE:
      ecore_timer_del(timer);


