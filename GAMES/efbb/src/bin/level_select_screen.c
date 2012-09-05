#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "efbb.h"
#include "sound.h"

static Elm_Gengrid_Item_Class *gic = NULL;

static void
_menu_cb(void *data, Evas_Object *obj __UNUSED__,
         const char *emission __UNUSED__, const char *source __UNUSED__)
{
   game_level_select_to_main(data);
}

static void
_level_selected(void *data, Evas_Object *obj __UNUSED__, void *event_info)
{
   Elm_Object_Item *item = event_info;
   Game *game = data;
   Level *level;

   elm_gengrid_item_selected_set(item, EINA_FALSE);
   level = elm_object_item_data_get(item);
   game_level_select_to_level(game, level);
}

static char *
_grid_text_get(void *data, Evas_Object *obj __UNUSED__,
               const char *part __UNUSED__)
{
   Level *level = data;
   char name[32];

   snprintf(name, sizeof(name), "%02i", level_level_num_get(level));
   return strdup(name);
}

Evas_Object *
level_select_screen_add(Evas_Object *win, Game *game,
                        Etrophy_Gamescore *gamescore, Eina_List *levels)
{
   Evas_Object *level_select_screen, *grid;
   Eina_List *l;
   Level *level;

   level_select_screen = elm_layout_add(win);
   elm_win_resize_object_add(win, level_select_screen);
   elm_layout_file_set(level_select_screen, PACKAGE_DATA_DIR "/"
                       GAME_THEME ".edj", "lvlsel_win");
   evas_object_size_hint_min_set(level_select_screen, WIDTH, HEIGHT);
   evas_object_size_hint_max_set(level_select_screen, WIDTH, HEIGHT);
   evas_object_layer_set(level_select_screen, LAYER_UI);

   grid = elm_gengrid_add(win);
   elm_object_style_set(grid, "efbb");
   elm_gengrid_item_size_set(grid, 280, 280);
   evas_object_size_hint_weight_set(grid, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_scroller_bounce_set(grid, EINA_FALSE, EINA_TRUE);

   gic = elm_gengrid_item_class_new();
   gic->item_style = "default";
   gic->func.text_get = _grid_text_get;

   EINA_LIST_FOREACH(levels, l, level)
      elm_gengrid_item_append(grid, gic, level, _level_selected, game);

   elm_layout_content_set(level_select_screen, "grid", grid);

   elm_layout_signal_callback_add(level_select_screen, "menu", "btn_menu",
                                  _menu_cb, game);
   elm_layout_signal_callback_add(level_select_screen, "*", "sound",
                                  sound_play_cb, NULL);

   evas_object_data_set(level_select_screen, "gamescore", gamescore);

   return level_select_screen;
}

void
level_select_screen_show(Evas_Object *level_select_screen)
{
   Etrophy_Gamescore *gamescore;
   Elm_Object_Item *item;
   Eina_List *locks, *l;
   Evas_Object *grid;

   gamescore = evas_object_data_get(level_select_screen, "gamescore");
   locks = etrophy_gamescore_locks_list_get(gamescore);
   grid = elm_layout_content_get(level_select_screen, "grid");

   item = elm_gengrid_first_item_get(grid);
   while(item)
     {
        Etrophy_Lock *etrophy_lock;
        EINA_LIST_FOREACH(locks, l, etrophy_lock)
          {
             if (!strcmp(level_level_id_get(elm_object_item_data_get(item)),
                 etrophy_lock_name_get(etrophy_lock)))
               {
                  if (etrophy_lock_state_get(etrophy_lock) ==
                      ETROPHY_LOCK_STATE_UNLOCKED)
                    elm_object_item_disabled_set(item, EINA_FALSE);
                  else
                    elm_object_item_disabled_set(item, EINA_TRUE);
               }
          }
        item = elm_gengrid_item_next_get(item);
     }

   evas_object_show(level_select_screen);

   if (!sound_playing_get(MENU_MUSIC_ID))
     sound_play_music(MENU_MUSIC, MENU_MUSIC_ID);
}

void
level_select_screen_hide(Evas_Object *level_select_screen)
{
   evas_object_hide(level_select_screen);
}
