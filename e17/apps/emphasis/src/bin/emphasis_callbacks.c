#include "emphasis.h"
#include "emphasis_gui.h"
#include "emphasis_misc.h"
#include "emphasis_callbacks.h"
#include <Evas.h>

/**
 * @brief Quit the main loop
 */
void
cb_quit(Etk_Object *object, void *data)
{
	Etk_Widget *window;
	Emphasis_Config *config;

	config = malloc(sizeof(Emphasis_Config));
	config = etk_object_data_get(object, "config");
	etk_widget_geometry_get(ETK_WIDGET(object), &(config->geometry.x), &(config->geometry.y), &(config->geometry.w), &(config->geometry.h));
	config_write(config);

	eet_shutdown();
	etk_main_quit();
}

/**
 * @brief Stop the playing
 */
void
cb_button_stop_clicked(Etk_Object *object, void *data)
{
	mpc_stop();
}

/**
 * @brief Re-play the song from the start or play the previous song
 */
void
cb_button_prev_clicked(Etk_Object *object, void *data)
{
	mpc_prev();
}

/**
 * @brief Toggle play/pause the current song
 */
void
cb_button_play_clicked(Etk_Object *object, void *data)
{
	mpc_toggle_play_pause();
}

/**
 * @brief Play the next song
 */
void
cb_button_next_clicked(Etk_Object *object, void *data)
{
	mpc_next();
}

/**
 * @brief Toggle the random mode
 */
void
cb_toggle_random(Etk_Object *object, void *data)
{
	mpc_toggle_random();
}

/**
 * @brief Toggle the repeat mode
 */
void
cb_toggle_repeat(Etk_Object *object, void *data)
{
	mpc_toggle_repeat();
}

/**
 * @brief Seek in the song the same percent position that was clicked on the progress bar
 */
void
cb_seek_time(Etk_Object *object, Etk_Event_Mouse_Up_Down *event, void *data)
{
	Etk_Widget *progress;
	int x_click, widget_width;
	
	progress = ETK_WIDGET(object);
	x_click = event->widget.x;
	
	etk_widget_geometry_get(progress, NULL, NULL, &widget_width, NULL);
	
	mpc_seek((float)x_click/widget_width);
}

/**
 * @brief Get the name of the artist selected, search for his albums and set the album tree with it
 */
void
cb_tree_artist_selected(Etk_Object *object, Etk_Tree_Row *row, void *data)
{
	Etk_Tree *tree;
	MpdData *list=NULL;
	char *artist;
	Emphasis_Gui *gui;
	
	gui = data;
	tree = ETK_TREE(gui->tree_album);
	
	artist = etk_tree_row_data_get(row);
	if (artist)
		list = mpc_mlib_album_get(artist);
	else
		list = mpc_mlib_album_get("");
		
	emphasis_tree_mlib_set(tree, list, MPD_DATA_TYPE_TAG);
	
	ETK_TREE(gui->tree_artist)->last_selected = row;
	etk_tree_row_select(etk_tree_first_row_get(tree));

	etk_object_data_set(ETK_OBJECT(gui->tree_artist), "artist", artist);
}

/**
 * @brief Get the album selected, search for songs matching this album and artist.
 *        Then set the tree track with it.
 */
void
cb_tree_album_selected(Etk_Object *object, Etk_Tree_Row *row, void *data)
{
	Emphasis_Gui *gui;
	Etk_Tree *tree;
	MpdData *list;
	char *album, *artist;
	
	tree = ETK_TREE(object);
	album = etk_tree_row_data_get(row);

	gui = data;
	row = ETK_TREE(gui->tree_artist)->last_selected;	
	
	artist = etk_tree_row_data_get(row);
	
	list = mpc_mlib_track_get(artist, album);
	/*
	Emphasis_Data *song;
	song = malloc(sizeof(Emphasis_Song));
	Evas_List *evaslist;
	evaslist = convert_mpd_data(list);
	if (evaslist)
	{
		song = evas_list_data(evaslist);
	}

	MpdData *platypus;
	platypus = convert_evas_list(evaslist);
	printf("%s\n", platypus->song->title);
	*/
	emphasis_tree_mlib_set(ETK_TREE(gui->tree_track), list, MPD_DATA_TYPE_SONG);
}

/**
 * @brief Set a drag widget from a list of artist selected
 * @param data A common drag widget
 */
void
cb_drag_artist(Etk_Object *object, void *data)
{
	Etk_Widget *drag_menu, *menu_item;
	Etk_Drag *drag;
	Etk_Tree *tree;
	Etk_Tree_Row *row;
	Evas_List *rowlist, *next;
	char *artist;
	const char **types;
	unsigned int num_types;
	MpdData *playlist=NULL, *tmplist;

	tree = ETK_TREE(object);
	drag = ETK_DRAG((ETK_WIDGET(object))->drag);
	rowlist = etk_tree_selected_rows_get(tree);
	
	drag_menu = etk_menu_new();
	
	while (rowlist)
	{
		artist = etk_tree_row_data_get(evas_list_data(rowlist));
		tmplist = mpc_mlib_track_get(artist, NULL);
		
		if (artist)
			menu_item = etk_menu_item_new_with_label(artist);
		else
			menu_item = etk_menu_item_new_with_label("All");
			
		etk_menu_shell_append(ETK_MENU_SHELL(drag_menu), ETK_MENU_ITEM(menu_item));
		if (!playlist)
		{
			playlist = tmplist;
		}
		else
		{
			playlist = mpd_data_concatenate(playlist, tmplist);
		}
		next = evas_list_next(rowlist);		
		rowlist = next;
	}
	evas_list_free(rowlist);
	
	types = calloc(1, sizeof(char));
	num_types = 1;
	types[0] = strdup("MpdData");
	
	etk_drag_types_set(drag, types, num_types);
	etk_drag_data_set(drag, playlist, 1);

	etk_container_add(ETK_CONTAINER(drag), drag_menu);
	(ETK_WIDGET(data))->drag = ETK_WIDGET(drag);
}

/**
 * @brief Set a drag widget from a list of albums selected
 * @param data An Emphasis_Gui
 */
void
cb_drag_album(Etk_Object *object, void *data)
{
	Etk_Widget *drag_menu, *menu_item;
	Etk_Drag *drag;
	Etk_Tree *tree;
	Evas_List *rowlist;
	char *album, *artist;
	const char **types;
	unsigned int num_types;
	MpdData *playlist=NULL, *tmplist;
	
	tree = ETK_TREE(object);
	drag = ETK_DRAG((ETK_WIDGET(object))->drag);
	rowlist = etk_tree_selected_rows_get(tree);
	
	tree = ETK_TREE(((Emphasis_Gui *)data)->tree_artist);
	artist = etk_tree_row_data_get(etk_tree_selected_row_get(tree));
	
	drag_menu = etk_menu_new();
		
	while (rowlist)
	{
		album = etk_tree_row_data_get(evas_list_data(rowlist));
		tmplist = mpc_mlib_track_get(artist, album);
		
		if (album)
			menu_item = etk_menu_item_new_with_label(album);
		else
			menu_item = etk_menu_item_new_with_label("All");

		etk_menu_shell_append(ETK_MENU_SHELL(drag_menu), ETK_MENU_ITEM(menu_item));
		if (!playlist)
		{
			playlist = tmplist;
		}
		else
		{
			playlist = mpd_data_concatenate(playlist, tmplist);
		}
		rowlist = evas_list_next(rowlist);
	}
	
	types = calloc(1, sizeof(char));
	num_types = 1;
	types[0] = strdup("MpdData");
	
	etk_drag_types_set(drag, types, num_types);
	etk_drag_data_set(drag, playlist, 1);
	
	etk_container_add(ETK_CONTAINER(drag), drag_menu);

	(ETK_WIDGET(((Emphasis_Gui *)data)->drag))->drag = ETK_WIDGET(drag);
}

/**
 * @brief Set a drag widget from a list of selected song's row
 * @param data A drag widget
 * @note this callback will set the data's drag and the object's drag
 */
void
cb_drag_track(Etk_Object *object, void *data)
{
	Etk_Widget *drag_menu, *menu_item;
	Etk_Drag *drag;
	Etk_Tree *tree;
	Evas_List *rowlist;
	char *title;
	const char **types;
	unsigned int num_types;
	MpdData *playlist;

	tree = ETK_TREE(object);
	drag = ETK_DRAG((ETK_WIDGET(object))->drag);
	
	drag_menu = etk_menu_new();
	
	rowlist = etk_tree_selected_rows_get(tree);
	playlist = convert_rowlist_in_playlist_with_file(rowlist);
	while (rowlist)
	{
		etk_tree_row_fields_get(evas_list_data(rowlist), etk_tree_nth_col_get(tree, 0), &title, NULL);
		
		menu_item = etk_menu_item_new_with_label(title);
		etk_menu_shell_append(ETK_MENU_SHELL(drag_menu), ETK_MENU_ITEM(menu_item));
		
		rowlist = evas_list_next(rowlist);
	}
	
	types = calloc(1, sizeof(char));
	num_types = 1;
	types[0] = strdup("MpdData");
	
	etk_drag_types_set(drag, types, num_types);
	etk_drag_data_set(drag, playlist, 1);
	
	etk_tree_row_fields_get(evas_list_data(rowlist), etk_tree_nth_col_get(tree, 0), &title, NULL);
	
	etk_container_add(ETK_CONTAINER(drag), drag_menu);

	(ETK_WIDGET(data))->drag = ETK_WIDGET(drag);

}

/**
 * @brief Get the playlist draged. Add it to the current playlist.
 *        And set the playlist tree up to date.
 * @param data A drag widget with a playlist in his data
 */
void
cb_drop_song(Etk_Object *object, void *event, void *data)
{
	Etk_Tree *tree;
	Etk_Tree_Row *row;
	Etk_Drag *drag;
	MpdData *list;
	
	tree = ETK_TREE(object);
	
	drag = ETK_DRAG((ETK_WIDGET(data))->drag);	
	list = drag->data;
	mpc_playlist_add(list);
	
	free(drag->types[0]);
	free(drag->types);	
	
	/*
	Etk_Widget *wid;
	Evas_List *evaslist;
	evaslist = etk_container_children_get(ETK_CONTAINER(drag));
	while (evaslist)
	{
		etk_container_remove(ETK_CONTAINER(drag), evas_list_data(evaslist));
		etk_object_destroy(ETK_OBJECT(evas_list_data(evaslist)));
		evaslist = evas_list_next(evaslist);
	}
	*/
}

/**
 * @brief Double-click on a row add the artist's song to the playlist
 */
void
cb_tree_mlib_clicked(Etk_Object *object, Etk_Tree_Row *row, Etk_Event_Mouse_Up_Down *event, void *data)
{
	Etk_Widget *widget, *tree;
	char *str;

	if ((event->button == 1) && (event->flags & EVAS_BUTTON_DOUBLE_CLICK))
	{
		emphasis_playlist_append_selected(ETK_TREE(object), (Emphasis_Type)data);
		mpc_play_if_stopped();
	}
	if ((event->button == 3) && (event->flags & EVAS_BUTTON_DOUBLE_CLICK))
	{
		widget = etk_widget_parent_get(ETK_WIDGET(object));
		widget = etk_widget_parent_get(ETK_WIDGET(widget));
		tree = etk_paned_child2_get(ETK_PANED(widget));
		
		etk_tree_row_fields_get(row, etk_tree_nth_col_get(ETK_TREE(object), 0), &str, NULL);
		
		emphasis_playlist_search_and_delete(ETK_TREE(tree), str, (Emphasis_Type)data);
	}
}

/**
 * @brief Play the song double-clicked
 */
void
cb_tree_pls_clicked(Etk_Object *object, Etk_Tree_Row *row, Etk_Event_Mouse_Up_Down *event, void *data)
{
	int id;
	
	if ((event->button == 1) && (event->flags & EVAS_BUTTON_DOUBLE_CLICK))
	{
		id = (int)etk_tree_row_data_get(row);
		mpc_play_id(id);
	}
	if (event->button == 1)
	{
		ETK_TREE(object)->last_selected = row;
	}
}

void
cb_emphasis_bindings_key(Etk_Object *object, Etk_Event_Key_Up_Down *event, void *data)
{
	if (!strcmp(event->key, "z")) 
	{
		cb_toggle_random(NULL, NULL);
		return;
	}
	if (!strcmp(event->key, "r"))
	{
		cb_toggle_repeat(NULL, NULL);
		return;
	}
	if (!strcmp(event->key, "s"))
	{
		cb_button_stop_clicked(NULL, NULL);
		return;
	}
	if (!strcmp(event->key, "p"))
	{
		cb_button_play_clicked(NULL, NULL);
		return;
	}
	if (!strcmp(event->key, "greater"))
	{
		cb_button_next_clicked(NULL, NULL);
		return;
	}
	if (!strcmp(event->key, "less"))
	{
		cb_button_prev_clicked(NULL, NULL);
		return;
	}
}

/**
 * @brief Fast find in mlib tree
 */
void
cb_mlib_bindings_key(Etk_Object *object, Etk_Event_Key_Up_Down *event, void *data)
{
	Etk_Tree *tree;
	Etk_Tree_Row *row, *row_next;
	Etk_Tree_Col *col;
	char *str;
	
	if (!event->string)
		return;
	
	tree = ETK_TREE(object);

	if (!strcmp(event->key, "a") && evas_key_modifier_is_set(event->modifiers, "Control")) 
	{
		etk_tree_select_all(tree);
		return;
	}

	col = etk_tree_nth_col_get(tree, 0);
	
/**/
	const char *cur_title, *base_title;
	char *title, *filter=NULL;
	int length;
	Emphasis_Type type;

	base_title = etk_object_data_get(ETK_OBJECT(tree), "title"); /*get the base title of the tree */
	cur_title = etk_tree_col_title_get(col); /*get the current title of tree */
	type = (Emphasis_Type)etk_object_data_get(ETK_OBJECT(tree), "Emphasis_Type");

	if (!strcmp(base_title, cur_title)) 
	{
		if (!strcmp("BackSpace", event->keyname) ||
			 !strcmp("Escape", event->keyname) ||
			 !strcmp("Return", event->keyname))
			return;

		asprintf(&title, "%s starting with : %s", base_title, event->string);
	}
	else
	{
		if (!strcmp("BackSpace", event->keyname))
		{
			title = strdup(cur_title);
			title[strlen(title)-1] = '\0';
			emphasis_tree_mlib_init(data, type);
			if (strlen(title) == strlen(base_title)+17)
			{
				etk_tree_col_title_set(col, base_title);
				return;
			}
		}
		else if (!strcmp("Escape", event->keyname))
		{
			emphasis_tree_mlib_init(data, type);
			etk_tree_col_title_set(col, base_title);
			return;
		}
		else if (!strcmp("Return", event->keyname))
		{
			etk_tree_col_title_set(col, base_title);
			return;
		}
		else
			asprintf(&title, "%s%s", cur_title, event->string);
	}

	etk_tree_col_title_set(etk_tree_nth_col_get(tree, 0), title);
	length = strlen(base_title) + 17; /*18 == strlen(" starting with : ") + 1*/
	filter = strdup(&title[length]);
	length = strlen(filter);
/**/
		
	row = etk_tree_first_row_get(tree);
	while (row)
	{
		row_next = etk_tree_next_row_get(row, ETK_FALSE, ETK_FALSE);
		etk_tree_row_fields_get(row, col, &str, NULL);
		if (!strncasecmp("The ", str, 4))
		{
			str = str+4;
		}
		if (!strcmp("All", str) || (strncasecmp(filter, str, length) !=0))
			etk_tree_row_del(row);
		row = row_next;
	}

	free(title);
	free(filter);
}

/**
 * @brief If right-clicked: popdown the contextual menu on the playlist
 */
void
cb_pls_contextual_menu(Etk_Object *object, Etk_Event_Mouse_Up_Down *event, void *data)
{
	if (event->button == 3)
	{
	 etk_menu_popup(ETK_MENU(data));
	}
}

/**
 * @brief Clear the playlist
 */
void
cb_playlist_clear(Etk_Object *object, Etk_Event_Mouse_Up_Down *event, void *data)
{
	mpc_playlist_clear();
}

/**
 * @brief Callback function of bindings key on the playlist
 */
void
cb_pls_bindings_key(Etk_Object *object, Etk_Event_Key_Up_Down *event, void *data)
{
	Emphasis_Gui *gui;
	Etk_Tree_Row *row;
	Evas_List *rowlist;
	MpdData *list;
	
	gui = (Emphasis_Gui *)data;
	
	if (!strcmp(event->key,"d") || !strcmp(event->key,"Delete"))
	{
		rowlist = etk_tree_selected_rows_get(ETK_TREE(gui->tree_pls));
		if (rowlist)
		{
			list = convert_rowlist_in_playlist_with_id(rowlist);
			mpc_playlist_delete(list);
		}
	}
	etk_widget_redraw_queue(ETK_WIDGET(gui->tree_pls));
}

/**
 * @brief Delete all the selected item from the playlist
 */
void
cb_playlist_delete(Etk_Object *object, Etk_Event_Mouse_Up_Down *event, void *data)
{
	Emphasis_Gui *gui;
	Etk_Tree_Row *row;
	Evas_List *rowlist;
	MpdData *list;
	
	gui = (Emphasis_Gui *)data;
	
	rowlist = etk_tree_selected_rows_get(ETK_TREE(gui->tree_pls));
	row = etk_tree_first_row_get(ETK_TREE(gui->tree_pls));
	list = convert_rowlist_in_playlist_with_id(rowlist);
	
	mpc_playlist_delete(list);
}

/**
 * @brief Changed the mpd output volume level
 */
void
cb_vol_slider_value_changed(Etk_Object *object, double value, void *data)
{
	mpc_change_vol(value);
}

/**
 * @brief Request an database update
 */
void
cb_database_update(Etk_Object *object, Etk_Event_Mouse_Up_Down *event, void *data)
{
	mpc_database_update("/home/lok/Music/");
}

void
cb_config_show(Etk_Object *object, void *data)
{
	Emphasis_Gui *gui;
	Emphasis_Config *config;

	gui = data;
	config_gui_init(gui->config_gui);
	config = config_load();
	config_gui_set(gui->config_gui, config);
	etk_widget_show_all(gui->config_gui->window);
	etk_signal_connect("clicked", ETK_OBJECT(gui->config_gui->buttons_box_ok), ETK_CALLBACK(cb_config_write), gui);
	etk_signal_connect("clicked", ETK_OBJECT(gui->config_gui->buttons_box_apply), ETK_CALLBACK(cb_config_write), gui);
	etk_signal_connect("clicked", ETK_OBJECT(gui->config_gui->buttons_box_cancel), ETK_CALLBACK(cb_config_hide), gui);
}

void
cb_config_hide(Etk_Object *object, void *data)
{
	Emphasis_Config_Gui *gui;
	
	gui = data;
	etk_object_destroy(ETK_OBJECT(gui->window));
}

void
cb_config_write(Etk_Object *object, void *data)
{
	Emphasis_Config *config;
	Emphasis_Gui *gui;
	const char *port;
	
	config = config_new();
	gui = data;
	
	config->hostname = etk_entry_text_get(ETK_ENTRY(gui->config_gui->hostname_entry));
	port = etk_entry_text_get(ETK_ENTRY(gui->config_gui->port_spin));
	config->port = atoi(port);
	config->password = etk_entry_text_get(ETK_ENTRY(gui->config_gui->password_entry));
	
	config_write(config);
	cb_config_hide(NULL, gui->config_gui);
	emphasis_try_connect(gui);
}

void
cb_switch_full(Etk_Object *object, void *data)
{
	Emphasis_Gui *gui;
	Emphasis_Config *config;
	
	gui=data;
	
	config = etk_object_data_get(ETK_OBJECT(gui->window), "config");

	etk_container_border_width_set(ETK_CONTAINER(gui->window), 6);
	etk_widget_show_all(gui->vpaned);
	etk_window_resize(ETK_WINDOW(gui->window), config->geometry.w, config->geometry.h);
}

void
cb_switch_small(Etk_Object *object, void *data)
{
	Emphasis_Gui *gui;
	Emphasis_Config *config;
	int w,h;
	
	gui=data;
	
	etk_widget_geometry_get(gui->window, NULL, NULL, &w, &h);
	config = etk_object_data_get(ETK_OBJECT(gui->window), "config");
	config->geometry.w = w;
	config->geometry.h = h;

	etk_container_border_width_set(ETK_CONTAINER(gui->window), 0);
	etk_widget_hide_all(gui->vpaned);
	etk_window_resize(ETK_WINDOW(gui->window), 0, 0);
}
