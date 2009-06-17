#include <Ecore_Evas.h>
#include <Ecore.h>

void key_down(void *data, Evas *e, Evas_Object *obj, void *event_info);
static int main_signal_exit(void *data, int ev_type, void *ev);
int show_image(char * filename);

#define MAXW 1280
#define MAXH 1024

	Evas_List   *   file_list;
	Evas_List   *   file_list_clean;
        Ecore_Evas  *   ee;
        Evas        *   evas;
        Evas_Object *   base_rect;
        Evas_Object *   image;
	int 		w, h;
	float		ratio;

int main(int argc, char *argv[]){

	int i, x;

	for(i = 1; i< argc; i++){
		file_list = evas_list_append(file_list, argv[i]);
	}

	x = evas_list_count(file_list);	
	file_list_clean = file_list;
	//printf("There are %d elements in the list\n", x);

        ecore_init();

   ee = ecore_evas_software_x11_new(NULL, 0,  0, 0, MAXW, MAXH);
        ecore_evas_title_set(ee, "eeh");
        ecore_evas_borderless_set(ee, 0);
        ecore_evas_show(ee);


   evas = ecore_evas_get(ee);
        evas_font_path_append(evas, "fonts/");


   base_rect = evas_object_rectangle_add(evas);
        evas_object_resize(base_rect, (double)MAXW, (double)MAXH);
        evas_object_color_set(base_rect, 244, 243, 242, 255);
        evas_object_show(base_rect);

   image = evas_object_image_add(evas);
	show_image(argv[1]);

	evas_object_focus_set(image, 1);
        evas_object_event_callback_add(image, 
			EVAS_CALLBACK_KEY_DOWN, key_down, NULL);  

        ecore_main_loop_begin();

        return 0;
}


void key_down(void *data, Evas *e, Evas_Object *obj, void *event_info) {
        Evas_Event_Key_Down *ev;

        ev = (Evas_Event_Key_Down *)event_info;
	
	if      (!strcmp(ev->keyname, "q")){
		printf("Quitting... later.\n");
		ecore_main_loop_quit();
	} else if (!strcmp(ev->keyname, "Escape")){
		printf("Quitting... later.\n");
                ecore_main_loop_quit();
	} else if (!strcmp(ev->keyname, "Left")){
		file_list = evas_list_prev(file_list);		
		if(evas_list_data(file_list)){
			show_image(evas_list_data(file_list));
		} else {
			printf("Prev past begining...\n");
			file_list = evas_list_last(file_list_clean);
			show_image(evas_list_data(file_list));
		}
	} else if (!strcmp(ev->keyname, "Right")){
		file_list = evas_list_next(file_list);		
		show_image(evas_list_data(file_list));
		if(evas_list_data(file_list)){
                        show_image(evas_list_data(file_list));
                } else {
                        printf("Hit end...\n");
                        file_list = file_list_clean;
                        show_image(evas_list_data(file_list));
                }
	} else {
        	//printf("You hit key: %s\n", ev->keyname);
	}
	

}

static int main_signal_exit(void *data, int ev_type, void *ev) {

   ecore_main_loop_quit();
   return 1;
}

int show_image(char * filename){

        evas_object_image_file_set(image, filename, NULL);

	if(evas_object_image_load_error_get(image)){
		printf("Can't load %s.\n", filename);
		return;
	}

        evas_object_image_size_get(image, &w, &h);
        printf("%s: Orig Size is %d by %d\n", filename, w, h);

        // Aspect Ratio Code
        if(h>MAXH){
                ratio =  (float)h / (float)w;
                //printf("Ratio is: %2f\n", (float)ratio);
                h = MAXH;
                w = h / ratio;
                //printf("Width %d = %d / %d\n", w, h, ratio);
        } else if(w>MAXW){
                ratio = (float)w * (float)h;
                //printf("Ratio is: %2f\n", ratio);
                w = MAXW;
                h = w / ratio;
                //printf("Height %d = %d / %d\n", h, w, ratio);
        }
        //printf("New Size is: %d by %d\n", w, h);

        evas_object_image_fill_set(image, 0, 0, (Evas_Coord)w, (Evas_Coord)h);
        evas_object_resize(image, w, h);
        ecore_evas_resize(ee, w, h);
        evas_object_resize(base_rect, w, h);
        evas_object_show(image);

	return(0);
}
