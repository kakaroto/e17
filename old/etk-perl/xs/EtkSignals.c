#include "EtkSignals.h"
#include "EtkTypes.h"



SV * GetSignalEvent_POINTER(Etk_Object * object, void * value, Callback_Signal_Data * cbd) {

   HV *event_hv;
   SV *event_rv;

   if(!strcmp(cbd->signal_name, "mouse-up"))
     {	
        Etk_Event_Mouse_Up *event = value;   
	event_rv = newSVEventMouseUp(event);
     }
   else if(!strcmp(cbd->signal_name, "mouse-down"))
     {	
        Etk_Event_Mouse_Down *event = value;   
	event_rv = newSVEventMouseDown(event);
     }
   else if(!strcmp(cbd->signal_name, "mouse-move"))
     {
	Etk_Event_Mouse_Move *event = value;
	event_rv = newSVEventMouseMove(event);
     }
   else if(!strcmp(cbd->signal_name, "row-mouse-in") || 
		   !strcmp(cbd->signal_name, "row-mouse-out")||
		   !strcmp(cbd->signal_name, "row-selected")||
		   !strcmp(cbd->signal_name, "row-unselected")||
		   !strcmp(cbd->signal_name, "row-activated")||
		   !strcmp(cbd->signal_name, "cell-value-changed")
		   )
     {
	event_rv = newSVObj(value, getClass("Etk_Tree_Row"));
     }
   else if(!strcmp(cbd->signal_name, "key-down"))
     {
	Etk_Event_Key_Down *key_event = value;
	event_rv = newSVEventKeyDown(key_event);
     }
   else if(!strcmp(cbd->signal_name, "key-up"))
     {
	Etk_Event_Key_Up *key_event = value;
	event_rv = newSVEventKeyUp(key_event);
     }
   else
     {
        printf("*** warning: undefined signal event (%s) please add to EtkSignals.c\n", cbd->signal_name);
	event_rv =  &PL_sv_undef; 
     }
 

   return event_rv;


}

void GetSignalEvent_POINTER_POINTER(Etk_Object * object, void * val1, void * val2, Callback_Signal_Data * cbd, SV **obj, SV **event) {

   if(!strcmp(cbd->signal_name, "row-clicked"))
     {
        Etk_Tree_Row * row = val1;
	Etk_Event_Mouse_Up *key_event = (Etk_Event_Mouse_Up *)val2;
	*obj = newSVObj(row, getClass("Etk_Tree_Row"));
	*event = newSVEventMouseUp(key_event);
     }

}

