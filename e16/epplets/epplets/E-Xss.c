#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include "epplet.h"

#if 0
#  define D(x) do {printf("%10s | %7d:  [debug] ", __FILE__, __LINE__); printf x; fflush(stdout);} while (0)
#else
#  define D(x) ((void) 0)
#endif
#define BEGMATCH(a, b)  (!strncasecmp((a), (b), (sizeof(b) - 1)))
#define NONULL(x)       ((x) ? (x) : (""))

#define PREV_HACK()      do {if (idx == 1) idx = hack_cnt - 1; else if (idx == 0) idx = hack_cnt - 2; else idx -= 2;} while (0)
#define CUR_HACK()       ((idx == 0) ? (hack_cnt - 1) : (idx - 1))
#define NEXT_HACK()      ((void) 0)
#define INC_HACK()       do {idx++; if (idx == hack_cnt) idx = 0;} while (0)

Epplet_gadget close_button, prev_button, next_button, zoom_button, draw_area;
unsigned long idx = 0, hack_cnt = 0;
double delay;
char **hacks = NULL;
unsigned char paused = 0;
pid_t hack_pid = -1;

static void start_hack(char *hack);
static void stop_hack(void);
static void change_hack(void *data);
static void child_cb(void *data, int pid, int exit_code);
static void close_cb(void *data);
static void zoom_cb(void *data);
static void play_cb(void *data);
static void in_cb(void *data, Window w);
static void out_cb(void *data, Window w);

static void
start_hack(char *hack) {

  char buff[1024];

  sprintf(buff, "%s -window-id %ld", hack, Epplet_get_drawingarea_window(draw_area));
  Epplet_register_child_handler(child_cb, NULL);
  hack_pid = Epplet_spawn_command(buff);
}

static void
stop_hack(void) {
  if (hack_pid != -1) {
    kill(hack_pid, SIGTERM);
  }
  hack_pid = -1;
}

static void
change_hack(void *data) {

  stop_hack();
  start_hack(hacks[idx]);
  INC_HACK();

  Epplet_timer(change_hack, NULL, delay, "CHANGE_HACK");
  return;
  data = NULL;
}

static void
child_cb(void *data, int pid, int exit_code) {

  if (pid == hack_pid) {
    fprintf(stderr, "E-Xss:  Child process exited with return code %d\n", exit_code);
    Epplet_remove_timer("CHANGE_HACK");
    Epplet_timer(change_hack, NULL, 0.1, "CHANGE_HACK");
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

  Epplet_spawn_command(hacks[CUR_HACK()]);

  return;
  data = NULL;
}

static void
play_cb(void *data) {

  int op = (int) data;

  switch (op) {
    case -1:
      /* Previous image */
      PREV_HACK();
      change_hack(NULL);
      break;
    case 1:
      /* Next image */
      NEXT_HACK();
      change_hack(NULL);
      break;
    default:
      break;
  }
  return;
  data = NULL;
}

static void
in_cb(void *data, Window w) {

  Epplet_gadget_hide(draw_area);
  Epplet_gadget_show(close_button);
  Epplet_gadget_show(zoom_button);
  Epplet_gadget_show(prev_button);
  Epplet_gadget_show(next_button);
  return;
  data = NULL;
  w = (Window) 0;
}

static void
out_cb(void *data, Window w) {

  Epplet_gadget_show(draw_area);
  Epplet_gadget_hide(close_button);
  Epplet_gadget_hide(zoom_button);
  Epplet_gadget_hide(prev_button);
  Epplet_gadget_hide(next_button);
  return;
  data = NULL;
  w = (Window) 0;
}

static void
parse_config(void) {

  delay = atof(Epplet_query_config_def("delay", "60.0"));
  hacks = (char **) malloc(sizeof(char *) * (hack_cnt + 1));
  for (; 1; hack_cnt++) {
    char *tmp;
    char buff[40];

    sprintf(buff, "hack_%lu", hack_cnt);
    tmp = Epplet_query_config(buff);
    if (!tmp) {
      break;
    }
    hacks = (char **) realloc(hacks, sizeof(char *) * (hack_cnt + 1));
    hacks[hack_cnt] = tmp;
  }
  if (hack_cnt == 0) {
    hack_cnt = 3;
    hacks = (char **) realloc(hacks, sizeof(char *) * (hack_cnt));
    hacks[0] = "kaleidescope";
    hacks[1] = "rorschach";
    hacks[2] = "qix -solid -delay 0 -segments 100";
    Epplet_add_config("hack_0", "kaleidescope");
    Epplet_add_config("hack_1", "strange");
    Epplet_add_config("hack_2", "qix -solid -delay 10 -segments 100");
    Epplet_add_config("hack_3", "demon");
    Epplet_add_config("hack_4", "bubbles");
    Epplet_add_config("hack_5", "xflame");
    Epplet_add_config("hack_6", "compass");
    Epplet_add_config("hack_7", "drift");
    Epplet_add_config("hack_8", "flow");
    Epplet_add_config("hack_9", "galaxy");
    Epplet_add_config("hack_10", "hypercube");
    Epplet_add_config("hack_11", "ifs");
    Epplet_add_config("hack_12", "kumppa -delay 20000");
    Epplet_add_config("hack_13", "laser");
    Epplet_add_config("hack_14", "swirl");
    Epplet_add_config("hack_15", "t3d");
    Epplet_add_config("hack_16", "triangle");
    Epplet_add_config("hack_17", "worm");
  }
}

int
main(int argc, char **argv) {

  int prio;

  prio = getpriority(PRIO_PROCESS, getpid());
  setpriority(PRIO_PROCESS, getpid(), prio + 10);
  atexit(Epplet_cleanup);
  Epplet_Init("E-Xss", "0.2", "Enlightenment Xscreensaver Epplet", 3, 3, argc, argv, 0);
  Epplet_load_config();
  parse_config();

  close_button = Epplet_create_button(NULL, NULL, 3, 3, 0, 0, "CLOSE", 0, NULL, close_cb, NULL);
  zoom_button = Epplet_create_button(NULL, NULL, 33, 3, 0, 0, "EJECT", 0, NULL, zoom_cb, NULL);
  prev_button = Epplet_create_button(NULL, NULL, 3, 33, 0, 0, "PREVIOUS", 0, NULL, play_cb, (void *) (-1));
  next_button = Epplet_create_button(NULL, NULL, 33, 33, 0, 0, "NEXT", 0, NULL, play_cb, (void *) (1));
  Epplet_gadget_show(prev_button);
  Epplet_gadget_show(next_button);
  draw_area = Epplet_create_drawingarea(3, 3, 43, 43);
  Epplet_gadget_show(draw_area);
  Epplet_show();

  Epplet_register_focus_in_handler(in_cb, NULL);
  Epplet_register_focus_out_handler(out_cb, NULL);
  change_hack(NULL);
  Epplet_Loop();

  return 0;
}
