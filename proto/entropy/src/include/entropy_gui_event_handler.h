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

/*File change*/
Entropy_Gui_Event_Handler* entropy_event_handler_file_change_handler();
Entropy_Gui_Event_Handler_Instance_Data* entropy_event_handler_file_change_instance_data(entropy_gui_event* event, 
	entropy_gui_component_instance* requestor) ;

/*File remove handler*/
Entropy_Gui_Event_Handler* entropy_event_handler_file_remove_handler();

Entropy_Gui_Event_Handler_Instance_Data* entropy_event_handler_file_remove_instance_data(entropy_gui_event* event, 
	entropy_gui_component_instance* requestor);

/*File remove directory handler*/
Entropy_Gui_Event_Handler* entropy_event_handler_file_remove_directory_handler();

Entropy_Gui_Event_Handler_Instance_Data* entropy_event_handler_file_remove_directory_instance_data(entropy_gui_event* event, 
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

/*Thumbnail available*/
Entropy_Gui_Event_Handler* entropy_event_handler_thumbnail_available_handler();
Entropy_Gui_Event_Handler_Instance_Data* entropy_event_handler_thumbnail_available_instance_data(entropy_gui_event* event, 
	entropy_gui_component_instance* requestor);

/*Progress*/
Entropy_Gui_Event_Handler* entropy_event_handler_progress_handler();
Entropy_Gui_Event_Handler_Instance_Data* entropy_event_handler_progress_instance_data(entropy_gui_event* event, 
	entropy_gui_component_instance* requestor); 

/*File list*/
Entropy_Gui_Event_Handler_Instance_Data* entropy_event_handler_folder_change_instance_data(entropy_gui_event* event, 
	entropy_gui_component_instance* requestor);
Entropy_Gui_Event_Handler* entropy_event_handler_folder_change_handler();
void entropy_event_handler_folder_change_cleanup(Entropy_Gui_Event_Handler_Instance_Data* data);

/*Metadata request*/
Entropy_Gui_Event_Handler* entropy_event_handler_metadata_request_handler();
Entropy_Gui_Event_Handler_Instance_Data* entropy_event_handler_metadata_request_instance_data(entropy_gui_event* event, 
	entropy_gui_component_instance* requestor);

/*Metadata available*/
Entropy_Gui_Event_Handler* entropy_event_handler_metadata_available_handler();
Entropy_Gui_Event_Handler_Instance_Data* entropy_event_handler_metadata_available_instance_data(entropy_gui_event* event, 
	entropy_gui_component_instance* requestor);

/*User interaction*/
Entropy_Gui_Event_Handler* entropy_event_handler_user_interaction_handler();
Entropy_Gui_Event_Handler_Instance_Data* entropy_event_handler_user_interaction_instance_data(entropy_gui_event* event, 
	entropy_gui_component_instance* requestor);

/*Extended Stat*/
Entropy_Gui_Event_Handler* entropy_event_handler_extended_stat_handler();
Entropy_Gui_Event_Handler_Instance_Data* entropy_event_handler_extended_stat_instance_data(entropy_gui_event* event, 
	entropy_gui_component_instance* requestor);

/*Metadata groups*/
Entropy_Gui_Event_Handler* entropy_event_handler_metadata_groups_handler();
Entropy_Gui_Event_Handler_Instance_Data* entropy_event_handler_metadata_groups_instance_data(entropy_gui_event* event, 
	entropy_gui_component_instance* requestor) ;

/*Copy Request */
Entropy_Gui_Event_Handler* entropy_event_handler_copy_request_handler();
Entropy_Gui_Event_Handler_Instance_Data* entropy_event_handler_copy_request_instance_data(entropy_gui_event* event, 
	entropy_gui_component_instance* requestor) ;

/*Cut request */
Entropy_Gui_Event_Handler* entropy_event_handler_cut_request_handler();
Entropy_Gui_Event_Handler_Instance_Data* entropy_event_handler_cut_request_instance_data(entropy_gui_event* event, 
	entropy_gui_component_instance* requestor);

/*Paste Request */
Entropy_Gui_Event_Handler* entropy_event_handler_paste_request_handler();
Entropy_Gui_Event_Handler_Instance_Data* 
entropy_event_handler_paste_request_instance_data(entropy_gui_event* event, 
	entropy_gui_component_instance* requestor) ;

/*Hover Request */
Entropy_Gui_Event_Handler* entropy_event_handler_hover_request_handler();
Entropy_Gui_Event_Handler_Instance_Data* 
entropy_event_handler_hover_request_instance_data(entropy_gui_event* event, 
			entropy_gui_component_instance* requestor);

/*DeHover Request */
Entropy_Gui_Event_Handler* entropy_event_handler_dehover_request_handler();
Entropy_Gui_Event_Handler_Instance_Data* 
entropy_event_handler_dehover_request_instance_data(entropy_gui_event* event, 
		                        entropy_gui_component_instance* requestor);


/*Auth Request */
Entropy_Gui_Event_Handler* entropy_event_handler_auth_request_handler();
void entropy_event_handler_auth_request_cleanup(Entropy_Gui_Event_Handler_Instance_Data* data);
Entropy_Gui_Event_Handler_Instance_Data* 
entropy_event_handler_auth_request_instance_data(entropy_gui_event* event, 
	entropy_gui_component_instance* requestor);

/*Metadata All */
Entropy_Gui_Event_Handler* entropy_event_handler_meta_all_request_handler();
Entropy_Gui_Event_Handler_Instance_Data* entropy_event_handler_metadata_all_instance_data(entropy_gui_event* event, 
			entropy_gui_component_instance* requestor);

#endif
