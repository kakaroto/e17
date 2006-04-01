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
			entropy_event_handler_file_create_instance_data,
			entropy_event_handler_instance_data_generic_cleanup);
}

Entropy_Gui_Event_Handler_Instance_Data* entropy_event_handler_file_create_instance_data(entropy_gui_event* event, 
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


/*File remove*/
Entropy_Gui_Event_Handler* entropy_event_handler_file_remove_handler()
{
	return entropy_gui_event_handler_new(
			entropy_event_handler_file_remove_instance_data,
			entropy_event_handler_instance_data_generic_cleanup);
	
}

Entropy_Gui_Event_Handler_Instance_Data* entropy_event_handler_file_remove_instance_data(entropy_gui_event* event, 
	entropy_gui_component_instance* requestor) 
{
	
	Entropy_Gui_Event_Handler_Instance_Data* data = entropy_malloc(sizeof(Entropy_Gui_Event_Handler_Instance_Data));
	entropy_notify_event* ev = entropy_notify_event_new();
	ev->event_type = ENTROPY_NOTIFY_FILE_REMOVE;
	ev->processed = 1;
	ev->data = event->data;

	data->notify = ev;

	return data;
}
/*---------------------------*/


/*File stat (outbound) */
Entropy_Gui_Event_Handler* entropy_event_handler_file_stat_handler()
{
	return entropy_gui_event_handler_new(
			entropy_event_handler_file_stat_instance_data,
			entropy_event_handler_file_stat_cleanup);
	
}

Entropy_Gui_Event_Handler_Instance_Data* entropy_event_handler_file_stat_instance_data(entropy_gui_event* event, 
	entropy_gui_component_instance* requestor) 
{
	
	Entropy_Gui_Event_Handler_Instance_Data* data = entropy_malloc(sizeof(Entropy_Gui_Event_Handler_Instance_Data));

	entropy_notify_event *ev = entropy_notify_event_new();
	entropy_file_request* request = entropy_malloc(sizeof(entropy_file_request));
	
	/*Set up the request..*/
	request->file = event->data;
	request->core = entropy_core_get_core();
	request->requester = requestor->layout_parent;
	/*--------------------------------------------*/

	ev->event_type = ENTROPY_NOTIFY_FILE_STAT_EXECUTED;
	ev->processed = 1;

	/*Actually request the stat*/
	entropy_plugin_filesystem_filestat_get(request);

	data->notify = ev;
	data->misc_data1 = request;

	return data;
}

void entropy_event_handler_file_stat_cleanup(Entropy_Gui_Event_Handler_Instance_Data* data)
{
	if (data->notify)
		entropy_notify_event_destroy(data->notify);

	/*Free the file request*/
	entropy_free(data->misc_data1);
			
	entropy_free(data);

}
/*-------------------------------------*/


/*File stat (inbound) */
Entropy_Gui_Event_Handler* entropy_event_handler_file_stat_available_handler()
{
	return entropy_gui_event_handler_new(
			entropy_event_handler_file_stat_available_instance_data,
			entropy_event_handler_instance_data_generic_cleanup);
	
}

Entropy_Gui_Event_Handler_Instance_Data* entropy_event_handler_file_stat_available_instance_data(entropy_gui_event* event, 
	entropy_gui_component_instance* requestor) 
{
	
	Entropy_Gui_Event_Handler_Instance_Data* data = entropy_malloc(sizeof(Entropy_Gui_Event_Handler_Instance_Data));
	
	entropy_notify_event* ev = entropy_notify_event_new();
	ev->event_type = ENTROPY_NOTIFY_FILE_STAT_AVAILABLE; 
	ev->processed = 1;

	ev->return_struct = event->data;
	if (event->data) ev->data = ((entropy_file_stat*)event->data)->file;

	data->notify = ev;

	return data;
}
/*-----------------------------------*/

/*Action file*/
Entropy_Gui_Event_Handler* entropy_event_handler_file_action_handler()
{
	return entropy_gui_event_handler_new(
			entropy_event_handler_file_action_instance_data,
			entropy_event_handler_instance_data_generic_cleanup);
	
}

Entropy_Gui_Event_Handler_Instance_Data* entropy_event_handler_file_action_instance_data(entropy_gui_event* event, 
	entropy_gui_component_instance* requestor) 
{
	
	Entropy_Gui_Event_Handler_Instance_Data* data = entropy_malloc(sizeof(Entropy_Gui_Event_Handler_Instance_Data));
	
	entropy_notify_event* ev = entropy_notify_event_new();
	ev->event_type = ENTROPY_NOTIFY_FILE_ACTION; 
	ev->key = event->key;
	ev->processed = 1;
	ev->data = event->data; /*An entropy generic file*/

	data->notify = ev;

	return data;
}


/*--------------------------------------*/
