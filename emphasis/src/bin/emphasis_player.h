/** @file emphasis_player.h */
#ifndef EMPHASIS_PLAYER_H_
#define EMPHASIS_PLAYER_H_

#define EMPHASIS_GLADE  "/emphasis.glade"

#define EMPHASIS_LOGO   "/images/haricot_musique.png"
#define EMPHASIS_SOUNDL "/images/sound_low.png"
#define EMPHASIS_SOUNDR "/images/sound_high.png"

typedef enum _Emphasis_Mode Emphasis_Mode;
enum _Emphasis_Mode
{
  EMPHASIS_SMALL = 0,
  EMPHASIS_FULL = 1
};

/**
 * @struct Emphasis_Enhance
 * @brief All enhance widget needed by the GUI
 */
typedef struct _Emphasis_Player_Gui Emphasis_Player_Gui;
struct _Emphasis_Player_Gui
{
  Enhance *en;
  
  struct /* small mode */
    {
      Etk_Widget *window;
      Etk_Widget *root;

      struct
        {
          Etk_Widget *window;
          Etk_Widget *root;
        } ctr;
      struct
        {
          Etk_Widget *window;
          Etk_Widget *root;
        } cov;

      Etk_Widget *cover;
      int         cover_size_w;
      int         cover_size_h;

      Etk_Widget *play;
      Etk_Widget *stop;
      Etk_Widget *prev;
      Etk_Widget *next;

      Etk_Widget *info;

      Etk_Widget *progress;

      Etk_Widget *sound_slider;
      Etk_Widget *sound_low;
      Etk_Widget *sound_high;

      Etk_Widget *random;
      Etk_Widget *repeat;
      Etk_Widget *full;
      Etk_Widget *media;

      Etk_Bool packed; /* ETK_TRUE: small mode; ETK_FALSE: tiny mode */
    }small;

  struct /* full mode */
    {
      Etk_Widget *window;
      Etk_Widget *root;

      Etk_Widget *cover;
      int         cover_size_w;
      int         cover_size_h;

      Etk_Widget *play;
      Etk_Widget *stop;
      Etk_Widget *prev;
      Etk_Widget *next;

      Etk_Widget *info;

      Etk_Widget *progress;
      Etk_Widget *time;

      Etk_Widget *sound_slider;
      Etk_Widget *sound_low;
      Etk_Widget *sound_high;

      Etk_Widget *random;
      Etk_Widget *repeat;
      Etk_Widget *full;
    }full;

  struct /* playlist and mlib */
    {
      Etk_Widget *window;
      Etk_Widget *root;

      Etk_Widget *button_lib;
      Etk_Widget *button_search;
      Etk_Widget *button_playlists;
      Etk_Widget *button_stats;

      Etk_Widget *notebook;
      Etk_Widget *toolbar;

      /* Pane 1 content */
      Etk_Widget *paned;

      Etk_Widget *pls;

      Etk_Widget *artist;
      Etk_Widget *album;
      Etk_Widget *track;

      /* Pane 2 content */
      Etk_Widget *search_root;
      Etk_Widget *search_combo;
      Etk_Widget *search_tree;

      /* Pane 3 content */
      Etk_Widget *pls_list;
      Etk_Widget *pls_content;
      Etk_Widget *pls_entry_save;

      Etk_Widget *drag;
    } media;

  Emphasis_Mode state;
};


void emphasis_init_player(Emphasis_Player_Gui *player);

void emphasis_player_cover_set(Emphasis_Player_Gui *player, const char *path);

void emphasis_player_cover_size_set(Emphasis_Player_Gui *player, int w, int h);

void emphasis_player_cover_size_update(Emphasis_Player_Gui *player);

void emphasis_player_info_set(Emphasis_Player_Gui *player,
                              Emphasis_Song * song, char *msg);
void emphasis_player_progress_set(Emphasis_Player_Gui *player,
                                  float elapsed_time, int total_time);
void emphasis_player_toggle_play(Emphasis_Player_Gui *player);

void emphasis_player_toggle_random(Emphasis_Player_Gui *player, int value);

void emphasis_player_toggle_repeat(Emphasis_Player_Gui *player, int value);

void emphasis_player_toggle_full(Emphasis_Player_Gui *player, int value);

void emphasis_player_vol_slider_set(Emphasis_Player_Gui *player, int value);

void emphasis_player_vol_image_set(Emphasis_Player_Gui *player,
                                   int high,
                                   const char *path);
void emphasis_player_mode_set(Emphasis_Player_Gui *player,
                              Emphasis_Mode state);

void emphasis_player_force_mode_set(Emphasis_Player_Gui *player,
                                    Emphasis_Mode state);

/** @} */
#endif /* EMPHASIS_PLAYER_H_ */
