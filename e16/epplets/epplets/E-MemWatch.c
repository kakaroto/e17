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

Epplet_gadget close_button, mem_bar, swap_bar, mem_label, swap_label;
int mem_val = 0, swap_val = 0;

static void timer_cb(void *data);
static void close_cb(void *data);
static void in_cb(void *data, Window w);
static void out_cb(void *data, Window w);

static void
timer_cb(void *data) {

  FILE *fp;
  char buff[1024];
  unsigned long total, used, buffers, cached;

  if ((fp = fopen("/proc/meminfo", "r")) == NULL) {
    D(("Failed to open /proc/meminfo -- %s\n", strerror(errno)));
    return;
  }
  fgets(buff, sizeof(buff), fp);  /* Ignore the first line */
  fgets(buff, sizeof(buff), fp);
  sscanf(buff, "%*s %lu %lu %*u %*u %lu %lu", 
	 &total, &used, &buffers, &cached);
  used -= (buffers + cached); 
  mem_val = (int) ((((float) used) / total) * 100.0);
  D(("%d = 100 * %lu / %lu\n", (100 * used) / total, used, total));
  D(("Memory:  %d%% (%lu/%lu)\n", mem_val, used, total));
  Epplet_gadget_data_changed(mem_bar);
  if (used < 1024) {
    Esnprintf(buff, sizeof(buff), "M: %lub", used);
  } else if (used < 1024 * 1024) {
    Esnprintf(buff, sizeof(buff), "M: %luK", used / 1024);
  } else if (used < 1024 * 1024 * 1024) {
    Esnprintf(buff, sizeof(buff), "M: %luM", used / (1024 * 1024));
  } else {
    Esnprintf(buff, sizeof(buff), "M: %luG", used / (1024 * 1024 * 1024));
  }
  Epplet_change_label(mem_label, buff);

  fgets(buff, sizeof(buff), fp);
  sscanf(buff, "%*s %lu %lu", &total, &used);
  swap_val = (int) ((((float) used) / total) * 100.0);
  D(("Swap:  %d%% (%lu/%lu)\n", swap_val, used, total));
  Epplet_gadget_data_changed(swap_bar);
  if (used < 1024) {
    Esnprintf(buff, sizeof(buff), "S: %lub", used);
  } else if (used < 1024 * 1024) {
    Esnprintf(buff, sizeof(buff), "S: %luK", used / 1024);
  } else if (used < 1024 * 1024 * 1024) {
    Esnprintf(buff, sizeof(buff), "S: %luM", used / (1024 * 1024));
  } else {
    Esnprintf(buff, sizeof(buff), "S: %luG", used / (1024 * 1024 * 1024));
  }
  Epplet_change_label(swap_label, buff);

  fclose(fp);
  Esync();
  Epplet_timer(timer_cb, NULL, 3.0, "TIMER");
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
  Epplet_Init("E-MemWatch", "0.1", "Enlightenment RAM/Swap Monitor Epplet", 3, 3, argc, argv, 0);

  close_button = Epplet_create_button(NULL, NULL, 2, 2, 0, 0, "CLOSE", 0, NULL, close_cb, NULL);
  mem_label = Epplet_create_label(4, 4, "M:", 1);
  swap_label = Epplet_create_label(4, 24, "S:", 1);
  mem_bar = Epplet_create_hbar(4, 14, 40, 8, 0, &mem_val);
  swap_bar = Epplet_create_hbar(4, 36, 40, 8, 0, &swap_val);
  Epplet_gadget_show(mem_label);
  Epplet_gadget_show(mem_bar);
  Epplet_gadget_show(swap_label);
  Epplet_gadget_show(swap_bar);
  Epplet_show();

  Epplet_register_focus_in_handler(in_cb, NULL);
  Epplet_register_focus_out_handler(out_cb, NULL);
  timer_cb(NULL);  /* Set everything up */
  Epplet_Loop();

  return 0;
}
