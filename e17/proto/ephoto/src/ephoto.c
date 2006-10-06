#include "ephoto.h"

Main *m;
char *current_directory;
Ecore_List *current_thumbs;

int
main(int argc, char **argv)
{
 Ewl_Widget *vbox, *hbox, *border, *menubar, *menu, *menu_item;
 Ewl_Widget *hseparator, *vseparator, *image, *text;
 
 if (!ewl_init(&argc, argv))
 {
  printf("Unable to init ewl\n");
  return 1;
 }
 
 /* NLS */
 setlocale( LC_ALL, "" );
 bindtextdomain( "ephoto", "/usr/share/locale" );
 textdomain( "ephoto" );
 
 m = NULL;
 current_thumbs = ecore_dlist_new();
 current_directory = strdup(getenv("HOME"));
 m = calloc(1, sizeof(Main));

 m->win = ewl_window_new();
 ewl_window_title_set(EWL_WINDOW(m->win), "Ephoto");
 ewl_window_name_set(EWL_WINDOW(m->win), "Ephoto");
 ewl_object_size_request(EWL_OBJECT(m->win), 580, 460);
 ewl_callback_append(m->win, EWL_CALLBACK_DELETE_WINDOW, destroy_cb, NULL);
 ewl_widget_show(m->win);

 vbox = ewl_vbox_new();
 ewl_box_spacing_set(EWL_BOX(vbox), 1);
 ewl_container_child_append(EWL_CONTAINER(m->win), vbox);
 ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_ALL);
 ewl_widget_show(vbox);

 menubar = ewl_hmenubar_new();
 ewl_container_child_append(EWL_CONTAINER(vbox), menubar);
 ewl_object_fill_policy_set(EWL_OBJECT(menubar), EWL_FLAG_FILL_HFILL);
 ewl_widget_show(menubar);

 menu = ewl_menu_new();
 ewl_button_label_set(EWL_BUTTON(menu), gettext("File"));
 ewl_container_child_append(EWL_CONTAINER(menubar), menu);
 ewl_object_fill_policy_set(EWL_OBJECT(menu), EWL_FLAG_FILL_NONE);
 ewl_widget_show(menu);

 menu_item = ewl_menu_item_new();
 ewl_button_image_set(EWL_BUTTON(menu_item), 
		      PACKAGE_DATA_DIR "/images/exit.png", NULL);
 ewl_button_label_set(EWL_BUTTON(menu_item), gettext("Exit"));
 ewl_object_alignment_set(EWL_OBJECT(menu_item), EWL_FLAG_ALIGN_CENTER);
 ewl_container_child_append(EWL_CONTAINER(menu), menu_item);
 ewl_callback_append(menu_item, EWL_CALLBACK_CLICKED, destroy_cb, NULL);
 ewl_object_fill_policy_set(EWL_OBJECT(menu_item), EWL_FLAG_FILL_ALL);
 ewl_widget_show(menu_item);

 menu = ewl_menu_new();
 ewl_button_label_set(EWL_BUTTON(menu), gettext("Albums"));
 ewl_container_child_append(EWL_CONTAINER(menubar), menu);
 ewl_object_fill_policy_set(EWL_OBJECT(menu), EWL_FLAG_FILL_NONE);
 ewl_widget_show(menu);

 menu_item = ewl_menu_item_new();
 ewl_button_image_set(EWL_BUTTON(menu_item),
		      PACKAGE_DATA_DIR "/images/add.png", NULL);
 ewl_button_label_set(EWL_BUTTON(menu_item), gettext("Add Album"));
 ewl_object_alignment_set(EWL_OBJECT(menu_item), EWL_FLAG_ALIGN_CENTER);
 ewl_container_child_append(EWL_CONTAINER(menu), menu_item);
 ewl_object_fill_policy_set(EWL_OBJECT(menu_item), EWL_FLAG_FILL_ALL);
 ewl_widget_show(menu_item);

 menu_item = ewl_menu_item_new();
 ewl_button_image_set(EWL_BUTTON(menu_item),
                      PACKAGE_DATA_DIR "/images/remove.png", NULL);
 ewl_button_label_set(EWL_BUTTON(menu_item), gettext("Remove Album"));
 ewl_object_alignment_set(EWL_OBJECT(menu_item), EWL_FLAG_ALIGN_CENTER);
 ewl_container_child_append(EWL_CONTAINER(menu), menu_item);
 ewl_object_fill_policy_set(EWL_OBJECT(menu_item), EWL_FLAG_FILL_ALL);
 ewl_widget_show(menu_item);
 
 menu = ewl_menu_new();
 ewl_button_label_set(EWL_BUTTON(menu), gettext("Viewer"));
 ewl_container_child_append(EWL_CONTAINER(menubar), menu);
 ewl_object_fill_policy_set(EWL_OBJECT(menu), EWL_FLAG_FILL_NONE);
 ewl_widget_show(menu);     
 
 menu_item = ewl_menu_item_new();
 ewl_button_image_set(EWL_BUTTON(menu_item),
		      PACKAGE_DATA_DIR "/images/emblem-photos.png", NULL);
 ewl_button_label_set(EWL_BUTTON(menu_item), gettext("Image Viewer"));
 ewl_object_alignment_set(EWL_OBJECT(menu_item), EWL_FLAG_ALIGN_CENTER);
 ewl_container_child_append(EWL_CONTAINER(menu), menu_item);
 ewl_callback_append(menu_item, EWL_CALLBACK_CLICKED, view_images, NULL);
 ewl_object_fill_policy_set(EWL_OBJECT(menu_item), EWL_FLAG_FILL_ALL);
 ewl_widget_show(menu_item); 

 menu = ewl_menu_new();
 ewl_button_label_set(EWL_BUTTON(menu), gettext("Slideshow"));
 ewl_container_child_append(EWL_CONTAINER(menubar), menu);
 ewl_object_fill_policy_set(EWL_OBJECT(menu), EWL_FLAG_FILL_NONE);
 ewl_widget_show(menu);

 menu_item = ewl_menu_item_new();
 ewl_button_image_set(EWL_BUTTON(menu_item), 
		      PACKAGE_DATA_DIR "/images/preferences-system.png", NULL);
 ewl_button_label_set(EWL_BUTTON(menu_item), gettext("Configure Slideshow"));
 ewl_object_alignment_set(EWL_OBJECT(menu_item), EWL_FLAG_ALIGN_CENTER);
 ewl_container_child_append(EWL_CONTAINER(menu), menu_item);
 ewl_callback_append(menu_item, EWL_CALLBACK_CLICKED, create_slideshow_config, NULL);
 ewl_object_fill_policy_set(EWL_OBJECT(menu_item), EWL_FLAG_FILL_ALL);
 ewl_widget_show(menu_item);
       
 menu_item = ewl_menu_item_new();
 ewl_button_image_set(EWL_BUTTON(menu_item), 
		      PACKAGE_DATA_DIR "/images/x-office-presentation.png", NULL);
 ewl_button_label_set(EWL_BUTTON(menu_item), gettext("Start Slideshow"));
 ewl_object_alignment_set(EWL_OBJECT(menu_item), EWL_FLAG_ALIGN_CENTER);
 ewl_container_child_append(EWL_CONTAINER(menu), menu_item);
 ewl_callback_append(menu_item, EWL_CALLBACK_CLICKED, start_slideshow, NULL);
 ewl_object_fill_policy_set(EWL_OBJECT(menu_item), EWL_FLAG_FILL_ALL);
 ewl_widget_show(menu_item);
 
 hseparator = ewl_hseparator_new();
 ewl_object_alignment_set(EWL_OBJECT(hseparator), EWL_FLAG_ALIGN_CENTER);
 ewl_container_child_append(EWL_CONTAINER(vbox), hseparator);
 ewl_object_fill_policy_set(EWL_OBJECT(hseparator), EWL_FLAG_FILL_ALL);
 ewl_widget_show(hseparator);
 
 hbox = ewl_hbox_new();
 ewl_box_spacing_set(EWL_BOX(hbox), 5);
 ewl_container_child_append(EWL_CONTAINER(vbox), hbox);
 ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_HFILL |
		 				 EWL_FLAG_FILL_VSHRINK);
 ewl_widget_show(hbox);

 image = ewl_image_new();
 ewl_container_child_append(EWL_CONTAINER(hbox), image);
 ewl_image_file_set(EWL_IMAGE(image), 
		    PACKAGE_DATA_DIR "/images/go-up.png", NULL);
 ewl_callback_append(image, EWL_CALLBACK_CLICKED, go_up, NULL);
 ewl_widget_show(image);

 image = ewl_image_new();
 ewl_container_child_append(EWL_CONTAINER(hbox), image);
 ewl_image_file_set(EWL_IMAGE(image), 
		    PACKAGE_DATA_DIR "/images/go-home.png", NULL);
 ewl_callback_append(image, EWL_CALLBACK_CLICKED, go_home, NULL);
 ewl_widget_show(image);

 image = ewl_image_new();
 ewl_container_child_append(EWL_CONTAINER(hbox), image);
 ewl_image_file_set(EWL_IMAGE(image), 
		    PACKAGE_DATA_DIR "/images/emblem-favorite.png", NULL);
 ewl_widget_show(image);
      
 text = ewl_text_new();
 ewl_text_text_set(EWL_TEXT(text), gettext("  Location:"));
 ewl_object_alignment_set(EWL_OBJECT(text), EWL_FLAG_ALIGN_CENTER);
 ewl_object_maximum_size_set(EWL_OBJECT(text), 66, 15);
 ewl_container_child_append(EWL_CONTAINER(hbox), text);
 ewl_widget_show(text);

 m->entry = ewl_entry_new();
 ewl_text_text_set(EWL_TEXT(m->entry), current_directory);
 ewl_container_child_append(EWL_CONTAINER(hbox), m->entry);
 ewl_callback_append(m->entry, EWL_CALLBACK_VALUE_CHANGED, entry_change, NULL);
 ewl_widget_show(m->entry);
 
 hseparator = ewl_hseparator_new();
 ewl_object_alignment_set(EWL_OBJECT(hseparator), EWL_FLAG_ALIGN_CENTER);
 ewl_container_child_append(EWL_CONTAINER(vbox), hseparator);
 ewl_object_fill_policy_set(EWL_OBJECT(hseparator), EWL_FLAG_FILL_ALL);
 ewl_widget_show(hseparator);
 
 hbox = ewl_hbox_new();
 ewl_object_alignment_set(EWL_OBJECT(hbox), EWL_FLAG_ALIGN_CENTER);
 ewl_container_child_append(EWL_CONTAINER(vbox), hbox);
 ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_ALL);
 ewl_widget_show(hbox);

 vbox = ewl_vbox_new();
 ewl_container_child_append(EWL_CONTAINER(hbox), vbox);
 ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_ALL);
 ewl_object_size_request(EWL_OBJECT(vbox), 220, 250);
 ewl_object_maximum_size_set(EWL_OBJECT(vbox), 195, 999999);
 ewl_widget_show(vbox);

 m->browser = ewl_tree_new(1);
 ewl_container_child_append(EWL_CONTAINER(vbox), m->browser);
 ewl_object_fill_policy_set(EWL_OBJECT(m->browser), EWL_FLAG_FILL_ALL);
 ewl_tree_headers_visible_set(EWL_TREE(m->browser), 0);
 ewl_tree_expandable_rows_set(EWL_TREE(m->browser), FALSE);
 ewl_tree_mode_set(EWL_TREE(m->browser), EWL_TREE_MODE_SINGLE);
 ewl_object_size_request(EWL_OBJECT(m->browser), 220, 250);
 ewl_widget_show(m->browser);

 hseparator = ewl_hseparator_new();
 ewl_object_alignment_set(EWL_OBJECT(hseparator), EWL_FLAG_ALIGN_CENTER);
 ewl_container_child_append(EWL_CONTAINER(vbox), hseparator);
 ewl_object_fill_policy_set(EWL_OBJECT(hseparator), EWL_FLAG_FILL_ALL);
 ewl_widget_show(hseparator);

 border = ewl_border_new();
 ewl_border_text_set(EWL_BORDER(border), gettext("Albums"));
 ewl_border_label_alignment_set(EWL_BORDER(border), EWL_FLAG_ALIGN_CENTER);
 ewl_container_child_append(EWL_CONTAINER(vbox), border);
 ewl_object_alignment_set(EWL_OBJECT(border), EWL_FLAG_ALIGN_CENTER);
 ewl_object_fill_policy_set(EWL_OBJECT(border), EWL_FLAG_FILL_ALL);
 ewl_object_size_request(EWL_OBJECT(border), 220, 250);
 ewl_widget_show(border);

 m->albums = ewl_tree_new(1);
 ewl_container_child_append(EWL_CONTAINER(border), m->albums);
 ewl_object_fill_policy_set(EWL_OBJECT(m->albums), EWL_FLAG_FILL_ALL);
 ewl_tree_headers_visible_set(EWL_TREE(m->albums), 0);
 ewl_tree_expandable_rows_set(EWL_TREE(m->albums), FALSE);
 ewl_tree_mode_set(EWL_TREE(m->albums), EWL_TREE_MODE_SINGLE);
 ewl_object_size_request(EWL_OBJECT(m->albums), 220, 250);
 ewl_widget_show(m->albums);
 
 vseparator = ewl_vseparator_new();
 ewl_container_child_append(EWL_CONTAINER(hbox), vseparator);
 ewl_widget_show(vseparator);
 
 m->viewer = ewl_scrollpane_new();
 ewl_container_child_append(EWL_CONTAINER(hbox), m->viewer);
 ewl_object_fill_policy_set(EWL_OBJECT(m->viewer), EWL_FLAG_FILL_ALL);
 ewl_widget_show(m->viewer);

 m->viewer_freebox = ewl_hfreebox_new();
 ewl_freebox_layout_type_set(EWL_FREEBOX(m->viewer_freebox), 
		 	EWL_FREEBOX_LAYOUT_AUTO);
 ewl_container_child_append(EWL_CONTAINER(m->viewer), m->viewer_freebox);
 ewl_object_fill_policy_set(EWL_OBJECT(m->viewer_freebox), EWL_FLAG_FILL_ALL);
 ewl_widget_show(m->viewer_freebox);

 ewl_callback_append(m->albums, EWL_CALLBACK_SHOW, populate_albums, NULL);
 ewl_callback_append(m->browser, EWL_CALLBACK_SHOW, populate_browser, 
			current_directory);
 ewl_widget_name_set(m->viewer, current_directory);
 ewl_callback_append(m->viewer, EWL_CALLBACK_SHOW, populate_images, NULL);
 
 ewl_main();
 return 0;
}
