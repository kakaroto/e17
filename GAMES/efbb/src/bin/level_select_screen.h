#ifndef __LEVEL_SELECT_SCREEN_H__
#define __LEVEL_SELECT_SCREEN_H__ 1

#ifdef __cplusplus
extern "C" {
#endif

void level_select_screen_show(Evas_Object *level_select_screen);
void level_select_screen_hide(Evas_Object *level_select_screen);

Evas_Object *level_select_screen_add(Evas_Object *win, Game *game,
                                     Etrophy_Gamescore *gamescore,
                                     Eina_List *levels);
#ifdef __cplusplus
}
#endif

#endif /* __LEVEL_SELECT_SCREEN_H__ */

