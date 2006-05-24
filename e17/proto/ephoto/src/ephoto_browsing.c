#include "ephoto.h"

void
populatei_cb(Ewl_Widget *w, void *event, void *data)
{
	/*****Setup the variables*****/
	char *bname;
	char *bname2;
	char  fn[PATH_MAX];
	char *itemp;
	const char *pathi;
	char  pathf[PATH_MAX];
	char  pathg[PATH_MAX];
	char  pathw[PATH_MAX];
	char  path[PATH_MAX];
	char *temp;
	char *up = "..";
	/****Get home directory****/
	char *home = getenv("HOME");
	/**************************/
	/*****************************/

	/***** Find out what directory we want to look in******/

	if (w == m->directory) {
		pathi = ewl_text_text_get(EWL_TEXT(w));
	}
	else if (data == m->directory) {
		pathi = ewl_text_text_get(EWL_TEXT(m->directory));
	}
	else {
		pathi = ewl_widget_name_get(w);
	}
	
	ewl_text_text_set(EWL_TEXT(m->directory), pathi);
	/****************************************************/	

	/****Lets make sure we have a trailing / ******/	
	snprintf(fn, PATH_MAX, "%s", pathi);
	
	if (fn[strlen(fn)-1] != '/') {
		snprintf(pathf, PATH_MAX, "%s/", pathi);
	}
	
	else {
		snprintf(pathf, PATH_MAX, "%s", pathi);
	}
	
	
	/**********************************************/
	
	/***********Populate the tree with directorys*********/
	if ( ecore_file_is_dir(pathf) ) {

		/**********Get the lists going!***********/
		if ( audiofiles != NULL ) {
			ecore_list_destroy(audiofiles);
		}
		audiofiles = ecore_list_new();
		imagefiles = ecore_list_new();
		files = ecore_list_new();
		/*****************************************/
		
		/************Get the tree ready!***********/
		ewl_container_reset(EWL_CONTAINER(m->dirtree));	
		ewl_container_reset(EWL_CONTAINER(m->imagetree));
		/******************************************/

		/*********Lets setup the parent dir row**********/
		m->hbox = ewl_hbox_new();
		ewl_object_alignment_set(EWL_OBJECT(m->hbox), 
							EWL_FLAG_ALIGN_CENTER);
		ewl_box_spacing_set(EWL_BOX(m->hbox), 5);
		ewl_widget_show(m->hbox);
		
		m->image = ewl_image_new();
		ewl_image_file_set(EWL_IMAGE(m->image), 
				PACKAGE_DATA_DIR "/images/up.png", NULL);
		ewl_container_child_append(EWL_CONTAINER(m->hbox), m->image);
		ewl_widget_show(m->image);
		
		m->texti = ewl_text_new();
		ewl_widget_name_set(m->texti, pathi);
		ewl_text_text_set(EWL_TEXT(m->texti), up);
		ewl_object_fill_policy_set(EWL_OBJECT(m->texti), 
							EWL_FLAG_FILL_ALL);
		ewl_object_alignment_set(EWL_OBJECT(m->texti), 
							EWL_FLAG_ALIGN_CENTER);
		ewl_container_child_append(EWL_CONTAINER(m->hbox), m->texti);
		ewl_widget_show(m->texti);
		
		m->children[0] = m->hbox;
		m->children[1] = NULL;
		m->row = ewl_tree_row_add(EWL_TREE(m->dirtree), NULL, 
								m->children);
		ewl_callback_append(m->texti, EWL_CALLBACK_CLICKED, up_cb, 
								NULL);
		
		/************************************************************/

		files = ecore_file_ls(pathf);
		
		while(!ecore_list_is_empty(files)) {
		
			temp = ecore_list_remove_first(files);	
			snprintf(path, PATH_MAX, "%s", pathf);
				
			if (path[strlen(path)-1] != '/') {
				snprintf(pathg, PATH_MAX, "%s/", path);
			}
			
			else {
				snprintf(pathg, PATH_MAX, "%s", path);
			}
			
			snprintf(pathw, PATH_MAX, "%s%s", pathg, temp);
			
		
			if ( fnmatch("*.[Pp][Nn][Gg]", pathw, 0) == 0 ) { 
				ecore_list_append(imagefiles, strdup(pathw));
			}
			if ( fnmatch("*.[Jj][Pp][Gg]", pathw, 0) == 0 ) {
				ecore_list_append(imagefiles, strdup(pathw));
			}
			if ( fnmatch("*.[Jj][Pp][Ee][Gg]", pathw, 0) == 0 ) {
				ecore_list_append(imagefiles, strdup(pathw));
			}
		
			if ( fnmatch("*.[Bb][Mm][Pp]", pathw, 0) == 0 ) {
				ecore_list_append(imagefiles, strdup(pathw));
			}
			if ( fnmatch(".[Ss][Vv][Gg]", pathw, 0) == 0 ) {
				ecore_list_append(imagefiles, strdup(pathw));
			}
			if ( fnmatch("*.[Mm][Pp][3]", pathw, 0) == 0 ) { 
				ecore_list_append(audiofiles, strdup(pathw));
			}
			
			if ( fnmatch("*.[Oo][Gg][Gg]", pathw, 0) == 0 ) {
				ecore_list_append(audiofiles, strdup(pathw));
			}

			if ( fnmatch("*.[Ww][Aa][Vv]", pathw, 0) == 0 ) {
            		ecore_list_append(audiofiles, strdup(pathw));
         	}
	
			bname = basename(pathw);
	
			if (ecore_file_is_dir(pathw) && *bname != '.') {
				
				m->hbox = ewl_hbox_new();
				ewl_box_spacing_set(EWL_BOX(m->hbox), 5);
				ewl_object_alignment_set(EWL_OBJECT(m->hbox), 
							EWL_FLAG_ALIGN_CENTER);
				ewl_object_fill_policy_set(EWL_OBJECT(m->hbox), 
							EWL_FLAG_FILL_ALL);
				ewl_widget_name_set(m->hbox, pathw);
				ewl_widget_show(m->hbox);
				
				m->image = ewl_image_new();
				ewl_image_file_set(EWL_IMAGE(m->image), 
				  PACKAGE_DATA_DIR "/images/folder.png", NULL);
				ewl_container_child_append(EWL_CONTAINER
							(m->hbox), m->image);
				ewl_widget_show(m->image);
				
				m->text = ewl_text_new();
				ewl_widget_name_set(m->text, pathw);
				ewl_text_text_set(EWL_TEXT(m->text), bname);
				ewl_object_fill_policy_set(EWL_OBJECT(m->text), 
							EWL_FLAG_FILL_SHRINK);
				ewl_object_alignment_set(EWL_OBJECT(m->text), 
							EWL_FLAG_ALIGN_CENTER);
				ewl_container_child_append(EWL_CONTAINER
							(m->hbox), m->text);
				ewl_widget_show(m->text);
				
				m->children[0] = m->hbox;
				m->children[1] = NULL;
				m->row = ewl_tree_row_add(EWL_TREE(m->dirtree), 
							NULL, m->children);
				ewl_callback_append(m->hbox, 
				  EWL_CALLBACK_CLICKED, populatei_cb, NULL);
			}
		}
		/*************************************************************/
		
		/************Populate Image files********************/
		while( !ecore_list_is_empty(imagefiles) ) {
			itemp = ecore_list_remove_first(imagefiles);
			
			bname2 = basename(itemp);
		
			m->hbox = ewl_hbox_new();
			ewl_box_spacing_set(EWL_BOX(m->hbox), 5);
			ewl_object_alignment_set(EWL_OBJECT(m->hbox), 
							EWL_FLAG_ALIGN_CENTER);
			ewl_object_fill_policy_set(EWL_OBJECT(m->hbox), 
							EWL_FLAG_FILL_ALL);
			ewl_widget_name_set(m->hbox, itemp);
			ewl_widget_show(m->hbox);

			m->image = ewl_image_thumbnail_new();
			ewl_image_constrain_set(EWL_IMAGE(m->image), 64);
			ewl_image_proportional_set(EWL_IMAGE(m->image), TRUE);
			ewl_image_thumbnail_request(EWL_IMAGE_THUMBNAIL
							(m->image), itemp);
			ewl_image_file_set(EWL_IMAGE(m->image), 
				PACKAGE_DATA_DIR "images/camera.png", NULL);
			ewl_container_child_append(EWL_CONTAINER(m->hbox), 
								m->image);
			ewl_widget_show(m->image);

			m->text = ewl_text_new();
			ewl_widget_name_set(m->text, itemp);
			ewl_text_text_set(EWL_TEXT(m->text), bname2);
   			ewl_object_fill_policy_set(EWL_OBJECT(m->text), 
							EWL_FLAG_FILL_SHRINK);
			ewl_object_alignment_set(EWL_OBJECT(m->text), 
							EWL_FLAG_ALIGN_CENTER);
			ewl_container_child_append(EWL_CONTAINER(m->hbox), 
								m->text);
			ewl_widget_show(m->text);
		
			m->children[0] = m->hbox;
			m->children[1] = NULL;
  			m->row = ewl_tree_row_add(EWL_TREE(m->imagetree), NULL, 
								m->children);
			ewl_callback_append(m->hbox, EWL_CALLBACK_CLICKED, 
							images_cb, NULL);
			free(itemp);
		}
		while (!ecore_list_is_empty(audiofiles)) {
			itemp = ecore_list_remove_first(audiofiles);

                        bname2 = basename(itemp);

                        m->hbox = ewl_hbox_new();
                        ewl_box_spacing_set(EWL_BOX(m->hbox), 5);
                        ewl_object_alignment_set(EWL_OBJECT(m->hbox), 
							EWL_FLAG_ALIGN_CENTER);
                        ewl_object_fill_policy_set(EWL_OBJECT(m->hbox), 
							EWL_FLAG_FILL_ALL);
                        ewl_widget_name_set(m->hbox, itemp);
                        ewl_widget_show(m->hbox);

                        m->image = ewl_image_new();
                        ewl_image_file_set(EWL_IMAGE(m->image), 
				PACKAGE_DATA_DIR "/images/audio.png", NULL);
                        ewl_container_child_append(EWL_CONTAINER(m->hbox), 
								m->image);
                        ewl_widget_show(m->image);

                        m->text = ewl_text_new();
                        ewl_widget_name_set(m->text, itemp);
                        ewl_text_text_set(EWL_TEXT(m->text), bname2);
                        ewl_object_fill_policy_set(EWL_OBJECT(m->text), 
							EWL_FLAG_FILL_SHRINK);
                        ewl_object_alignment_set(EWL_OBJECT(m->text), 
							EWL_FLAG_ALIGN_CENTER);
                        ewl_container_child_append(EWL_CONTAINER(m->hbox), 
								m->text);
                        ewl_widget_show(m->text);

                        m->children[0] = m->hbox;
                        m->children[1] = NULL;
                        m->row = ewl_tree_row_add(EWL_TREE(m->dirtree), 
							NULL, m->children);
                        ewl_callback_append(m->hbox, EWL_CALLBACK_CLICKED, 
								audio_cb, NULL);
                        free(itemp);

		} 	
		/*********************************************************/
		ecore_list_destroy(files);
		ecore_list_destroy(imagefiles);
	}
	return;
	w = NULL;
	event = NULL;
	data = NULL;
}

void
up_cb(Ewl_Widget *w, void *event, void *data)
{
	/*****Setup variables needed to move to parent directory******/
	const char *pathi;
	char *upname;
	/*************************************************************/
	/*******Move to parent directory in the image tree*******/
	if ( w == m->texta ) {
		pathi = ewl_widget_name_get(w);
		upname = dirname((char *)pathi);
		ewl_text_text_set(EWL_TEXT(m->directorya), upname);
		ewl_callback_call(m->directorya, EWL_CALLBACK_VALUE_CHANGED);
	}
	/**************************************/
	/******Move to parent directory in the audio tree*******/
	if ( w == m->texti ) {
		pathi = ewl_widget_name_get(w);
		upname = dirname((char *)pathi);
		ewl_text_text_set(EWL_TEXT(m->directory), upname);
		ewl_callback_call(m->directory, EWL_CALLBACK_VALUE_CHANGED);
	}
	/*******************************************************/
	return;
	w = NULL;
	event = NULL;
	data = NULL;
}

void
iremove_cb(Ewl_Widget *w, void *event, void *data)
{
	/***Lets remove the icon from the iconbox!***/
	const char *name;
	char *name2;
	
	name = ewl_widget_name_get(w);
	
	ewl_widget_destroy(w);

	//ewl_freebox_resort(EWL_FREEBOX(m->ib));
	
	/*********Lets remove the image from the list*********/
	name2 = ecore_dlist_goto_first(m->imagelist);
	
	while ( name2 = ecore_dlist_current(m->imagelist) ) {
		name2 = ecore_dlist_current(m->imagelist);
		if ( strcmp(name, name2) == 0 ) {
			ecore_dlist_remove(m->imagelist);
			slidenum--;
			if ( ecore_dlist_is_empty(m->imagelist) ) {
				ewl_widget_disable(m->slideshow);
				ewl_widget_disable(m->presentation);
				ewl_widget_state_set(m->slideshow, "disabled");
				ewl_widget_state_set(m->presentation, "disabled");
			}
		}
		else {
			ecore_dlist_next(m->imagelist);
		}
	}
	/******************************************************/
	return;
	w = NULL;
	event = NULL;
	data = NULL;
}

void
images_cb(Ewl_Widget *w, void *event, void *data)
{
	/****Setup variables for adding images to the iconbox****/
	const char *pathi;
	const char *name;
	char *home;
	char imagedb[PATH_MAX];
	char tempcheck[PATH_MAX];
	char equiv[PATH_MAX];
	Ewl_Widget *page;
	/********************************************************/
	if ( w == m->vbutton ) {
		pathi = ewl_image_file_path_get(EWL_IMAGE(m->vimage));
		name = basename((char *)pathi);
	}
	
	else {
		pathi = ewl_widget_name_get(w);
		name = basename((char *)pathi);
	}
	
	page = ewl_notebook_visible_page_get(EWL_NOTEBOOK(m->notebook));
	
	/**********Add the images to the iconbox and list**********/
	if ( page == m->vbox2 || w == m->vbutton ) {
	        m->i = ewl_image_thumbnail_new();
                ewl_widget_name_set(m->i, pathi);
		ewl_image_constrain_set(EWL_IMAGE(m->i), 64);
                ewl_image_proportional_set(EWL_IMAGE(m->i), TRUE);
                ewl_image_thumbnail_request(EWL_IMAGE_THUMBNAIL(m->i), pathi);
                ewl_image_file_set(EWL_IMAGE(m->i), PACKAGE_DATA_DIR 
						"images/camera.png", NULL);
                ewl_container_child_append(EWL_CONTAINER(m->ib), m->i);
                ewl_callback_append(m->i, EWL_CALLBACK_CLICKED, iremove_cb, 
									NULL);
		ewl_widget_show(m->i);
		
		ecore_dlist_append(m->imagelist, strdup(pathi));
		slidenum++;
	
	}
	if ( page == m->viewbox && w != m->vbutton ) {
		ewl_widget_destroy(m->viewscroll);
		ewl_widget_destroy(m->vimage);
		ewl_widget_destroy(m->vbutton);
		
                m->viewscroll = ewl_scrollpane_new();
                ewl_container_child_append(EWL_CONTAINER(m->viewbox), 
								m->viewscroll);
                ewl_object_alignment_set(EWL_OBJECT(m->viewscroll), 
							EWL_FLAG_ALIGN_CENTER);
                ewl_object_fill_policy_set(EWL_OBJECT(m->viewscroll), 
							EWL_FLAG_FILL_ALL);
                ewl_widget_show(m->viewscroll);

                m->vimage = ewl_image_new();
                ewl_object_fill_policy_set(EWL_OBJECT(m->vimage), 
							EWL_FLAG_FILL_SHRINK);
                ewl_container_child_append(EWL_CONTAINER(m->viewscroll), 
							m->vimage);
                ewl_widget_show(m->vimage);

                m->vbutton = ewl_button_new();
                ewl_button_label_set(EWL_BUTTON(m->vbutton), 
						"Add image to slideshow");
                ewl_container_child_append(EWL_CONTAINER(m->viewbox), 
							m->vbutton);
                ewl_object_maximum_size_set(EWL_OBJECT(m->vbutton), 150 , 25);
                ewl_object_alignment_set(EWL_OBJECT(m->vbutton), 
							EWL_FLAG_ALIGN_CENTER);
                ewl_callback_append(m->vbutton, EWL_CALLBACK_CLICKED, 
							images_cb, NULL);
                ewl_widget_disable(m->vbutton);
                ewl_widget_state_set(m->vbutton, "disabled");
                ewl_widget_show(m->vbutton);


		ewl_image_file_set(EWL_IMAGE(m->vimage), pathi, NULL);
		ewl_widget_enable(m->vbutton);
		ewl_widget_state_set(m->vbutton, "enabled");
	}
	/**********************************************************/

	/****Enable the slideshow and presentation buttons 
					so we can get to work****/
	ewl_widget_enable(m->slideshow);
	ewl_widget_state_set(m->slideshow, "enabled");
	ewl_widget_enable(m->presentation);
	ewl_widget_state_set(m->presentation, "enabled");
	/***************************************************/
	return;
	w = NULL;
	event = NULL;
	data = NULL;
}

void
audio_cb(Ewl_Widget *w, void *event, void *data)
{
	/****Setup variables for adding audio to the slideshow****/
	char *name;
	const char *pathi;
	/*********************************************************/
	pathi = ewl_widget_name_get(w);
	name = basename((char *)pathi);
	/****Set Audio in slideshow settings****/
	ewl_text_text_set(EWL_TEXT(m->atext), name);
	/***************************************/
	audios = strdup(pathi);
	audio = 1;
	if ( ewl_media_is_available() ) {
		if (audio != 0) {
			ewl_widget_enable(m->audiolen);
			ewl_widget_state_set(m->audiolen, "enabled");
		}
	}
	return;
	w = NULL;
	event = NULL;
	data = NULL;
}

