/*
 * $Id$
 * vim:expandtab:ts=3:sts=3:sw=3
 */

#include "envision.h"
#include "callbacks.h"

/**
 * canvas_resize
 * If the canvas is resized, resize the video too
 */
void
canvas_resize(Ecore_Evas * ee)
{
   Evas *evas = ecore_evas_get(ee);
   Evas_Object *edje = evas_object_name_find(evas, "edje");
   Evas_Object *emotion = evas_object_name_find(evas, "emotion");
   int w, h, vw, vh;
   double ratio, x, y;

   ecore_evas_geometry_get(ee, NULL, NULL, &w, &h);
   evas_object_resize(edje, (Evas_Coord)w, (Evas_Coord)h);

   emotion_object_size_get(emotion, &vw, &vh);
   ratio = emotion_object_ratio_get(emotion);
   printf("Ratio is: %f\n", ratio);

   if (ratio > 0.0) {
      x = 0;
      y = (h - (w / ratio)) / 2;
      if (y < 0) {
         y = 0;
         x = (w - (h * ratio)) / 2;
         w = h * ratio;
      } else {
         h = w / ratio;
      }

      evas_object_resize (edje, (Evas_Coord)w, (Evas_Coord)h+106);
   } else {
      if (vh > 0) {
         ratio = (double) vw / (double) vh;
      } else {
         ratio = 1.0;
      }
      x = 0;
      y = (h - (w / ratio)) / 2;

      if (y < 0) {
         y = 0;
         x = (w - (h * ratio)) / 2;
         w = h * ratio;
      } else {
         h = w / ratio;
      }

      evas_object_resize (edje, (Evas_Coord)w+25, (Evas_Coord)h+106);
   }
}

void
update_timer(void *data, Evas_Object * obj, void *event_info)
{
   Envision *e = data;
   char buffer[512];
   double len, pos, pos_s, len_s;
   int pos_h, pos_m, len_h, len_m;

   /* get the current position and length (in seconds) */
   pos = emotion_object_position_get(e->gui.emotion);
   len = emotion_object_play_length_get(e->gui.emotion);

   /* now convert this into hrs:mins:secs */
   pos_h = (int)pos / (60 * 60);
   pos_m = ((int)pos / (60)) - (pos_h * 60);
   pos_s = pos - (pos_h * 60 * 60) - (pos_m * 60);
   len_h = (int)len / (60 * 60);
   len_m = ((int)len / (60)) - (len_h * 60);
   len_s = len - (len_h * 60 * 60) - (len_m * 60);

   /* print this to a string buffer */
   /* snprintf(buffer, sizeof(buffer), "%02i:%02i:%02.2f / %02i:%02i:%02.2f",
         pos_h, pos_m, pos_s, len_h, len_m, len_s); */
   snprintf(buffer, sizeof(buffer), "%02i:%02i:%02.0f", pos_h, pos_m, pos_s);
   edje_object_part_text_set(e->gui.edje, "time_text", buffer);
}

void
callback_evas_keydown(void *data, Evas * e, Evas_Object * obj, void *event_info)
{
   Evas_Event_Key_Down *ev;

   ev = (Evas_Event_Key_Down *)event_info;

   // Debug
   printf("You hit key: %s\n", ev->keyname);

   if (!strcmp(ev->keyname, "space")) {
      emotion_object_play_set(obj, 0);
   } else if (!strcmp(ev->keyname, "Return")) {
      emotion_object_play_set(obj, 1);
   } else if (!strcmp(ev->keyname, "Escape") || !strcmp(ev->keyname, "q")) {
      ecore_main_loop_quit();
   } else if (!strcmp(ev->keyname, "m")) {
      if (!emotion_object_audio_mute_get(obj)) {
         emotion_object_audio_mute_set(obj, 1);
      } else {
         emotion_object_audio_mute_set(obj, 0);
      }
   } else if (!strcmp(ev->keyname, "Down")) {
      double pos;

      pos = emotion_object_position_get(obj);
      emotion_object_position_set(obj, pos - 5);
   } else if (!strcmp(ev->keyname, "Up")) {
      double pos;
      int min;

      pos = emotion_object_position_get(obj);
      emotion_object_position_set(obj, pos + 5);
      pos = emotion_object_position_get(obj);
      min = pos / 60;
      printf("Position is: %d:00\n", min);
   } else if (!strcmp(ev->keyname, "Right")) {
      double pos;

      pos = emotion_object_position_get(obj);
      emotion_object_position_set(obj, pos + 10);
   } else if (!strcmp(ev->keyname, "Left")) {
      double pos;

      pos = emotion_object_position_get(obj);
      emotion_object_position_set(obj, pos - 10);
   }
}


EDJE_CALLBACK(quit)
{
   ecore_main_loop_quit();
}

EDJE_CALLBACK(play)
{
   Envision *e = data;
   printf("DEBUG: Play\n");
   emotion_object_play_set(e->gui.emotion, 1);
}

EDJE_CALLBACK(pause)
{
   Envision *e = data;
   printf("DEBUG: Pause\n");
   emotion_object_play_set(e->gui.emotion, 0);
}

EDJE_CALLBACK(volume_raise)
{
   Envision *e = data;
   double volume;
   char vol_str[3];

   volume = emotion_object_audio_volume_get(e->gui.emotion);
   volume = volume + 0.10;
   emotion_object_audio_volume_set(e->gui.emotion, volume);

   sprintf(vol_str, "%2.0f", volume * 100);
   edje_object_part_text_set(e->gui.edje, "vol_display_text", vol_str);

   evas_object_show(e->gui.edje);
}

EDJE_CALLBACK(volume_lower)
{
   Envision *e = data;
   double volume;
   char vol_str[3];

   volume = emotion_object_audio_volume_get(e->gui.emotion);
   volume = volume - 0.10;
   emotion_object_audio_volume_set(e->gui.emotion, volume);

   sprintf(vol_str, "%2.0f", volume * 100);
   edje_object_part_text_set(e->gui.edje, "vol_display_text", vol_str);
   evas_object_show(e->gui.edje);
}

EDJE_CALLBACK(seek_forward)
{
   Envision *e = data;
   double pos;

   pos = emotion_object_position_get(e->gui.emotion);
   printf("DEBUG: Position is %2f - Forward\n", pos);
   emotion_object_position_set(e->gui.emotion, pos + 60);
}

EDJE_CALLBACK(seek_backward)
{
   Envision *e = data;
   double pos;

   pos = emotion_object_position_get(e->gui.emotion);
   printf("DEBUG: Position is %2f - Backward\n", pos);
   emotion_object_position_set(e->gui.emotion, pos - 60);
}
