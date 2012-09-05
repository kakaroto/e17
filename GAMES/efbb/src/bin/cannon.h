#ifndef _CANNON_H
#define _CANNON_H

#include <Eina.h>
#include <Evas.h>

#ifdef __cplusplus
extern "C" {
#endif

Evas_Object *cannon_add(Evas_Object *win);

void cannon_shoot(Evas_Object *cannon);
void cannon_load(Evas_Object *cannon);
Eina_Bool cannon_loaded_get(Evas_Object *cannon);

void cannon_angle_set(Evas_Object *cannon, int angle);
int cannon_angle_get(Evas_Object *cannon);
int cannon_angle_min_get(Evas_Object *cannon);
int cannon_angle_max_get(Evas_Object *cannon);

void cannon_strength_set(Evas_Object *cannon, int strength);
int cannon_strength_get(Evas_Object *cannon);
int cannon_strength_min_get(Evas_Object *cannon);
int cannon_strength_max_get(Evas_Object *cannon);

void cannon_ammo_set(Evas_Object *cannon, int ammo);
int cannon_ammo_get(Evas_Object *cannon);

#ifdef __cplusplus
}
#endif

#endif
