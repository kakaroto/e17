/* mouse events processing */
#include <stdlib.h>
#include <stdio.h>
#include <Evas.h>

/* highlight : shades the button in blue when the mouse pointer is over it */
void highlight(void *rectangle, Evas canvas, Evas_Object object,
                  int button, int x, int y)
{
  evas_set_color(canvas, rectangle, 0,  0,  255, 180);
}

/* my_exit: this function is called when the rectangle is clicked */
void my_exit(void *data, Evas canvas, Evas_Object object,
            int button, int x, int y)
{
  printf("exit\n");
  exit(0);
}

int main(int argc, char *argv[])
{
  Display     *display;
  Window       window;
  Evas         canvas;
  Evas_Object  rectangle, text;

  /* Canvas creation */
  display = XOpenDisplay(NULL);
  canvas = evas_new_all(display,
                         RootWindow(display, DefaultScreen(display)),
                         100, 100, 300, 200,
                         RENDER_METHOD_ALPHA_SOFTWARE,
                         256, 0, 0, "./");
  window = evas_get_window(canvas);
  XSelectInput(display, window, ButtonPressMask | ButtonReleaseMask 
                  | PointerMotionMask);
  XMapWindow(display, window);
  XSync(display, False);
  /* setting the size of the canvas to that of the window */
  evas_set_output_size(canvas, 300, 200);
  
  /* some text */
  text = evas_add_text(canvas, "grunge", 20, "Vive Evas !");
  evas_show(canvas, text);
  evas_set_color(canvas, text, 255, 255, 255, 255);
  evas_move(canvas, text, 50, 70);

  /* this rectangle will be used as a button */
  rectangle = evas_add_rectangle(canvas);
  evas_show(canvas, rectangle);
  evas_set_color(canvas, rectangle, 0, 255, 0, 180);
  evas_move(canvas, rectangle, 50, 50);
  evas_resize(canvas, rectangle, 200, 100);

  /* connecting callback functions to our rectangle */
  evas_callback_add(canvas, rectangle, CALLBACK_MOUSE_DOWN, my_exit, NULL);
  evas_callback_add(canvas, rectangle, CALLBACK_MOUSE_MOVE, highlight, rectangle);

  /* updating */
  evas_update_rect(canvas, 0, 0, 300, 200);
  evas_render(canvas);

  /* event processing loop */
  for (;;)
  {
    XEvent ev;

    /* waiting for the next event */
    XNextEvent(display, &ev);
    switch (ev.type)
    {
      case MotionNotify: /* the mouse moved */
        evas_set_color(canvas, rectangle, 0,  255, 0, 180);
        evas_event_move(canvas, ev.xmotion.x, ev.xmotion.y);
        break;

      case ButtonPress: /* a mouse button has been pressed */
        evas_event_button_down(canvas, ev.xbutton.x, ev.xbutton.y, ev.xbutton.button);
        break;

      case ButtonRelease: /* a mouse button has been released */
        evas_event_button_up(canvas, ev.xbutton.x, ev.xbutton.y, ev.xbutton.button);
        break;
      default:
        break;
    }
    evas_update_rect(canvas, 0, 0, 300, 200);
    evas_render(canvas);
  }
  return 0;
}

