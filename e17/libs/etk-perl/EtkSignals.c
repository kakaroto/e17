#include "EtkSignals.h"
#include "EtkTypes.h"



SV * GetSignalEvent(Etk_Object * object, void * value, Callback_Signal_Data * cbd) {

   HV *event_hv;
   SV *event_rv;
   
   if(!strcmp(cbd->signal_name, "mouse_up") || !strcmp(cbd->signal_name, "mouse_down"))
     {	
        Etk_Event_Mouse_Up_Down *event = value;   
	event_rv = newSVEventMouseUpDown(event);
     }
   else if(!strcmp(cbd->signal_name, "mouse_move"))
     {
	Etk_Event_Mouse_Move *event = value;
	event_rv = newSVEventMouseMove(event);
     }
   else if(!strcmp(cbd->signal_name, "row_mouse_in") || !strcmp(cbd->signal_name, "row_mouse_out"))
     {
	event_rv = newSVEtkTreeRowPtr((Etk_Tree_Row *)value);
     }
   else if(!strcmp(cbd->signal_name, "key_down") || !strcmp(cbd->signal_name, "key_up"))
     {
	Etk_Event_Key_Up_Down *key_event = value;
	event_rv = newSVEventKeyUpDown(key_event);

     }
   else
     {
	event_rv =  &PL_sv_undef; 
     }
 

   return event_rv;


}


