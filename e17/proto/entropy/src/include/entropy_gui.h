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

/*File copy/move etc progress event*/
#define ENTROPY_GUI_EVENT_FILE_PROGRESS "entropy_gui_event_file_progress"

void entropy_gui_component_instance_disable(entropy_gui_component_instance*);
void entropy_gui_component_instance_enable(entropy_gui_component_instance*);

#endif

