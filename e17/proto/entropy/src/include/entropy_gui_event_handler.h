#ifndef _ENTROPY_GUI_EVENT_HANDLER_H_
#define _ENTROPY_GUI_EVENT_HANDLER_H_

#include "entropy.h"

struct Entropy_Gui_Event_Handler_Instance_Data {
	entropy_notify_event* notify;

	/*Ugly way of keeping things generic*/
	void* misc_data1;
	void* misc_data2;
};
typedef struct Entropy_Gui_Event_Handler_Instance_Data Entropy_Gui_Event_Handler_Instance_Data;


struct Entropy_Gui_Event_Handler {
	Entropy_Gui_Event_Handler_Instance_Data* 
		(*notify_event_cb)(entropy_gui_event* event, entropy_gui_component_instance* instance);
	void (*cleanup_cb)(struct Entropy_Gui_Event_Handler_Instance_Data*);
};
typedef struct Entropy_Gui_Event_Handler Entropy_Gui_Event_Handler;

Entropy_Gui_Event_Handler* entropy_gui_event_handler_new(
		Entropy_Gui_Event_Handler_Instance_Data* 
			(*notify_event_cb)(entropy_gui_event* event, entropy_gui_component_instance* instance),
		void (*cleanup_cb)(struct Entropy_Gui_Event_Handler_Instance_Data*));

void entropy_event_handler_instance_data_generic_cleanup(Entropy_Gui_Event_Handler_Instance_Data* data);

/*File create handler*/
Entropy_Gui_Event_Handler* entropy_event_handler_file_create_handler();

Entropy_Gui_Event_Handler_Instance_Data*
	entropy_event_handler_file_create_instance_data(entropy_gui_event* event, 
		entropy_gui_component_instance* requestor);

/*File remove handler*/
Entropy_Gui_Event_Handler* entropy_event_handler_file_remove_handler();

Entropy_Gui_Event_Handler_Instance_Data* entropy_event_handler_file_remove_instance_data(entropy_gui_event* event, 
	entropy_gui_component_instance* requestor);

/*File stat (outbound)*/
Entropy_Gui_Event_Handler* entropy_event_handler_file_stat_handler();
Entropy_Gui_Event_Handler_Instance_Data* entropy_event_handler_file_stat_instance_data(entropy_gui_event* event, 
	entropy_gui_component_instance* requestor);
void entropy_event_handler_file_stat_cleanup(Entropy_Gui_Event_Handler_Instance_Data* data);

/*File stat (inbound)*/
Entropy_Gui_Event_Handler* entropy_event_handler_file_stat_available_handler();
Entropy_Gui_Event_Handler_Instance_Data* entropy_event_handler_file_stat_available_instance_data(entropy_gui_event* event, 
	entropy_gui_component_instance* requestor);

/*File action*/
Entropy_Gui_Event_Handler* entropy_event_handler_file_action_handler();
Entropy_Gui_Event_Handler_Instance_Data* entropy_event_handler_file_action_instance_data(entropy_gui_event* event, 
	entropy_gui_component_instance* requestor);


#endif
