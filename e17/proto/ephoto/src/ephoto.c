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
int arglload = 0;
int argviewi = 0;
int argloadidir = 0;
int argzoom = 0;
char *audios;
char buf[PATH_MAX];
char argimage[PATH_MAX];
char argaudio[PATH_MAX];
char argheight[PATH_MAX] = "480";
char argload[PATH_MAX];
char argwidth[PATH_MAX] = "600";
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
		if ( argint < argc && !strcmp(argv[argint], "--slideshow") 
						&& noslide != 0 ) {
			int imageint;
			imageint = argint;
			imageint++;
			while ( imageint < argc && 
				ecore_file_exists(argv[imageint]) != 0 ) {
				ecore_dlist_append(m->imagelist, 
						strdup(argv[imageint]));
				slidenum++;
				imageint++;
			}
			mainwin = 0;
			nopresent = 0;
		}
		if ( argint < argc && !strcmp(argv[argint], 
				"--album-slideshow") && noslide != 0 ) {
			int imageint;
			char *albuma;
			imageint = argint;
			imageint++;
			albuma = argv[imageint];
			create_list_cb(NULL, NULL, albuma);
			mainwin = 0;
			nopresent = 0;
		}
		if ( argint < argc 
				&& !strcmp(argv[argint], "--list-albums") ) {
			list_albums_cb(NULL, NULL, NULL);
			mainwin = 0;
		}
		if ( argint < argc && !strcmp(argv[argint], "--slideshow-dir") 
						&& noslide != 0 ) {
			int imageint;
			Ecore_List *image_list;
			image_list = ecore_list_new();
			imageint = argint;
			imageint++;
			if ( ecore_file_is_dir(argv[imageint]) ) {
				char trues[PATH_MAX];
		                if (argv[imageint][strlen(argv[imageint])-1] 
								!= '/') {
        	                        snprintf(trues, PATH_MAX, 
							"%s/", argv[imageint]);
	                        }
				else {
					snprintf(trues, PATH_MAX, "%s", 
								argv[imageint]);
				}
				image_list = get_images(argv[imageint]);
				while ( !ecore_list_is_empty(image_list) ) {
					char *pathi;
					ecore_dlist_append(m->imagelist, 
								strdup(pathi));
					slidenum++;
				}
			mainwin = 0;
			nopresent = 0;
			}
		}
		else if ( argint < argc && !strcmp(argv[argint], 
				"--presentation-dir") && nopresent != 0 ) {
                        int imageint;
                        Ecore_List *image_list;
                        image_list = ecore_list_new();
                        imageint = argint;
                        imageint++;
                        if ( ecore_file_is_dir(argv[imageint]) ) {
                                char trues[PATH_MAX];
                                if (argv[imageint][strlen(argv[imageint])-1] 
								!= '/') {
                                        snprintf(trues, PATH_MAX, "%s/", 
							argv[imageint]);
                                }
                                else {
                                        snprintf(trues, PATH_MAX, "%s", 
							argv[imageint]);
                                }
                                image_list = get_images(argv[imageint]);
                                while ( !ecore_list_is_empty(image_list) ) {
                                        char *pathi;
                                        pathi = 
					  ecore_list_remove_first(image_list);
					ecore_dlist_append(m->imagelist,
								strdup(pathi));
					slidenum++;
                                }
                        mainwin = 0;
                        noslide = 0;
                        }

		}      
		else if ( argint < argc && !strcmp(argv[argint], 
				"--presentation") && nopresent != 0 ) {
			int imageint;
			imageint = argint;
			imageint++;
			while ( imageint < argc 
				&& ecore_file_exists(argv[imageint]) != 0 ) {
				ecore_dlist_append(m->imagelist, 
						strdup(argv[imageint]));
				imageint++;
			}
			mainwin	= 0;
			noslide = 0;
		}
		else if ( argint < argc && !strcmp(argv[argint], 
				"--album-presentation") && nopresent != 0 ) {
			int imageint;
			char *albuma;
			imageint = argint;
			imageint++;
			albuma = argv[imageint];
			create_list_cb(NULL, NULL, albuma);
			mainwin = 0;
			noslide = 0;
		}
		else if ( argint < argc 
				&& !strcmp(argv[argint], "--view-image") ) {
			int imageint;
			imageint = argint;
			imageint++;
			snprintf(argimage, PATH_MAX, "%s", argv[imageint]);
			argviewi = 1;
		}
		else if ( argint < argc && !strcmp(argv[argint], "--audio") ) {
			int imageint;
			imageint = argint;
			imageint++;
			snprintf(argaudio, PATH_MAX, "%s", argv[imageint]);
			audios = argaudio;
			audio = 1;
		}
		else if ( argint < argc && !strcmp(argv[argint], "--length") ) {
			int imageint;
			imageint = argint;
			imageint++;
			arglength = atoi(argv[imageint]);
		}
		else if ( argint < argc 
				&& !strcmp(argv[argint], "--win-size") ) {
			int imageint;
			imageint = argint;
			imageint++;
			snprintf(argwidth, PATH_MAX, "%s", argv[imageint]);
			imageint++;
			snprintf(argheight, PATH_MAX, "%s", argv[imageint]);
		}
		else if ( argint < argc && !strcmp(argv[argint], "--loop") ) {
			argloop = 1;
		}
		else if ( argint < argc && !strcmp(argv[argint], 
						"--fit-to-audio") ) {
			argfit = 1;
		}
		else if ( argint < argc && !strcmp(argv[argint], 
						"--fullscreen") ) {
			argfullscreen = 1;
		}
		else if ( argint < argc && !strcmp(argv[argint], 
						"--load-album") ) {
			int imageint;
			imageint = argint;
			imageint++;
			snprintf(argload, PATH_MAX, "%s", argv[imageint]);
			arglload = 1;
		}
		else if ( argint < argc && !strcmp(argv[argint], 
						"--load-dir-images") ) {
			argloadidir = 1;
		}
		else if ( argint < argc && !strcmp(argv[argint],
						"--zoom-images") ) {
			argzoom = 1;
		}
		else if ( argint < argc && !strcmp(argv[argint], "--help") ) {
			printf("ephoto /path/to/dir loads /path/to/dir"
				"as default directory | " 
			       "ephoto --album-slideshow album does"
			  	" a slideshow of album. No "
			       "need to specify a path. Just put the"
				" basename of the album. | "
			       "ephoto --audio /path/to/audio sets"
				"/path/to/audio as default audio for"
			 	"slideshow ephoto --fit-to-audio sets"
				" the slideshow to fit audio | ephoto"
				" --fullscreen sets the presentation/slideshow"
				" window to be fullscreen | ephoto --help"
				" displays all available options | "
			       "ephoto --length slidelength sets the"
				" integer slidelength(seconds) as the"
				" transition time for slideshow | "
				"ephoto --list_albums lists all of your"
				" albums. | ephoto --load-album album opens"
				" ephoto with album showing. No need to"
				" specify a path. Just put the basename"
				" of the album. | ephoto --load-dir-images"
				" loads every image specified "
				"from ephoto /path/to/dir or from your"
				" home dir if no dir is specified, into the"
				" content window | ephoto --loop sets"
				" the slideshow to loop | ephoto"
				" --presentation-dir /path/to/dir loads every"
				" image from /path/to/dir into a presentation"
				" | ephoto --presentation /path/to/image"
				" /path/to/image /path/to/image starts the"
				" presentation using the specified images |"
				" ephoto --slideshow-dir /path/to/dir loads"
				" every image from /path/to/dir into a"
				" slideshow | ephoto --slideshow"
				" /path/to/image /path/to/image"
				" /path/to/image etc. starts the slideshow"
				" using the specified images | ephoto"
				" --view-image /path/to/image sets "
			       "/path/to/image as the default image in the"
				" image viewer tab. | ephoto --win-size"
				" integer1 integer2 sets the first integer"
				" as the width and the second integer as the"
				" height of the presentation/slideshow"
				" window | ephoto --zoom-images zooms the"
				" images to fit the window\n");
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
		ewl_object_size_request(EWL_OBJECT(m->win), 585, 470);
		ewl_callback_append(m->win, EWL_CALLBACK_KEY_DOWN, 
							key_cb, NULL);
		ewl_callback_append(m->win, EWL_CALLBACK_DELETE_WINDOW, 
							destroy_cb, NULL);
		ewl_widget_show(m->win);

		mainwin = 1;

		m->avbox = ewl_vbox_new();
		ewl_container_child_append(EWL_CONTAINER(m->win), m->avbox);
		ewl_object_fill_policy_set(EWL_OBJECT(m->avbox), 
					EWL_FLAG_FILL_ALL);
		ewl_widget_show(m->avbox);

		m->menubar = ewl_hmenubar_new();
		ewl_container_child_append(EWL_CONTAINER(m->avbox), m->menubar);
		ewl_object_fill_policy_set(EWL_OBJECT(m->menubar), 
					EWL_FLAG_FILL_HFILL);
		ewl_widget_show(m->menubar);

		m->menu = ewl_menu_new();
		ewl_button_label_set(EWL_BUTTON(m->menu), "File");
		ewl_container_child_append(EWL_CONTAINER(m->menubar), m->menu);
		ewl_object_fill_policy_set(EWL_OBJECT(m->menu), 
						EWL_FLAG_FILL_NONE);
		ewl_widget_show(m->menu);
		
		m->menu_item = ewl_menu_item_new();
		ewl_button_label_set(EWL_BUTTON(m->menu_item), "Save Album");
		ewl_container_child_append(EWL_CONTAINER(m->menu), 
							m->menu_item);
		ewl_callback_append(m->menu_item, EWL_CALLBACK_CLICKED, 
						slideshow_save_cb, NULL);
		ewl_object_fill_policy_set(EWL_OBJECT(m->menu_item), 
						EWL_FLAG_FILL_ALL);
		ewl_widget_show(m->menu_item);

		m->menu_item = ewl_menu_item_new();
                ewl_button_label_set(EWL_BUTTON(m->menu_item), "Load Album");
                ewl_container_child_append(EWL_CONTAINER(m->menu), 
						m->menu_item);
                ewl_callback_append(m->menu_item, EWL_CALLBACK_CLICKED, 
						slideshow_load_cb, NULL);
		ewl_object_fill_policy_set(EWL_OBJECT(m->menu_item),
                                                EWL_FLAG_FILL_ALL);
		ewl_widget_show(m->menu_item);

		m->menu_item = ewl_menu_item_new();
		ewl_button_label_set(EWL_BUTTON(m->menu_item), "Exit");
		ewl_container_child_append(EWL_CONTAINER(m->menu), 
						m->menu_item);
		ewl_callback_append(m->menu_item, EWL_CALLBACK_CLICKED, 
						destroy_cb, NULL);
		ewl_object_fill_policy_set(EWL_OBJECT(m->menu_item),
                                                EWL_FLAG_FILL_ALL);
		ewl_widget_show(m->menu_item);

		m->menu = ewl_menu_new();
		ewl_button_label_set(EWL_BUTTON(m->menu), "Actions");
		ewl_container_child_append(EWL_CONTAINER(m->menubar), m->menu);
		ewl_object_fill_policy_set(EWL_OBJECT(m->menu), 
						EWL_FLAG_FILL_NONE);
		ewl_widget_show(m->menu);

		m->menu_item = ewl_menu_item_new();
                ewl_button_label_set(EWL_BUTTON(m->menu_item), 
						"Add All Images from Dir");
                ewl_container_child_append(EWL_CONTAINER(m->menu), 
						m->menu_item);
                ewl_callback_append(m->menu_item, EWL_CALLBACK_CLICKED, 
						addi, NULL);
		ewl_object_fill_policy_set(EWL_OBJECT(m->menu_item),
                                                EWL_FLAG_FILL_ALL);
                ewl_widget_show(m->menu_item);

		m->menu_item = ewl_menu_item_new();
                ewl_button_label_set(EWL_BUTTON(m->menu_item), "Reset Images");
                ewl_container_child_append(EWL_CONTAINER(m->menu), 
						m->menu_item);
                ewl_callback_append(m->menu_item, EWL_CALLBACK_CLICKED, 
						reseti_cb, NULL);
		ewl_object_fill_policy_set(EWL_OBJECT(m->menu_item),
                                                EWL_FLAG_FILL_ALL);
                ewl_widget_show(m->menu_item);

		m->menu_item = ewl_menu_item_new();
                ewl_button_label_set(EWL_BUTTON(m->menu_item), "Reset Audio");
                ewl_container_child_append(EWL_CONTAINER(m->menu), 
						m->menu_item);
                ewl_callback_append(m->menu_item, EWL_CALLBACK_CLICKED, 
						reseta_cb, NULL);
		ewl_object_fill_policy_set(EWL_OBJECT(m->menu_item),
                                                EWL_FLAG_FILL_ALL);
                ewl_widget_show(m->menu_item);

		m->menu_item = ewl_menu_item_new();
                ewl_button_label_set(EWL_BUTTON(m->menu_item), "Slideshow");
                ewl_container_child_append(EWL_CONTAINER(m->menu), 
						m->menu_item);
                ewl_callback_append(m->menu_item, EWL_CALLBACK_CLICKED, 
						slideshow_cb, NULL);
		ewl_object_fill_policy_set(EWL_OBJECT(m->menu_item),
                                                EWL_FLAG_FILL_ALL);
		ewl_widget_show(m->menu_item);

		m->menu_item = ewl_menu_item_new();
                ewl_button_label_set(EWL_BUTTON(m->menu_item), "Presentation");
                ewl_container_child_append(EWL_CONTAINER(m->menu), 
						m->menu_item);
                ewl_callback_append(m->menu_item, EWL_CALLBACK_CLICKED, 
						presentation_cb, NULL);
		ewl_object_fill_policy_set(EWL_OBJECT(m->menu_item),
                                                EWL_FLAG_FILL_ALL);
                ewl_widget_show(m->menu_item);

		m->menu = ewl_menu_new();
                ewl_button_label_set(EWL_BUTTON(m->menu), "Help");
                ewl_container_child_append(EWL_CONTAINER(m->menubar), m->menu);
                ewl_object_fill_policy_set(EWL_OBJECT(m->menu), 
						EWL_FLAG_FILL_NONE);
		ewl_object_alignment_set(EWL_OBJECT(m->menu), 
						EWL_FLAG_ALIGN_RIGHT);
                ewl_widget_show(m->menu);

                m->menu_item = ewl_menu_item_new();
                ewl_button_label_set(EWL_BUTTON(m->menu_item), "Ephoto Help");
                ewl_container_child_append(EWL_CONTAINER(m->menu), 
						m->menu_item);
                ewl_callback_append(m->menu_item, EWL_CALLBACK_CLICKED, 
						ephoto_men_cb, "help");
		ewl_object_fill_policy_set(EWL_OBJECT(m->menu_item),
                                                EWL_FLAG_FILL_ALL);
                ewl_widget_show(m->menu_item);

		m->menu_item = ewl_menu_item_new();
                ewl_button_label_set(EWL_BUTTON(m->menu_item), "About Ephoto");
                ewl_container_child_append(EWL_CONTAINER(m->menu), 
						m->menu_item);
                ewl_callback_append(m->menu_item, EWL_CALLBACK_CLICKED, 
						ephoto_men_cb, "about");
		ewl_object_fill_policy_set(EWL_OBJECT(m->menu_item),
                                                EWL_FLAG_FILL_ALL);
                ewl_widget_show(m->menu_item);

		m->hbox = ewl_hbox_new();
		ewl_object_alignment_set(EWL_OBJECT(m->hbox), 
						EWL_FLAG_ALIGN_CENTER);
		ewl_container_child_append(EWL_CONTAINER(m->avbox), m->hbox);
		ewl_object_fill_policy_set(EWL_OBJECT(m->hbox), 
				EWL_FLAG_FILL_ALL | EWL_FLAG_FILL_SHRINK);
		ewl_widget_show(m->hbox);

		m->vbox = ewl_vbox_new();
		ewl_object_alignment_set(EWL_OBJECT(m->vbox), 
						EWL_FLAG_ALIGN_CENTER);
		ewl_box_spacing_set(EWL_BOX(m->vbox), 3);
		ewl_container_child_append(EWL_CONTAINER(m->hbox), m->vbox);
		ewl_object_fill_policy_set(EWL_OBJECT(m->vbox), 
	EWL_FLAG_FILL_VFILL | EWL_FLAG_FILL_VSHRINK | EWL_FLAG_FILL_HSHRINK);
		ewl_widget_show(m->vbox);

		m->images = ewl_border_new();
		ewl_border_text_set(EWL_BORDER(m->images), "Add Content");
		ewl_border_label_alignment_set(EWL_BORDER(m->images), 
						EWL_FLAG_ALIGN_CENTER);
		ewl_container_child_append(EWL_CONTAINER(m->vbox), m->images);
		ewl_object_fill_policy_set(EWL_OBJECT(m->images), 
			EWL_FLAG_FILL_VFILL | EWL_FLAG_FILL_VSHRINK);	
		ewl_widget_show(m->images);

		m->directory = ewl_entry_new();
		ewl_text_text_set(EWL_TEXT(m->directory), home);
		ewl_object_alignment_set(EWL_OBJECT(m->directory), 
						EWL_FLAG_ALIGN_CENTER);
		ewl_container_child_append(EWL_CONTAINER(m->images), 
						m->directory);
		ewl_callback_append(m->directory, EWL_CALLBACK_VALUE_CHANGED, 
						populatei_cb, NULL);
		ewl_widget_show(m->directory);
	
                m->dirtree = ewl_tree_new(1);
                ewl_container_child_append(EWL_CONTAINER(m->images),
                                                                m->dirtree);
                ewl_object_custom_size_set(EWL_OBJECT(m->dirtree), 200, 210);
                ewl_tree_headers_visible_set(EWL_TREE(m->dirtree), 0);
                ewl_tree_expandable_rows_set(EWL_TREE(m->dirtree), FALSE);
                ewl_widget_show(m->dirtree);

                m->spacer = ewl_spacer_new();
                ewl_object_maximum_size_set(EWL_OBJECT(m->spacer), 10, 10);
                ewl_container_child_append(EWL_CONTAINER(m->images), m->spacer);
                ewl_widget_show(m->spacer);

                m->imagetree = ewl_tree_new(1);
                ewl_container_child_append(EWL_CONTAINER(m->images),
                                                                m->imagetree);
                ewl_tree_headers_visible_set(EWL_TREE(m->imagetree), 0);
                ewl_tree_expandable_rows_set(EWL_TREE(m->imagetree), FALSE);
		ewl_object_fill_policy_set(EWL_OBJECT(m->imagetree),
					   EWL_FLAG_FILL_ALL);
                ewl_widget_show(m->imagetree);


		m->vsep = ewl_vseparator_new();
		ewl_container_child_append(EWL_CONTAINER(m->hbox), m->vsep);
		ewl_widget_show(m->vsep);

		m->notebook = ewl_notebook_new();
		ewl_notebook_tabbar_position_set(EWL_NOTEBOOK(m->notebook), 
						EWL_POSITION_TOP);
		ewl_container_child_append(EWL_CONTAINER(m->hbox), m->notebook);
		ewl_object_alignment_set(EWL_OBJECT(m->notebook), 
						EWL_FLAG_ALIGN_CENTER);
		ewl_object_fill_policy_set(EWL_OBJECT(m->notebook), 
						EWL_FLAG_FILL_ALL);
		ewl_widget_show(m->notebook);

		m->viewbox = ewl_vbox_new();
		ewl_container_child_append(EWL_CONTAINER(m->notebook), 
						m->viewbox);
		ewl_object_alignment_set(EWL_OBJECT(m->viewbox), 
						EWL_FLAG_ALIGN_CENTER);
		ewl_box_spacing_set(EWL_BOX(m->viewbox), 3);
		ewl_object_fill_policy_set(EWL_OBJECT(m->viewbox), 
						EWL_FLAG_FILL_ALL);
		ewl_widget_show(m->viewbox);

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

		ewl_notebook_page_tab_text_set(EWL_NOTEBOOK(m->notebook), 
					m->viewbox, "Simple Image Viewer");	

		m->vbox2 = ewl_vbox_new();
		ewl_container_child_append(EWL_CONTAINER(m->notebook), 
								m->vbox2);
		ewl_object_alignment_set(EWL_OBJECT(m->vbox2), 
						EWL_FLAG_ALIGN_CENTER);
		ewl_box_spacing_set(EWL_BOX(m->vbox2), 3);
		ewl_widget_show(m->vbox2);

		ewl_notebook_page_tab_text_set(EWL_NOTEBOOK(m->notebook), 
					m->vbox2, "Slideshow/Presentation");

		m->content = ewl_border_new();
		ewl_border_text_set(EWL_BORDER(m->content), "Content");
		ewl_border_label_alignment_set(EWL_BORDER(m->content), 
						EWL_FLAG_ALIGN_CENTER);
		ewl_container_child_append(EWL_CONTAINER(m->vbox2), m->content);
		ewl_object_fill_policy_set(EWL_OBJECT(m->content), 
							EWL_FLAG_FILL_ALL);
		ewl_object_alignment_set(EWL_OBJECT(m->content), 
						EWL_FLAG_ALIGN_CENTER);
		ewl_widget_show(m->content);

		m->ib = ewl_freebox_new();
		ewl_freebox_layout_type_set(EWL_FREEBOX(m->ib), 
						EWL_FREEBOX_LAYOUT_AUTO);
		ewl_widget_show(m->ib);

		m->iscroll = ewl_scrollpane_new();
                ewl_container_child_append(EWL_CONTAINER(m->content), 
						m->iscroll);
                ewl_widget_show(m->iscroll);
		
		ewl_container_child_append(EWL_CONTAINER(m->iscroll), m->ib);		

		m->settings = ewl_border_new();
		ewl_border_text_set(EWL_BORDER(m->settings), "Settings");
		ewl_border_label_alignment_set(EWL_BORDER(m->settings), 
						EWL_FLAG_ALIGN_CENTER);
		ewl_container_child_append(EWL_CONTAINER(m->vbox2), 
						m->settings);
		ewl_object_alignment_set(EWL_OBJECT(m->settings), 
						EWL_FLAG_ALIGN_CENTER);
		ewl_box_spacing_set(EWL_BOX(m->settings), 2);
		ewl_object_maximum_size_set(EWL_OBJECT(m->settings), 
								999999, 225);
		ewl_widget_show(m->settings);

		m->hboxv = ewl_hbox_new();
		ewl_container_child_append(EWL_CONTAINER(m->settings), 
						m->hboxv);
		ewl_object_alignment_set(EWL_OBJECT(m->hboxv), 
						EWL_FLAG_ALIGN_CENTER);
		ewl_box_spacing_set(EWL_BOX(m->hboxv), 5);
		ewl_object_maximum_size_set(EWL_OBJECT(m->hboxv),  220, 100);
		ewl_widget_show(m->hboxv);

		m->text = ewl_text_new();
		ewl_container_child_append(EWL_CONTAINER(m->hboxv), m->text);
		ewl_object_alignment_set(EWL_OBJECT(m->text), 
						EWL_FLAG_ALIGN_CENTER);
		ewl_text_font_size_set(EWL_TEXT(m->text), 12);
		ewl_text_styles_set(EWL_TEXT(m->text), 
						EWL_TEXT_STYLE_UNDERLINE);
		ewl_text_underline_color_set(EWL_TEXT(m->text), 
						128, 128, 128, 128);
		ewl_text_text_set(EWL_TEXT(m->text), "Length of Slide(secs)\n");
		ewl_widget_show(m->text);

		m->slidetime = ewl_spinner_new();
		if ( arglength != 0 ) {
			ewl_spinner_value_set(EWL_SPINNER(m->slidetime), 
						arglength);
		}
		else if ( arglength == 0 ) {
			ewl_spinner_value_set(EWL_SPINNER(m->slidetime), 3);
		}
		ewl_spinner_min_val_set(EWL_SPINNER(m->slidetime), 1);
		ewl_spinner_max_val_set(EWL_SPINNER(m->slidetime), 100000);
		ewl_container_child_append(EWL_CONTAINER(m->hboxv), 
						m->slidetime);
		ewl_spinner_step_set(EWL_SPINNER(m->slidetime), 1);
		ewl_spinner_digits_set(EWL_SPINNER(m->slidetime), 0);
		ewl_object_alignment_set(EWL_OBJECT(m->slidetime), 
						EWL_FLAG_ALIGN_CENTER);
		ewl_object_maximum_size_set(EWL_OBJECT(m->slidetime),  55, 20);
		ewl_callback_append(m->slidetime, EWL_CALLBACK_CLICKED, 
						rad_cb, NULL);
		ewl_widget_show(m->slidetime);

		m->hboxv = ewl_hbox_new();
		ewl_container_child_append(EWL_CONTAINER(m->settings), 
						m->hboxv);
		ewl_object_alignment_set(EWL_OBJECT(m->hboxv), 
						EWL_FLAG_ALIGN_CENTER);
		ewl_box_spacing_set(EWL_BOX(m->hboxv), 5);
		ewl_object_maximum_size_set(EWL_OBJECT(m->hboxv),  220, 100);
		ewl_widget_show(m->hboxv);

		m->loopcheck = ewl_checkbutton_new();
		ewl_button_label_set(EWL_BUTTON(m->loopcheck), 
						"Loop Slideshow");
		ewl_container_child_append(EWL_CONTAINER(m->hboxv), 
						m->loopcheck);
		if ( argloop != 0 ) {
			ewl_checkbutton_checked_set(EWL_CHECKBUTTON
						(m->loopcheck), TRUE);
		}
		ewl_object_maximum_size_set(EWL_OBJECT(m->loopcheck), 130, 50);
		ewl_object_alignment_set(EWL_OBJECT(m->loopcheck), 
						EWL_FLAG_ALIGN_CENTER);
		ewl_widget_show(m->loopcheck);

		m->audiolen = ewl_checkbutton_new();
		ewl_button_label_set(EWL_BUTTON(m->audiolen), "Fit to Audio");
		if ( argfit == 1 ) {
			ewl_checkbutton_checked_set(EWL_CHECKBUTTON
						(m->audiolen), TRUE);
		}
		ewl_container_child_append(EWL_CONTAINER(m->hboxv), 
						m->audiolen);
		ewl_object_maximum_size_set(EWL_OBJECT(m->audiolen), 150, 50);
		ewl_object_alignment_set(EWL_OBJECT(m->audiolen), 
						EWL_FLAG_ALIGN_CENTER);
		ewl_callback_append(m->audiolen, EWL_CALLBACK_CLICKED, 
						rad_cb, NULL);
		ewl_widget_show(m->audiolen);
		ewl_widget_disable(m->audiolen);

		m->text = ewl_text_new();
		ewl_container_child_append(EWL_CONTAINER(m->settings), m->text);
		ewl_object_alignment_set(EWL_OBJECT(m->text), 
						EWL_FLAG_ALIGN_CENTER);
		ewl_text_font_size_set(EWL_TEXT(m->text), 12);
		ewl_text_styles_set(EWL_TEXT(m->text), 
						EWL_TEXT_STYLE_UNDERLINE);
		ewl_text_underline_color_set(EWL_TEXT(m->text), 
						128, 128, 128, 128);
		ewl_text_text_set(EWL_TEXT(m->text), 
				"Size of Slideshow/Presentation(pixels)");
		ewl_widget_show(m->text);

		m->hboxv = ewl_hbox_new();
		ewl_container_child_append(EWL_CONTAINER(m->settings), 
						m->hboxv);
		ewl_object_alignment_set(EWL_OBJECT(m->hboxv), 
						EWL_FLAG_ALIGN_CENTER);
		ewl_box_spacing_set(EWL_BOX(m->hboxv), 5);
		ewl_object_maximum_size_set(EWL_OBJECT(m->hboxv),  220, 100);
		ewl_object_fill_policy_set(EWL_OBJECT(m->hboxv), 
						EWL_FLAG_FILL_SHRINK);
		ewl_widget_show(m->hboxv);

		m->rad4 = ewl_radiobutton_new();
		ewl_button_label_set(EWL_BUTTON(m->rad4), "Custom");
		ewl_container_child_append(EWL_CONTAINER(m->hboxv), m->rad4);
		ewl_radiobutton_checked_set(EWL_RADIOBUTTON(m->rad4), TRUE);
		ewl_object_maximum_size_set(EWL_OBJECT(m->rad4), 130, 50);
		ewl_object_alignment_set(EWL_OBJECT(m->rad4), 
						EWL_FLAG_ALIGN_LEFT);
		ewl_callback_append(m->rad4, EWL_CALLBACK_CLICKED, 
						rad_cb, NULL);
		ewl_callback_append(m->rad4, EWL_CALLBACK_REALIZE, 
						rad_cb, NULL);
		ewl_widget_show(m->rad4);

		m->fullrad = ewl_radiobutton_new();
		ewl_button_label_set(EWL_BUTTON(m->fullrad), "Fullscreen");
		if ( argfullscreen == 1 ) {
			ewl_checkbutton_checked_set(EWL_CHECKBUTTON
						(m->fullrad), TRUE);
			ewl_checkbutton_checked_set(EWL_CHECKBUTTON(m->rad4), 
						FALSE);
		}
		ewl_container_child_append(EWL_CONTAINER(m->hboxv), m->fullrad);
		ewl_object_maximum_size_set(EWL_OBJECT(m->fullrad), 130, 50);
		ewl_object_alignment_set(EWL_OBJECT(m->fullrad), 
						EWL_FLAG_ALIGN_LEFT);
		ewl_callback_append(m->fullrad, EWL_CALLBACK_CLICKED, 
						rad_cb, NULL);
		ewl_widget_show(m->fullrad);

		ewl_radiobutton_chain_set(EWL_RADIOBUTTON(m->rad4), 
						EWL_RADIOBUTTON(m->fullrad));

		m->zoom = ewl_checkbutton_new();
		ewl_button_label_set(EWL_BUTTON(m->zoom), 
						"Zoom Images to Fit Window");
                ewl_container_child_append(EWL_CONTAINER(m->settings), m->zoom);
                ewl_object_alignment_set(EWL_OBJECT(m->zoom), 
						EWL_FLAG_ALIGN_CENTER);
		if ( argzoom == 1 ) {
			ewl_checkbutton_checked_set(EWL_CHECKBUTTON(m->zoom), 
								TRUE);
		}
                ewl_widget_show(m->zoom);

		m->hboxv = ewl_hbox_new();
		ewl_container_child_append(EWL_CONTAINER(m->settings), 
						m->hboxv);
		ewl_object_alignment_set(EWL_OBJECT(m->hboxv), 
						EWL_FLAG_ALIGN_CENTER);
		ewl_box_spacing_set(EWL_BOX(m->hboxv), 5);
		ewl_object_maximum_size_set(EWL_OBJECT(m->hboxv),  220, 100);
		ewl_object_fill_policy_set(EWL_OBJECT(m->hboxv), 
						EWL_FLAG_FILL_SHRINK);
		ewl_widget_show(m->hboxv);

		m->text = ewl_text_new();
		ewl_container_child_append(EWL_CONTAINER(m->hboxv), m->text);
		ewl_object_alignment_set(EWL_OBJECT(m->text), 
						EWL_FLAG_ALIGN_CENTER);
		ewl_text_font_size_set(EWL_TEXT(m->text), 10);
		ewl_text_text_set(EWL_TEXT(m->text), "Width");
		ewl_widget_show(m->text);

		m->wsize = ewl_entry_new();
		ewl_text_text_set(EWL_TEXT(m->wsize), argwidth);
		ewl_entry_editable_set(EWL_ENTRY(m->wsize), 0);
		ewl_object_maximum_size_set(EWL_OBJECT(m->wsize), 50, 10);
		ewl_object_alignment_set(EWL_OBJECT(m->wsize), 
						EWL_FLAG_ALIGN_CENTER);
		ewl_container_child_append(EWL_CONTAINER(m->hboxv), m->wsize);
		ewl_text_font_size_set(EWL_TEXT(m->wsize), 10);
		ewl_widget_show(m->wsize);

		m->text = ewl_text_new();
		ewl_container_child_append(EWL_CONTAINER(m->hboxv), m->text);
		ewl_object_alignment_set(EWL_OBJECT(m->text), 
						EWL_FLAG_ALIGN_CENTER);
		ewl_text_font_size_set(EWL_TEXT(m->text), 10);
		ewl_text_text_set(EWL_TEXT(m->text), "Height");
		ewl_widget_show(m->text);

		m->hsize = ewl_entry_new();
		ewl_text_text_set(EWL_TEXT(m->hsize), argheight);
		ewl_entry_editable_set(EWL_ENTRY(m->hsize), 0);
		ewl_object_maximum_size_set(EWL_OBJECT(m->hsize), 50, 10);
		ewl_object_alignment_set(EWL_OBJECT(m->hsize), 
						EWL_FLAG_ALIGN_CENTER);
		ewl_container_child_append(EWL_CONTAINER(m->hboxv), m->hsize);
		ewl_text_font_size_set(EWL_TEXT(m->hsize), 10);
		ewl_widget_show(m->hsize);

		m->text = ewl_text_new();
		ewl_container_child_append(EWL_CONTAINER(m->settings), m->text);
		ewl_object_alignment_set(EWL_OBJECT(m->text), 
						EWL_FLAG_ALIGN_CENTER);
		ewl_text_font_size_set(EWL_TEXT(m->text), 12);
		ewl_text_styles_set(EWL_TEXT(m->text), 
						EWL_TEXT_STYLE_UNDERLINE);
		ewl_text_underline_color_set(EWL_TEXT(m->text), 
						128, 128, 128, 128);
		ewl_text_text_set(EWL_TEXT(m->text), "Audio");
		ewl_widget_show(m->text);

		m->atext = ewl_text_new();
		ewl_container_child_append(EWL_CONTAINER(m->settings), 
						m->atext);
		ewl_object_alignment_set(EWL_OBJECT(m->atext), 
						EWL_FLAG_ALIGN_CENTER);
		ewl_widget_show(m->atext);

		m->hboxv = ewl_hbox_new();
		ewl_container_child_append(EWL_CONTAINER(m->settings), 
						m->hboxv);
		ewl_object_alignment_set(EWL_OBJECT(m->hboxv), 
						EWL_FLAG_ALIGN_CENTER);
		ewl_box_spacing_set(EWL_BOX(m->hboxv), 5);
		ewl_object_maximum_size_set(EWL_OBJECT(m->hboxv),  220, 100);
		ewl_widget_show(m->hboxv);

		m->slideshow = ewl_button_new();
		ewl_button_label_set(EWL_BUTTON(m->slideshow), "Slideshow");
		ewl_container_child_append(EWL_CONTAINER(m->hboxv), 
						m->slideshow);
		ewl_object_alignment_set(EWL_OBJECT(m->slideshow), 
						EWL_FLAG_ALIGN_CENTER);
		ewl_object_maximum_size_set(EWL_OBJECT(m->slideshow), 80, 15);
		ewl_callback_append(m->slideshow, EWL_CALLBACK_CLICKED, 
						slideshow_cb, NULL);
		ewl_widget_show(m->slideshow);
		ewl_widget_disable(m->slideshow);

		m->presentation = ewl_button_new();
		ewl_button_label_set(EWL_BUTTON(m->presentation), 
						"Presentation");
		ewl_container_child_append(EWL_CONTAINER(m->hboxv), 
						m->presentation);
		ewl_object_alignment_set(EWL_OBJECT(m->presentation), 
						EWL_FLAG_ALIGN_CENTER);
		ewl_object_maximum_size_set(EWL_OBJECT(m->presentation), 
						80, 15);
		ewl_callback_append(m->presentation, EWL_CALLBACK_CLICKED, 
						presentation_cb, NULL);
		ewl_widget_show(m->presentation);
		ewl_widget_disable(m->presentation);

		ewl_object_fill_policy_set(EWL_OBJECT(m->vbox2), 
						EWL_FLAG_FILL_ALL);
		ewl_object_fill_policy_set(EWL_OBJECT(m->hbox), 
						EWL_FLAG_FILL_ALL);
		/**********************************************************/

                /************LETS POPULATE THEM TREES******************/
                ewl_callback_append(m->directory, EWL_CALLBACK_SHOW, 
						populatei_cb, NULL);
		ewl_callback_append(m->vimage, EWL_CALLBACK_SHOW,
						imagerealize_cb, NULL);
                /******************************************************/
		if ( arglload == 1 ) {
			load_cb(NULL, NULL, NULL);
		}
		if ( argviewi == 0 ) {
			ewl_notebook_visible_page_set(EWL_NOTEBOOK
						(m->notebook), m->vbox2);
		}
		if ( argviewi == 1 ) {
			ewl_notebook_visible_page_set(EWL_NOTEBOOK
						(m->notebook), m->viewbox);
		}
		if ( argloadidir == 1 ) {
			addi(NULL, NULL, NULL);
		}
		ewl_main();
	return 0;
	}
}
