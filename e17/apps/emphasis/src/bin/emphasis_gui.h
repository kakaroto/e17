/** @file emphasis_gui.h */
#ifndef _GUI_H_
#define _GUI_H_

/**
 * @defgroup gui
 * @{
 */

/**
 * @enum Emphasis_Type
 * @deprecated
 */
typedef enum {
	EMPHASIS_ARTIST,
	EMPHASIS_ALBUM,
	EMPHASIS_TRACK
} Emphasis_Type;

/**
 * @struct Emphasis_Gui
 * @brief All the widget used by the GUI
 */
typedef struct _Emphasis_Gui {
	Etk_Widget *window;
	Etk_Widget *vbox;
	
	Etk_Widget *hbox_player;

	Etk_Widget *vbox_controls;
	Etk_Widget *button_box;
	Etk_Widget *btn_stop;
	Etk_Widget *btn_prev;
	Etk_Widget *btn_play;
	Etk_Widget *btn_next;
	Etk_Widget *vol_hbox;
	Etk_Widget *vol_imagel;
	Etk_Widget *vol_slider;
	Etk_Widget *vol_imager;
	Etk_Widget *hbox_modes;
	Etk_Widget *checkb_random;
	Etk_Widget *checkb_repeat;

	Etk_Widget *vbox_info;
	Etk_Widget *song_info;
	Etk_Widget *hbox_progress;
	Etk_Widget *progress;
	Etk_Widget *progress_time;
					
	Etk_Widget *hbox_medialib;
	Etk_Widget *tree_artist;
	Etk_Widget *tree_album;
	Etk_Widget *tree_track;
	Etk_Widget *vpaned;
	Etk_Widget *tree_pls;

	Etk_Widget *drag;
	
	Etk_Widget *menu;
	Etk_Widget *menu_item;
	
	Emphasis_Config_Gui *config_gui;
	Ecore_Timer *timer;
} Emphasis_Gui ;

void emphasis_init_gui(Emphasis_Gui *gui);
void emphasis_clear(Emphasis_Gui *gui);
void emphasis_menu_append(Etk_Widget *menu, ...);

void emphasis_tree_mlib_init(Emphasis_Gui *gui, Emphasis_Type type);
void emphasis_tree_mlib_set(Etk_Tree *tree, MpdData *list, MpdDataType mpd_type);
void emphasis_tree_pls_set(Etk_Tree *tree, MpdData *playlist);
void emphasis_pls_mark_current(Etk_Tree *tree, int id);

void emphasis_unknow_if_null(char **table[]);
void emphasis_player_info_set(mpd_Song *song, char *msg, Emphasis_Gui *gui);
void emphasis_player_progress_set(float elapsed_time, int total_time, Emphasis_Gui *gui);

void emphasis_toggle_play(Emphasis_Gui *gui);
void emphasis_toggle_random(int value, Emphasis_Gui *gui);
void emphasis_toggle_repeat(int value, Emphasis_Gui *gui);

void emphasis_vol_slider_set(int value, Emphasis_Gui *gui);

/** @} */
#endif /*_GUI_H_*/
