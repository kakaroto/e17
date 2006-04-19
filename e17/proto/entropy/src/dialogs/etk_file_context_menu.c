#include "entropy.h"
#include <Etk.h>
#include "entropy_etk_context_menu.h"
#include "etk_directory_add_dialog.h"
#include "etk_properties_dialog.h"
#include "etk_file_rename_dialog.h"

static Etk_Widget* _entropy_etk_context_menu = NULL;
static Etk_Widget* _entropy_etk_context_menu_open_with = NULL;
static Etk_Widget* _entropy_etk_context_menu_open_with_item = NULL;
static entropy_generic_file* _entropy_etk_context_menu_current_folder = NULL;
static entropy_generic_file* _entropy_etk_context_menu_current_file = NULL;
static entropy_gui_component_instance* _entropy_etk_context_menu_current_instance = NULL;

static void (*entropy_etk_context_menu_stat_cb)(void*, entropy_generic_file*) = NULL;
static void *entropy_etk_context_menu_stat_cb_data = NULL;


typedef enum _Etk_Menu_Item_Type
{
   ETK_MENU_ITEM_NORMAL,
   ETK_MENU_ITEM_SEPARATOR
} Etk_Menu_Item_Type;


static Etk_Widget *_entropy_etk_menu_item_new(Etk_Menu_Item_Type item_type, const char *label,
   Etk_Stock_Id stock_id, Etk_Menu_Shell *menu_shell, Etk_Widget *statusbar)
{
   Etk_Widget *menu_item = NULL;
   
   switch (item_type)
   {
      case ETK_MENU_ITEM_NORMAL:
         menu_item = etk_menu_item_image_new_with_label(label);
         break;
      case ETK_MENU_ITEM_SEPARATOR:
         menu_item = etk_menu_item_separator_new();
         break;
      default:
         return NULL;
   }
   if (stock_id != ETK_STOCK_NO_STOCK)
   {
      Etk_Widget *image;
      
      image = etk_image_new_from_stock(stock_id, ETK_STOCK_SMALL);
      etk_menu_item_image_set(ETK_MENU_ITEM_IMAGE(menu_item), ETK_IMAGE(image));
   }
   etk_menu_shell_append(menu_shell, ETK_MENU_ITEM(menu_item));
   
   /*etk_signal_connect("selected", ETK_OBJECT(menu_item), ETK_CALLBACK(_etk_test_menu_item_selected_cb), statusbar);
   etk_signal_connect("deselected", ETK_OBJECT(menu_item), ETK_CALLBACK(_etk_test_menu_item_deselected_cb), statusbar);*/
   
   return menu_item;
}


static void _entropy_etk_context_menu_properties_cb(Etk_Object *object, void *data)
{
	if (_entropy_etk_context_menu_current_file) {
		if (entropy_etk_context_menu_stat_cb) 
			(*entropy_etk_context_menu_stat_cb)(entropy_etk_context_menu_stat_cb_data, 
				_entropy_etk_context_menu_current_file);

		entropy_event_stat_request(_entropy_etk_context_menu_current_file, 
			_entropy_etk_context_menu_current_instance);
	}
}


void _entropy_etk_context_menu_directory_add_cb(Etk_Object *object, void *data)
{
	if (_entropy_etk_context_menu_current_folder) {
		etk_directory_add_dialog_create(_entropy_etk_context_menu_current_folder);
	}
}

static void
_open_with_item_cb(Etk_Object *obj, void *data)
{
	int i;
	entropy_gui_event* gui_event;
	
	i = (int)etk_object_data_get(obj, "INDEX");

	if (_entropy_etk_context_menu_current_file) {
		gui_event = entropy_malloc (sizeof (entropy_gui_event));
		gui_event->event_type =
			entropy_core_gui_event_get (ENTROPY_GUI_EVENT_ACTION_FILE);
		gui_event->data = _entropy_etk_context_menu_current_file;
		gui_event->key = i;
		entropy_core_layout_notify_event (_entropy_etk_context_menu_current_instance, 
			gui_event, ENTROPY_EVENT_GLOBAL);
	}
}

static void 
_open_folder_window_cb(Etk_Object* obj, void* data)
{
	entropy_gui_event* gui_event;
	
	if (_entropy_etk_context_menu_current_file) {
		printf("New folder handler...\n");
		
		gui_event = entropy_malloc (sizeof (entropy_gui_event));
		gui_event->event_type =
			entropy_core_gui_event_get (ENTROPY_GUI_EVENT_ACTION_FILE);
		gui_event->data = _entropy_etk_context_menu_current_file;
		gui_event->hints |= ENTROPY_GUI_EVENT_HINT_WINDOW_NEW;
		entropy_core_layout_notify_event (_entropy_etk_context_menu_current_instance,
			gui_event, ENTROPY_EVENT_GLOBAL);
	}
	
}

static void
_entropy_etk_context_menu_file_rename_cb(Etk_Object *object, void *data)
{
	etk_file_rename_dialog_create(_entropy_etk_context_menu_current_file);
}

static void
_entropy_etk_context_menu_popup_cb(Etk_Object *object, void *data)
{
	Entropy_Config_Mime_Binding* binding;
	Entropy_Config_Mime_Binding_Action* action;
	Evas_List* l;
	entropy_generic_file* file;

	file = _entropy_etk_context_menu_current_file;

	if (file && strlen(file->mime_type)) {
		
		   binding = entropy_config_mime_binding_for_type_get(file->mime_type);

		   if (ETK_MENU_ITEM(_entropy_etk_context_menu_open_with_item)->submenu) {
			   etk_menu_item_submenu_set(ETK_MENU_ITEM(_entropy_etk_context_menu_open_with_item), NULL);
			   etk_object_destroy(ETK_OBJECT(_entropy_etk_context_menu_open_with));
			   _entropy_etk_context_menu_open_with = NULL;
		   }

		   /*If it's a folder, add an 'Open in new layout..' entry*/
		   if (!strcmp(file->mime_type, "file/folder")) {
			   Etk_Widget* w;
			   
			   _entropy_etk_context_menu_open_with = etk_menu_new();
			   etk_menu_item_submenu_set(ETK_MENU_ITEM(_entropy_etk_context_menu_open_with_item), 
					  ETK_MENU(_entropy_etk_context_menu_open_with)); 

			   w = _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, "Open in new window", 
				  ETK_STOCK_EDIT_COPY, ETK_MENU_SHELL(_entropy_etk_context_menu_open_with),NULL);
			   etk_signal_connect("activated", ETK_OBJECT(w), ETK_CALLBACK(_open_folder_window_cb), NULL);
		   }

	
		   if (binding) {
			   Etk_Widget* w; 
			   int i=0;
		   
			   if (!_entropy_etk_context_menu_open_with) {
				   _entropy_etk_context_menu_open_with = etk_menu_new();
				   etk_menu_item_submenu_set(ETK_MENU_ITEM(_entropy_etk_context_menu_open_with_item), 
						  ETK_MENU(_entropy_etk_context_menu_open_with)); 
			   }

			   for (l = binding->actions; l; ) {
				   action = l->data;

				   w = _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _(action->app_description),
					ETK_STOCK_EDIT_COPY, ETK_MENU_SHELL(_entropy_etk_context_menu_open_with),NULL);
				   etk_object_data_set(ETK_OBJECT(w), "INDEX", (int*)i);

				   etk_signal_connect("activated", ETK_OBJECT(w), ETK_CALLBACK(_open_with_item_cb), NULL);
				   
				   l = l->next;
				   i++;
			   }
		  }
	}
	
}


void entropy_etk_context_menu_init()
{
	Etk_Widget* menu_item;
	Etk_Widget* menu;
	Etk_Widget* new_menu;
   
	
	
   	if (!_entropy_etk_context_menu) {
		menu = etk_menu_new();
		_entropy_etk_context_menu = menu;
		
		etk_signal_connect("popped_up", ETK_OBJECT(menu), ETK_CALLBACK(_entropy_etk_context_menu_popup_cb), NULL);

		_entropy_etk_context_menu_open_with_item =  
		   _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Open With"), ETK_STOCK_EDIT_COPY, ETK_MENU_SHELL(menu),NULL);
		_entropy_etk_context_menu_open_with = etk_menu_new();
		etk_menu_item_submenu_set(ETK_MENU_ITEM(_entropy_etk_context_menu_open_with_item), ETK_MENU(_entropy_etk_context_menu_open_with)); 
 
		_entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Copy"), ETK_STOCK_EDIT_COPY, ETK_MENU_SHELL(menu),NULL);
		_entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Cut"), ETK_STOCK_EDIT_CUT, ETK_MENU_SHELL(menu),NULL);
		_entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Paste"), ETK_STOCK_EDIT_PASTE, ETK_MENU_SHELL(menu),NULL);
		_entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Delete"), ETK_STOCK_EDIT_COPY, ETK_MENU_SHELL(menu),NULL);
		menu_item = _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Rename"), ETK_STOCK_EDIT_COPY, ETK_MENU_SHELL(menu),NULL);
		etk_signal_connect("activated", ETK_OBJECT(menu_item), ETK_CALLBACK(_entropy_etk_context_menu_file_rename_cb), NULL);
		menu_item = _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Properties"), ETK_STOCK_EDIT_COPY, ETK_MENU_SHELL(menu),NULL);
		etk_signal_connect("activated", ETK_OBJECT(menu_item), ETK_CALLBACK(_entropy_etk_context_menu_properties_cb), NULL);

		menu_item =  _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("New"), ETK_STOCK_EDIT_COPY, ETK_MENU_SHELL(menu),NULL);
		new_menu = etk_menu_new();
		etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(new_menu));

		menu_item =  _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Folder.."), ETK_STOCK_EDIT_COPY, ETK_MENU_SHELL(new_menu),NULL);
		etk_signal_connect("activated", ETK_OBJECT(menu_item), ETK_CALLBACK(_entropy_etk_context_menu_directory_add_cb), NULL);
	}

}

void entropy_etk_context_menu_stat_cb_register(
	void (*cb)(void*, entropy_generic_file*), void* data) {


	entropy_etk_context_menu_stat_cb = cb;
	entropy_etk_context_menu_stat_cb_data = data;

}

void entropy_etk_context_menu_popup(entropy_gui_component_instance* instance, entropy_generic_file* current_file)
{
        _entropy_etk_context_menu_current_file = current_file;
	_entropy_etk_context_menu_current_instance = instance;

	entropy_etk_context_menu_stat_cb = NULL;
	
	if (!_entropy_etk_context_menu) 
		entropy_etk_context_menu_init();

	etk_menu_popup(ETK_MENU(_entropy_etk_context_menu));
}
