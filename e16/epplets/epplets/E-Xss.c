#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <dirent.h>
#include "epplet.h"

#if 0
#  define D(x) do {printf("%10s | %7d:  [debug] ", __FILE__, __LINE__); printf x; fflush(stdout);} while (0)
#else
#  define D(x) ((void) 0)
#endif
#define BEGMATCH(a, b)  (!strncasecmp((a), (b), (sizeof(b) - 1)))
#define NONULL(x)       ((x) ? (x) : (""))

#define PREV_HACK()      do {if (idx == 1) idx = image_cnt - 1; else if (idx == 0) idx = image_cnt - 2; else idx -= 2;} while (0)
#define CUR_HACK()       ((idx == 0) ? (image_cnt - 1) : (idx - 1))
#define NEXT_HACK()      ((void) 0)
#define INC_HACK()       do {idx++; if (idx == image_cnt) idx = 0;} while (0)

Epplet_gadget close_button, play_button, pause_button, prev_button, next_button, zoom_button, draw_area;
unsigned long idx = 0, hack_cnt = 0;
double delay = 5.0;
char **hacks = NULL, *hack = NULL;
unsigned char paused = 0;
pid_t hack_pid = -1;

static void start_hack(void);
static void stop_hack(void);
static void change_hack(void *data);
static void child_cb(void *data, int pid, int exit_code);
static void close_cb(void *data);
static void zoom_cb(void *data);
static void play_cb(void *data);
static void in_cb(void *data, Window w);
static void out_cb(void *data, Window w);

static void
start_hack(void) {

  char buff[1024];

  sprintf(buff, "%s -window-id %ld", hack, Epplet_get_drawingarea_window(draw_area));
  Epplet_register_child_handler(child_cb, NULL);
  hack_pid = Epplet_spawn_command(buff);
}

static void
stop_hack(void) {
  kill(hack_pid, SIGTERM);
  hack_pid = -1;
}

static void
change_hack(void *data) {

#if 0
  /* Test-load each image to make sure it's a valid image file. */
  for (; ((filenames[idx] == NULL) || ((im = Imlib_load_image(Epplet_get_imlib_data(), filenames[idx])) == NULL)); idx++) {
    /* It isn't, so NULL out its name. */
    filenames[idx] = NULL;
  }
  Imlib_destroy_image(Epplet_get_imlib_data(), im);  /* Destroy the image, but keep it in cache. */

  Epplet_change_image(picture, 42, 42, filenames[idx]);
  INC_PIC();

  Epplet_remove_timer("CHANGE_IMAGE");
  if (!paused) {
    Epplet_timer(change_image, NULL, delay, "CHANGE_IMAGE");
  }
#endif

  return;
  data = NULL;
}

static void
child_cb(void *data, int pid, int exit_code) {

  if (pid == hack_pid) {
    fprintf(stderr, "E-Xss:  Child process exited with return code %d\n", exit_code);
    exit(0);
  }
  return;
  data = NULL;
}

static void
close_cb(void *data) {

  stop_hack();
  Epplet_unremember();
  Esync();
  exit(0);
  data = NULL;
}

static void
zoom_cb(void *data) {

#if 0
  char buff[1024];

  sprintf(buff, zoom_cmd, filenames[CUR_PIC()]);
  Epplet_spawn_command(buff);
#endif
  return;
  data = NULL;
}

static void
play_cb(void *data) {

#if 0
  int op = (int) data;

  switch (op) {
    case -1:
      /* Previous image */
      PREV_PIC();
      change_image(NULL);
      break;
    case 0:
      /* Pause */
      Epplet_remove_timer("CHANGE_IMAGE");
      paused = 1;
      Epplet_gadget_hide(pause_button);
      Epplet_gadget_show(play_button);
      break;
    case 1:
      /* Play */
      paused = 0;
      Epplet_gadget_hide(play_button);
      Epplet_gadget_show(pause_button);
      change_image(NULL);
      break;
    case 2:
      /* Next image */
      NEXT_PIC();
      change_image(NULL);
      break;
    default:
      break;
  }
#endif
  return;
  data = NULL;
}

static void
in_cb(void *data, Window w) {

  if (w == Epplet_get_drawingarea_window(draw_area) || w == Epplet_get_main_window()) {
    stop_hack();
    Epplet_gadget_show(close_button);
    Epplet_gadget_show(zoom_button);
#if 0
    Epplet_gadget_show(prev_button);
    Epplet_gadget_show(next_button);
    if (paused) {
      Epplet_gadget_show(play_button);
    } else {
      Epplet_gadget_show(pause_button);
    }
#endif
  }
  return;
  data = NULL;
  w = (Window) 0;
}

static void
out_cb(void *data, Window w) {

  if (w == Epplet_get_drawingarea_window(draw_area) || w == Epplet_get_main_window()) {
    start_hack();
    Epplet_gadget_hide(close_button);
    Epplet_gadget_hide(zoom_button);
#if 0
    Epplet_gadget_hide(prev_button);
    Epplet_gadget_hide(next_button);
    Epplet_gadget_hide(play_button);
    Epplet_gadget_hide(pause_button);
#endif
  }
  return;
  data = NULL;
  w = (Window) 0;
}

static void
parse_config(void) {

  hack = Epplet_query_config_def("hack", "bubbles");
}

int
main(int argc, char **argv) {

  int prio;

  prio = getpriority(PRIO_PROCESS, getpid());
  setpriority(PRIO_PROCESS, getpid(), prio + 10);
  atexit(Epplet_cleanup);
  Epplet_Init("E-Xss", "0.1", "Enlightenment Xscreensaver Epplet", 3, 3, argc, argv, 0);
  Epplet_load_config();
  parse_config();

  close_button = Epplet_create_button(NULL, NULL, 3, 3, 0, 0, "CLOSE", 0, NULL, close_cb, NULL);
  zoom_button = Epplet_create_button(NULL, NULL, 33, 3, 0, 0, "EJECT", 0, NULL, zoom_cb, NULL);
  prev_button = Epplet_create_button(NULL, NULL, 3, 33, 0, 0, "PREVIOUS", 0, NULL, play_cb, (void *) (-1));
  play_button = Epplet_create_button(NULL, NULL, 18, 33, 0, 0, "PLAY", 0, NULL, play_cb, (void *) (1));
  pause_button = Epplet_create_button(NULL, NULL, 18, 33, 0, 0, "PAUSE", 0, NULL, close_cb, (void *) (0));
  next_button = Epplet_create_button(NULL, NULL, 33, 33, 0, 0, "NEXT", 0, NULL, play_cb, (void *) (2));
  Epplet_gadget_show(prev_button);
  Epplet_gadget_show(next_button);
  Epplet_gadget_show(pause_button);
  draw_area = Epplet_create_drawingarea(3, 3, 42, 28);
  Epplet_gadget_show(draw_area);
  Epplet_show();

  Epplet_register_mouse_enter_handler(in_cb, NULL);
  Epplet_register_mouse_leave_handler(out_cb, NULL);
  Epplet_Loop();

  return 0;
}
