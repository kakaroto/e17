#ifndef __BG_H__
#define __BG_H__ 1

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
     BG_GROUND = 1,
     BG_ISLAND,
     BG_SEA,
} Game_Bg_Type;

Evas_Object *bg_add(Evas_Object *win, Game_Bg_Type type);
void bg_update(Evas_Object *obj, int delta_x);
void bg_resume(Evas_Object *obj);
void bg_pause(Evas_Object *obj);

#ifdef __cplusplus
}
#endif

#endif /* __BG_H__ */
