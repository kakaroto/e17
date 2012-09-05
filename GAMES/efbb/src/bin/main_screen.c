#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "efbb.h"
#include "sound.h"

static void
_play_cb(void *data, Evas_Object *obj __UNUSED__,
         const char *emission __UNUSED__, const char *source __UNUSED__)
{
   game_main_to_level_select(data);
}

static void
_exit_cb(void *data, Evas_Object *obj __UNUSED__,
         const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Evas_Object *win = data;
   evas_object_del(win);
   elm_shutdown();
}

Evas_Object *
main_screen_add(Evas_Object *win, Game *game)
{
   Evas_Object *main_screen;

   main_screen = elm_layout_add(win);
   elm_win_resize_object_add(win, main_screen);
   evas_object_size_hint_min_set(main_screen, WIDTH, HEIGHT);
   evas_object_size_hint_max_set(main_screen, WIDTH, HEIGHT);
   elm_layout_file_set(main_screen, PACKAGE_DATA_DIR "/"
                       GAME_THEME ".edj", "main_win");

   elm_layout_signal_callback_add(main_screen, "play", "btn_play",
                                  _play_cb, game);
   elm_layout_signal_callback_add(main_screen, "exit", "btn_exit",
                                  _exit_cb, win);
   elm_layout_signal_callback_add(main_screen, "*", "sound",
                                  sound_play_cb, NULL);
   evas_object_layer_set(main_screen, LAYER_UI);

   return main_screen;
}

void
main_screen_show(Evas_Object *main_screen)
{
   if (!sound_playing_get(MENU_MUSIC_ID))
     sound_play_music(MENU_MUSIC, MENU_MUSIC_ID);
   evas_object_show(main_screen);
   elm_layout_signal_emit(main_screen, "show", "game");
}

void
main_screen_hide(Evas_Object *main_screen)
{
   evas_object_hide(main_screen);
}
