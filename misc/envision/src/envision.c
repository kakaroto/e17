#include <Ecore_Evas.h>
#include <Ecore.h>
#include <Emotion.h>
#include <Edje.h>

void quit_edjecallback(void *data, Evas_Object *obj, const char *emission, const char *source);
void raisevol_edjecallback(void *data, Evas_Object *obj, const char *emission, const char *source);
void lowervol_edjecallback(void *data, Evas_Object *obj, const char *emission, const char *source);
void keydown_evascallback(void *data, Evas *e, Evas_Object *obj, void *event_info);
void seekforward_edjecallback(void *data, Evas_Object *obj, const char *emission, const char *source);
void seekbackward_edjecallback(void *data, Evas_Object *obj, const char *emission, const char *source);
void ecore_resize(Ecore_Evas *ee);
void update_timer(void *data, Evas_Object *obj, void *event_info);


#define WIDTH 400
#define HEIGHT 400

	Ecore_Evas  *   ee;
	Evas        *   evas;
	Evas_Object *   edje;
        Evas_Object *   emotion;
	int 		w, h;
	Evas_Coord	minw, minh;
	double		volume, vid_len;
	char 		vol_str[3];
	int 		muted = 0;

void keydown_evascallback(void *data, Evas *e, Evas_Object *obj, void *event_info) {
        Evas_Event_Key_Down *ev;

        ev = (Evas_Event_Key_Down *)event_info;

	// Debug
        printf("You hit key: %s\n", ev->keyname);

        if (!strcmp(ev->keyname, "space")){
		emotion_object_play_set(obj, 0);
	}
        else if (!strcmp(ev->keyname, "Return")){
		emotion_object_play_set(obj, 1);
	}
        else if (!strcmp(ev->keyname, "Escape")){
		ecore_main_loop_quit();
	}
        else if (!strcmp(ev->keyname, "q")){
		ecore_main_loop_quit();
	}
        else if (!strcmp(ev->keyname, "m")){
		if(muted == 0){
			emotion_object_audio_mute_set(emotion, 1);
			muted = 1;
		} else {
			emotion_object_audio_mute_set(emotion, 0);
			muted = 0;
		}
        }
        else if (!strcmp(ev->keyname, "Down")){
		double pos;

		pos = emotion_object_position_get(obj);
		emotion_object_position_set(obj, pos-5);
	}	
        else if (!strcmp(ev->keyname, "Up")){
		double pos;
		int min, sec;

		pos = emotion_object_position_get(obj);
		emotion_object_position_set(obj, pos+5);
		pos = emotion_object_position_get(obj);
		min = pos / 60;
		printf("Position is: %d:00\n", min);
	}
        else if (!strcmp(ev->keyname, "Right")){
		double pos;

		pos = emotion_object_position_get(obj);
		emotion_object_position_set(obj, pos+10);
	}
        else if (!strcmp(ev->keyname, "Left")){
		double pos;

		pos = emotion_object_position_get(obj);
		emotion_object_position_set(obj, pos-10);
	}


}


int main(int argc, char *argv[]){

        if(argc < 1) {
                printf("Usage: %s [video]\n", argv[0]);
                return(1);
        }
	
	ecore_init();
	ecore_evas_init();
	edje_init();
	

   ee = ecore_evas_software_x11_new(NULL, 0,  0, 0, WIDTH, HEIGHT);
        ecore_evas_title_set(ee, "Envision");
        ecore_evas_borderless_set(ee, 1);
	ecore_evas_shaped_set(ee, 1);
        ecore_evas_show(ee);


   evas = ecore_evas_get(ee);
        //evas_font_path_append(evas, "data/");

	/* E D J E */
	
   edje	= edje_object_add(evas);
	edje_object_file_set(edje, "../data/default/trakvision.eet", "envision");
	evas_object_move(edje, 0, 0);	
	edje_object_size_min_get(edje, &minw, &minh);
	ecore_evas_resize(ee, (int)minw, (int)minh);
	evas_object_show(edje);
	

	/* Emotion */

   emotion = emotion_object_add(evas);
	emotion_object_file_set(emotion, argv[1]);
	emotion_object_size_get(emotion, &w, &h);

	// Trakvision: 106 25
	ecore_evas_resize(ee, w+25, h+106);	// Resize EVAS
	evas_object_resize(edje, w+25, h+106);	// Resize Edje
	//evas_object_move(emotion, 0, 0);	// Move Video 
	evas_object_resize(emotion, w, h);	// Resize Video

	evas_object_focus_set(emotion, 1);
	edje_object_part_swallow(edje, "swallow", emotion);
        evas_object_show(emotion);

	printf("Emotion: Playing - %s  Length: %d\n", 
			argv[1], (double)emotion_object_play_length_get(emotion));

	emotion_object_play_set(emotion, 1);
	
		/* EDJE STUFF */
	/* Get and Display the volume */
	volume = emotion_object_audio_volume_get(emotion);
	printf("DEBUG: Volume is: %0f\n", volume*100);
	sprintf(vol_str, "%2.0f", volume*100);
	edje_object_part_text_set(edje, "vol_display_text", vol_str);

	edje_object_part_text_set(edje, "video_name", argv[1]);
	vid_len = emotion_object_play_length_get(emotion);
	

	/* Callbacks */
	ecore_evas_callback_resize_set(ee, ecore_resize);
        evas_object_event_callback_add(emotion, 
			EVAS_CALLBACK_KEY_DOWN, keydown_evascallback, NULL); 
	evas_object_smart_callback_add(emotion, "frame_decode", update_timer, NULL);
	edje_object_signal_callback_add(edje, "VOL_INCR", "vol_incr_button", raisevol_edjecallback, NULL);
	edje_object_signal_callback_add(edje, "VOL_DECR", "vol_decr_button", lowervol_edjecallback, NULL);
	edje_object_signal_callback_add(edje, "QUIT", "quit", quit_edjecallback, NULL);
        edje_object_signal_callback_add(edje, "SEEK_BACK", "seekback_button", seekbackward_edjecallback, NULL);
        edje_object_signal_callback_add(edje, "SEEK_FORWARD", "seekforward_button", seekforward_edjecallback, NULL);

	

        ecore_main_loop_begin();

        return 0;
}

void quit_edjecallback(void *data, Evas_Object *obj, const char *emission, const char *source){

		ecore_main_loop_quit();
}

void raisevol_edjecallback(void *data, Evas_Object *obj, const char *emission, const char *source){
	double v;

        v = emotion_object_audio_volume_get(emotion);
        v = v + 0.10;
        emotion_object_audio_volume_set(emotion, v);

        sprintf(vol_str, "%2.0f", v*100);
        edje_object_part_text_set(edje, "vol_display_text", vol_str);

	evas_object_show(edje);
}

void lowervol_edjecallback(void *data, Evas_Object *obj, const char *emission, const char *source){
        double v;

        v = emotion_object_audio_volume_get(emotion);
        v = v - 0.10;
        emotion_object_audio_volume_set(emotion, v);
        sprintf(vol_str, "%2.0f", v*100);
        edje_object_part_text_set(edje, "vol_display_text", vol_str);
	evas_object_show(edje);
}

void seekbackward_edjecallback(void *data, Evas_Object *obj, const char *emission, const char *source){
	double pos;

	pos = emotion_object_position_get(emotion);
	printf("DEBUG: Position is %2f - Backward\n", pos);
	emotion_object_position_set(emotion, pos-10);
}
void seekforward_edjecallback(void *data, Evas_Object *obj, const char *emission, const char *source){
        double pos;

        pos = emotion_object_position_get(emotion);
	printf("DEBUG: Position is %2f - Forward\n", pos);
        emotion_object_position_set(emotion, pos+10);
}


void ecore_resize(Ecore_Evas *ee) {
        int ws = 0, hs = 0;

	ecore_evas_geometry_get(ee, NULL, NULL, &ws, &hs);
        evas_object_resize(edje, (Evas_Coord) ws, (Evas_Coord) hs);	
}

void update_timer(void *data, Evas_Object *obj, void *event_info){
	char buffer[512];
	double len, pos;
	int pos_h, pos_m, len_h, len_m;
	double pos_s, len_s;

        /* get the current position and length (in seconds) */
        pos = emotion_object_position_get(emotion);
        len = emotion_object_play_length_get(emotion);
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
        snprintf(buffer, sizeof(buffer), "%02i:%02i:%02.0f",
                 pos_h, pos_m, pos_s);
	edje_object_part_text_set(edje, "time_text", buffer);
}



