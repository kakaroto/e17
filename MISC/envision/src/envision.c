/*
 * $Id$
 * vim:expandtab:ts=3:sts=3:sw=3
 */

#include <../config.h>
#include "config.h"
#include "envision.h"
#include "callbacks.h"

Envision *
envision_new(void)
{
   return calloc(1, sizeof (Envision));
}

void
envision_delete(Envision * e)
{
   free(e);
}

int
main(int argc, char **argv)
{
   Envision *e = NULL;
   double volume, vid_len;
   char vol_str[3], *videofile = NULL;
   Evas_Coord minw, minh, w, h;

   e = envision_new();
   e->config.width = 400;
   e->config.height = 400;
   e->config.engine = NULL;

   /*  Configuration  */
   if (argc > 0 && argv[1] != NULL) {
      videofile = strdup(argv[1]);
   }

   if (envision_config_get(&e->config, argc, argv) == -1) {
      free(videofile);
      envision_delete(e);
      return (0);
   }

   // Initialization
   ecore_init ();
   ecore_evas_init ();
   edje_init ();


   // Setup evas canvas
   e->gui.ee = ecore_evas_software_x11_new(NULL, 0, 0, 0, e->config.width,
         e->config.height);
   ecore_evas_title_set(e->gui.ee, "Envision Media Player");
   ecore_evas_name_class_set(e->gui.ee, "envision", "Envision");
   ecore_evas_borderless_set(e->gui.ee, 1);
   ecore_evas_shaped_set(e->gui.ee, 1);
   ecore_evas_show(e->gui.ee);
   e->gui.evas = ecore_evas_get(e->gui.ee);


   // Setup edje objects
   e->gui.edje = edje_object_add(e->gui.evas);
   edje_object_file_set(e->gui.edje, DATA_DIR "/themes/trakvision.edj",
         "envision");
   evas_object_move(e->gui.edje, 0, 0);
   edje_object_size_min_get(e->gui.edje, &minw, &minh);
   ecore_evas_resize(e->gui.ee, (int)minw, (int)minh);
   evas_object_name_set(e->gui.edje, "edje");
   evas_object_show(e->gui.edje);


   // Setup emotion objects
   e->gui.emotion = emotion_object_add(e->gui.evas);
   if (!emotion_object_init(e->gui.emotion, "emotion_decoder_xine.so"))
      return (0);
   emotion_object_file_set(e->gui.emotion, videofile);
   emotion_object_size_get(e->gui.emotion, &w, &h);
   evas_object_name_set(e->gui.emotion, "emotion");


   // Trakvision: 106 25
   ecore_evas_resize(e->gui.ee, w+25, h+106);	// Resize EVAS
   evas_object_resize(e->gui.edje, w+25, h+106);	// Resize Edje
   evas_object_resize(e->gui.emotion, w, h);	// Resize Video
   evas_object_focus_set(e->gui.emotion, 1);
   edje_object_part_swallow(e->gui.edje, "swallow", e->gui.emotion);
   evas_object_show(e->gui.emotion);

   printf("Emotion: Playing - %s  Length: %.0f\n", argv[1],
          emotion_object_play_length_get(e->gui.emotion));
   emotion_object_play_set(e->gui.emotion, 1);


   /* EDJE STUFF */
   /* Get and Display the volume */
   volume = emotion_object_audio_volume_get(e->gui.emotion);
   printf("DEBUG: Volume is: %0f\n", volume * 100);
   sprintf(vol_str, "%2.0f", volume * 100);
   edje_object_part_text_set(e->gui.edje, "vol_display_text", vol_str);
   edje_object_part_text_set(e->gui.edje, "video_name", argv[1]);
   vid_len = emotion_object_play_length_get(e->gui.emotion);


   /* Callbacks */
   ecore_evas_callback_resize_set(e->gui.ee, canvas_resize);
   evas_object_event_callback_add(e->gui.emotion, EVAS_CALLBACK_KEY_DOWN,
         callback_evas_keydown, NULL);
   evas_object_smart_callback_add(e->gui.emotion, "frame_decode",
         update_timer, e);
   edje_object_signal_callback_add(e->gui.edje, "VOL_INCR", "vol_incr_button",
         callback_edje_volume_raise, e);
   edje_object_signal_callback_add(e->gui.edje, "VOL_DECR", "vol_decr_button",
         callback_edje_volume_lower, e);
   edje_object_signal_callback_add(e->gui.edje, "QUIT", "quit",
         callback_edje_quit, NULL);
   edje_object_signal_callback_add(e->gui.edje, "SEEK_BACK",
         "seekback_button", callback_edje_seek_backward, e);
   edje_object_signal_callback_add(e->gui.edje, "SEEK_FORWARD",
         "seekforward_button", callback_edje_seek_forward, e);
   edje_object_signal_callback_add(e->gui.edje, "PAUSE", "pause_button",
         callback_edje_pause, e);
   edje_object_signal_callback_add(e->gui.edje, "PLAY", "play_button",
         callback_edje_play, e);


   // Begin the program
   ecore_main_loop_begin();


   // Program ends
   envision_delete(e);
   return 0;
}
