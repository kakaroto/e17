#include <Ecore_Evas.h>
#include <Ecore.h>
#include <Emotion.h>
#include <Edje.h>

#define WIDTH 800
#define HEIGHT 500

	Ecore_Evas  *   ee;
	Evas        *   evas;
	Evas_Object *   edje;
        Evas_Object *   emotion;
	int 		w, h;
	Evas_Coord	minw, minh;

void key_down(void *data, Evas *e, Evas_Object *obj, void *event_info) {
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
        else if (!strcmp(ev->keyname, "Down")){
		double pos;

		pos = emotion_object_position_get(obj);
		emotion_object_position_set(obj, pos-10.0);
	}	
        else if (!strcmp(ev->keyname, "Up")){
		double pos;
		int min, sec;

		pos = emotion_object_position_get(obj);
		emotion_object_position_set(obj, pos+10.0);
		pos = emotion_object_position_get(obj);
		min = pos / 60;
		printf("Position is: %d:00\n", min);
	}
        else if (!strcmp(ev->keyname, "Right")){
		double pos;

		pos = emotion_object_position_get(obj);
		emotion_object_position_set(obj, pos+5.0);
	}
        else if (!strcmp(ev->keyname, "Left")){
		double pos;

		pos = emotion_object_position_get(obj);
		emotion_object_position_set(obj, pos-5.0);
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

	ecore_evas_resize(ee, w+21, h+46);	// Resize EVAS
	evas_object_resize(edje, w+21, h+46);	// Resize Edje
	//evas_object_move(emotion, 0, 0);	// Move Video 
	evas_object_resize(emotion, w, h);	// Resize Video

	evas_object_focus_set(emotion, 1);
	edje_object_part_swallow(edje, "swallow", emotion);
        evas_object_show(emotion);

	printf("Emotion: Playing - %s  Length: %d\n", 
			argv[1], (double)emotion_object_play_length_get(emotion));

	emotion_object_play_set(emotion, 1);

        evas_object_event_callback_add(emotion, 
			EVAS_CALLBACK_KEY_DOWN, key_down, NULL); 


        ecore_main_loop_begin();

        return 0;
}

