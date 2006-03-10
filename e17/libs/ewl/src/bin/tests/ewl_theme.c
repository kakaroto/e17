#include "Ewl_Test.h"
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

static int create_test(Ewl_Container *box);
static Ewl_Widget *widgets_build(void);
static void cb_select_theme(Ewl_Widget *w, void *ev, void *data);

void 
test_info(Ewl_Test *test)
{
	test->name = "Theme";
	test->tip = "Shows the utilization of themes\n"
			"inside a EWL application.";
	test->filename = "ewl_theme.c";
	test->func = create_test;
	test->type = EWL_TEST_TYPE_ADVANCED;
}

static int
create_test(Ewl_Container *box)
{
        Ewl_Widget *box2, *vbox, *list, *misc;
        DIR *rep;
        struct dirent *file;

	box2 = ewl_hbox_new();
	ewl_container_child_append(box, box2);
	ewl_widget_show(box2);

	/* the theme list tree */
        list = ewl_border_new();
	ewl_border_text_set(EWL_BORDER(list), "Theme name");
	ewl_container_child_append(EWL_CONTAINER(box2), list);
	ewl_widget_show(list);
	
	rep = opendir(PACKAGE_DATA_DIR "/themes");
	while ((file = readdir(rep))) 
	{
		char *name;
        	int len;

	        name = file->d_name;
		len = strlen(name);

		if ((len >= 4) && (!strcmp(name + len - 4, ".edj"))) {
			Ewl_Widget *w;

			w = ewl_button_new();
			ewl_button_label_set(EWL_BUTTON(w), name);
			ewl_object_fill_policy_set(EWL_OBJECT(w), 
				EWL_FLAG_FILL_VSHRINK | EWL_FLAG_FILL_HFILL);
			ewl_callback_append(w, EWL_CALLBACK_CLICKED,
					    cb_select_theme, strdup(name));
			ewl_container_child_append(EWL_CONTAINER(list), w);
			ewl_widget_show(w);
		}
	}
	
	vbox = ewl_border_new();
	ewl_border_text_set(EWL_BORDER(vbox), "Theme Visualization");
	ewl_container_child_append(EWL_CONTAINER(box2), vbox);
	ewl_widget_show(vbox);
	
	misc = widgets_build();
	ewl_container_child_append(EWL_CONTAINER(vbox), misc);
	ewl_widget_show(misc);

	return 1;
}

static void
cb_select_theme(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__, void *data)
{
	Ewl_Widget *n;
	char theme_filename[PATH_MAX];
	
	n = ewl_widget_name_find("notebook");
	snprintf(theme_filename, sizeof(theme_filename), 
			PACKAGE_DATA_DIR "/themes/%s", (char *)data);
	ewl_theme_data_str_set(n, "/file", theme_filename);
}

static Ewl_Widget *
widgets_build(void)
{
        Ewl_Widget *misc, *item, *vbox, *notebook;
	char *str, *str_col[2];
	int i;
	struct {
		char *name;
		char *image;
	} menus[] = {
		{"image", "dia-diagram.png"},
		{NULL, NULL},
		{"label", NULL}
	};

	notebook = ewl_notebook_new();
	ewl_widget_name_set(notebook, "notebook");
	
	/* buttons */
	vbox = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(notebook), vbox);
	ewl_notebook_page_tab_text_set(EWL_NOTEBOOK(notebook), vbox, "Buttons");
	ewl_widget_show(vbox);
	
	misc = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(misc), "Normal button");
	ewl_object_alignment_set(EWL_OBJECT(misc), EWL_FLAG_ALIGN_LEFT);
	ewl_container_child_append(EWL_CONTAINER(vbox), misc);
	ewl_widget_show(misc);
	
	misc = ewl_hseparator_new();
	ewl_container_child_append(EWL_CONTAINER(vbox), misc);
	ewl_widget_show(misc);
	
	misc = ewl_checkbutton_new();
	ewl_button_label_set(EWL_BUTTON(misc), "Check button");
	ewl_object_alignment_set(EWL_OBJECT(misc), EWL_FLAG_ALIGN_LEFT);
	ewl_container_child_append(EWL_CONTAINER(vbox), misc);
	ewl_widget_show(misc);
	
	misc = ewl_hseparator_new();
	ewl_container_child_append(EWL_CONTAINER(vbox), misc);
	ewl_widget_show(misc);
	
	misc = ewl_radiobutton_new();
	ewl_button_label_set(EWL_BUTTON(misc), "Radio button");
	ewl_object_alignment_set(EWL_OBJECT(misc), EWL_FLAG_ALIGN_LEFT);
	ewl_container_child_append(EWL_CONTAINER(vbox), misc);
	ewl_widget_show(misc);
	
	/* numerical/text entries */
	vbox = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(notebook), vbox);
	ewl_notebook_page_tab_text_set(EWL_NOTEBOOK(notebook), vbox, 
						"Numerical & Text Entries");
	ewl_widget_show(vbox);
	
	misc = ewl_entry_new();
	ewl_text_text_set(EWL_TEXT(misc), "Normal entry");
	ewl_object_alignment_set(EWL_OBJECT(misc), EWL_FLAG_ALIGN_LEFT);
	ewl_container_child_append(EWL_CONTAINER(vbox), misc);
	ewl_widget_show(misc);
	
	misc = ewl_hseparator_new();
	ewl_container_child_append(EWL_CONTAINER(vbox), misc);
	ewl_widget_show(misc);
	
	misc = ewl_password_new();
	ewl_password_text_set(EWL_PASSWORD(misc), "Password entry");
	ewl_object_alignment_set(EWL_OBJECT(misc), EWL_FLAG_ALIGN_LEFT);
	ewl_container_child_append(EWL_CONTAINER(vbox), misc);
	ewl_widget_show(misc);
	
	misc = ewl_hseparator_new();
	ewl_container_child_append(EWL_CONTAINER(vbox), misc);
	ewl_widget_show(misc);
	
	misc = ewl_spinner_new();
	ewl_spinner_digits_set(EWL_SPINNER(misc), 0);
	ewl_spinner_min_val_set(EWL_SPINNER(misc), 0);
	ewl_spinner_max_val_set(EWL_SPINNER(misc), 1024);
	ewl_spinner_step_set(EWL_SPINNER(misc), 1.0);
	ewl_spinner_value_set(EWL_SPINNER(misc), 15.0);
	ewl_object_alignment_set(EWL_OBJECT(misc), EWL_FLAG_ALIGN_LEFT);
	ewl_container_child_append(EWL_CONTAINER(vbox), misc);
	ewl_widget_show(misc);
	
	misc = ewl_hseparator_new();
	ewl_container_child_append(EWL_CONTAINER(vbox), misc);
	ewl_widget_show(misc);
	
	misc = ewl_hseeker_new();
	ewl_seeker_range_set(EWL_SEEKER(misc), 10);
	ewl_seeker_step_set(EWL_SEEKER(misc), 1.0);
	ewl_seeker_value_set(EWL_SEEKER(misc), 15.0);
	ewl_object_alignment_set(EWL_OBJECT(misc), EWL_FLAG_ALIGN_LEFT);
	ewl_container_child_append(EWL_CONTAINER(vbox), misc);
	ewl_widget_show(misc);
	
	/* menus */
	vbox = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(notebook), vbox);
	ewl_notebook_page_tab_text_set(EWL_NOTEBOOK(notebook), vbox, "Menus");
	ewl_widget_show(vbox);
	
	misc = ewl_imenu_new();
	ewl_button_label_set(EWL_BUTTON(misc), "Imenu");
	ewl_container_child_append(EWL_CONTAINER(vbox), misc);
	ewl_widget_show(misc);

	for (i = 0; i < 3; i++) 
	{
		item = ewl_menu_item_new();
		if (menus[i].name)
			ewl_button_label_set(EWL_BUTTON(item), 
						menus[i].name);

		if (menus[i].image)
			ewl_button_image_set(EWL_BUTTON(item), 
						menus[i].image, NULL);

		ewl_container_child_append(EWL_CONTAINER(misc), item);
		ewl_widget_show(item);
	}
	
	misc = ewl_hseparator_new();
	ewl_container_child_append(EWL_CONTAINER(vbox), misc);
	ewl_widget_show(misc);
	
	misc = ewl_menu_new();
	ewl_button_label_set(EWL_BUTTON(misc), "Menu");
	ewl_container_child_append(EWL_CONTAINER(vbox), misc);
	ewl_widget_show(misc);

	for (i = 0; i < 3; i++) 
	{
		item = ewl_menu_item_new();
		if (menus[i].name)
			ewl_button_label_set(EWL_BUTTON(item), 
						menus[i].name);

		if (menus[i].image)
			ewl_button_image_set(EWL_BUTTON(item), 
						menus[i].image, NULL);

		ewl_container_child_append(EWL_CONTAINER(misc), item);
		ewl_widget_show(item);
	}
	
	misc = ewl_hseparator_new();
	ewl_container_child_append(EWL_CONTAINER(vbox), misc);
	ewl_widget_show(misc);
	
	misc = ewl_combo_new("Combo entry");
	ewl_container_child_append(EWL_CONTAINER(vbox), misc);
	ewl_widget_show(misc);

	for (i = 0; i < 3; i++) 
	{
		item = ewl_menu_item_new();
		if (menus[i].name)
			ewl_button_label_set(EWL_BUTTON(item), 
						menus[i].name);

		if (menus[i].image)
			ewl_button_image_set(EWL_BUTTON(item), 
						menus[i].image, NULL);

		ewl_container_child_append(EWL_CONTAINER(misc), item);
		ewl_widget_show(item);
	}
	
	/* List/tree */
	vbox = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(notebook), vbox);
	ewl_notebook_page_tab_text_set(EWL_NOTEBOOK(notebook), vbox, 
							"List and Tree");
	ewl_widget_show(vbox);
	
	str = "List";
	misc = ewl_tree_new(1);
	ewl_tree_headers_set(EWL_TREE(misc), &str);
	ewl_container_child_append(EWL_CONTAINER(vbox), misc);
	ewl_widget_show(misc);
	
	str = "Row 1";
	item = ewl_tree_text_row_add(EWL_TREE(misc), NULL, &str);

	str = "Row 2";
	item = ewl_tree_text_row_add(EWL_TREE(misc), NULL, &str);

	str = "Row 3";
	item = ewl_tree_text_row_add(EWL_TREE(misc), NULL, &str);
	
	misc = ewl_hseparator_new();
	ewl_container_child_append(EWL_CONTAINER(vbox), misc);
	ewl_widget_show(misc);
	
	str_col[0] = "Tree";
	str_col[1] = "Column";
	misc = ewl_tree_new(2);
	ewl_tree_headers_set(EWL_TREE(misc), str_col);
	ewl_container_child_append(EWL_CONTAINER(vbox), misc);
	ewl_widget_show(misc);
	
	str_col[0] = "row1";
	str_col[1] = "label";
	item = ewl_tree_text_row_add(EWL_TREE(misc), NULL, str_col);

	str_col[0] = "sub row";
	str_col[1] = "label";
	item = ewl_tree_text_row_add(EWL_TREE(misc), EWL_ROW(item), str_col);
	
	str_col[0] = "row2";
	str_col[1] = "label";
	item = ewl_tree_text_row_add(EWL_TREE(misc), NULL, str_col);

	str_col[0] = "sub row";
	str_col[1] = "label";
	item = ewl_tree_text_row_add(EWL_TREE(misc), EWL_ROW(item), str_col);
	
	/* Misc */
	vbox = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(notebook), vbox);
	ewl_notebook_page_tab_text_set(EWL_NOTEBOOK(notebook), vbox, "Misc");
	ewl_widget_show(vbox);
	
	misc = ewl_hpaned_new();
	ewl_container_child_append(EWL_CONTAINER(vbox), misc);
	ewl_widget_show(misc);

	item = ewl_text_new();
	ewl_text_text_set(EWL_TEXT(item), "left");
	ewl_container_child_append(EWL_CONTAINER(misc), item);
	ewl_widget_show(item);

	item = ewl_text_new();
	ewl_text_text_set(EWL_TEXT(item), "center");
	ewl_container_child_append(EWL_CONTAINER(misc), item);
	ewl_widget_show(item);

	item = ewl_text_new();
	ewl_text_text_set(EWL_TEXT(item), "right");
	ewl_container_child_append(EWL_CONTAINER(misc), item);
	ewl_widget_show(item);
	
	misc = ewl_hseparator_new();
	ewl_container_child_append(EWL_CONTAINER(vbox), misc);
	ewl_widget_show(misc);
	
	misc = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(misc), "Tooltip");
	ewl_container_child_append(EWL_CONTAINER(vbox), misc);
	ewl_attach_tooltip_text_set(misc, "blah blah blah");
	ewl_widget_show(misc);

	return notebook;
}

