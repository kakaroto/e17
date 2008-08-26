#include <Etk.h>
#include "entropy.h"

static Etk_Widget* _entropy_etk_options_dialog = NULL;
static Ecore_Hash* _entropy_global_options_hash;
static Etk_Widget* _entropy_etk_options_local_box;

typedef struct _Entropy_Etk_Options_Object {
	char* name;
	char* value;
} Entropy_Etk_Options_Object;

Entropy_Etk_Options_Object* entropy_etk_options_object_create(char* name) 
{
	Entropy_Etk_Options_Object* obj;
	char* val;

	obj = calloc(1, sizeof(Entropy_Etk_Options_Object));
	obj->name = strdup(name);
	if ( (val = entropy_config_misc_item_str_get(name))) {
		obj->value = strdup(val);
	}

	ecore_hash_set(_entropy_global_options_hash, obj->name, obj);

	return obj;
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

void entropy_etk_options_slider_generic_cb(Etk_Object* obj, double value, void* data)
{
	char* name;
	char px[10];
	Entropy_Etk_Options_Object* opt;

	name = (char*)data;
	
	opt = ecore_hash_get(_entropy_global_options_hash, name);	
	if (opt) {
		snprintf(px,sizeof(px), "%.0f", value);
		if (opt->value) free(opt->value);
		opt->value = strdup(px);

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
		etk_container_remove(ETK_WIDGET(widget));
			
		children = children->next;
	}

	etk_box_append(ETK_BOX(_entropy_etk_options_local_box), frame , ETK_BOX_START, ETK_BOX_NONE, 0);
	etk_widget_show_all(frame);
}

void entropy_etk_options_dialog_close(Etk_Object* obj, void* data)
{
	Entropy_Etk_Options_Object* c_obj;
	Ecore_List* keys;
	char* key;
	
	if (!((int)data ==1)) etk_widget_hide(_entropy_etk_options_dialog);

	if ((int)data == 0 || (int)data == 1 ) {
		printf("Save config selected..\n");

		keys = ecore_hash_keys(_entropy_global_options_hash);
		while ((key = ecore_list_first_remove(keys))) {
			c_obj = ecore_hash_get(_entropy_global_options_hash, key);
			
			if (c_obj->value) {
				printf("'%s' -> '%s'\n", key, c_obj->value);
				entropy_config_misc_item_str_set(key,c_obj->value, ENTROPY_CONFIG_LOC_HASH);
			}
		}
		ecore_list_destroy(keys);
	}
}

void etk_options_dialog_slider_cb(Etk_Object* obj, double value, void* data)
{
	Etk_Widget* label;
	char px[10];

	label = data;
	snprintf(px,sizeof(px), "%.0f", value);
	etk_label_set(ETK_LABEL(label), px);

	
}

Etk_Widget* etk_options_dialog_checkbox_new(char* label, char* config_item)
{
	Etk_Widget* check;
	
        check = etk_check_button_new_with_label(label);
	etk_signal_connect("toggled", ETK_OBJECT(check), 
		ETK_CALLBACK(entropy_etk_options_radio_generic_cb), config_item);

	if (entropy_config_misc_is_set(config_item))
		etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(check), ETK_TRUE);
	else
		etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(check), ETK_FALSE);

	return check;
}

Etk_Widget* etk_options_dialog_radiobutton_new(char* label, char* config_item, Etk_Widget* from)
{
	Etk_Widget* radio;
	
        if (!from) 
		radio = etk_radio_button_new_with_label(label,NULL);
	else
		radio = etk_radio_button_new_with_label_from_widget(label, ETK_RADIO_BUTTON(from));
	
	etk_signal_connect("toggled", ETK_OBJECT(radio), 
		ETK_CALLBACK(entropy_etk_options_radio_generic_cb), config_item);

	if (entropy_config_misc_is_set(config_item))
		etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(radio), ETK_TRUE);
	else
		etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(radio), ETK_FALSE);

	return radio;
}

void entropy_etk_options_dialog_create()
{
	Etk_Widget* toolbar;
	Etk_Widget* button;
	Etk_Widget* vbox;
	Etk_Widget* frame;
	Etk_Widget* iframe;
	Etk_Widget* ivbox;
	Etk_Widget* iivbox;
	Etk_Widget* radio;
	Etk_Widget* hbox;
	Etk_Widget* slider;
	Etk_Widget* label;
	
	_entropy_etk_options_dialog = etk_window_new();

	vbox = etk_vbox_new(ETK_FALSE,0);
	_entropy_etk_options_local_box = etk_vbox_new(ETK_FALSE,0);
	etk_container_add(ETK_CONTAINER(_entropy_etk_options_dialog), vbox);
	
	toolbar = etk_toolbar_new();
	etk_toolbar_orientation_set(ETK_TOOLBAR(toolbar), ETK_TOOLBAR_HORIZ);
	etk_toolbar_style_set(ETK_TOOLBAR(toolbar), ETK_TOOLBAR_ICON);
	etk_box_append(ETK_BOX(vbox), toolbar, ETK_BOX_START, ETK_BOX_NONE, 0);

	etk_box_append(ETK_BOX(vbox),_entropy_etk_options_local_box , ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
	
	/*General settings*/
	button = etk_tool_button_new_from_stock(ETK_STOCK_APPLICATIONS_SYSTEM);
	etk_toolbar_append(ETK_TOOLBAR(toolbar), button, ETK_BOX_START);

	/*General frame*/
	frame = etk_frame_new("General Settings");
	etk_box_append(ETK_BOX(_entropy_etk_options_local_box), frame, ETK_BOX_START, ETK_BOX_NONE, 0);
	etk_signal_connect("pressed", ETK_OBJECT(button), ETK_CALLBACK(entropy_etk_options_dialog_frame_set), frame);
	
	ivbox = etk_vbox_new(ETK_FALSE,0);
	etk_container_add(ETK_CONTAINER(frame), ivbox);
	   iframe = etk_frame_new("Default Local Viewer");
	   etk_box_append(ETK_BOX(ivbox), iframe, ETK_BOX_START, ETK_BOX_NONE, 0);
	      iivbox = etk_vbox_new(ETK_FALSE,0);
	      etk_container_add(ETK_CONTAINER(iframe), iivbox);

	   radio=etk_options_dialog_radiobutton_new("Icon view", "general.iconviewer", NULL);
	   etk_box_append(ETK_BOX(iivbox),radio , ETK_BOX_START, ETK_BOX_NONE, 0);
	   etk_box_append(ETK_BOX(iivbox), etk_options_dialog_radiobutton_new(
			"List view", "general.listviewer", radio), ETK_BOX_START, ETK_BOX_NONE, 0);
	   
	   etk_box_append(ETK_BOX(ivbox), 
			   etk_options_dialog_checkbox_new("Show trackback viewer", "general.trackback"), 
			   ETK_BOX_START, ETK_BOX_NONE, 0); 

	   etk_box_append(ETK_BOX(ivbox), 
			   etk_options_dialog_checkbox_new("Show tree view", "general.treeviewer"), 
			   ETK_BOX_START, ETK_BOX_NONE, 0); 

   	   etk_box_append(ETK_BOX(ivbox), 
			   etk_options_dialog_checkbox_new("Show folders before files", "general.presortfolders"), 
			   ETK_BOX_START, ETK_BOX_NONE, 0); 
	      
           
	   etk_box_append(ETK_BOX(ivbox), 
			   etk_options_dialog_checkbox_new("Show hidden and backup files", "general.hiddenbackup"), 
			   ETK_BOX_START, ETK_BOX_NONE, 0); 
			   
	   iframe = etk_frame_new("Icon View Settings");
	   etk_box_append(ETK_BOX(ivbox), iframe, ETK_BOX_START, ETK_BOX_NONE, 0);
	      iivbox = etk_vbox_new(ETK_FALSE,0);
	      etk_container_add(ETK_CONTAINER(iframe), iivbox);

	      label = etk_label_new("Icon size (pixels)");
	      etk_box_append(ETK_BOX(iivbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);

	      hbox = etk_hbox_new(ETK_FALSE,0);  
	      etk_box_append(ETK_BOX(iivbox), hbox, ETK_BOX_START, ETK_BOX_NONE, 0);
	      slider = etk_hslider_new(10,128, 48, 1, 1);
	      etk_box_append(ETK_BOX(hbox), slider, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
	      label = etk_label_new("");
	      etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);
	      etk_signal_connect("value-changed", ETK_OBJECT(slider), ETK_CALLBACK(etk_options_dialog_slider_cb), 
			      label);
	      etk_signal_connect("value-changed", ETK_OBJECT(slider), ETK_CALLBACK(entropy_etk_options_slider_generic_cb), 
			      "general.iconsize");
	   

	   iframe = etk_frame_new("List View Settings");
	   etk_box_append(ETK_BOX(ivbox), iframe, ETK_BOX_START, ETK_BOX_NONE, 0);



	   

	/*Advanced*/
	button = etk_tool_button_new_from_stock(ETK_STOCK_PREFERENCES_SYSTEM);
	etk_toolbar_append(ETK_TOOLBAR(toolbar), button, ETK_BOX_START);
	frame = etk_frame_new("Advanced Settings");
	etk_signal_connect("pressed", ETK_OBJECT(button), ETK_CALLBACK(entropy_etk_options_dialog_frame_set), frame);

	/*Thumbnail*/
	button = etk_tool_button_new_from_stock(ETK_STOCK_IMAGE_X_GENERIC);
	etk_toolbar_append(ETK_TOOLBAR(toolbar), button, ETK_BOX_START);

	frame = etk_frame_new("Thumbnail Settings");
	etk_signal_connect("pressed", ETK_OBJECT(button), ETK_CALLBACK(entropy_etk_options_dialog_frame_set), frame);	


	//etk_widget_size_request_set(_entropy_etk_options_dialog, 560, 460);


	hbox = etk_hbox_new(ETK_FALSE,5);
	etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_NONE, 0);

	button = etk_button_new_from_stock(ETK_STOCK_DIALOG_OK);
	etk_box_append(ETK_BOX(hbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);
	etk_signal_connect("pressed", ETK_OBJECT(button), ETK_CALLBACK(entropy_etk_options_dialog_close), (void*)0);
	button = etk_button_new_from_stock(ETK_STOCK_DIALOG_APPLY);
	etk_box_append(ETK_BOX(hbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);
	etk_signal_connect("pressed", ETK_OBJECT(button), ETK_CALLBACK(entropy_etk_options_dialog_close), (void*)1);
	button = etk_button_new_from_stock(ETK_STOCK_DIALOG_CANCEL);
	etk_box_append(ETK_BOX(hbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);
	etk_signal_connect("pressed", ETK_OBJECT(button), ETK_CALLBACK(entropy_etk_options_dialog_close), (void*)2);
}


void entropy_etk_options_dialog_show()
{
	if (!_entropy_etk_options_dialog) {
		_entropy_global_options_hash = ecore_hash_new(ecore_str_hash, ecore_str_compare);
		
		entropy_etk_options_object_create("general.listviewer");
		entropy_etk_options_object_create("general.iconviewer");
		entropy_etk_options_object_create("general.trackback");
		entropy_etk_options_object_create("general.presortfolders");
		entropy_etk_options_object_create("general.hiddenbackup");
		entropy_etk_options_object_create("general.iconsize");
		entropy_etk_options_object_create("general.treeviewer");
		
		entropy_etk_options_dialog_create();
	}

	etk_widget_show_all(_entropy_etk_options_dialog);
}
