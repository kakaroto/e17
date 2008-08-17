#ifndef __ENTROPY_GUI_H_
#define __ENTROPY_GUI_H_

#define ENTROPY_GUI_EVENT_THUMBNAIL_AVAILABLE "entropy_gui_event_thumbnail_available"	      //A thumbnail is ready
#define ENTROPY_GUI_EVENT_FOLDER_CHANGE_CONTENTS "entropy_gui_event_folder_change_contents"   //A request for the contents of a dir
#define ENTROPY_GUI_EVENT_FOLDER_CHANGE_CONTENTS_EXTERNAL "entropy_gui_event_folder_change_contents_external"   //A request for the contents of a dir (external - SEE DOCS)
#define ENTROPY_GUI_EVENT_FOLDER_CHANGE_INFO "entropy_gui_event_folder_change_info"	      //A request for the name of a dir
#define ENTROPY_GUI_EVENT_FILE_CHANGE "entropy_gui_event_file_change"			      //A file change event
#define ENTROPY_GUI_EVENT_FILE_CREATE "entropy_gui_event_file_create"			      //A file change event
#define ENTROPY_GUI_EVENT_FILE_REMOVE "entropy_gui_event_file_remove"			      //A file change event
#define ENTROPY_GUI_EVENT_FILE_REMOVE_DIRECTORY "entropy_gui_event_file_remove_directory"			      //A file change event
#define ENTROPY_GUI_EVENT_ACTION_FILE "entropy_gui_event_action_file"			      //Execute an action
#define ENTROPY_GUI_EVENT_FILE_STAT   "entropy_gui_event_file_stat"			      //Request a stat of a file
#define ENTROPY_GUI_EVENT_FILE_STAT_AVAILABLE   "entropy_gui_event_file_stat_available"			      //Request a stat of a file
#define ENTROPY_GUI_EVENT_USER_INTERACTION_YES_NO_ABORT "entropy_gui_event_user_interaction_yes_no_abort"		      //User needs to respond in some way
#define ENTROPY_GUI_EVENT_FILE_METADATA "entropy_gui_event_file_metadata"  
#define ENTROPY_GUI_EVENT_FILE_METADATA_AVAILABLE "entropy_gui_event_file_metadata_available"
#define ENTROPY_GUI_EVENT_EXTENDED_STAT	"entropy_gui_event_extended_stat"	//Register that the next incoming stat for a file
										// is an extended stat - e.g. a properties dialog request
#define ENTROPY_GUI_EVENT_METADATA_GROUPS "entropy_gui_event_metadata_groups"   //Metadata groups available from file client
#define ENTROPY_GUI_EVENT_COPY_REQUEST "entropy_gui_event_copy_request"
#define ENTROPY_GUI_EVENT_CUT_REQUEST "entropy_gui_event_cut_request"
#define ENTROPY_GUI_EVENT_PASTE_REQUEST "entropy_gui_event_paste_request"

#define ENTROPY_GUI_EVENT_HOVER "entropy_gui_event_hover"
#define ENTROPY_GUI_EVENT_DEHOVER "entropy_gui_event_dehover"

#define ENTROPY_GUI_EVENT_AUTH_REQUEST "entropy_gui_event_auth_request"
#define ENTROPY_GUI_EVENT_META_ALL_REQUEST "entropy_gui_event_meta_all_request"

#define ENTROPY_TOOLKIT_EWL "ewl"
#define ENTROPY_TOOLKIT_ETK "etk"

/*File copy/move etc progress event*/
#define ENTROPY_GUI_EVENT_FILE_PROGRESS "entropy_gui_event_file_progress"

void entropy_gui_component_instance_disable(entropy_gui_component_instance*);
void entropy_gui_component_instance_enable(entropy_gui_component_instance*);

/*A structure for dual-referencing a file, and a gui component instance*/
typedef struct entropy_file_gui_component entropy_file_gui_component;
struct entropy_file_gui_component {
	entropy_generic_file* file;
	entropy_gui_component_instance* instance;
};

typedef struct entropy_file_progress_window {
        void* progress_window;
        void* file_from;
        void* file_to;
        void* progressbar;
} entropy_file_progress_window;


entropy_file_gui_component* entropy_file_gui_component_new();
void entropy_file_gui_component_destroy(entropy_file_gui_component*);
entropy_file_gui_component* entropy_file_gui_component_new_with_data(entropy_generic_file* file, 
					entropy_gui_component_instance* instance);

#endif

