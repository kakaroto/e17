#ifndef __SOUND_H__
#define __SOUND_H__ 1

#include <Evas.h>
#include <Eina.h>
#include <config.h>
#include <canberra.h>

Eina_Bool sound_init(void);
Eina_Bool sound_shutdown(void);

void sound_play(const char *filename);
void sound_play_music(const char *filename, int id);
void sound_mute_toggle(Eina_Bool islevel);
void sound_play_cb(void *data, Evas_Object *obj, const char *sig,
                   const char *src);

void sound_stop(int id);

Eina_Bool sound_playing_get(int id);

#endif /* __SOUND_H__ */
