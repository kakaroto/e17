#include <Etk.h>
#include "entropy.h"
#include <evfs/evfs.h>

void entropy_etk_about_dialog_close(Etk_Object* obj, void* data)
{
	etk_object_destroy(ETK_OBJECT(data));
}

void entropy_etk_about_dialog_show()
{
	Etk_Widget* dialog;
	Etk_Widget *logo;
	Etk_Widget *hbox, *vbox;
	Etk_Widget* ok;
	

	dialog = etk_window_new();
	etk_window_title_set(ETK_WINDOW(dialog), "About..");

	logo = etk_image_new_from_file(PACKAGE_DATA_DIR "/icons/entropy.png", NULL);
	hbox = etk_hbox_new(ETK_FALSE,1);
	vbox = etk_vbox_new(ETK_FALSE,1);
	
	etk_container_add(ETK_CONTAINER(dialog), vbox);
	etk_box_append(ETK_BOX(hbox), logo, ETK_BOX_START, ETK_BOX_NONE, 0);
	etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_NONE, 0);

	ok = etk_button_new_from_stock(ETK_STOCK_DIALOG_OK);
	etk_signal_connect("pressed", ETK_OBJECT(ok), entropy_etk_about_dialog_close,dialog);

	etk_box_append(ETK_BOX(vbox), ok, ETK_BOX_START, ETK_BOX_NONE, 0);
	
	etk_widget_show_all(dialog);
}
