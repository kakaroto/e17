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

Epplet_gadget close_button, picture;
ImlibImage *im = NULL;
unsigned long idx = 0, image_cnt = 0;
double delay = 5.0;
char **filenames = NULL, *path;

static char **dirscan(char *dir, unsigned long *num);
static void change_image(void *data);
static void close_cb(void *data);
static void in_cb(void *data, Window w);
static void out_cb(void *data, Window w);

static char **
dirscan(char *dir, unsigned long *num)
{
  int i, dirlen;
  int done = 0;
  DIR *dirp;
  char **names;
  struct dirent *dp;

  D(("dirscan(\"%s\", %8p) called.\n", dir, num));

  if ((!dir) || (!*dir)) {
    return ((char **) NULL);
  }
  dirp = opendir(dir);
  if (!dirp) {
    *num = 0;
    return ((char **) NULL);
  }
  /* count # of entries in dir (worst case) */
  for (dirlen = 0; (dp = readdir(dirp)) != NULL; dirlen++);
  D((" -> Got %d entries.\n", dirlen));
  if (!dirlen) {
    closedir(dirp);
    *num = 0;
    return ((char **) NULL);
  }
  names = (char **) malloc(dirlen * sizeof(char *));
  D((" -> Storing names at %8p.\n", names));

  if (!names) {
    *num = 0;
    return ((char **) NULL);
  }

  rewinddir(dirp);
  for (i = 0; (dp = readdir(dirp)) != NULL;) {
    if ((strcmp(dp->d_name, ".")) && (strcmp(dp->d_name, ".."))) {
      D((" -> Adding name \"%s\" at index %d (%8p)\n", dp->d_name, i, names + i));
      names[i] = strdup(dp->d_name);
      i++;
    }
  }

  if (i < dirlen) {
    dirlen = i;
  }
  closedir(dirp);
  *num = dirlen;
  names = (char **) realloc(names, dirlen * sizeof(char *));
  D((" -> Final directory length is %lu.  List moved to %8p\n", *num, names));

  /* do a simple bubble sort here to alphanumberic it */
  while (!done) {
    done = 1;
    for (i = 0; i < dirlen - 1; i++) {
      if (strcmp(names[i], names[i + 1]) > 0) {
        char *tmp;

        tmp = names[i];
        names[i] = names[i + 1];
        names[i + 1] = tmp;
        done = 0;
      }
    }
  }
  return (names);
}

static void
change_image(void *data) {

  Epplet_change_image(picture, 42, 42, filenames[idx]);
  idx++;
  if (idx == image_cnt) {
    idx = 0;
  }

  Epplet_timer(change_image, NULL, delay, "TIMER");
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

static void
parse_config(void) {

  char buff[1024], *s;

  path = Epplet_query_config("image_dir");
  if (path == NULL) {
    sprintf(buff, "%s/.enlightenment/backgrounds", getenv("HOME"));
    path = strdup(buff);
    Epplet_add_config("image_dir", buff);
  }
  s = Epplet_query_config("delay");
  if (s != NULL) {
    delay = atof(s);
  } else {
    Epplet_add_config("delay", "5.0");
  }
}

int
main(int argc, char **argv) {

  int prio;

  prio = getpriority(PRIO_PROCESS, getpid());
  setpriority(PRIO_PROCESS, getpid(), prio + 10);
  atexit(Epplet_cleanup);
  Epplet_Init("E-Slides", "0.1", "Enlightenment Slideshow Epplet", 3, 3, argc, argv, 0);
  Epplet_load_config();
  parse_config();
  filenames = dirscan(path, &image_cnt);
  if (image_cnt == 0) {
    char err[255];

    sprintf(err, "Unable to find any files in %s, nothing to do!", path);
    Epplet_dialog_ok(err);
    exit(-1);
  }
  chdir(path);

  close_button = Epplet_create_button(NULL, NULL, 2, 2, 0, 0, "CLOSE", 0, NULL, close_cb, NULL);
  picture = Epplet_create_image(3, 3, 42, 42, "/dev/null");
  Epplet_show();

  Epplet_register_focus_in_handler(in_cb, NULL);
  Epplet_register_focus_out_handler(out_cb, NULL);
  change_image(NULL);  /* Set everything up */
  Epplet_Loop();

  return 0;
}
