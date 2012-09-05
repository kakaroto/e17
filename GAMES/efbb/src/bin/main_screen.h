#ifndef __MAIN_SCREEN_H__
#define __MAIN_SCREEN_H__ 1

#ifdef __cplusplus
extern "C" {
#endif

Evas_Object *main_screen_add(Evas_Object *win, Game *game);
void main_screen_show(Evas_Object *main_screen);
void main_screen_hide(Evas_Object *main_screen);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_SCREEN_H__ */
