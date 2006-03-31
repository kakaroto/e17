#include "entropy.h"
#include "entropy_gui_event_handler.h"

Entropy_Gui_Event_Handler* entropy_gui_event_handler_new(
		Entropy_Gui_Event_Handler_Instance_Data* 
			(*notify_event_cb)(entropy_gui_event* event, entropy_gui_component_instance* instance),
		void (*cleanup_cb)(struct Entropy_Gui_Event_Handler_Instance_Data*) )
{
	Entropy_Gui_Event_Handler* handler = entropy_malloc(sizeof(Entropy_Gui_Event_Handler));

	handler->notify_event_cb =  notify_event_cb;
	handler->cleanup_cb = cleanup_cb;

	return handler;
}

void entropy_event_handler_instance_data_generic_cleanup(Entropy_Gui_Event_Handler_Instance_Data* data)
{
	if (data->notify)
		entropy_notify_event_destroy(data->notify);

	entropy_free(data);

}

/*File create*/
Entropy_Gui_Event_Handler* entropy_event_handler_file_create_handler()
{
	return entropy_gui_event_handler_new(
			entropy_event_handler_file_create_notify_event,
			entropy_event_handler_instance_data_generic_cleanup);
}

Entropy_Gui_Event_Handler_Instance_Data* entropy_event_handler_file_create_notify_event(entropy_gui_event* event, 
		entropy_gui_component_instance* requestor) 
{
	Entropy_Gui_Event_Handler_Instance_Data* data = entropy_malloc(sizeof(Entropy_Gui_Event_Handler_Instance_Data));
	entropy_notify_event* ev = entropy_notify_event_new();
	ev->event_type = ENTROPY_NOTIFY_FILE_CREATE;
	ev->processed = 1;
	ev->data = event->data;

	data->notify = ev;

	return data;
}


/*----------------------*/
