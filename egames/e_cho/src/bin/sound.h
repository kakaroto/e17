#ifndef __SOUND_H__
#define __SOUND_H__ 1

#include <Evas.h>
#include <Eina.h>
#include <config.h>
#include <canberra.h>

Eina_Bool sound_disabled;

void sound_play_cb(void *data, Evas_Object *obj, const char *sig, const char *src);
Eina_Bool sound_init(void);
Eina_Bool sound_shutdown(void);

#endif /* __SOUND_H__ */
