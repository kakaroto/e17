#include <Etk.h>
#include "entropy.h"

static int _entropy_ekt_options_visible = 0;
Etk_Widget* _entropy_etk_options_dialog = NULL;

void entropy_etk_options_dialog_create()
{
	Etk_Widget* toolbar;
	Etk_Widget* button;
	Etk_Widget* vbox;
	
	_entropy_etk_options_dialog = etk_window_new();

	toolbar = etk_toolbar_new();
	etk_toolbar_orientation_set(ETK_TOOLBAR(toolbar), ETK_TOOLBAR_HORIZ);
	etk_toolbar_style_set(ETK_TOOLBAR(toolbar), ETK_TOOLBAR_ICONS);
	etk_container_add(ETK_CONTAINER(_entropy_etk_options_dialog), toolbar);
	

	button = etk_tool_button_new_from_stock(ETK_STOCK_SYSTEM_FILE_MANAGER);
	etk_toolbar_prepend(ETK_TOOLBAR(toolbar), button);
	
	
}


void entropy_etk_options_dialog_show()
{

	
	if (!_entropy_etk_options_dialog)
		entropy_etk_options_dialog_create();

	etk_widget_show_all(_entropy_etk_options_dialog);
}
