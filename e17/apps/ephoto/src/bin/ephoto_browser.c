#include "ephoto.h"

static void destroy(Ewl_Widget *w, void *event, void *data);

/*Ephoto Directory Tree*/
static Ewl_Widget *add_dtree(Ewl_Widget *c);

/*Ephoto MVC Callbacks*/
static Ewl_Widget *directory_view_new(void *data, unsigned int row, unsigned int column);
static Ewl_Widget *directory_header_fetch(void *data, unsigned int column);
static void *directory_data_fetch(void *data, unsigned int row, unsigned int column);
static unsigned int directory_data_count(void *data);

static void destroy(Ewl_Widget *w, void *event, void *data)
{
	ewl_widget_destroy(w);
	em->bwin = NULL;
}

void show_browser(Ewl_Widget *w, void *event, void *data)
{
	if (!em->bwin)
	{
		em->bwin = add_window("Browser!", 200, 400, destroy, NULL);

		em->dtree = add_dtree(em->bwin);
        	ewl_object_maximum_w_set(EWL_OBJECT(em->dtree), 200);

		populate_directories(NULL, NULL, NULL);
	}
}

/*Create and Add a Tree to the Container c*/
static Ewl_Widget *add_dtree(Ewl_Widget *c)
{
        Ewl_Widget *tree;
        Ewl_Model *model;
        Ewl_View *view;

        model = ewl_model_new();
        ewl_model_data_fetch_set(model, directory_data_fetch);
        ewl_model_data_count_set(model, directory_data_count);

        view = ewl_view_new();
        ewl_view_widget_fetch_set(view, directory_view_new);
        ewl_view_header_fetch_set(view, directory_header_fetch);

        tree = ewl_tree_new();
        ewl_tree_headers_visible_set(EWL_TREE(tree), 0);
        ewl_tree_fixed_rows_set(EWL_TREE(tree), 1);
        ewl_tree_column_count_set(EWL_TREE(tree), 1);
        ewl_mvc_model_set(EWL_MVC(tree), model);
        ewl_mvc_view_set(EWL_MVC(tree), view);
	ewl_mvc_selection_mode_set(EWL_MVC(tree), EWL_SELECTION_MODE_NONE);
        ewl_object_fill_policy_set(EWL_OBJECT(tree), EWL_FLAG_FILL_ALL);
        ewl_container_child_append(EWL_CONTAINER(c), tree);
        ewl_widget_show(tree);

        return tree;
}

/* The view of the users directories */
static Ewl_Widget *directory_view_new(void *data, unsigned int row, unsigned int column)
{
	char *current_directory, *directory;
	int len;
        Ewl_Widget *icon;

	len = strlen(em->current_directory);
	current_directory = alloca(len + 1);
	strcpy(current_directory, em->current_directory);
        directory = data;

	icon = add_icon(NULL, basename(directory), 
				PACKAGE_DATA_DIR "/images/folder.png", 0, 
						populate_directories, NULL);

        ewl_icon_constrain_set(EWL_ICON(icon), 25);
        ewl_box_orientation_set(EWL_BOX(icon), EWL_ORIENTATION_HORIZONTAL);
        ewl_object_alignment_set(EWL_OBJECT(icon), EWL_FLAG_ALIGN_LEFT);
        ewl_object_fill_policy_set(EWL_OBJECT(icon), EWL_FLAG_FILL_ALL);
	if (!strncmp(directory, "..", 2))
	{
		ewl_icon_image_set(EWL_ICON(icon), PACKAGE_DATA_DIR "/images/go-up.png", NULL);
		ewl_icon_constrain_set(EWL_ICON(icon), 25);
		ewl_widget_name_set(icon, dirname(current_directory));
	}
	else
	{
        	ewl_widget_name_set(icon, directory);
        }
	
        return icon;
}

/* The header for the tree */
static Ewl_Widget *directory_header_fetch(void *data, unsigned int column)
{
        Ewl_Widget *label;

        label = add_label(NULL, "Directories");

        return label;
}

/*The directories that will be displayed*/
static void *directory_data_fetch(void *data, unsigned int row, unsigned int column)
{
        const char *directory;
        void *val = NULL;

        directory = ecore_list_index_goto(em->directories, row);
        if (directory)
        {
                val = (void *)directory;
        }

        return val;
}

/* The number of directories the view is displaying */
static unsigned int directory_data_count(void *data)
{
        int val;

        val = ecore_list_count(em->directories);

        return val;
}

/*Update the Image List*/
void populate_directories(Ewl_Widget *w, void *event, void *data)
{
	const char *directory;
	char *imagef;
        Ewl_Widget *thumb;

	directory = NULL;

	if (w)
	{
		directory = ewl_widget_name_get(w);
		em->current_directory = strdup(directory);
	}
	if (!ecore_list_empty_is(em->directories))
	{
		ecore_list_destroy(em->directories);
	}
	
	em->directories = ecore_list_new();
	em->directories = get_directories(em->current_directory);

	ecore_dlist_first_goto(em->directories);
        ewl_mvc_data_set(EWL_MVC(em->dtree), em->directories);

      	if (!ecore_list_empty_is(em->images))
       	{
               	ecore_dlist_destroy(em->images);
       	}

        em->images = ecore_dlist_new();
	em->images = get_images(em->current_directory);
	
	ecore_dlist_first_goto(em->images);
		
	ewl_container_reset(EWL_CONTAINER(em->fbox));
	while (ecore_dlist_current(em->images))
	{
		imagef = ecore_dlist_current(em->images);
		if(imagef)
		{
  	        	thumb = add_image(em->fbox, imagef, 1, 
						freebox_image_clicked, NULL);
		       	ewl_image_constrain_set(EWL_IMAGE(thumb), 
					ewl_range_value_get(EWL_RANGE(em->fthumb_size)));
       		      	ewl_object_alignment_set(EWL_OBJECT(thumb), 
							EWL_FLAG_ALIGN_CENTER);
	               	ewl_widget_name_set(thumb, imagef);
		}
        	ecore_dlist_next(em->images);
	}
	ewl_widget_configure(em->fbox_vbox);

	ewl_mvc_data_set(EWL_MVC(em->ltree), em->images);
	ecore_dlist_first_goto(em->images);	

	if (ecore_dlist_current(em->images)) 
	{
		ewl_image_file_path_set(EWL_IMAGE(em->simage), 
					ecore_dlist_current(em->images));
	}

	return;
}
