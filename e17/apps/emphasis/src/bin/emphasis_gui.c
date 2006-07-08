#include "emphasis.h"
#include "emphasis_mpc.h"
#include "emphasis_callbacks.h"
#include "emphasis_gui.h"

/**
 * @brief Build all widgets for Emphasis GUI
 * @param gui A gui to initialize
 */
void
emphasis_init_gui(Emphasis_Gui *gui)
{	
	/* window setup */
	gui->window = etk_window_new();
	etk_window_title_set(ETK_WINDOW(gui->window),"Emphasis");
	etk_window_wmclass_set(ETK_WINDOW(gui->window), "Emphasis", "emphasis");
	etk_container_border_width_set(ETK_CONTAINER(gui->window), 6);
	etk_window_resize(ETK_WINDOW(gui->window), 700, 600);
	etk_signal_connect("destroyed", ETK_OBJECT(gui->window), ETK_CALLBACK(cb_quit), gui);
	
	/* ??? */
	gui->drag = etk_drag_new(gui->window);
	
	/* vbox setup */
	/* contain three parts : player, medialib, playlist */
	gui->vbox = etk_vbox_new(ETK_FALSE, 0);
	etk_container_add(ETK_CONTAINER(gui->window), gui->vbox);

	/**********
	 * Player *
	 **********/
	
	/* player's root hbox */
	/* left: player commands | right: player info */
	gui->hbox_player = etk_hbox_new(ETK_FALSE, 2);
	etk_box_pack_start(ETK_BOX(gui->vbox), gui->hbox_player, ETK_FALSE, ETK_FALSE, 0);

	/* player control vbox */
	/* contain: button_box, vol control, random/repeat */
	gui->vbox_controls = etk_vbox_new(ETK_FALSE, 4);
	etk_box_pack_start(ETK_BOX(gui->hbox_player), gui->vbox_controls, ETK_FALSE, ETK_FALSE, 5);

	/* Button box setup */	
	gui->button_box = etk_hbox_new(ETK_FALSE, 0);
	gui->btn_stop = etk_button_new_from_stock(ETK_STOCK_MEDIA_PLAYBACK_STOP);
	gui->btn_prev = etk_button_new_from_stock(ETK_STOCK_MEDIA_SKIP_BACKWARD);
	gui->btn_play = etk_button_new_from_stock(ETK_STOCK_MEDIA_PLAYBACK_START);
	gui->btn_next = etk_button_new_from_stock(ETK_STOCK_MEDIA_SKIP_FORWARD);
	etk_button_label_set(ETK_BUTTON(gui->btn_stop), NULL);
	etk_button_label_set(ETK_BUTTON(gui->btn_prev), NULL);
	etk_button_label_set(ETK_BUTTON(gui->btn_play), NULL);
	etk_button_label_set(ETK_BUTTON(gui->btn_next), NULL);
	etk_box_pack_start(ETK_BOX(gui->button_box), gui->btn_stop, ETK_FALSE, ETK_FALSE, 3);
	etk_box_pack_start(ETK_BOX(gui->button_box), gui->btn_prev, ETK_FALSE, ETK_FALSE, 3);
	etk_box_pack_start(ETK_BOX(gui->button_box), gui->btn_play, ETK_FALSE, ETK_FALSE, 3);
	etk_box_pack_start(ETK_BOX(gui->button_box), gui->btn_next, ETK_FALSE, ETK_FALSE, 3);
	etk_box_pack_start(ETK_BOX(gui->vbox_controls), gui->button_box, ETK_FALSE, ETK_FALSE, 0);
	etk_signal_connect("clicked", ETK_OBJECT(gui->btn_stop), ETK_CALLBACK(cb_button_stop_clicked), NULL);
	etk_signal_connect("clicked", ETK_OBJECT(gui->btn_prev), ETK_CALLBACK(cb_button_prev_clicked), NULL);
	etk_signal_connect("clicked", ETK_OBJECT(gui->btn_play), ETK_CALLBACK(cb_button_play_clicked), NULL);
	etk_signal_connect("clicked", ETK_OBJECT(gui->btn_next), ETK_CALLBACK(cb_button_next_clicked), NULL);
	
	/* Vol slider setup */
	gui->vol_hbox = etk_hbox_new(ETK_FALSE, 0);
	gui->vol_imagel = etk_image_new_from_file(PACKAGE_DATA_DIR"/images/sound_low.png");
	gui->vol_imager = etk_image_new_from_file(PACKAGE_DATA_DIR"/images/sound_high.png");
	gui->vol_slider = etk_hslider_new(0, 100, 0, 1, 1);
	etk_box_pack_start(ETK_BOX(gui->vol_hbox), gui->vol_imagel, ETK_FALSE, ETK_FALSE, 5);
	etk_box_pack_start(ETK_BOX(gui->vol_hbox), gui->vol_slider, ETK_TRUE, ETK_TRUE, 5);
	etk_box_pack_start(ETK_BOX(gui->vol_hbox), gui->vol_imager, ETK_FALSE, ETK_FALSE, 5);
	etk_box_pack_start(ETK_BOX(gui->vbox_controls), gui->vol_hbox, ETK_TRUE, ETK_TRUE, 5);
	etk_signal_connect("value_changed", ETK_OBJECT(gui->vol_slider), ETK_CALLBACK(cb_vol_slider_value_changed), NULL);
	etk_signal_connect("mouse_down", ETK_OBJECT(gui->vol_imagel), ETK_CALLBACK(cb_vol_image_clicked), gui);
	etk_signal_connect("mouse_down", ETK_OBJECT(gui->vol_imager), ETK_CALLBACK(cb_vol_image_clicked), gui);

	/* Repeat/Random checkbox setup */
	gui->hbox_modes = etk_hbox_new(ETK_FALSE, 5);
	gui->checkb_random = etk_check_button_new_with_label("Random");
	gui->checkb_repeat = etk_check_button_new_with_label("Repeat");
	etk_box_pack_start(ETK_BOX(gui->hbox_modes), gui->checkb_random, ETK_FALSE, ETK_FALSE, 0);
	etk_box_pack_start(ETK_BOX(gui->hbox_modes), gui->checkb_repeat, ETK_FALSE, ETK_FALSE, 0);
	etk_box_pack_start(ETK_BOX(gui->vbox_controls), gui->hbox_modes, ETK_TRUE, ETK_FALSE, 5);
	etk_signal_connect("clicked", ETK_OBJECT(gui->checkb_random), ETK_CALLBACK(cb_toggle_random), NULL);
	etk_signal_connect("clicked", ETK_OBJECT(gui->checkb_repeat), ETK_CALLBACK(cb_toggle_repeat), NULL);

	/* Player info vbox */
	/* contain: song info, progress info */
	gui->vbox_info = etk_vbox_new(ETK_FALSE, 2);
	etk_box_pack_start(ETK_BOX(gui->hbox_player), gui->vbox_info, ETK_TRUE, ETK_TRUE, 5);

	/* Song info setup */
	/*
	Etk_Widget *hbox, *image;
	hbox = etk_hbox_new(ETK_TRUE, 0);
	etk_box_pack_start(ETK_BOX(gui->vbox_info), hbox, ETK_TRUE, ETK_TRUE, 0);
	image = etk_image_new_from_file("logo_beta1.png");
	*/	

	gui->song_info = etk_text_view_new();
	/*
	etk_box_pack_start(ETK_BOX(hbox), gui->song_info, ETK_TRUE, ETK_TRUE, 0);
	etk_box_pack_start(ETK_BOX(hbox), image, ETK_FALSE, ETK_FALSE, 0);
	*/
	etk_box_pack_start(ETK_BOX(gui->vbox_info), gui->song_info, ETK_TRUE, ETK_TRUE, 0);

	/* Progress info setup */
	gui->hbox_progress = etk_hbox_new(ETK_FALSE, 0);
	gui->progress = etk_progress_bar_new();
	gui->progress_time = etk_label_new("0:00 / 0:00");
	etk_box_pack_start(ETK_BOX(gui->hbox_progress), gui->progress, ETK_TRUE, ETK_TRUE, 0);
	etk_box_pack_start(ETK_BOX(gui->hbox_progress), gui->progress_time, ETK_FALSE, ETK_FALSE, 2);
	etk_box_pack_start(ETK_BOX(gui->vbox_info), gui->hbox_progress, ETK_TRUE, ETK_TRUE, 0);
	etk_signal_connect("mouse_down", ETK_OBJECT(gui->progress), ETK_CALLBACK(cb_seek_time), NULL);

	/************
	 * Medialib *
	 ************/
	 
	/* medialib root hbox */
	/* contain: tree_artist, tree_album, tree_track */
	gui->hbox_medialib = etk_hbox_new(ETK_TRUE, 0);

	/* tree_artist setup */
	gui->tree_artist = etk_tree_new();
	etk_tree_multiple_select_set(ETK_TREE(gui->tree_artist), ETK_TRUE);
	etk_tree_mode_set(ETK_TREE(gui->tree_artist), ETK_TREE_MODE_LIST);
	etk_tree_col_new(ETK_TREE(gui->tree_artist), "Artist", etk_tree_model_text_new(ETK_TREE(gui->tree_artist)), 60);
	etk_tree_build(ETK_TREE(gui->tree_artist));
	etk_object_data_set(ETK_OBJECT(gui->tree_artist), "title", "Artist");
	etk_object_data_set(ETK_OBJECT(gui->tree_artist), "Emphasis_Type", (void*)EMPHASIS_ARTIST);
	etk_box_pack_start(ETK_BOX(gui->hbox_medialib), gui->tree_artist, ETK_TRUE, ETK_TRUE, 0);
	etk_widget_dnd_source_set(ETK_WIDGET(gui->tree_artist), ETK_TRUE);
	etk_signal_connect("drag_begin", ETK_OBJECT(gui->tree_artist),
	                   ETK_CALLBACK(cb_drag_artist), gui->drag);
	etk_signal_connect("key_down", ETK_OBJECT(gui->tree_artist), 
	                   ETK_CALLBACK(cb_mlib_bindings_key), gui);
	etk_signal_connect("row_selected", ETK_OBJECT(gui->tree_artist),
	                   ETK_CALLBACK(cb_tree_artist_selected), gui);
	etk_signal_connect("row_clicked", ETK_OBJECT(gui->tree_artist),
	                   ETK_CALLBACK(cb_tree_mlib_clicked), EMPHASIS_ARTIST);
	//etk_tree_row_height_set(ETK_TREE(gui->tree_artist), 17);

	/* tree_album setup */
	gui->tree_album = etk_tree_new();
	etk_tree_multiple_select_set(ETK_TREE(gui->tree_album), ETK_TRUE);
	etk_tree_mode_set(ETK_TREE(gui->tree_album), ETK_TREE_MODE_LIST);
	etk_tree_col_new(ETK_TREE(gui->tree_album), "Album", etk_tree_model_text_new(ETK_TREE(gui->tree_album)), 60);
	etk_tree_build(ETK_TREE(gui->tree_album));
	etk_object_data_set(ETK_OBJECT(gui->tree_album), "title", "Album");
	etk_object_data_set(ETK_OBJECT(gui->tree_album), "Emphasis_Type", (void*)EMPHASIS_ALBUM);
	etk_box_pack_start(ETK_BOX(gui->hbox_medialib), gui->tree_album, ETK_TRUE, ETK_TRUE, 0);
	etk_widget_dnd_source_set(ETK_WIDGET(gui->tree_album), ETK_TRUE);
	etk_signal_connect("drag_begin", ETK_OBJECT(gui->tree_album),
	                   ETK_CALLBACK(cb_drag_album), gui);
	etk_signal_connect("key_down", ETK_OBJECT(gui->tree_album), 
	                   ETK_CALLBACK(cb_mlib_bindings_key), gui);
	etk_signal_connect("row_selected", ETK_OBJECT(gui->tree_album),
	                   ETK_CALLBACK(cb_tree_album_selected), gui);
	etk_signal_connect("row_clicked", ETK_OBJECT(gui->tree_album),
	                   ETK_CALLBACK(cb_tree_mlib_clicked), (void*)EMPHASIS_ALBUM);
	
	/* tree_track setup */
	gui->tree_track = etk_tree_new();
	etk_tree_multiple_select_set(ETK_TREE(gui->tree_track), ETK_TRUE);
	etk_tree_mode_set(ETK_TREE(gui->tree_track), ETK_TREE_MODE_LIST);
	etk_tree_col_new(ETK_TREE(gui->tree_track), "Track", etk_tree_model_text_new(ETK_TREE(gui->tree_track)), 60);
	etk_tree_build(ETK_TREE(gui->tree_track));
	etk_object_data_set(ETK_OBJECT(gui->tree_track), "title", "Track");
	etk_object_data_set(ETK_OBJECT(gui->tree_track), "Emphasis_Type", (void*)EMPHASIS_TRACK);
	etk_box_pack_start(ETK_BOX(gui->hbox_medialib), gui->tree_track, ETK_TRUE, ETK_TRUE, 0);
	etk_widget_dnd_source_set(ETK_WIDGET(gui->tree_track), ETK_TRUE);
	etk_signal_connect("drag_begin", ETK_OBJECT(gui->tree_track),
	                   ETK_CALLBACK(cb_drag_track), gui->drag);
	etk_signal_connect("key_down", ETK_OBJECT(gui->tree_track), 
	                   ETK_CALLBACK(cb_mlib_bindings_key), gui);
	etk_signal_connect("row_clicked", ETK_OBJECT(gui->tree_track),
	                   ETK_CALLBACK(cb_tree_mlib_clicked), (void*)EMPHASIS_TRACK);
	
	/************
	 * Playlist *
	 ************/
	
	/* tree_pls setup */
	gui->tree_pls = etk_tree_new();
	etk_tree_mode_set(ETK_TREE(gui->tree_pls), ETK_TREE_MODE_LIST);
	etk_tree_multiple_select_set(ETK_TREE(gui->tree_pls), ETK_TRUE);
	etk_tree_col_new(ETK_TREE(gui->tree_pls), "Title", etk_tree_model_icon_text_new(ETK_TREE(gui->tree_pls), ETK_TREE_FROM_FILE), 140);
	etk_tree_col_new(ETK_TREE(gui->tree_pls), "Time", etk_tree_model_text_new(ETK_TREE(gui->tree_pls)), 30);
	etk_tree_col_new(ETK_TREE(gui->tree_pls), "Artist", etk_tree_model_text_new(ETK_TREE(gui->tree_pls)), 120);
	etk_tree_col_new(ETK_TREE(gui->tree_pls), "Album", etk_tree_model_text_new(ETK_TREE(gui->tree_pls)), 120);
	etk_tree_build(ETK_TREE(gui->tree_pls));
	etk_widget_dnd_dest_set(ETK_WIDGET(gui->tree_pls), ETK_TRUE);
	etk_signal_connect("drag_drop", ETK_OBJECT(gui->tree_pls), 
	                   ETK_CALLBACK(cb_drop_song), gui->drag);
	etk_signal_connect("row_clicked", ETK_OBJECT(gui->tree_pls),
	                   ETK_CALLBACK(cb_tree_pls_clicked), NULL);
	etk_signal_connect("key_down", ETK_OBJECT(gui->tree_pls),
	                   ETK_CALLBACK(cb_pls_bindings_key), gui);

	/*********
	 * Paned *
	 *********/
	 
	/* paned setup*/
	/* separate: medialib, playlist */
	gui->vpaned = etk_vpaned_new();
	etk_paned_child1_set(ETK_PANED(gui->vpaned), gui->hbox_medialib, ETK_TRUE);
	etk_paned_child2_set(ETK_PANED(gui->vpaned), gui->tree_pls, ETK_TRUE);
	etk_box_pack_start(ETK_BOX(gui->vbox), gui->vpaned, ETK_TRUE, ETK_TRUE, 0);
	
	
	/*********
	 * Menus *
	 *********/
	
	gui->config_gui = malloc(sizeof *(gui->config_gui));
//	config_gui_init(gui->config_gui);
//	etk_widget_show(gui->config_gui->window);
	
	/* playlist menu setup*/ 
	gui->menu = etk_menu_new();
	
	Etk_Widget *menu_item, *separator, *radio_item=NULL, *menu;
	
	emphasis_menu_append(gui->menu, "clear", ETK_STOCK_EDIT_CLEAR, cb_playlist_clear, NULL,
	                                "delete", ETK_STOCK_EDIT_DELETE, cb_playlist_delete, gui,
	                                "update", ETK_STOCK_VIEW_REFRESH, cb_database_update, gui,
	                                "config", ETK_STOCK_PREFERENCES_SYSTEM, cb_config_show, gui,
	                                NULL);
	separator = etk_menu_item_separator_new();
	etk_menu_shell_append(ETK_MENU_SHELL(gui->menu), ETK_MENU_ITEM(separator));

	radio_item = etk_menu_item_radio_new_with_label_from_widget("full", NULL);
	etk_menu_shell_append(ETK_MENU_SHELL(gui->menu), ETK_MENU_ITEM(radio_item));
	etk_signal_connect("activated", ETK_OBJECT(radio_item), ETK_CALLBACK(cb_switch_full), gui);
	radio_item = etk_menu_item_radio_new_with_label_from_widget("small", ETK_MENU_ITEM_RADIO(radio_item));
	etk_menu_shell_append(ETK_MENU_SHELL(gui->menu), ETK_MENU_ITEM(radio_item));
	etk_signal_connect("activated", ETK_OBJECT(radio_item), ETK_CALLBACK(cb_switch_small), gui);
	
	etk_signal_connect("mouse_down", ETK_OBJECT(gui->window), 
	                   ETK_CALLBACK(cb_pls_contextual_menu), gui);
	etk_signal_connect("mouse_down", ETK_OBJECT(gui->tree_pls), 
	                   ETK_CALLBACK(cb_pls_contextual_menu), gui);
	                   
//	etk_signal_connect("key_down", ETK_OBJECT(gui->window),
//	                   ETK_CALLBACK(cb_emphasis_bindings_key), gui);
	
	etk_widget_show_all(gui->window);
}

void
emphasis_clear(Emphasis_Gui *gui)
{
	etk_tree_clear(ETK_TREE(gui->tree_artist));
	etk_tree_clear(ETK_TREE(gui->tree_album));
	etk_tree_clear(ETK_TREE(gui->tree_track));
	etk_tree_clear(ETK_TREE(gui->tree_pls));
}

/**
 * @brief Make a menu with small stock image and sets a callback on "activated" on each elements
 * @param menu The Etk_Menu to setup
 * @param ... An (char*)menu_item name, an (Etk_Stock_Id)image id, a Etk_Callback function and 
 * it data ... terminated by NULL
 */
void
emphasis_menu_append(Etk_Widget *menu, ...)
{
	Etk_Widget *menu_item, *item_image=NULL;
	char *item_name;
	Etk_Stock_Id item_image_id;
	void *callback, *data;
	va_list arglist;
	
	va_start(arglist, menu);
	
	while ((item_name = va_arg(arglist, char*)) != NULL)
	{
		menu_item = etk_menu_item_image_new_with_label(item_name);
		item_image_id = va_arg(arglist, Etk_Stock_Id);
		if (item_image_id)
		{
			item_image = etk_image_new_from_stock(item_image_id, ETK_STOCK_SMALL);
			etk_menu_item_image_set(ETK_MENU_ITEM_IMAGE(menu_item), ETK_IMAGE(item_image));
		}
		callback = va_arg(arglist, void*);
		data = va_arg(arglist, void*);
		if (callback)
		{
			etk_signal_connect("activated", ETK_OBJECT(menu_item),
	                           ETK_CALLBACK(callback), data);
		}
	                   
	    etk_menu_shell_append(ETK_MENU_SHELL(menu), ETK_MENU_ITEM(menu_item));
	}
	va_end(arglist);
}

void
emphasis_tree_mlib_init(Emphasis_Gui *gui, Emphasis_Type type)
{
	Etk_Tree_Row *row;
	Evas_List *list;
	
	switch (type)
	{
		case EMPHASIS_ARTIST:
			list = mpc_mlib_artist_get();
			emphasis_tree_mlib_set(ETK_TREE(gui->tree_artist), list, MPD_DATA_TYPE_TAG);
			break;
		case EMPHASIS_ALBUM:
			row = etk_tree_selected_row_get(ETK_TREE(gui->tree_artist));
			cb_tree_artist_selected(ETK_OBJECT(gui->tree_artist), row, gui);
			break;
		case EMPHASIS_TRACK:
			row = etk_tree_selected_row_get(ETK_TREE(gui->tree_album));
			cb_tree_album_selected(ETK_OBJECT(gui->tree_album), row, gui);
			break;
	}
}

/**
 * @brief Set a list to one of the medialib's tree
 * @param tree One of the medialib's tree
 * @param list The list to set in the treeview
 * @param mpd_type The type of the list
 */
void
emphasis_tree_mlib_set(Etk_Tree *tree, Evas_List *list, MpdDataType mpd_type)
{
	Etk_Tree_Col *col;
	Etk_Tree_Row *row;
	Emphasis_Data *data;

	etk_tree_clear(tree);

	etk_tree_freeze(tree);
	col = etk_tree_nth_col_get(tree, 0);

	if (mpd_type == MPD_DATA_TYPE_TAG)
	{
		row = etk_tree_append(tree, col, "All", NULL);
		etk_tree_row_data_set(row, NULL);
		
		while (list)
		{
			data = evas_list_data(list);
			row = etk_tree_append(tree, col, data->tag, NULL);
			etk_tree_row_data_set(row, data->tag);
			list = evas_list_next(list);
		}
	}
	else
	{
		if (mpd_type == MPD_DATA_TYPE_SONG)
		{
			while (list)
			{
				data = evas_list_data(list);
				if (data->song->title)
					row = etk_tree_append(tree, col, data->song->title, NULL);
				else
					row = etk_tree_append(tree, col, data->song->file, NULL);
				
				etk_tree_row_data_set(row, data->song->file);
				list = evas_list_next(list);
			}
		}
	}
	
	etk_tree_thaw(tree);
	emphasis_list_free(list, mpd_type); 
}

/**
 * @brief Add rows to the playlist treeview
 * @param tree A playlist tree
 * @param playlist The full playlist used by mpd
 */
void
emphasis_tree_pls_set(Etk_Tree *tree, Evas_List *playlist)
{
	char *song_time;
	Etk_Tree_Col *col_title, *col_time, *col_artist, *col_album;
	Etk_Tree_Row *row, *row_next;
	int id;
	char *title, *time, *album, *artist, *truc;
	Emphasis_Data *data;
	
	col_title = etk_tree_nth_col_get(tree, 0);
	col_time = etk_tree_nth_col_get(tree, 1);
	col_artist = etk_tree_nth_col_get(tree, 2);
	col_album = etk_tree_nth_col_get(tree, 3);

	etk_tree_freeze(tree);
	row = etk_tree_first_row_get(tree);
	
	while (row && playlist)
	{
		data = evas_list_data(playlist);
		row_next = etk_tree_next_row_get(row, ETK_FALSE, ETK_FALSE);
		id = (int)etk_tree_row_data_get(row);
		if (data->song->id != id )
		{
			etk_tree_row_del(row);
		}
		else
		{
			playlist = evas_list_next(playlist);
		}
		row = row_next;
	}
	
	while (row)
	{
		row_next = etk_tree_next_row_get(row, ETK_FALSE, ETK_FALSE);
		etk_tree_row_del(row);
		row = row_next;
	}
	
	while (playlist)
	{
		data = evas_list_data(playlist);
		if (!data->song->title)
			title = data->song->file;
		else
			title = data->song->title;
		asprintf(&song_time, "%d:%02d", (data->song->time)/60, (data->song->time)%60);
		row = etk_tree_append(tree, col_title, NULL, title, 
		                            col_time, song_time,
		                            col_artist, data->song->artist,
		                            col_album, data->song->album, NULL);

		
		etk_tree_row_data_set(row, (int *)data->song->id);
		free(song_time);
		playlist = evas_list_next(playlist);
	}	

	emphasis_list_free(playlist, MPD_DATA_TYPE_SONG);
	etk_tree_thaw(tree);
}

/**
 * @brief browse the tree_pls and set the playing image to the @e id
 * @note can be improve by stoping the while loop when the to cond has been validated
 */
void
emphasis_pls_mark_current(Etk_Tree *tree, int id)
{
	Etk_Tree_Col *col_current;
	Etk_Tree_Row *row;
	int row_id;
	char *image, *title;

	col_current = etk_tree_nth_col_get(tree, 0);
	
	etk_tree_freeze(tree);
	row = etk_tree_first_row_get(tree);
	
	while (row)
	{
		row_id = (int)etk_tree_row_data_get(row);
		etk_tree_row_fields_get(row, col_current, &image, &title, NULL);
		if (image)
		{
			title = strdup(title);
			etk_tree_row_fields_set(row, col_current, NULL, title, NULL);
		}
		if (row_id == id)
		{
			title = strdup(title);
			etk_tree_row_fields_set(row, col_current, PACKAGE_DATA_DIR"/images/note.png", title, NULL);
		}
		row = etk_tree_next_row_get(row, ETK_FALSE, ETK_FALSE);
	}
	
	etk_tree_thaw(tree);
}

/**
 * @brief Replace a null string by "Unkown"
 * @param table A table of char** terminated by NULL
 */
void
emphasis_unknow_if_null(char **table[])
{
	int i=0;
	
	while (table[i])
	{
		if (!*table[i])
		{
			*table[i] = strdup("Unknown");
		}
		i++;
	}
}

/**
 * @brief Set the message in th textblock
 * @param song The current playing song
 * @param msg Additional, if song != NULL or the message displayed
 * @param gui A Emphasis_Gui
 */
void
emphasis_player_info_set(mpd_Song *song, char *msg, Emphasis_Gui *gui)
{
	char *info;	
	
	etk_textblock_clear(ETK_TEXT_VIEW(gui->song_info)->textblock);
	if (song)
	{
		char **table[] = {&(song->artist), &(song->title), &(song->album), NULL};
		emphasis_unknow_if_null(table);
		
		asprintf(&info, "<b><font size=12>%s</font></b>\n \n"
		         "<i>by</i>  <font size=11>%s</font>  "
		         "<i>in</i>  <font size=11>%s</font>  ", 
		         song->title, song->artist, song->album);
		if (msg)
		{
			asprintf(&info, "%s (%s)", info, msg);
		}
		etk_textblock_text_set(ETK_TEXT_VIEW(gui->song_info)->textblock, info,
		                       ETK_TRUE);
		free(info);
	}
	else
	{
		if (msg)
		{
			asprintf(&info, "%s", msg);
			etk_textblock_text_set(ETK_TEXT_VIEW(gui->song_info)->textblock, info,
			                       ETK_TRUE);
			free(info);
		}
	}
}

/**
 * @brief Set the progress bar to elapsed_time/total_time
 * @param elapsed_time The time elapsed
 * @param total_time The lenght of the song
 * @param gui A Emphasis_Gui
 */
void
emphasis_player_progress_set(float elapsed_time, int total_time, Emphasis_Gui *gui)
{
	char *time;
	etk_progress_bar_fraction_set(ETK_PROGRESS_BAR(gui->progress), (elapsed_time/total_time));
	asprintf(&time, "%d:%02d / %d:%02d", (int)elapsed_time/60, (int)elapsed_time%60, total_time/60, total_time%60);
	etk_label_set(ETK_LABEL(gui->progress_time), time);
	free(time);
}

/**
 * @brief Set the image of btn_play to play if mpd is playing or to pause if mpd is stopped or paused. 
 * @param gui A Emphasis_Gui
 */
void
emphasis_toggle_play(Emphasis_Gui *gui)
{
	if (mpc_assert_status(MPD_PLAYER_PLAY))
	{
		etk_button_set_from_stock(ETK_BUTTON(gui->btn_play), ETK_STOCK_MEDIA_PLAYBACK_PAUSE);
		etk_button_label_set(ETK_BUTTON(gui->btn_play), NULL);
	}
	else
	{
		if (mpc_assert_status(MPD_PLAYER_PAUSE) || mpc_assert_status(MPD_PLAYER_STOP))
		{
			etk_button_set_from_stock(ETK_BUTTON(gui->btn_play), ETK_STOCK_MEDIA_PLAYBACK_START);
			etk_button_label_set(ETK_BUTTON(gui->btn_play), NULL);
		}
	}
}

/**
 * @brief Set the random checkbox state to value (1 is ticked)
 * @param value A boolean
 * @param gui A Emphasis_Gui
 */
void
emphasis_toggle_random(int value, Emphasis_Gui *gui)
{
	etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(gui->checkb_random), value);
}

/**
 * @brief Set the repeat checkbox state to value (1 is ticked)
 * @param value A boolean
 * @param gui A Emphasis_Gui
 */
void
emphasis_toggle_repeat(int value, Emphasis_Gui *gui)
{
	etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(gui->checkb_repeat), value);
}

/**
 * @brief Set the volume slider to the value position
 * @param value A vol value in percent
 * @param gui A Emphasis_Gui
 */
void
emphasis_vol_slider_set(int value, Emphasis_Gui *gui)
{
	etk_range_value_set(ETK_RANGE(gui->vol_slider), (double)value);
}
