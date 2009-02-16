#include "ephoto.h"

static void destroy_main_window(Ewl_Widget *w, void *event, void *data);
static void fullscreen_main_window(Ewl_Widget *w, void *event, void *data);
Ephoto *em;

void create_main_window(void) {
	Ewl_Widget *window, *vbox, *mbar, *menu, *menu_item;
	Ewl_Widget *view_box;
	Ecore_List *fsystem, *images;
	char buf[4096];

	em = NULL;	
	em = calloc(1, sizeof(Ephoto));

	fsystem = ecore_list_new();
	ephoto_set_fsystem(fsystem);

	images = ecore_dlist_new();
	ephoto_set_images(images);

	getcwd(buf, 4096);
	ephoto_set_current_directory(strdup(buf));

	window = ewl_window_new();
	ewl_window_title_set(EWL_WINDOW(window), "Ephoto");
	ewl_object_size_request(EWL_OBJECT(window), 640, 480);
	ewl_callback_append(window, EWL_CALLBACK_DELETE_WINDOW,
				destroy_main_window, NULL);
	ewl_widget_show(window);
	ephoto_set_main_window(window);

	vbox = ewl_vbox_new();
	ewl_box_spacing_set(EWL_BOX(vbox), 5);
	ewl_container_child_append(EWL_CONTAINER(window), vbox);
	ewl_widget_show(vbox);

	mbar = ewl_hmenubar_new();
	ewl_object_fill_policy_set(EWL_OBJECT(mbar), EWL_FLAG_FILL_HFILL);
	ewl_container_child_append(EWL_CONTAINER(vbox), mbar);
	ewl_widget_show(mbar);	

	menu = ewl_menu_new();
	ewl_button_label_set(EWL_BUTTON(menu), "File");
	ewl_object_fill_policy_set(EWL_OBJECT(menu), EWL_FLAG_FILL_NONE);
	ewl_container_child_append(EWL_CONTAINER(mbar), menu);
	ewl_widget_show(menu);

	menu_item = ewl_menu_item_new();
        ewl_button_label_set(EWL_BUTTON(menu_item), "Fullscreen");
        ewl_button_image_set(EWL_BUTTON(menu_item),
                                PACKAGE_DATA_DIR "/images/stock_fullscreen.png", NULL);
        ewl_object_alignment_set(EWL_OBJECT(menu_item),
                                        EWL_FLAG_ALIGN_CENTER);
        ewl_object_fill_policy_set(EWL_OBJECT(menu_item),
                                        EWL_FLAG_FILL_ALL);
        ewl_container_child_append(EWL_CONTAINER(menu), menu_item);
        ewl_callback_append(menu_item, EWL_CALLBACK_CLICKED,
                                fullscreen_main_window, NULL);
        ewl_widget_show(menu_item);

	menu_item = ewl_menu_item_new();
	ewl_button_label_set(EWL_BUTTON(menu_item), "Exit");
	ewl_button_image_set(EWL_BUTTON(menu_item), 
				PACKAGE_DATA_DIR "/images/exit.png", NULL);
	ewl_object_alignment_set(EWL_OBJECT(menu_item), 
					EWL_FLAG_ALIGN_CENTER);
	ewl_object_fill_policy_set(EWL_OBJECT(menu_item), 
					EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(menu), menu_item);
	ewl_callback_append(menu_item, EWL_CALLBACK_CLICKED, 
				destroy_main_window, NULL);
	ewl_widget_show(menu_item);

	menu = ewl_menu_new();
        ewl_button_label_set(EWL_BUTTON(menu), "Slideshow");
        ewl_object_fill_policy_set(EWL_OBJECT(menu), EWL_FLAG_FILL_NONE);
        ewl_container_child_append(EWL_CONTAINER(mbar), menu);
        ewl_widget_show(menu);

	menu_item = ewl_menu_item_new();
        ewl_button_label_set(EWL_BUTTON(menu_item), "Settings");
        ewl_button_image_set(EWL_BUTTON(menu_item),
                                PACKAGE_DATA_DIR "/images/preferences-system.png", NULL);
        ewl_object_alignment_set(EWL_OBJECT(menu_item),
                                        EWL_FLAG_ALIGN_CENTER);
        ewl_object_fill_policy_set(EWL_OBJECT(menu_item),
                                        EWL_FLAG_FILL_ALL);
        ewl_container_child_append(EWL_CONTAINER(menu), menu_item);
        //ewl_callback_append(menu_item, EWL_CALLBACK_CLICKED,
        //                        destroy_main_window, NULL);
        ewl_widget_show(menu_item);

	menu_item = ewl_menu_item_new();
        ewl_button_label_set(EWL_BUTTON(menu_item), "Slideshow");
        ewl_button_image_set(EWL_BUTTON(menu_item),
                                PACKAGE_DATA_DIR "/images/x-office-presentation.png", NULL);
        ewl_object_alignment_set(EWL_OBJECT(menu_item),
                                        EWL_FLAG_ALIGN_CENTER);
        ewl_object_fill_policy_set(EWL_OBJECT(menu_item),
                                        EWL_FLAG_FILL_ALL);
        ewl_container_child_append(EWL_CONTAINER(menu), menu_item);
        //ewl_callback_append(menu_item, EWL_CALLBACK_CLICKED,
        //                        destroy_main_window, NULL);
        ewl_widget_show(menu_item);

	view_box = ewl_notebook_new();
	ewl_notebook_tabbar_visible_set(EWL_NOTEBOOK(view_box), FALSE);
	ewl_object_fill_policy_set(EWL_OBJECT(view_box), EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(vbox), view_box);
	ewl_widget_show(view_box);
	ephoto_set_view_box(view_box);

	add_normal_view(view_box);
	add_single_view(view_box);
}

static void fullscreen_main_window(Ewl_Widget *w, void *event, void *data) {
	Ewl_Widget *win;

	win = ephoto_get_main_window();

	if (!ewl_window_fullscreen_get(EWL_WINDOW(win)))
		ewl_window_fullscreen_set(EWL_WINDOW(win), TRUE);
	else
		ewl_window_fullscreen_set(EWL_WINDOW(win), FALSE);
}

static void destroy_main_window(Ewl_Widget *w, void *event, void *data) {
	ewl_widget_destroy(ephoto_get_main_window());
	ecore_list_destroy(em->fsystem);
	ecore_dlist_destroy(em->images);
	free(em->current_directory);
	free(em);
	ewl_main_quit();
}

