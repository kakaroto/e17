#include "envision.h"

Envision *
envision_new()
{
   return calloc (1, sizeof (Envision));
}

void
envision_delete(Envision *e)
{
   free(e);
}

int
main(int argc, char **argv)
{
   if(argc < 1) {
      printf("Envision - EFL-Powered DVD Player\n");
      printf("=================================\n");
      printf("Usage: %s [video]\n", argv[0]);
      return(1);
   }

   Envision * e = NULL;
   double volume, vid_len;
   Evas_Coord minw, minh;

   e = envision_new();
   e->config.width = 400;
   e->config.height = 400;

   // Initialization
   ecore_init();
   ecore_evas_init();
   edje_init();
	

   // Setup evas canvas
   e->gui.ee = ecore_evas_software_x11_new(NULL, 0,  0, 0, e->config.width,
         e->config.height);
   ecore_evas_title_set(e->gui.ee, "Envision DVD Player");
   ecore_evas_borderless_set(e->gui.ee, 1);
   ecore_evas_shaped_set(e->gui.ee, 1);
   ecore_evas_show(e->gui.ee);
   e->gui.evas = ecore_evas_get(e->gui.ee);


   // Setup edje objects
   e->gui.edje = edje_object_add(e->gui.evas);
   edje_object_file_set(e->gui.edje, "../data/themes/trakvision/trakvision.eet",
         "envision");
   evas_object_move(e->gui.edje, 0, 0);	
   edje_object_size_min_get(e->gui.edje, &minw, &minh);
   ecore_evas_resize(e->gui.ee, (int)minw, (int)minh);
   evas_object_show(e->gui.edje);
	

   // Setup emotion objects
   e->gui.emotion = emotion_object_add(e->gui.evas);
   emotion_object_file_set(e->gui.emotion, argv[1]);
   emotion_object_size_get(e->gui.emotion, &w, &h);

   // Trakvision: 106 25
   ecore_evas_resize(e->gui.ee, w+25, h+106);	// Resize EVAS
   evas_object_resize(e->gui.edje, w+25, h+106);	// Resize Edje
   //evas_object_move(e->gui.emotion, 0, 0);	// Move Video 
   evas_object_resize(e->gui.emotion, w, h);	// Resize Video

   evas_object_focus_set(e->gui.emotion, 1);
   edje_object_part_swallow(e->gui.edje, "swallow", e->gui.emotion);
   evas_object_show(e->gui.emotion);

   printf("Emotion: Playing - %s  Length: %d\n",
         argv[1], (double)emotion_object_play_length_get(e->gui.emotion));
   emotion_object_play_set(e->gui.emotion, 1);


   /* EDJE STUFF */
   /* Get and Display the volume */
   volume = emotion_object_audio_volume_get(e->gui.emotion);
   printf("DEBUG: Volume is: %0f\n", volume*100);
   sprintf(vol_str, "%2.0f", volume*100);
   edje_object_part_text_set(e->gui.edje, "vol_display_text", vol_str);
   edje_object_part_text_set(e->gui.edje, "video_name", argv[1]);
   vid_len = emotion_object_play_length_get(e->gui.emotion);
   evas_object_name_set(e->gui.edje, "edje");
	

   /* Callbacks */
   ecore_evas_callback_resize_set(e->gui.ee, ecore_resize);
   evas_object_event_callback_add(e->gui.emotion, EVAS_CALLBACK_KEY_DOWN,
         keydown_evascallback, NULL);
   evas_object_smart_callback_add(e->gui.emotion, "frame_decode", update_timer,
         e);
   edje_object_signal_callback_add(e->gui.edje, "VOL_INCR", "vol_incr_button",
         raisevol_edjecallback, e);
   edje_object_signal_callback_add(e->gui.edje, "VOL_DECR", "vol_decr_button",
         lowervol_edjecallback, e);
   edje_object_signal_callback_add(e->gui.edje, "QUIT", "quit", quit_edjecallback,
         NULL);
   edje_object_signal_callback_add(e->gui.edje, "SEEK_BACK", "seekback_button",
         seekbackward_edjecallback, e);
   edje_object_signal_callback_add(e->gui.edje, "SEEK_FORWARD", "seekforward_button",
         seekforward_edjecallback, e);
   edje_object_signal_callback_add(e->gui.edje, "PAUSE", "pause_button",
         pause_callback, e);
   edje_object_signal_callback_add(e->gui.edje, "PLAY", "play_button",
         play_callback, e);

   // Begin the program
   ecore_main_loop_begin();

   envision_delete(e);

   return 0;
}
