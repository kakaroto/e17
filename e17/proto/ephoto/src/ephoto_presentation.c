#include "ephoto.h"

int
_change_picture_cb(void *data)
{
	/***Setup variables***/
	char *picture1;
	char *picture2;
	char *wsize;
	char *hsize;
	int w;
	int h;
	/*********************/
	
	/****Make sure audio keeps playing :)*****/
printf("res %d\n", ewl_media_is_available());
if ( ewl_media_is_available() ) {
		if (audio != 0) {
			ewl_media_play_set(EWL_MEDIA(s->audio1), 1);
		}
}
	/*****************************************/
	
	/***Cycle the slideshow!***/
	ecore_dlist_next(m->imagelist);
	picture1 = ecore_dlist_current(m->imagelist);

	ewl_widget_destroy(s->screen);
	if ( mainwin == 1 ) {
		wsize = ewl_text_text_get(EWL_TEXT(m->wsize));
		w = atoi(wsize);
		hsize = ewl_text_text_get(EWL_TEXT(m->hsize));
		h = atoi(hsize);
	}
	if ( mainwin == 0 ) {
		if ( argwidth == NULL && argheight == NULL ) {	
			w = 600;
			h = 480;
		}
		else {
			w = atoi(argwidth);
			h = atoi(argheight);
		}
	}
	s->screen = ewl_image_new();
	ewl_image_proportional_set(EWL_IMAGE(s->screen), TRUE);
	ewl_theme_data_str_set(s->screen, "/image/group", "entry");
	ewl_image_file_set(EWL_IMAGE(s->screen), picture1, NULL);
	ewl_object_alignment_set(EWL_OBJECT(s->screen), EWL_FLAG_ALIGN_CENTER);
	if ( mainwin == 1 ) {
		if ( ewl_checkbutton_is_checked(EWL_CHECKBUTTON(m->fullrad)) == 0 ) {
			ewl_object_maximum_size_set(EWL_OBJECT(s->screen), w, h);
		}
	}
	if ( mainwin == 0 && argfullscreen == 0 ) {
		ewl_object_maximum_size_set(EWL_OBJECT(s->screen), w, h);
	}
	ewl_object_fill_policy_set(EWL_OBJECT(s->screen), EWL_FLAG_FILL_SHRINK);
	ewl_container_child_append(EWL_CONTAINER(s->cell), s->screen);
	ewl_widget_show(s->screen);	
	
	if ( !picture1 ) {
		if ( mainwin == 0 ) {
			if ( argloop == 0 ) {
				ewl_callback_call(s->wins, EWL_CALLBACK_CLICKED);
			}
	
			if ( argloop == 1 ) {
       	        	 	picture2 = ecore_dlist_goto_first(m->imagelist);
		                ewl_widget_destroy(s->screen);
		                s->screen = ewl_image_new();
		                ewl_image_proportional_set(EWL_IMAGE(s->screen), TRUE);
		                ewl_theme_data_str_set(s->screen, "/image/group", "entry");
		                ewl_object_fill_policy_set(EWL_OBJECT(s->screen), EWL_FLAG_FILL_SHRINK);
 		                ewl_image_file_set(EWL_IMAGE(s->screen), picture2, NULL);
       			        ewl_object_alignment_set(EWL_OBJECT(s->screen), EWL_FLAG_ALIGN_CENTER);
       		         	if ( argfullscreen == 0 ) {
		                	ewl_object_maximum_size_set(EWL_OBJECT(s->screen), w, h);
               		 	}
                		ewl_container_child_append(EWL_CONTAINER(s->cell), s->screen);
                		ewl_widget_show(s->screen);

			}

		}
	}
	if ( mainwin == 1 ) {
		if (!picture1 && ewl_checkbutton_is_checked(EWL_CHECKBUTTON(m->loopcheck)) == 1 ) {
			picture2 = ecore_dlist_goto_first(m->imagelist);
		
			ewl_widget_destroy(s->screen);
		
			wsize = ewl_text_text_get(EWL_TEXT(m->wsize));
			w = atoi(wsize);
			hsize = ewl_text_text_get(EWL_TEXT(m->hsize));
			h = atoi(hsize);
			s->screen = ewl_image_new();
			ewl_image_proportional_set(EWL_IMAGE(s->screen), TRUE);
			ewl_theme_data_str_set(s->screen, "/image/group", "entry");
			ewl_object_fill_policy_set(EWL_OBJECT(s->screen), EWL_FLAG_FILL_SHRINK);
			ewl_image_file_set(EWL_IMAGE(s->screen), picture2, NULL);
			ewl_object_alignment_set(EWL_OBJECT(s->screen), EWL_FLAG_ALIGN_CENTER);
			if ( ewl_checkbutton_is_checked(EWL_CHECKBUTTON(m->fullrad)) == 0 ) {
				ewl_object_maximum_size_set(EWL_OBJECT(s->screen), w, h);
			}
			ewl_container_child_append(EWL_CONTAINER(s->cell), s->screen);
			ewl_widget_show(s->screen);
		}
		if (!picture1 && ewl_checkbutton_is_checked(EWL_CHECKBUTTON(m->loopcheck)) == 0 ) {
			ewl_callback_call(s->wins, EWL_CALLBACK_CLICKED);
		}
	}
	/******************************************************************/
	data = NULL;
}

void
realize_cb(Ewl_Widget *w, void *event, void *data)
{
	/****Lets go fullscreen*****/
	Ewl_Window *win = EWL_WINDOW(w);
	Ecore_X_Window_State states[1] = {
	ECORE_X_WINDOW_STATE_FULLSCREEN };
	ecore_x_netwm_window_state_set((Ecore_X_Window)win->window, states, 1);
	ewl_object_size_request(EWL_OBJECT(w), 10000, 10000);
	/***************************/
}

void
play_cb(Ewl_Widget *w, void *event, void *data)
{
printf("res %d\n", ewl_media_is_available());
if ( ewl_media_is_available() ) {
		if (audio != 0) {
			ewl_media_play_set(EWL_MEDIA(s->audio1), 1);
			audiolen = ewl_media_length_get(EWL_MEDIA(s->audio1));
		}
}
}

void
slideshow_cb(Ewl_Widget *w, void *event, void *data)
{
	/***Variables***/
	char *get;
	char *picture1;
	char *wsize;
	char *hsize;
	int ws;
	int h;
	int time;
	char *pic1;
	/**************/
	
	/****Make sure s->wins isn't open****/
	if ( wino == 1 ) {
		ewl_callback_call(s->wins, EWL_CALLBACK_DELETE_WINDOW);
	}
	/************************************/

	/*****Setup slideshow layout*****/
	if ( mainwin == 1 ) {
		wsize = ewl_text_text_get(EWL_TEXT(m->wsize));
		ws = atoi(wsize);
		hsize = ewl_text_text_get(EWL_TEXT(m->hsize));
		h = atoi(hsize);
	}
	
	if ( mainwin == 0 ) {
		ws = 800;
		h= 600;
	}

	s->wins = ewl_window_new();
	wino = 1;
	ewl_window_title_set(EWL_WINDOW(s->wins), "Slideshow");
	ewl_window_name_set(EWL_WINDOW(s->wins), "Slideshow");
	ewl_window_class_set(EWL_WINDOW(s->wins), "Slideshow");
	if ( mainwin == 1 ) {
		if ( ewl_checkbutton_is_checked(EWL_CHECKBUTTON(m->fullrad)) == 0 ) {
			ewl_object_size_request(EWL_OBJECT(s->wins), ws, h);
		}
	}
	if ( mainwin == 0 ) {
		ewl_object_size_request(EWL_OBJECT(s->wins), ws, h);
	}
	ewl_object_fill_policy_set(EWL_OBJECT(s->wins), EWL_FLAG_FILL_ALL);
	if ( mainwin == 1 ) {
		if ( ewl_checkbutton_is_checked(EWL_CHECKBUTTON(m->fullrad)) == 1 ) {
			ewl_callback_append(s->wins, EWL_CALLBACK_REALIZE, realize_cb, NULL);
		}
	}
	ewl_callback_append(s->wins, EWL_CALLBACK_DELETE_WINDOW, destroys_cb, NULL);
	ewl_callback_append(s->wins, EWL_CALLBACK_CLICKED, destroys_cb, NULL);
	ewl_widget_show(s->wins);
	
	s->screen = ewl_image_new();
	ewl_image_file_set(EWL_IMAGE(s->screen), PACKAGE_DATA_DIR "/images/black.png", NULL);
	ewl_object_fill_policy_set(EWL_OBJECT(s->screen), EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(s->wins), s->screen);
	ewl_widget_show(s->screen);	

	s->cell = ewl_cell_new();
	ewl_object_fill_policy_set(EWL_OBJECT(s->cell), EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(s->wins), s->cell);
	ewl_widget_show(s->cell);

printf("res %d\n", ewl_media_is_available());
if ( ewl_media_is_available() ) {
		if (audio != 0) {
			s->audio1 = ewl_media_new();
			ewl_container_child_append(EWL_CONTAINER(s->wins), s->audio1);
			ewl_media_module_get(EWL_MEDIA(s->audio1));
			ewl_media_module_set(EWL_MEDIA(s->audio1), EWL_MEDIA_MODULE_XINE);
			ewl_media_media_set(EWL_MEDIA(s->audio1), audios);
			ewl_object_maximum_size_set(EWL_OBJECT(s->audio1), 1, 1);
			ewl_widget_show(s->audio1);
		}
}
	/*******************************************************************/
	
	/*******Start the slideshow*******/	
	if ( s->audio1 != NULL ) {
		ewl_callback_append(s->audio1, EWL_CALLBACK_REALIZE, play_cb, NULL);
	}	

	if ( mainwin == 1 ) {
		if ( ewl_checkbutton_is_checked(EWL_CHECKBUTTON(m->audiolen)) == 1 ) {
			time = audiolen / slidenum;
		}
		if ( ewl_checkbutton_is_checked(EWL_CHECKBUTTON(m->audiolen)) == 0 ) {
			time = ewl_spinner_value_get(EWL_SPINNER(m->slidetime));
		}
	}
	else {
		if ( arglength != 0 ) {
			time = arglength;
		}
		if ( arglength == 0 ) {
			time = 3;
		}
	}
	s->timer = ecore_timer_add(time, _change_picture_cb, NULL);
	/*******************************************************************/
	pic1 = ecore_dlist_goto_first(m->imagelist);
	s->screen = ewl_image_new();
	ewl_image_proportional_set(EWL_IMAGE(s->screen), TRUE);
	ewl_theme_data_str_set(s->screen, "/image/group", "entry");
	ewl_image_file_set(EWL_IMAGE(s->screen), pic1, NULL);
	ewl_object_alignment_set(EWL_OBJECT(s->screen), EWL_FLAG_ALIGN_CENTER);
	if ( mainwin == 1 ) {
		if ( ewl_checkbutton_is_checked(EWL_CHECKBUTTON(m->fullrad)) == 0 ) {
			ewl_object_maximum_size_set(EWL_OBJECT(s->screen), ws, h);
		}
	}
	if ( mainwin == 0 ) {
		ewl_object_maximum_size_set(EWL_OBJECT(s->screen), ws, h);
	}
	ewl_object_fill_policy_set(EWL_OBJECT(s->screen), EWL_FLAG_FILL_SHRINK);
	ewl_container_child_append(EWL_CONTAINER(s->cell), s->screen);
	ewl_widget_show(s->screen);
}

void
first_cb(Ewl_Widget * w, void *event, void *data)
{	
	/***Goto the firs image in the presentation***/
	char *picturep;	
	
	picturep = ecore_dlist_goto_first(m->imagelist);
	ecore_dlist_current(m->imagelist);

	ewl_widget_destroy(p->imagep);
	
	p->imagep = ewl_image_new();
	ewl_image_proportional_set(EWL_IMAGE(p->imagep), TRUE);
	/* ewl_theme_data_str_set(p->imagep, "/image/file", PACKAGE_DATA_DIR "/images/images.edc");
	ewl_theme_data_str_set(p->imagep, "/image/group", "entry"); */
	ewl_object_fill_policy_set(EWL_OBJECT(p->imagep), EWL_FLAG_FILL_SHRINK);
	ewl_image_file_set(EWL_IMAGE(p->imagep), picturep, NULL);
	ewl_object_alignment_set(EWL_OBJECT(p->imagep), EWL_FLAG_ALIGN_CENTER);
	ewl_container_child_append(EWL_CONTAINER(p->vboxp), p->imagep);
	ewl_widget_show(p->imagep);
	/*********************************************/
}	

void
next_cb(Ewl_Widget * w, void *event, void *data)
{
	/***Goto the next image in the presentation***/
	char *picturep;

	ecore_dlist_next(m->imagelist);
	picturep = ecore_dlist_current(m->imagelist);

	if (picturep == NULL) {
		picturep = ecore_dlist_goto_first(m->imagelist);
	}

	ewl_widget_destroy(p->imagep);
	
	p->imagep = ewl_image_new();
	ewl_image_proportional_set(EWL_IMAGE(p->imagep), TRUE);
	/* ewl_theme_data_str_set(p->imagep, "/image/file", PACKAGE_DATA_DIR "/images/images.edc");
	ewl_theme_data_str_set(p->imagep, "/image/group", "entry"); */
	ewl_object_fill_policy_set(EWL_OBJECT(p->imagep), EWL_FLAG_FILL_SHRINK);
	ewl_image_file_set(EWL_IMAGE(p->imagep), picturep, NULL);
	ewl_object_alignment_set(EWL_OBJECT(p->imagep), EWL_FLAG_ALIGN_CENTER);
	ewl_container_child_append(EWL_CONTAINER(p->vboxp), p->imagep);
	ewl_widget_show(p->imagep);
	/***************************************************************/
}	
void
previous_cb(Ewl_Widget * w, void *event, void *data)
{
	/********Goto the previous image in the presentation********/
	char *picturep;

	ecore_dlist_previous(m->imagelist);
	picturep = ecore_dlist_current(m->imagelist);

	if (picturep == NULL) {
		picturep = ecore_dlist_goto_last(m->imagelist);
	}

	ewl_widget_destroy(p->imagep);
	
	p->imagep = ewl_image_new();
	ewl_image_proportional_set(EWL_IMAGE(p->imagep), TRUE);
	/* ewl_theme_data_str_set(p->imagep, "/image/file", PACKAGE_DATA_DIR "/images/images.edc");
	ewl_theme_data_str_set(p->imagep, "/image/group", "entry"); */
	ewl_object_fill_policy_set(EWL_OBJECT(p->imagep), EWL_FLAG_FILL_SHRINK);
	ewl_image_file_set(EWL_IMAGE(p->imagep), picturep, NULL);
	ewl_object_alignment_set(EWL_OBJECT(p->imagep), EWL_FLAG_ALIGN_CENTER);
	ewl_container_child_append(EWL_CONTAINER(p->vboxp), p->imagep);
	ewl_widget_show(p->imagep);
	/********************************************************************/
}
void
last_cb(Ewl_Widget * w, void *event, void *data)
{
	/*****Goto the last image in the presentation!******/
	char *picturep;
	
	picturep = ecore_dlist_goto_last(m->imagelist);
	
	ewl_widget_destroy(p->imagep);
	
	p->imagep = ewl_image_new();
	ewl_image_proportional_set(EWL_IMAGE(p->imagep), TRUE);
	/* ewl_theme_data_str_set(p->imagep, "/image/file", PACKAGE_DATA_DIR "/images/images.edc");
	ewl_theme_data_str_set(p->imagep, "/image/group", "entry"); */
	ewl_object_fill_policy_set(EWL_OBJECT(p->imagep), EWL_FLAG_FILL_SHRINK);
	ewl_image_file_set(EWL_IMAGE(p->imagep), picturep, NULL);
	ewl_object_alignment_set(EWL_OBJECT(p->imagep), EWL_FLAG_ALIGN_CENTER);
	ewl_container_child_append(EWL_CONTAINER(p->vboxp), p->imagep);
	ewl_widget_show(p->imagep);
	/*****************************************************************/
}
void
presentation_cb(Ewl_Widget * w, void *event, void *data)
{
	char *pictureone;
	char *wsize;
	char *hsize;
	int ws;
	int h;
	
	/***Make sure s->wins isn't already open!***/
	if ( wino == 1 ) {
		ewl_callback_call(s->wins, EWL_CALLBACK_DELETE_WINDOW);
	}
	/*******************************************/

	/***Setup the presentation layout!***/
	wsize = ewl_text_text_get(EWL_TEXT(m->wsize));
	ws = atoi(wsize);
	hsize = ewl_text_text_get(EWL_TEXT(m->hsize));
	h = atoi(hsize);

	s->wins = ewl_window_new();
	wino = 1;
	ewl_window_title_set(EWL_WINDOW(s->wins), "Presentation");
	ewl_window_name_set(EWL_WINDOW(s->wins), "Presentation");
	ewl_window_class_set(EWL_WINDOW(s->wins), "Presentation");
	if ( ewl_checkbutton_is_checked(EWL_CHECKBUTTON(m->fullrad)) == 0 ) {
		ewl_object_maximum_size_set(EWL_OBJECT(s->wins), ws, h);
	}
	if ( ewl_checkbutton_is_checked(EWL_CHECKBUTTON(m->fullrad)) == 1 ) {
		ewl_callback_append(s->wins, EWL_CALLBACK_REALIZE, realize_cb, NULL);
	}
	ewl_callback_append(s->wins, EWL_CALLBACK_DELETE_WINDOW, destroyp_cb, NULL);
	ewl_widget_show(s->wins);

	p->vbox1p = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(s->wins), p->vbox1p);
	ewl_object_alignment_set(EWL_OBJECT(p->vbox1p), EWL_FLAG_ALIGN_CENTER);
	ewl_object_fill_policy_set(EWL_OBJECT(p->vbox1p), EWL_FLAG_FILL_ALL);
	ewl_box_spacing_set(EWL_BOX(p->vbox1p), 10);
	ewl_widget_show(p->vbox1p);

	p->vboxp = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(p->vbox1p), p->vboxp);
	ewl_object_alignment_set(EWL_OBJECT(p->vboxp), EWL_FLAG_ALIGN_CENTER);
	ewl_object_fill_policy_set(EWL_OBJECT(p->vboxp), EWL_FLAG_FILL_ALL);
	ewl_box_spacing_set(EWL_BOX(p->vboxp), 10);
	ewl_callback_append(p->vboxp, EWL_CALLBACK_CLICKED, destroyp_cb, NULL);
	ewl_widget_show(p->vboxp);
	
	pictureone = ecore_dlist_goto_first(m->imagelist);
	
	p->imagep = ewl_image_new();
	ewl_image_proportional_set(EWL_IMAGE(p->imagep), TRUE);
	ewl_object_fill_policy_set(EWL_OBJECT(p->imagep), EWL_FLAG_FILL_SHRINK);
	ewl_image_file_set(EWL_IMAGE(p->imagep), pictureone, NULL);
	ewl_object_alignment_set(EWL_OBJECT(p->imagep), EWL_FLAG_ALIGN_CENTER);
	ewl_container_child_append(EWL_CONTAINER(p->vboxp), p->imagep);
	ewl_widget_show(p->imagep);	
	
	p->vbox2p = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(p->vbox1p), p->vbox2p);
	ewl_object_alignment_set(EWL_OBJECT(p->vbox2p), EWL_FLAG_ALIGN_CENTER);
	ewl_object_fill_policy_set(EWL_OBJECT(p->vbox2p), EWL_FLAG_FILL_SHRINK);
	ewl_box_spacing_set(EWL_BOX(p->vbox2p), 10);
	ewl_widget_show(p->vbox2p);

	p->hboxp = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(p->vbox2p), p->hboxp);
	ewl_object_alignment_set(EWL_OBJECT(p->hboxp), EWL_FLAG_ALIGN_CENTER);
	ewl_object_fill_policy_set(EWL_OBJECT(p->hboxp), EWL_FLAG_FILL_VFILL);
	ewl_box_spacing_set(EWL_BOX(p->hboxp), 20);
	ewl_widget_show(p->hboxp);

	p->first = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(p->first), "Go to First");
	ewl_container_child_append(EWL_CONTAINER(p->hboxp), p->first);
	ewl_object_alignment_set(EWL_OBJECT(p->first), EWL_FLAG_ALIGN_CENTER);
	ewl_object_maximum_size_set(EWL_OBJECT(p->first), 70, 50);
	ewl_object_minimum_size_set(EWL_OBJECT(p->first), 70, 50);
	ewl_callback_append(p->first, EWL_CALLBACK_CLICKED, first_cb, NULL);
	ewl_widget_show(p->first);

	p->previous = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(p->previous), "Previous");
	ewl_container_child_append(EWL_CONTAINER(p->hboxp), p->previous);
	ewl_object_alignment_set(EWL_OBJECT(p->previous), EWL_FLAG_ALIGN_CENTER);
	ewl_object_maximum_size_set(EWL_OBJECT(p->previous), 70, 50);
	ewl_object_minimum_size_set(EWL_OBJECT(p->previous), 70, 50);
	ewl_callback_append(p->previous, EWL_CALLBACK_CLICKED, previous_cb, NULL);
	ewl_widget_show(p->previous);

	p->next = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(p->next), "Next");
	ewl_container_child_append(EWL_CONTAINER(p->hboxp), p->next);
	ewl_object_alignment_set(EWL_OBJECT(p->next), EWL_FLAG_ALIGN_CENTER);
	ewl_object_maximum_size_set(EWL_OBJECT(p->next), 70, 50);
	ewl_object_minimum_size_set(EWL_OBJECT(p->next), 70, 50);
	ewl_callback_append(p->next, EWL_CALLBACK_CLICKED, next_cb, NULL);
	ewl_widget_show(p->next);

	p->last = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(p->last), "Go to Last");
	ewl_container_child_append(EWL_CONTAINER(p->hboxp), p->last);
	ewl_object_alignment_set(EWL_OBJECT(p->last), EWL_FLAG_ALIGN_CENTER);
	ewl_object_maximum_size_set(EWL_OBJECT(p->last), 70, 50);
	ewl_object_minimum_size_set(EWL_OBJECT(p->last), 70, 50);
	ewl_callback_append(p->last, EWL_CALLBACK_CLICKED, last_cb, NULL);
	ewl_widget_show(p->last);
	/*******************************************************************/
}

/*******************************************/

