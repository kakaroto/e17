/* properties */
#include <stdlib.h>
#include <stdio.h>
#include <Evas.h>

void highlight(Evas canvas, int index)
{
  double      x, y, width, height;
  double      x_text, y_text;
  Evas_Object rectangle, text;

  text = evas_object_get_named(canvas, "Texte");
  evas_get_geometry(canvas, text,
                    &x_text, &y_text, NULL, NULL);
  evas_text_at(canvas, text, index, &x, &y,
               &width, &height);
  rectangle = evas_add_rectangle(canvas);
  evas_show(canvas, rectangle);
  evas_set_color(canvas, rectangle, 255, 0, 0, 128);
  evas_move(canvas, rectangle, x_text+x, y_text+y);
  evas_resize(canvas, rectangle, width, height);  
}

int main(int argc, char *argv[])
{
  XSetWindowAttributes  att;
  Display              *display;
  Visual               *visual;
  Colormap              colormap;
  Window                window;
  Evas                  canvas;
  Evas_Object           rectangle, text;
  int                   i;
  Evas_List             list;

  /* Creating the canvas */
  canvas = evas_new();
  /* Choosing the rendering method */
  evas_set_output_method(canvas, RENDER_METHOD_ALPHA_SOFTWARE);
  /* Getting the X Window variables */
  display = XOpenDisplay(NULL);
  visual = evas_get_optimal_visual(canvas, display);
  colormap = evas_get_optimal_colormap(canvas, display);

  /* Opening a window */
  att.colormap = colormap;
  window = XCreateWindow(display,
                          RootWindow(display, DefaultScreen(display)),
                          0, 0, 300, 200, 0,
                          imlib_get_visual_depth(display, visual),
                          InputOutput, visual, CWColormap, &att);
  XMapWindow(display, window);
  XSync(display, False);

  /* attaching the canvas to the window */
  evas_set_output(canvas, display, window, visual, colormap);

  /* setting the canvas' size to that of the window */
  evas_set_output_size(canvas, 300, 200);
  evas_set_output_viewport(canvas, 0, 0, 300, 200);

  /* Drawing a bunch of rectangles */
  for (i=0 ; i<100 ; i++)
  {
    int x1, y1, x2, y2;
    rectangle = evas_add_rectangle(canvas);
    evas_show(canvas, rectangle);
    evas_set_color(canvas, rectangle,
    rand()%256, rand()%256, rand()%256, rand()%256);
    x1 = rand()%300; y1 = rand()%100;
    x2 = rand()%300; y2 = rand()%100;
    evas_move(canvas, rectangle, x1, y1);
    evas_resize(canvas, rectangle, x2-x1, y2-y1);
    evas_put_data(canvas, rectangle, "x1", (void*)x1);
    evas_put_data(canvas, rectangle, "x2", (void*)x2);
    evas_put_data(canvas, rectangle, "y1", (void*)y1);
    evas_put_data(canvas, rectangle, "y2", (void*)y2);
  }

  /* some text */
  text = evas_add_text(canvas, "grunge", 20, "Vive Evas !");
  evas_object_set_name(canvas, text, "Texte");
  evas_show(canvas, text);
  evas_set_color(canvas, text, 255, 255, 255, 255);
  evas_move(canvas, text, 20, 100);

  /* highlighting the letters of 'Evas' */
  for (i=5 ; i<9 ; i++)
    highlight(canvas, i);
 
  /* asking Evas to display it all */
  evas_update_rect(canvas, 0, 0, 300, 200);
  evas_render(canvas);

  /* display the properties of some rectangles */
  list = evas_objects_in_rect(canvas, 20, 20, 260, 160);
  while (list)
  {
    int x1, x2, y1, y2, rouge, vert, bleu, opacite;
    rectangle = (Evas_Object)(list->data);
    
    /* coordinates */
    x1 = (int)evas_get_data(canvas, rectangle, "x1");
    x2 = (int)evas_get_data(canvas, rectangle, "x2");
    y1 = (int)evas_get_data(canvas, rectangle, "y1");
    y2 = (int)evas_get_data(canvas, rectangle, "y2");
    /* color */
    evas_get_color(canvas, rectangle,
                   &rouge, &vert, &bleu, &opacite);
    
    printf("(%d, %d) - (%d, %d) <%d, %d, %d,  %d>\n",
           x1, y1, x2, y2, rouge, vert, bleu, opacite);
    list = evas_list_remove_list(list, list);
  }
  
  /* infinite loop so that we have time to admire it all */
  for (;;);
  return 0;
}
