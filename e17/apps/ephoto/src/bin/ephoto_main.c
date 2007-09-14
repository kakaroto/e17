#include "ephoto.h"

/*Ewl Callbacks*/
static void destroy(Ewl_Widget *w, void *event, void *data);
static void window_fullscreen(Ewl_Widget *w, void *event, void *data);

/*Ephoto Global Variables*/
Ephoto_Main *em;

/*Destroy the Main Window*/
static void destroy(Ewl_Widget *w, void *event, void *data)
{
        ewl_widget_destroy(em->win);
	ecore_hash_destroy(em->types);
	ecore_list_destroy(em->directories);
	ecore_dlist_destroy(em->images);
	free(em->current_directory);
	free(em);
	ewl_main_quit();
	return;
}

static void window_fullscreen(Ewl_Widget *w, void *event, void *data)
{
	int fullscreen;

	fullscreen = ewl_window_fullscreen_get(EWL_WINDOW(em->win));
	if(!fullscreen)
	{
		ewl_window_fullscreen_set(EWL_WINDOW(em->win), 1);
	}
	else
	{
		ewl_window_fullscreen_set(EWL_WINDOW(em->win), 0);
	}
	return;
}

/*Create the Main Ephoto Window*/
void create_main(void)
{
	char buf[PATH_MAX];
	Ewl_Widget *vbox, *hbox, *vsep, *button;
	Ewl_Widget *mb, *menu, *mi;

	/*Initialize our main Structure*/
	em = NULL;
	em = calloc(1, sizeof(Ephoto_Main));

	/*Set up our Lists*/
	em->directories = ecore_list_new();
	em->images = ecore_dlist_new();

	/*Set up the image types hash*/
        em->types = ecore_hash_new(ecore_str_hash, ecore_str_compare);
        ecore_hash_set(em->types, "image/gif", "image");
        ecore_hash_set(em->types, "image/jpeg", "image");
        ecore_hash_set(em->types, "image/png", "image");
        ecore_hash_set(em->types, "image/svg+xml", "image");
        ecore_hash_set(em->types, "image/tiff", "image");

	getcwd(buf, PATH_MAX);
        buf[sizeof(buf)-1] = '\0';
        em->current_directory = strdup(buf);

	em->bwin = NULL;

	em->win = add_window("Ephoto!", 600, 475, destroy, NULL);

	vbox = add_box(em->win, EWL_ORIENTATION_VERTICAL, 5);
	ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_ALL);

	mb = add_menubar(vbox);
	
	menu = add_menu(mb, "File");
	mi = add_menu_item(menu, "Exit", 
				PACKAGE_DATA_DIR "/images/exit.png", 
							destroy, NULL);
	menu = add_menu(mb, "Albums");

	menu = add_menu(mb, "View");
	mi = add_menu_item(menu, "Browser",
				NULL, show_browser, NULL);

	menu = add_menu(mb, "Help");
        mi = add_menu_item(menu, "About",
                                PACKAGE_DATA_DIR "/images/stock_help.png",
                                                        about_dialog, NULL);

	em->view = add_box(vbox, EWL_ORIENTATION_HORIZONTAL, 1);
	ewl_object_fill_policy_set(EWL_OBJECT(em->view), EWL_FLAG_FILL_ALL);

	em->view_box = ewl_notebook_new();
	ewl_notebook_tabbar_visible_set(EWL_NOTEBOOK(em->view_box), 0);
	ewl_object_fill_policy_set(EWL_OBJECT(em->view_box), EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(em->view), em->view_box);
	ewl_widget_show(em->view_box);

	add_normal_view(em->view_box);

	add_single_view(em->view_box);

	add_list_view(em->view_box);

	add_edit_view(em->view_box);	

	em->toolbar = add_box(vbox, EWL_ORIENTATION_HORIZONTAL, 5);
	ewl_object_alignment_set(EWL_OBJECT(em->toolbar), EWL_FLAG_ALIGN_CENTER);
	ewl_object_fill_policy_set(EWL_OBJECT(em->toolbar), EWL_FLAG_FILL_SHRINK);

        button = add_button(em->toolbar, NULL, 
				PACKAGE_DATA_DIR "/images/normal_view.png", 
							show_normal_view, NULL);
        ewl_button_image_size_set(EWL_BUTTON(button), 30, 30);
	ewl_attach_tooltip_text_set(button, _("Normal View"));
	ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_SHRINK);
	ewl_object_alignment_set(EWL_OBJECT(button), EWL_FLAG_ALIGN_CENTER);

        button = add_button(em->toolbar, NULL, 
				PACKAGE_DATA_DIR "/images/list_view.png", 
							show_list_view, NULL);
        ewl_button_image_size_set(EWL_BUTTON(button), 30, 30);
	ewl_attach_tooltip_text_set(button, _("List View"));      
	ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_SHRINK);
        ewl_object_alignment_set(EWL_OBJECT(button), EWL_FLAG_ALIGN_CENTER);

	button = add_button(em->toolbar, NULL, 
				PACKAGE_DATA_DIR "/images/image.png", 
							show_single_view, NULL);
        ewl_button_image_size_set(EWL_BUTTON(button), 30, 30);
	ewl_attach_tooltip_text_set(button, _("Single View"));
	ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_SHRINK);
        ewl_object_alignment_set(EWL_OBJECT(button), EWL_FLAG_ALIGN_CENTER);

	button = add_button(em->toolbar, NULL,
                                PACKAGE_DATA_DIR "/images/edit_view.png",
                                                        show_edit_view, NULL);
        ewl_button_image_size_set(EWL_BUTTON(button), 30, 30);
        ewl_attach_tooltip_text_set(button, _("Move to edit view"));
        ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_SHRINK);
        ewl_object_alignment_set(EWL_OBJECT(button), EWL_FLAG_ALIGN_CENTER);

	vsep = ewl_vseparator_new();
	ewl_container_child_append(EWL_CONTAINER(em->toolbar), vsep);
	ewl_widget_show(vsep);

        button = add_button(em->toolbar, NULL, 
				PACKAGE_DATA_DIR "/images/get_exif.png", 
								NULL, NULL);
        ewl_button_image_size_set(EWL_BUTTON(button), 30, 30);
        ewl_attach_tooltip_text_set(button, _("You do not have libexif 0.6.13"));
        ewl_object_alignment_set(EWL_OBJECT(button), EWL_FLAG_ALIGN_CENTER);
        ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_SHRINK);
#ifdef BUILD_EXIF_SUPPORT
        ewl_callback_append(button, EWL_CALLBACK_CLICKED, display_exif_dialog, 
									NULL);
        ewl_attach_tooltip_text_set(button, _("View Exif Data"));
#endif

	button = add_button(em->toolbar, NULL, 
			PACKAGE_DATA_DIR "/images/stock_fullscreen.png",
						 window_fullscreen, NULL);
        ewl_button_image_size_set(EWL_BUTTON(button), 30, 30);
	ewl_attach_tooltip_text_set(button, _("Toggle Fullscreen"));
	ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_SHRINK);
        ewl_object_alignment_set(EWL_OBJECT(button), EWL_FLAG_ALIGN_CENTER);

        button = add_button(em->toolbar, NULL, 
			PACKAGE_DATA_DIR "/images/x-office-presentation.png", 
							start_slideshow, NULL);
	ewl_button_image_size_set(EWL_BUTTON(button), 30, 30);
	ewl_attach_tooltip_text_set(button, _("Start a Slideshow"));
	ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_SHRINK);
        ewl_object_alignment_set(EWL_OBJECT(button), EWL_FLAG_ALIGN_CENTER);

	show_browser(NULL, NULL, NULL);

	return;
}


