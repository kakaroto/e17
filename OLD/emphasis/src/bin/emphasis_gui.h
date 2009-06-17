/** @file emphasis_gui.h */
#ifndef EMPHASIS_GUI_H_
#define EMPHASIS_GUI_H_

/**
 * @defgroup gui
 * @{
 */

/**
 * @enum Emphasis_Type
 * @deprecated
 */
typedef enum _Emphasis_Type Emphasis_Type;
enum _Emphasis_Type
{
  EMPHASIS_ARTIST = 1,
  EMPHASIS_ALBUM,
  EMPHASIS_TRACK
};

typedef struct _Cover_Info Cover_Info;
struct _Cover_Info
{
  char *artist;
  char *album;
};

/**
 * @struct Emphasis_Gui
 * @brief All the widget used by the GUI
 */
typedef struct _Emphasis_Gui Emphasis_Gui;
struct _Emphasis_Gui
{
  Emphasis_Player_Gui *player;

  Etk_Widget *menu;
  Etk_Widget *menu_item;

  Emphasis_Config_Gui *config_gui;
  Ecore_Timer *timer;
  
  Cover_Info *cover_queue;
};

void emphasis_init_gui(Emphasis_Gui *gui);
void emphasis_clear(Emphasis_Gui *gui);
void emphasis_init_menu(Emphasis_Gui *gui);
void emphasis_menu_append(Etk_Widget *menu, ...);
void emphasis_unknow_if_null(char **table[]);
void emphasis_cover_clear_queue(Cover_Info *queue);
void emphasis_cover_change(Emphasis_Gui *gui, char *artist, char *album);

/** @} */
#endif /* EMPHASIS_GUI_H_*/
