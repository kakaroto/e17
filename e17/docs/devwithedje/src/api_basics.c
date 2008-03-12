#include <stdlib.h>
#include <stdio.h>
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Edje.h>

//The pointer to a canvas wrapper
Ecore_Evas *ecore_evas = NULL;
//The pointer to an Evas canvas
Evas *evas = NULL;
//The pointer to a given Edje object
Evas_Object *edje = NULL;
//Width and height for resizing Evas/Edje objects
Evas_Coord width, height;
//The pointer to an Ecore handler
Ecore_Event_Handler* close = NULL;

int
good_bye(void *data, int type, void *event)
{
    //Remove the handler for no practical reason but showoff the API
    if (ecore_event_handler_del(close)) printf("Handler deleted\n");
    
    printf("Good bye! \n");
    ecore_main_loop_quit();
    ecore_evas_shutdown();
    ecore_shutdown();
    edje_shutdown();
}

void
colorize(void *data, Evas_Object *o, const char *emission, const char *source)
{
    Evas_Coord x,y;
    evas_pointer_canvas_xy_get(evas,&x,&y);
    if(x > 255) x = 255;
    if(y > 255) y = 255;             //R,   G,   B,   A,
    edje_color_class_set("main color", 190, x,   y,   255,
                                       255, 255, 255, 255,
                                       255, 255, 255, 255);
}

int main() {

    //Control that the libraries are properly initialized
    if (!ecore_init()) return EXIT_FAILURE;
    if (!ecore_evas_init()) return EXIT_FAILURE;
    if (!edje_init()) return EXIT_FAILURE;
 
    //Check the canvas wrapper (800x600 X11 window) is created correctly
    ecore_evas = ecore_evas_software_x11_new(NULL, 0, 0, 0, 800, 600);
    if (!ecore_evas) return EXIT_FAILURE;

    //We set some window attributes and make the wrapper visible
    ecore_evas_title_set(ecore_evas, "Example Application");
    ecore_evas_name_class_set(ecore_evas, "testapp", "Testapp");
    ecore_evas_show(ecore_evas);

    //Get the pointer to the canvas and add an object
    evas = ecore_evas_get(ecore_evas);
    edje = edje_object_add(evas);
    edje_object_file_set(edje, "testfile.edj", "testgroup");

    //Setting the object and canvas to the minimal size of the object
    evas_object_move(edje, 0, 0);
    edje_object_size_min_get(edje, &width, &height);
    evas_object_resize(edje, width, height);
    ecore_evas_resize(ecore_evas, width, height);
    evas_object_show(edje);

    //Setting up two handlers
    close = ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT,good_bye,"data");
    edje_object_signal_callback_add(edje, "mouse,move", "*", colorize, "data");

    //Starting the main application loop
    ecore_main_loop_begin();
}
