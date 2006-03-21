#include "ephoto.h"

/***Global Defs***/
Main           *m = NULL;
Slide          *s = NULL;
Present        *p = NULL;
Ecore_List *files;
Ecore_List *imagefiles;
Ecore_List *audiofiles;


int audio = 0;
int wino = 0;
int mainwin = 15;
int nopresent = 15;
int noslide = 15;
int audiolen;
int slidenum;
int arglength = 0;
int argslideshow = 0;
int argloop = 0;
int argfit = 0;
int argfullscreen = 0;
char *audios;
char argimage[PATH_MAX];
char argaudio[PATH_MAX];
char *argheight;
char *argwidth;
char tempdb[PATH_MAX];
char db[PATH_MAX];
/*****************/
int
main(int argc, char **argv)
{
	
	if (!ewl_init(&argc, argv)) {
		printf("Unable to init ewl\n");
		return 1;
	}

	/****allocate mem for structs****/
	m = calloc(1, sizeof(Main));
	s = calloc(1, sizeof(Slide));
	p = calloc(1, sizeof(Present));
	/********************************/
	/****Setup the list/hashes ephoto uses****/
	m->imagelist = ecore_dlist_new();
	/*****************************************/
	/****Get db directory****/
	char *home;
	int argint = 1;
	while ( argint < argc ) { 	
		if ( argint < argc && !strcmp(argv[argint], "--slideshow") && noslide != 0 ) {
			int imageint;
			imageint = argint;
			imageint++;
			while ( imageint < argc && ecore_file_exists(argv[imageint]) != 0 ) {
				ecore_dlist_append(m->imagelist, strdup(argv[imageint]));
				imageint++;
			}
			mainwin = 0;
			nopresent = 0;
		}
		else if ( argint < argc && !strcmp(argv[argint], "--presentation") && nopresent != 0 ) {
			int imageint;
			imageint = argint;
			imageint++;
			while ( imageint < argc && ecore_file_exists(argv[imageint]) != 0 ) {
				ecore_dlist_append(m->imagelist, strdup(argv[imageint]));
				imageint++;
			}
			mainwin	= 0;
			noslide = 0;
		}
		else if ( argint < argc && !strcmp(argv[argint], "--view-image") ) {
			int imageint;
			imageint = argint;
			imageint++;
			snprintf(argimage, PATH_MAX, "%s", argv[imageint]);
			printf("%s\n", argimage);
		}
		else if ( argint < argc && !strcmp(argv[argint], "--audio") ) {
			int imageint;
			imageint = argint;
			imageint++;
			snprintf(argaudio, PATH_MAX, "%s", argv[imageint]);
			audios = argaudio;
		}
		else if ( argint < argc && !strcmp(argv[argint], "--length") ) {
			int imageint;
			imageint = argint;
			imageint++;
			arglength = atoi(argv[imageint]);
		}
		else if ( argint < argc && !strcmp(argv[argint], "--loop") ) {
			argloop = 1;
		}
		else if ( argint < argc && !strcmp(argv[argint], "--fit-to-audio") ) {
			argfit = 1;
		}
		else if ( argint < argc && !strcmp(argv[argint], "--fullscreen") ) {
			argfullscreen = 1;
		}
		else if ( argint < argc && !strcmp(argv[argint], "--help") ) {
			printf("ephoto /path/to/dir loads /path/to/dir as default directory\n");
			printf("ephoto --view-image /full/path/to/image sets /full/path/to/image as the default image in the image viewer tab.\n");
			printf("ephoto --slideshow /full/path/to/image /full/path/to/image /full/path/to/image starts the slideshow using the specified images\n");
			printf("ephoto --presentation /full/path/to/image /full/path/to/image /full/path/to/image starts the presentation using the specified images\n");
			printf("ephoto --audio /full/path/to/audio sets /full/path/to/audio as default audio for slideshow\n");
			printf("ephoto --length length sets the integer length(seconds) as the transition time for slideshow\n");
			printf("ephoto --fullscreen sets the presentation/slideshow window to be fullscreen\n");
			printf("ephoto --win-size integer integer sets the first integer as the width and the second integer as the height of the presentation/slideshow window\n");
			printf("ephoto --loop sets the slideshow to loop\n");
			printf("ephoto --fit-to-audio sets the slideshow to fit audio\n");
			mainwin = 0;
		}
		argint++;
	}
	if ( mainwin == 0 ) {
		if ( nopresent == 0 ) {
			slideshow_cb(NULL, NULL, NULL);
			ewl_main();	
		}
		if ( noslide == 0 ) {
			presentation_cb(NULL, NULL, NULL);
			ewl_main();
		}
	}
	else if ( mainwin != 0 ) {	
		if ( argv[1] != NULL && ecore_file_is_dir(argv[1]) ) {
			home = argv[1];
		}
		else {
			home = getenv("HOME");
		}	
		/****Setup the layout****/
		m->win = ewl_window_new();
		ewl_window_title_set(EWL_WINDOW(m->win), "ephoto");
		ewl_window_name_set(EWL_WINDOW(m->win), "ephoto");
		ewl_object_size_request(EWL_OBJECT(m->win), 550, 480);
		ewl_callback_append(m->win, EWL_CALLBACK_DELETE_WINDOW, destroy_cb, NULL);
		ewl_widget_show(m->win);
		mainwin = 1;
	
		m->hbox = ewl_hpaned_new();
		ewl_object_alignment_set(EWL_OBJECT(m->hbox), EWL_FLAG_ALIGN_CENTER);
		ewl_container_child_append(EWL_CONTAINER(m->win), m->hbox);
		ewl_widget_show(m->hbox);
		
		m->vbox = ewl_vbox_new();
		ewl_container_child_append(EWL_CONTAINER(m->hbox), m->vbox);
		ewl_object_alignment_set(EWL_OBJECT(m->vbox), EWL_FLAG_ALIGN_CENTER);
		ewl_box_spacing_set(EWL_BOX(m->vbox), 10);
		ewl_object_size_request(EWL_OBJECT(m->vbox), 20, 400);
		ewl_widget_show(m->vbox);
	
		m->images = ewl_border_new();
		ewl_border_text_set(EWL_BORDER(m->images), "Select Images For Slideshow");
		ewl_border_label_alignment_set(EWL_BORDER(m->images), EWL_FLAG_ALIGN_CENTER);
		ewl_container_child_append(EWL_CONTAINER(m->vbox), m->images);
		ewl_object_alignment_set(EWL_OBJECT(m->images), EWL_FLAG_ALIGN_CENTER);
		ewl_object_size_request(EWL_OBJECT(m->images), 250, 210);
		ewl_widget_show(m->images);
		
		m->directory = ewl_entry_new();
		ewl_text_text_set(EWL_TEXT(m->directory), home);
		ewl_object_alignment_set(EWL_OBJECT(m->directory), EWL_FLAG_ALIGN_CENTER);
		ewl_container_child_append(EWL_CONTAINER(m->images), m->directory);
		ewl_callback_append(m->directory, EWL_CALLBACK_VALUE_CHANGED, populatei_cb, NULL);
		ewl_widget_show(m->directory);
			
		m->imagetree = ewl_tree_new(1);
		ewl_container_child_append(EWL_CONTAINER(m->images), m->imagetree);
		ewl_object_maximum_size_set(EWL_OBJECT(m->imagetree), 200, 160);
		ewl_widget_show(m->imagetree);
	
		m->songs = ewl_border_new();
		ewl_border_text_set(EWL_BORDER(m->songs), "Select Audio For Slideshow");
		ewl_border_label_alignment_set(EWL_BORDER(m->songs), EWL_FLAG_ALIGN_CENTER);
		ewl_container_child_append(EWL_CONTAINER(m->vbox), m->songs);
		ewl_object_alignment_set(EWL_OBJECT(m->songs), EWL_FLAG_ALIGN_CENTER);
		ewl_object_size_request(EWL_OBJECT(m->songs), 250, 210);
		ewl_widget_show(m->songs);
	
		m->directorya = ewl_entry_new();
		ewl_text_text_set(EWL_TEXT(m->directorya), home);
		ewl_object_alignment_set(EWL_OBJECT(m->directorya), EWL_FLAG_ALIGN_CENTER);
		ewl_container_child_append(EWL_CONTAINER(m->songs), m->directorya);
		ewl_callback_append(m->directorya, EWL_CALLBACK_VALUE_CHANGED, populatea_cb, NULL);
		ewl_widget_show(m->directorya);
	
		m->audiotree = ewl_tree_new(1);
		ewl_container_child_append(EWL_CONTAINER(m->songs), m->audiotree);
		ewl_object_maximum_size_set(EWL_OBJECT(m->audiotree), 200, 160);
		ewl_widget_show(m->audiotree);
		
		m->notebook = ewl_notebook_new();
		ewl_notebook_tabbar_position_set(EWL_NOTEBOOK(m->notebook), EWL_POSITION_TOP);
		ewl_container_child_append(EWL_CONTAINER(m->hbox), m->notebook);
		ewl_object_alignment_set(EWL_OBJECT(m->notebook), EWL_FLAG_ALIGN_CENTER);
		ewl_object_fill_policy_set(EWL_OBJECT(m->notebook), EWL_FLAG_FILL_ALL);
		ewl_widget_show(m->notebook);
		
		m->viewbox = ewl_vbox_new();
		ewl_container_child_append(EWL_CONTAINER(m->notebook), m->viewbox);
		ewl_object_alignment_set(EWL_OBJECT(m->viewbox), EWL_FLAG_ALIGN_CENTER);
		ewl_box_spacing_set(EWL_BOX(m->viewbox), 10);
		ewl_object_fill_policy_set(EWL_OBJECT(m->viewbox), EWL_FLAG_FILL_ALL);
		ewl_widget_show(m->viewbox);
		
		m->viewscroll = ewl_vbox_new();
		ewl_theme_data_str_set(m->viewscroll, "/vbox/group", "entry");
		ewl_container_child_append(EWL_CONTAINER(m->viewbox), m->viewscroll);
		ewl_object_alignment_set(EWL_OBJECT(m->viewscroll), EWL_FLAG_ALIGN_CENTER);
		ewl_object_fill_policy_set(EWL_OBJECT(m->viewscroll), EWL_FLAG_FILL_ALL);
		ewl_widget_show(m->viewscroll);

		m->vimage = ewl_image_new();
		if ( argimage != NULL ) {
			ewl_image_file_set(EWL_IMAGE(m->vimage), argimage, NULL);
		}
		ewl_object_fill_policy_set(EWL_OBJECT(m->vimage), EWL_FLAG_FILL_ALL);
		ewl_image_proportional_set(EWL_IMAGE(m->vimage), TRUE);
		ewl_container_child_append(EWL_CONTAINER(m->viewscroll), m->vimage);
		ewl_widget_show(m->vimage);
		
		m->vbutton = ewl_button_new();
		ewl_button_label_set(EWL_BUTTON(m->vbutton), "Add image to slideshow");
		ewl_container_child_append(EWL_CONTAINER(m->viewbox), m->vbutton);
		ewl_object_maximum_size_set(EWL_OBJECT(m->vbutton), 150 , 25);
		ewl_object_alignment_set(EWL_OBJECT(m->vbutton), EWL_FLAG_ALIGN_CENTER);
		ewl_callback_append(m->vbutton, EWL_CALLBACK_CLICKED, images_cb, NULL);
		ewl_widget_disable(m->vbutton);
		ewl_widget_state_set(m->vbutton, "disabled");
		ewl_widget_show(m->vbutton);
		
		ewl_notebook_page_tab_text_set(EWL_NOTEBOOK(m->notebook), m->viewbox, "Simple Image Viewer");	
		
		m->vbox2 = ewl_vbox_new();
		ewl_container_child_append(EWL_CONTAINER(m->notebook), m->vbox2);
		ewl_object_alignment_set(EWL_OBJECT(m->vbox2), EWL_FLAG_ALIGN_CENTER);
		ewl_box_spacing_set(EWL_BOX(m->vbox2), 10);
		ewl_object_size_request(EWL_OBJECT(m->vbox2), 20, 400);
		ewl_widget_show(m->vbox2);
		
		ewl_notebook_page_tab_text_set(EWL_NOTEBOOK(m->notebook), m->vbox2, "Slideshow/Presentation");
	
		m->content = ewl_border_new();
		ewl_border_text_set(EWL_BORDER(m->content), "Content");
		ewl_border_label_alignment_set(EWL_BORDER(m->content), EWL_FLAG_ALIGN_CENTER);
		ewl_container_child_append(EWL_CONTAINER(m->vbox2), m->content);
		ewl_object_alignment_set(EWL_OBJECT(m->content), EWL_FLAG_ALIGN_CENTER);
		ewl_widget_show(m->content);

		m->ib = ewl_iconbox_new();
		ewl_iconbox_editable_set(EWL_ICONBOX(m->ib), 1);
		ewl_object_size_request(EWL_OBJECT(m->ib), 520, 400);
		ewl_container_child_append(EWL_CONTAINER(m->content), m->ib);
		ewl_widget_show(m->ib);
	
		m->settings = ewl_border_new();
		ewl_border_text_set(EWL_BORDER(m->settings), "Settings");
		ewl_border_label_alignment_set(EWL_BORDER(m->settings), EWL_FLAG_ALIGN_CENTER);
		ewl_container_child_append(EWL_CONTAINER(m->vbox2), m->settings);
		ewl_object_alignment_set(EWL_OBJECT(m->settings), EWL_FLAG_ALIGN_CENTER);
		ewl_box_spacing_set(EWL_BOX(m->settings), 5);
		ewl_object_maximum_size_set(EWL_OBJECT(m->settings), 99999, 200);
		ewl_widget_show(m->settings);
	
		m->hboxv = ewl_hbox_new();
		ewl_container_child_append(EWL_CONTAINER(m->settings), m->hboxv);
		ewl_object_alignment_set(EWL_OBJECT(m->hboxv), EWL_FLAG_ALIGN_CENTER);
		ewl_box_spacing_set(EWL_BOX(m->hboxv), 5);
		ewl_object_maximum_size_set(EWL_OBJECT(m->hboxv),  220, 100);
		ewl_widget_show(m->hboxv);
		
		m->text = ewl_text_new();
		ewl_container_child_append(EWL_CONTAINER(m->hboxv), m->text);
		ewl_object_alignment_set(EWL_OBJECT(m->text), EWL_FLAG_ALIGN_CENTER);
		ewl_text_font_size_set(EWL_TEXT(m->text), 12);
		ewl_text_text_set(EWL_TEXT(m->text), "Length of Slide(secs)");
		ewl_widget_show(m->text);
		
		m->slidetime = ewl_spinner_new();
		if ( arglength != 0 ) {
			ewl_spinner_value_set(EWL_SPINNER(m->slidetime), arglength);
		}
		else if ( arglength == 0 ) {
			ewl_spinner_value_set(EWL_SPINNER(m->slidetime), 3);
		}
		ewl_spinner_min_val_set(EWL_SPINNER(m->slidetime), 1);
		ewl_spinner_max_val_set(EWL_SPINNER(m->slidetime), 1000);
		ewl_container_child_append(EWL_CONTAINER(m->hboxv), m->slidetime);
		ewl_spinner_step_set(EWL_SPINNER(m->slidetime), 1);
		ewl_spinner_digits_set(EWL_SPINNER(m->slidetime), 0);
		ewl_object_alignment_set(EWL_OBJECT(m->slidetime), EWL_FLAG_ALIGN_CENTER);
		ewl_object_maximum_size_set(EWL_OBJECT(m->slidetime),  55, 20);
		ewl_object_minimum_size_set(EWL_OBJECT(m->slidetime),  55, 20);
		ewl_callback_append(m->slidetime, EWL_CALLBACK_CLICKED, rad_cb, NULL);
		ewl_widget_show(m->slidetime);
		
		m->hboxv = ewl_hbox_new();
		ewl_container_child_append(EWL_CONTAINER(m->settings), m->hboxv);
		ewl_object_alignment_set(EWL_OBJECT(m->hboxv), EWL_FLAG_ALIGN_CENTER);
		ewl_box_spacing_set(EWL_BOX(m->hboxv), 5);
		ewl_object_maximum_size_set(EWL_OBJECT(m->hboxv),  220, 100);
		ewl_widget_show(m->hboxv);
	
		m->loopcheck = ewl_checkbutton_new();
		ewl_button_label_set(EWL_BUTTON(m->loopcheck), "Loop Slideshow");
		ewl_container_child_append(EWL_CONTAINER(m->hboxv), m->loopcheck);
		if ( argloop != 0 ) {
			ewl_checkbutton_checked_set(EWL_CHECKBUTTON(m->loopcheck), TRUE);
		}
		ewl_object_maximum_size_set(EWL_OBJECT(m->loopcheck), 130, 50);
		ewl_object_size_request(EWL_OBJECT(m->loopcheck), 130, 50);
		ewl_object_alignment_set(EWL_OBJECT(m->loopcheck), EWL_FLAG_ALIGN_CENTER);
		ewl_widget_show(m->loopcheck);
	
		m->audiolen = ewl_checkbutton_new();
		ewl_button_label_set(EWL_BUTTON(m->audiolen), "Fit to Audio");
		if ( argfit == 1 ) {
			ewl_checkbutton_checked_set(EWL_CHECKBUTTON(m->audiolen), TRUE);
		}
		ewl_container_child_append(EWL_CONTAINER(m->hboxv), m->audiolen);
		ewl_object_maximum_size_set(EWL_OBJECT(m->audiolen), 150, 50);
		ewl_object_size_request(EWL_OBJECT(m->audiolen), 150, 50);
		ewl_object_alignment_set(EWL_OBJECT(m->audiolen), EWL_FLAG_ALIGN_CENTER);
		ewl_callback_append(m->audiolen, EWL_CALLBACK_CLICKED, rad_cb, NULL);
		ewl_widget_show(m->audiolen);
		ewl_widget_disable(m->audiolen);
	
		m->text = ewl_text_new();
		ewl_container_child_append(EWL_CONTAINER(m->settings), m->text);
		ewl_object_alignment_set(EWL_OBJECT(m->text), EWL_FLAG_ALIGN_CENTER);
		ewl_text_font_size_set(EWL_TEXT(m->text), 12);
		ewl_text_text_set(EWL_TEXT(m->text), "Size of Slideshow/Presentation(pixels)");
		ewl_widget_show(m->text);
	
		m->hboxv = ewl_hbox_new();
		ewl_container_child_append(EWL_CONTAINER(m->settings), m->hboxv);
		ewl_object_alignment_set(EWL_OBJECT(m->hboxv), EWL_FLAG_ALIGN_CENTER);
		ewl_box_spacing_set(EWL_BOX(m->hboxv), 5);
		ewl_object_maximum_size_set(EWL_OBJECT(m->hboxv),  220, 100);
		ewl_object_fill_policy_set(EWL_OBJECT(m->hboxv), EWL_FLAG_FILL_SHRINK);
		ewl_widget_show(m->hboxv);
		
		m->rad4 = ewl_radiobutton_new();
		ewl_button_label_set(EWL_BUTTON(m->rad4), "Custom");
		ewl_container_child_append(EWL_CONTAINER(m->hboxv), m->rad4);
		ewl_radiobutton_checked_set(EWL_RADIOBUTTON(m->rad4), TRUE);
		ewl_object_maximum_size_set(EWL_OBJECT(m->rad4), 130, 50);
		ewl_object_size_request(EWL_OBJECT(m->rad4), 130, 50);
		ewl_object_alignment_set(EWL_OBJECT(m->rad4), EWL_FLAG_ALIGN_LEFT);
		ewl_callback_append(m->rad4, EWL_CALLBACK_CLICKED, rad_cb, NULL);
		ewl_callback_append(m->rad4, EWL_CALLBACK_REALIZE, rad_cb, NULL);
		ewl_widget_show(m->rad4);
		
		m->fullrad = ewl_radiobutton_new();
		ewl_button_label_set(EWL_BUTTON(m->fullrad), "Fullscreen");
		if ( argfullscreen == 1 ) {
			ewl_checkbutton_checked_set(EWL_CHECKBUTTON(m->fullrad), TRUE);
			ewl_checkbutton_checked_set(EWL_CHECKBUTTON(m->rad4), FALSE);
		}
		ewl_container_child_append(EWL_CONTAINER(m->hboxv), m->fullrad);
		ewl_object_maximum_size_set(EWL_OBJECT(m->fullrad), 130, 50);
		ewl_object_size_request(EWL_OBJECT(m->fullrad), 130, 50);
		ewl_object_alignment_set(EWL_OBJECT(m->fullrad), EWL_FLAG_ALIGN_LEFT);
		ewl_callback_append(m->fullrad, EWL_CALLBACK_CLICKED, rad_cb, NULL);
		ewl_widget_show(m->fullrad);
		
		ewl_radiobutton_chain_set(EWL_RADIOBUTTON(m->rad4), EWL_RADIOBUTTON(m->fullrad));
		
		m->hboxv = ewl_hbox_new();
		ewl_container_child_append(EWL_CONTAINER(m->settings), m->hboxv);
		ewl_object_alignment_set(EWL_OBJECT(m->hboxv), EWL_FLAG_ALIGN_CENTER);
		ewl_box_spacing_set(EWL_BOX(m->hboxv), 5);
		ewl_object_maximum_size_set(EWL_OBJECT(m->hboxv),  220, 100);
		ewl_object_fill_policy_set(EWL_OBJECT(m->hboxv), EWL_FLAG_FILL_SHRINK);
		ewl_widget_show(m->hboxv);
		
		m->text = ewl_text_new();
		ewl_container_child_append(EWL_CONTAINER(m->hboxv), m->text);
		ewl_object_alignment_set(EWL_OBJECT(m->text), EWL_FLAG_ALIGN_CENTER);
		ewl_text_font_size_set(EWL_TEXT(m->text), 12);
		ewl_text_text_set(EWL_TEXT(m->text), "Width");
			ewl_widget_show(m->text);
		
		m->wsize = ewl_entry_new();
		ewl_entry_editable_set(EWL_ENTRY(m->wsize), 0);
		if ( argwidth == NULL ) {
			ewl_text_text_set(EWL_TEXT(m->wsize), "640");
		}
		if ( argwidth != NULL ) {
			ewl_text_text_set(EWL_TEXT(m->wsize), argwidth);
		}
		ewl_object_maximum_size_set(EWL_OBJECT(m->wsize), 50, 10);
		ewl_object_alignment_set(EWL_OBJECT(m->wsize), EWL_FLAG_ALIGN_CENTER);
		ewl_container_child_append(EWL_CONTAINER(m->hboxv), m->wsize);
		ewl_text_font_size_set(EWL_TEXT(m->wsize), 12);
		ewl_widget_show(m->wsize);
		
		m->text = ewl_text_new();
		ewl_container_child_append(EWL_CONTAINER(m->hboxv), m->text);
		ewl_object_alignment_set(EWL_OBJECT(m->text), EWL_FLAG_ALIGN_CENTER);
		ewl_text_font_size_set(EWL_TEXT(m->text), 12);
		ewl_text_text_set(EWL_TEXT(m->text), "Height");
		ewl_widget_show(m->text);
		
		m->hsize = ewl_entry_new();
		ewl_entry_editable_set(EWL_ENTRY(m->hsize), 0);
		if ( argheight == NULL ) {
			ewl_text_text_set(EWL_TEXT(m->hsize), "480");
		}
		if ( argheight != NULL ) {
			ewl_text_text_set(EWL_TEXT(m->hsize), argheight);
		}
		ewl_object_maximum_size_set(EWL_OBJECT(m->hsize), 50, 10);
		ewl_object_alignment_set(EWL_OBJECT(m->hsize), EWL_FLAG_ALIGN_CENTER);
		ewl_container_child_append(EWL_CONTAINER(m->hboxv), m->hsize);
		ewl_text_font_size_set(EWL_TEXT(m->hsize), 12);
		ewl_widget_show(m->hsize);
		
		m->text = ewl_text_new();
		ewl_container_child_append(EWL_CONTAINER(m->settings), m->text);
		ewl_object_alignment_set(EWL_OBJECT(m->text), EWL_FLAG_ALIGN_CENTER);
		ewl_text_font_size_set(EWL_TEXT(m->text), 12);
		ewl_text_text_set(EWL_TEXT(m->text), "Audio");
		ewl_widget_show(m->text);
		
		m->atext = ewl_text_new();
		if ( audios != NULL ) {
			ewl_text_text_set(EWL_TEXT(m->atext), argaudio);
		}
		ewl_container_child_append(EWL_CONTAINER(m->settings), m->atext);
		ewl_object_alignment_set(EWL_OBJECT(m->atext), EWL_FLAG_ALIGN_CENTER);
		ewl_text_font_size_set(EWL_TEXT(m->atext), 12);
		ewl_widget_show(m->atext);
			
		m->hboxv = ewl_hbox_new();
		ewl_container_child_append(EWL_CONTAINER(m->settings), m->hboxv);
		ewl_object_alignment_set(EWL_OBJECT(m->hboxv), EWL_FLAG_ALIGN_CENTER);
		ewl_box_spacing_set(EWL_BOX(m->hboxv), 5);
		ewl_object_maximum_size_set(EWL_OBJECT(m->hboxv),  220, 100);
		ewl_widget_show(m->hboxv);
	
		m->slideshow = ewl_button_new();
		ewl_button_label_set(EWL_BUTTON(m->slideshow), "Slideshow");
		ewl_container_child_append(EWL_CONTAINER(m->hboxv), m->slideshow);
		ewl_object_alignment_set(EWL_OBJECT(m->slideshow), EWL_FLAG_ALIGN_CENTER);
		ewl_object_maximum_size_set(EWL_OBJECT(m->slideshow), 80, 15);
		ewl_callback_append(m->slideshow, EWL_CALLBACK_CLICKED, slideshow_cb, NULL);
		ewl_widget_show(m->slideshow);
		ewl_widget_disable(m->slideshow);
	
		m->presentation = ewl_button_new();
		ewl_button_label_set(EWL_BUTTON(m->presentation), "Presentation");
		ewl_container_child_append(EWL_CONTAINER(m->hboxv), m->presentation);
		ewl_object_alignment_set(EWL_OBJECT(m->presentation), EWL_FLAG_ALIGN_CENTER);
		ewl_object_maximum_size_set(EWL_OBJECT(m->presentation), 80, 15);
		ewl_callback_append(m->presentation, EWL_CALLBACK_CLICKED, presentation_cb, NULL);
		ewl_widget_show(m->presentation);
		ewl_widget_disable(m->presentation);

		ewl_object_fill_policy_set(EWL_OBJECT(m->win), EWL_FLAG_FILL_ALL);
		ewl_object_fill_policy_set(EWL_OBJECT(m->vbox), EWL_FLAG_FILL_ALL);
		ewl_object_fill_policy_set(EWL_OBJECT(m->vbox2), EWL_FLAG_FILL_ALL);
		ewl_object_fill_policy_set(EWL_OBJECT(m->hbox), EWL_FLAG_FILL_ALL);
		/**********************************************************/
		/************LETS POPULATE THEM TREES******************/
		ewl_callback_call(m->directory, EWL_CALLBACK_VALUE_CHANGED);
		ewl_callback_call(m->directorya, EWL_CALLBACK_VALUE_CHANGED);
		/******************************************************/
	ewl_main();

	return 0;
	}
}


	

