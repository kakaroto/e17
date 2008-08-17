#include "retina.h"

void
retina_cb_attach()
{
	evas_object_focus_set(retina_catch, 1);
	
	ecore_evas_callback_resize_set(retina_win, retina_cb_win_resize);
	ecore_evas_callback_delete_request_set(retina_win, retina_cb_delete_event);

	evas_object_event_callback_add(retina_catch, EVAS_CALLBACK_MOUSE_DOWN,
			retina_cb_mouse_down, NULL);
	
	evas_object_event_callback_add(retina_catch, EVAS_CALLBACK_KEY_DOWN,
			 retina_cb_key_down, NULL);
}

void
retina_cb_delete_event()
{
	ecore_main_loop_quit();
}

void
retina_cb_win_resize()
{
	/* center the image on window resize */
	int w, h, x = 0, y = 0;
	int im_w, im_h;
	
	if(retina_img){
		im_w = retina_image.zoom_w;
		im_h = retina_image.zoom_h;
		ecore_evas_geometry_get(retina_win, NULL, NULL, &w, &h);

		if(im_w > w)
			ecore_evas_resize(retina_win,
					retina_image.zoom_w, retina_image.zoom_h);

		if(w > im_w){
			x = (w - im_w) / 2;
		} else {
			x = 0;
		}

		if(h > im_h){
			y = (h - im_h) / 2;
		} else {
			y = 0;
		}
	}
	
	evas_object_move(retina_img, x, y);

	if(diag_state == 1) evas_object_resize(retina_diag_bg, w, h);
	if(help_state == 1){
		x = (w - 320) / 2;
		y = (h - 164) / 2;
		evas_object_move(retina_help_img, x, y);
	}
}

void
retina_cb_mouse_down(void *data, Evas *e, Evas_Object*obj, void *event_info)
{
	Evas_Event_Mouse_Down *event = event_info;
		
	printf("button down: %d\n", event->button);
	
	switch(event->button){
		case 1: retina_img_show_help(); break;
		case 2: retina_img_show_image_info(); break;
		case 3: retina_thumbnail_browser_show(); break;
	}
}

void
retina_cb_key_down(void *data, Evas *e, Evas_Object*obj, void *event_info)
{
	Evas_Event_Key_Down *event = event_info;
	printf("key down: %s\n", event->keyname);

	if(!strcmp(event->keyname, "minus")) retina_img_zoom_out();
	if(!strcmp(event->keyname, "equal"))  retina_img_zoom_in();
	if(!strcmp(event->keyname, "Return"))  retina_img_zoom_set();
	if(!strcmp(event->keyname, "Down")) retina_img_load_next();
	if(!strcmp(event->keyname, "Up")) retina_img_load_back();
	if(!strcmp(event->keyname, "Escape")) retina_cb_delete_event();
}
