/*
 * esmart_main.c
 * 
 * a test program for the thumbnail and container smart objects.
 * usage: esmart_test file1 [file2] [file3] ...
 *
 * esmart_test creates thumbnails out of all valid image files given
 * as arguments, and puts them into a horizontal container.
 * click on the white background to scroll the contents.
 */


#include <stdio.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include "E_Thumb.h"
#include "container.h"

static void
window_del_cb(Ecore_Evas *ee)
{
    ecore_main_loop_quit();
}

static void
window_resize_cb(Ecore_Evas *ee)
{
    int w, h;
    Evas_Object *o = NULL;

    ecore_evas_geometry_get(ee, NULL, NULL, &w, &h);

    if((o = evas_object_name_find(ecore_evas_get(ee), "background")))
	evas_object_resize(o, w, h);
    if((o = evas_object_name_find(ecore_evas_get(ee), "container")))
	evas_object_resize(o, w, h);
}

static int
exit_cb(void *ev, int ev_type, void *data)
{
    ecore_main_loop_quit();
    return(0);
}

static int
bg_down_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
  Evas_Event_Mouse_Down *event = event_info;
  Evas_Object *cont = data;
  double x, w;
  
  evas_object_geometry_get(obj, &x, NULL, &w, NULL);
  if (event->canvas.x < x + w/2)
    e_container_scroll_start(cont, -1);
  else
    e_container_scroll_start(cont, 1);
  

}

static int
bg_up_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
  Evas_Event_Mouse_Up *event = event_info;
  Evas_Object *cont = data;

  printf("up!\n");
  e_container_scroll_stop(cont);
}

int
main(int argc, char *argv[])
{
    Evas *evas = NULL;
    Ecore_Evas *ee = NULL;
    Evas_Object *o = NULL;
    Evas_Object *cont = NULL;

    ecore_init();
    ecore_app_args_set(argc, (const char**)argv);

    ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, exit_cb, NULL);

    if(ecore_evas_init())
    {
	ee = ecore_evas_software_x11_new(NULL, 0, 0, 0, 300, 120);
	ecore_evas_title_set(ee, "Enlightenment Thumbnail Test");
	ecore_evas_callback_delete_request_set(ee, window_del_cb);
	ecore_evas_callback_resize_set(ee, window_resize_cb);

	evas = ecore_evas_get(ee);
	o = evas_object_rectangle_add(evas);
	evas_object_move(o, 0, 0);
	evas_object_resize(o, 300, 120);
	evas_object_color_set(o, 255, 255, 255, 255);
	evas_object_layer_set(o, -5);
	evas_object_name_set(o, "background");
	evas_object_show(o);

	cont = e_container_new(evas);
	evas_object_move(cont, 0, 0);
	evas_object_resize(cont, 300, 120);
	evas_object_layer_set(cont, 0);
	evas_object_name_set(cont, "container");
	evas_object_show(cont);
	e_container_padding_set(cont, 10, 10, 10, 10);
	e_container_spacing_set(cont, 5);
	e_container_fill_policy_set(cont,
				    CONTAINER_FILL_POLICY_FILL_Y | 
				    CONTAINER_FILL_POLICY_KEEP_ASPECT);

	evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN, bg_down_cb, cont);
	evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_UP, bg_up_cb, cont);

	while(--argc)
	{
	    if((o = e_thumb_new(ecore_evas_get(ee), argv[argc])))
	    {
		evas_object_layer_set(o, 2);
		evas_object_show(o);
		e_container_element_append(cont, o);
	    }
	}

	ecore_evas_show(ee);
	ecore_main_loop_begin();
    }
    return(0);
}
