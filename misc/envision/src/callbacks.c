#include "envision.h"

/* if the canvas is resized - resize the video too */
void
canvas_resize(Ecore_Evas *ee)
{
   Evas * evas = ecore_evas_get(ee);
   Evas_Object * edje = evas_object_name_find(evas, "edje");
   Evas_Object * emotion = evas_object_name_find(evas, "emotion");
   Evas_Coord x, y, w, h;
   int vw, vh;
   double ratio;
   
   /* resize the video object AND retain aspect ratio */
   /* get the video size in pixels */
   emotion_object_size_get(emotion, &vw, &vh);
   /* get the video ratio */
   ratio = emotion_object_ratio_get(emotion);
   /* if the ratio > 0.0 then the video wants awidth / height ratio on the */
   /* screen as returned indicated by this ratio value. this is irrespective */
   /* of the pixel size of the video and indicates the video wants to scale */
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
      
      evas_object_resize(edje, w, h);
   } else {
      if (vh > 0) {
         /* generate ratio from the pixel size */
         ratio = (double)vw / (double)vh;
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
      
      evas_object_resize(edje, w, h);
   }
}

void
ecore_resize(Ecore_Evas *ee)
{
   Evas * evas = ecore_evas_get(ee);
   Evas_Object * edje = evas_object_name_find(evas, "edje");
   int ws = 0, hs = 0;

   ecore_evas_geometry_get(ee, NULL, NULL, &ws, &hs);
   evas_object_resize(edje, (Evas_Coord)ws, (Evas_Coord)hs);	
}

void
pause_callback(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Envision * e = data;
   printf("DEBUG: Pause\n");
   emotion_object_play_set(e->gui.emotion, 0);
}

void
play_callback(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Envision * e = data;
   printf("DEBUG: Play\n");
   emotion_object_play_set(e->gui.emotion, 1);
}

void
keydown_evascallback(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Key_Down *ev;

   int muted = 0;

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
      if(muted == 0) {
         emotion_object_audio_mute_set(obj, 1);
         muted = 1;
      } else {
         emotion_object_audio_mute_set(obj, 0);
         muted = 0;
      }
   } else if (!strcmp(ev->keyname, "Down")) {
      double pos;

      pos = emotion_object_position_get(obj);
      emotion_object_position_set(obj, pos-5);
   } else if (!strcmp(ev->keyname, "Up")) {
      double pos;
      int min, sec;

      pos = emotion_object_position_get(obj);
      emotion_object_position_set(obj, pos+5);
      pos = emotion_object_position_get(obj);
      min = pos / 60;
      printf("Position is: %d:00\n", min);
   } else if (!strcmp(ev->keyname, "Right")) {
      double pos;

      pos = emotion_object_position_get(obj);
      emotion_object_position_set(obj, pos+10);
   } else if (!strcmp(ev->keyname, "Left")) {
      double pos;

      pos = emotion_object_position_get(obj);
      emotion_object_position_set(obj, pos-10);
   }
}

void
quit_edjecallback(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   ecore_main_loop_quit();
}

void
raisevol_edjecallback(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Envision * e = data;
   double volume;
   char vol_str[3];

   volume = emotion_object_audio_volume_get(e->gui.emotion);
   volume = volume + 0.10;
   emotion_object_audio_volume_set(e->gui.emotion, volume);

   sprintf(vol_str, "%2.0f", volume * 100);
   edje_object_part_text_set(e->gui.edje, "vol_display_text", vol_str);

   evas_object_show(e->gui.edje);
}

void
lowervol_edjecallback(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Envision * e = data;
   double volume;
   char vol_str[3];

   volume = emotion_object_audio_volume_get(e->gui.emotion);
   volume = volume - 0.10;
   emotion_object_audio_volume_set(e->gui.emotion, volume);

   sprintf(vol_str, "%2.0f", volume * 100);
   edje_object_part_text_set(e->gui.edje, "vol_display_text", vol_str);
   evas_object_show(e->gui.edje);
}

void
seekbackward_edjecallback(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Envision * e = data;
   double pos;

   pos = emotion_object_position_get(e->gui.emotion);
   printf("DEBUG: Position is %2f - Backward\n", pos);
   emotion_object_position_set(e->gui.emotion, pos-60);
}

void
seekforward_edjecallback(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Envision * e = data;
   double pos;

   pos = emotion_object_position_get(e->gui.emotion);
   printf("DEBUG: Position is %2f - Forward\n", pos);
   emotion_object_position_set(e->gui.emotion, pos+60);
}

void
update_timer(void *data, Evas_Object *obj, void *event_info)
{
   Envision * e = data;
   char buffer[512];
   double len, pos;
   int pos_h, pos_m, len_h, len_m;
   double pos_s, len_s;

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
   /* print this to a stirng buffer */
   /* snprintf(buffer, sizeof(buffer), "%02i:%02i:%02.2f / %02i:%02i:%02.2f",
         pos_h, pos_m, pos_s, len_h, len_m, len_s);	*/
   snprintf(buffer, sizeof(buffer), "%02i:%02i:%02.0f", pos_h, pos_m, pos_s);
   edje_object_part_text_set(e->gui.edje, "time_text", buffer);
}
