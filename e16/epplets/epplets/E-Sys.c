#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include "epplet.h"

#ifdef HAVE_LIBGTOP
#include <glibtop.h>
#include <glibtop/uptime.h>
#include "proc.h"
#endif

#if 0
#  define D(x) do {printf("%10s | %7d:  [debug] ", __FILE__, __LINE__); printf x; fflush(stdout);} while (0)
#else
#  define D(x) ((void) 0)
#endif
#define BEGMATCH(a, b)  (!strncasecmp((a), (b), (sizeof(b) - 1)))
#define NONULL(x)       ((x) ? (x) : (""))

Epplet_gadget close_button, label1, label2, label3, label4;
int mem_val = 0, swap_val = 0;

static void timer_cb(void *data);
static void close_cb(void *data);
static void in_cb(void *data, Window w);
static void out_cb(void *data, Window w);

static void
timer_cb(void *data) {

  char buff[1024];
  unsigned long days, hours, mins, secs;
  double total_secs, delay;

#ifdef HAVE_LIBGTOP

  glibtop_uptime uptime;

  glibtop_get_uptime(&uptime);
  secs = (unsigned long)uptime.uptime;

#else

  FILE *fp;

  if ((fp = fopen("/proc/uptime", "r")) == NULL) {
    D(("Failed to open /proc/uptime -- %s\n", strerror(errno)));
    return;
  }
  fgets(buff, sizeof(buff), fp);
  sscanf(buff, "%lf", &total_secs);
  secs = (unsigned long) total_secs;
  fclose(fp);

#endif

  days = secs / 86400;
  secs %= 86400;
  hours = secs / 3600;
  secs %= 3600;
  mins = secs / 60;
  secs %= 60;
  if (secs == 0) {
    secs = 60;
  }
  delay = (double) secs;

  Esnprintf(buff, sizeof(buff), "%lu days", days);
  Epplet_change_label(label2, buff);
  Esnprintf(buff, sizeof(buff), "%lu hours", hours);
  Epplet_change_label(label3, buff);
  Esnprintf(buff, sizeof(buff), "%lu mins", mins);
  Epplet_change_label(label4, buff);

  Esync();
  Epplet_timer(timer_cb, NULL, delay, "TIMER");
  return;
  data = NULL;
}

static void
close_cb(void *data) {

  Epplet_unremember();
  Esync();
  exit(0);
  data = NULL;
}

static void
in_cb(void *data, Window w) {

  Epplet_gadget_show(close_button);
  return;
  data = NULL;
  w = (Window) 0;
}

static void
out_cb(void *data, Window w) {

  Epplet_gadget_hide(close_button);
  return;
  data = NULL;
  w = (Window) 0;
}

int
main(int argc, char **argv) {

  int prio;

  atexit(Epplet_cleanup);
  prio = getpriority(PRIO_PROCESS, getpid());
  setpriority(PRIO_PROCESS, getpid(), prio + 10);
  Epplet_Init("E-Sys", "0.1", "Enlightenment Uptime Epplet", 3, 3, argc, argv, 0);

  close_button = Epplet_create_button(NULL, NULL, 34, 2, 0, 0, "CLOSE", 0, NULL, close_cb, NULL);
  label1 = Epplet_create_label(4, 4, "Uptime", 1);
  label2 = Epplet_create_label(4, 16, "", 1);
  label3 = Epplet_create_label(4, 26, "", 1);
  label4 = Epplet_create_label(4, 36, "", 1);
  Epplet_gadget_show(label1);
  Epplet_gadget_show(label2);
  Epplet_gadget_show(label3);
  Epplet_show();

  Epplet_register_focus_in_handler(in_cb, NULL);
  Epplet_register_focus_out_handler(out_cb, NULL);
  timer_cb(NULL);  /* Set everything up */
  Epplet_Loop();

  return 0;
}
