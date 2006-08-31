#include <Etk.h>
#include "entropy.h"

static int _entropy_etk_options_visible = 0;
static Etk_Widget* _entropy_etk_options_dialog = NULL;
static Ecore_Hash* _entropy_global_options_hash;
static Etk_Widget* _entropy_etk_options_local_box;

typedef struct _Entropy_Etk_Options_Object {
	char* name;
	char* value;
} Entropy_Etk_Options_Object;

void entropy_etk_options_object_create(char* name) 
{
	Entropy_Etk_Options_Object* obj;

	obj = calloc(1, sizeof(Entropy_Etk_Options_Object));
	obj->name = strdup(name);

	ecore_hash_set(_entropy_global_options_hash, obj->name, obj);
}

/*CB Handlers*/
void entropy_etk_options_radio_generic_cb(Etk_Object* obj, void* data)
{
	char* name;
	Etk_Bool status;
	Entropy_Etk_Options_Object* opt;

	name = (char*)data;
	opt = ecore_hash_get(_entropy_global_options_hash, name);
	
	if (opt) {
		status = etk_toggle_button_active_get(ETK_TOGGLE_BUTTON(obj));	

		if (status == ETK_TRUE) {
			opt->value = "1";
		} else {
			opt->value = "0";
		}

		printf("Set '%s' for '%s'\n", opt->name, opt->value);
	}
}

void entropy_etk_options_dialog_frame_set(Etk_Object* obj, void* data)
{
	Etk_Widget* frame;
	Etk_Widget* widget;
	Evas_List* children;

	frame = data;

	for (children = etk_container_children_get(ETK_CONTAINER(_entropy_etk_options_local_box)); children; ) {
		widget = children->data;
		etk_container_remove(ETK_CONTAINER(_entropy_etk_options_local_box), widget);
			
		children = children->next;
	}

	etk_box_append(ETK_BOX(_entropy_etk_options_local_box), frame , ETK_BOX_START, ETK_BOX_NONE, 0);
	etk_widget_show_all(frame);
}

void entropy_etk_options_dialog_create()
{
	Etk_Widget* toolbar;
	Etk_Widget* button;
	Etk_Widget* vbox;
	Etk_Widget* frame;
	
	_entropy_global_options_hash = ecore_hash_new(ecore_str_hash, ecore_str_compare);
	
	_entropy_etk_options_dialog = etk_window_new();

	vbox = etk_vbox_new(ETK_FALSE,0);
	_entropy_etk_options_local_box = etk_vbox_new(ETK_FALSE,0);
	etk_container_add(ETK_CONTAINER(_entropy_etk_options_dialog), vbox);
	
	toolbar = etk_toolbar_new();
	etk_toolbar_orientation_set(ETK_TOOLBAR(toolbar), ETK_TOOLBAR_HORIZ);
	etk_toolbar_style_set(ETK_TOOLBAR(toolbar), ETK_TOOLBAR_ICONS);
	etk_box_append(ETK_BOX(vbox), toolbar, ETK_BOX_START, ETK_BOX_NONE, 0);

	etk_box_append(ETK_BOX(vbox),_entropy_etk_options_local_box , ETK_BOX_START, ETK_BOX_NONE, 0);
	
	/*General settings*/
	button = etk_tool_button_new_from_stock(ETK_STOCK_APPLICATIONS_SYSTEM);
	etk_toolbar_append(ETK_TOOLBAR(toolbar), button);

	/*General frame*/
	/*etk_signal_connect("pressed", ETK_OBJECT(button), ETK_CALLBACK(_entropy_etk_location_radio_cb), dialog );*/
	frame = etk_frame_new("General Settings");
	etk_box_append(ETK_BOX(_entropy_etk_options_local_box), frame, ETK_BOX_START, ETK_BOX_NONE, 0);
	etk_signal_connect("pressed", ETK_OBJECT(button), ETK_CALLBACK(entropy_etk_options_dialog_frame_set), frame);
	

	/*Advanced*/
	button = etk_tool_button_new_from_stock(ETK_STOCK_PREFERENCES_SYSTEM);
	etk_toolbar_append(ETK_TOOLBAR(toolbar), button);
	frame = etk_frame_new("Advanced Settings");
	etk_signal_connect("pressed", ETK_OBJECT(button), ETK_CALLBACK(entropy_etk_options_dialog_frame_set), frame);

	/*Thumbnail*/
	button = etk_tool_button_new_from_stock(ETK_STOCK_IMAGE_X_GENERIC);
	etk_toolbar_prepend(ETK_TOOLBAR(toolbar), button);

	frame = etk_frame_new("Thumbnail Settings");
	etk_signal_connect("pressed", ETK_OBJECT(button), ETK_CALLBACK(entropy_etk_options_dialog_frame_set), frame);	
	
}


void entropy_etk_options_dialog_show()
{
	if (!_entropy_etk_options_dialog)
		entropy_etk_options_dialog_create();

	etk_widget_show_all(_entropy_etk_options_dialog);
}
