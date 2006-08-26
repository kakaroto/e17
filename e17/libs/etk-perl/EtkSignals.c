#include "EtkSignals.h"
#include "EtkTypes.h"



SV * GetSignalEvent(Etk_Object * object, void * value, Callback_Signal_Data * cbd) {

   HV *event_hv;
   SV *event_rv;
   
   if(!strcmp(cbd->signal_name, "mouse_up"))
     {	
        Etk_Event_Mouse_Up *event = value;   
	event_rv = newSVEventMouseUp(event);
     }
   else if(!strcmp(cbd->signal_name, "mouse_down"))
     {	
        Etk_Event_Mouse_Down *event = value;   
	event_rv = newSVEventMouseDown(event);
     }
   else if(!strcmp(cbd->signal_name, "mouse_move"))
     {
	Etk_Event_Mouse_Move *event = value;
	event_rv = newSVEventMouseMove(event);
     }
   else if(!strcmp(cbd->signal_name, "row_mouse_in") || 
		   !strcmp(cbd->signal_name, "row_mouse_out")||
		   !strcmp(cbd->signal_name, "row_selected")||
		   !strcmp(cbd->signal_name, "row_unselected")||
		   !strcmp(cbd->signal_name, "row_activated")||
		   !strcmp(cbd->signal_name, "cell_value_changed")
		   )
     {
	event_rv = newSVEtkTreeRowPtr((Etk_Tree_Row *)value);
     }
   else if(!strcmp(cbd->signal_name, "key_down"))
     {
	Etk_Event_Key_Down *key_event = value;
	event_rv = newSVEventKeyDown(key_event);
     }
   else if(!strcmp(cbd->signal_name, "key_up"))
     {
	Etk_Event_Key_Up *key_event = value;
	event_rv = newSVEventKeyUp(key_event);
     }
   else
     {
	event_rv =  &PL_sv_undef; 
     }
 

   return event_rv;


}


