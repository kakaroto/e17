/* primitives */
#include <stdlib.h>
#include <Evas.h>

int main(int argc, char *argv[])
{
  XSetWindowAttributes  att;
  Display              *display;
  Visual               *visual;
  Colormap              colormap;
  Window                window;
  Evas                  canvas;
  Evas_Object           line, rectangle;
  Evas_Object           polygon, gradient;
  Evas_Object           text, image;
  Evas_Gradient         liste;
  int                   i, j;

  /* Creating the canvas */
  canvas = evas_new();
  /* choosing the rendering method */
  evas_set_output_method(canvas, RENDER_METHOD_ALPHA_SOFTWARE);
  /* the three usual X-Window variables */
  display = XOpenDisplay(NULL);
  visual = evas_get_optimal_visual(canvas, display);
  colormap = evas_get_optimal_colormap(canvas, display);

  /* Then we create the window with compatible parameters */
  att.colormap = colormap;
  window = XCreateWindow(display,
                          RootWindow(display, DefaultScreen(display)),
                          0, 0, 400, 600, 0,
                          imlib_get_visual_depth(display, visual),
                          InputOutput, visual, CWColormap, &att);
  XMapWindow(display, window);
  XSync(display, False);

  /* attaching the canvas to the window */
  evas_set_output(canvas, display, window, visual, colormap);

  /* setting the size of the canvas to that of the window */
  evas_set_output_size(canvas, 400, 600);
  evas_set_output_viewport(canvas, 0, 0, 400, 600);

  /* creating a few lines */
  for (i=0 ; i<1000 ; i++)
  {
    line = evas_add_line(canvas);
    evas_show(canvas, line);
    evas_set_color(canvas, line,
                   rand()%256, rand()%256, rand()%256, rand()%256);
    evas_set_line_xy(canvas, line,
                     rand()%200, rand()%200,
                     rand()%200, rand()%200);
  
  } 
  /* a few rectangles */
  for (i=0 ; i<100 ; i++)
  {
    int x1, y1, x2, y2;
    rectangle = evas_add_rectangle(canvas);
    evas_show(canvas, rectangle);
    evas_set_color(canvas, rectangle,
                   rand()%256, rand()%256, rand()%256, rand()%256);
    x1 = rand()%200; y1 = rand()%200;
    x2 = rand()%200; y2 = rand()%200;
    evas_move(canvas, rectangle, 200+x1, y1);
    evas_resize(canvas, rectangle, x2-x1, y2-y1);
  }

  /* polygons... */
  for (i=0 ; i<20 ; i++)
  {
    polygon = evas_add_poly(canvas);
    evas_show(canvas, polygon);
    evas_set_color(canvas, polygon,
                   rand()%256, rand()%256, rand()%256, rand()%256);
    for (j=0 ; j<5 ; j++)
      evas_add_point(canvas, polygon, rand()%200, 200+rand()%200);
  }

  /* two gradients */
  gradient = evas_add_gradient_box(canvas);
  evas_show(canvas, gradient);
  evas_resize(canvas, gradient, 200, 100);
  evas_move(canvas, gradient, 200, 200);
  liste = evas_gradient_new();
  evas_gradient_add_color(liste, 0, 0, 255, 255, 0);
  evas_gradient_add_color(liste, 255, 255, 255, 255, 100);
  evas_gradient_add_color(liste, 255, 0, 0, 255, 100);
  evas_set_gradient(canvas, gradient, liste);
  evas_set_angle(canvas, gradient, 270);
  
  gradient = evas_add_gradient_box(canvas);
  evas_show(canvas, gradient);
  evas_resize(canvas, gradient, 200, 100);
  evas_move(canvas, gradient, 200, 300);
  liste = evas_gradient_new();
  evas_gradient_add_color(liste, 0, 0, 255, 255, 0);
  evas_gradient_add_color(liste, 0, 0, 255, 255, 100);
  evas_gradient_add_color(liste, 255, 255, 255, 255, 0);
  evas_gradient_add_color(liste, 255, 255, 255, 255, 100);
  evas_gradient_add_color(liste, 255, 0, 0, 255, 0);
  evas_gradient_add_color(liste, 255, 0, 0, 255, 100);
  evas_set_gradient(canvas, gradient, liste);
  evas_set_angle(canvas, gradient, 270);
  
  /* some text */
  text = evas_add_text(canvas, "andover", 40, "Linux!");
  evas_show(canvas, text);
  evas_set_color(canvas, text, 255, 255, 255, 255);
  evas_move(canvas, text, 20, 450);

  /* and an image ! */
  image = evas_add_image_from_file(canvas, "sky001.png");
  evas_show(canvas, image);
  evas_set_color(canvas, image, 255, 255, 255, 255);
  evas_move(canvas, image, 200, 400);
 
  evas_update_rect(canvas, 0, 0, 400, 600);
  evas_render(canvas);
  /* infinite loop so that we have time to admire it all */
  for (;;);
  return 0;
}
