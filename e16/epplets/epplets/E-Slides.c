#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <dirent.h>
#include <unistd.h>
#include "epplet.h"

#if 0
#  define D(x) do {printf("%10s | %7d:  [debug] ", __FILE__, __LINE__); printf x; fflush(stdout);} while (0)
#else
#  define D(x) ((void) 0)
#endif
#define BEGMATCH(a, b)  (!strncasecmp((a), (b), (sizeof(b) - 1)))
#define NONULL(x)       ((x) ? (x) : (""))

#define PREV_PIC()      do {if (idx == 1) idx = image_cnt - 1; else if (idx == 0) idx = image_cnt - 2; else idx -= 2;} while (0)
#define CUR_PIC()       ((idx == 0) ? (image_cnt - 1) : (idx - 1))
#define NEXT_PIC()      ((void) 0)
#define INC_PIC()       do {idx++; if (idx == image_cnt) idx = 0;} while (0)

Epplet_gadget close_button, play_button, pause_button, prev_button, next_button, zoom_button, picture;
unsigned long idx = 0, image_cnt = 0;
double delay = 5.0;
char **filenames = NULL, *path, *zoom_cmd;
unsigned char paused = 0;
Window zoom_win = None;
int w = 3, h = 3;

static char **dirscan(char *dir, unsigned long *num);
static void change_image(void *data);
static void close_cb(void *data);
static void zoom_cb(void *data);
static void play_cb(void *data);
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
  struct stat filestat;
  char fullname[256];

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
      Esnprintf(fullname, sizeof(fullname), "%s/%s", dir, dp->d_name);
      D((" -> About to stat() %s\n", fullname));
      if (stat(fullname, &filestat)) {
        D((" -> Couldn't stat() file %s -- %s\n", dp->d_name, strerror(errno)));
      } else {
        if (S_ISREG(filestat.st_mode)) {
          D((" -> Adding name \"%s\" at index %d (%8p)\n", dp->d_name, i, names + i));
          names[i] = strdup(dp->d_name);
          i++;
        } else if (S_ISDIR(filestat.st_mode)) {
          /* Recurse directories here at some point, maybe? */
        }
      }
    }
  }

  if (i < dirlen) {
    dirlen = i;
  }

  if (!dirlen) {
    closedir(dirp);
    *num = 0;
    return ((char **) NULL);
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

  ImlibImage *im = NULL;

  /* Test-load each image to make sure it's a valid image file. */
  for (; ((filenames[idx] == NULL) || ((im = Imlib_load_image(Epplet_get_imlib_data(), filenames[idx])) == NULL));) {
    /* It isn't, so NULL out its name. */
    filenames[idx] = NULL;
    INC_PIC();
  }
  Imlib_destroy_image(Epplet_get_imlib_data(), im);  /* Destroy the image, but keep it in cache. */

  Epplet_change_image(picture, (w * 16 - 6), (h * 16 - 6), filenames[idx]);
  INC_PIC();

  Epplet_remove_timer("CHANGE_IMAGE");
  if (!paused) {
    Epplet_timer(change_image, NULL, delay, "CHANGE_IMAGE");
  }
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
zoom_cb(void *data) {

  char buff[1024];

  Esnprintf(buff, sizeof(buff), zoom_cmd, filenames[CUR_PIC()]);
  Epplet_spawn_command(buff);
  return;
  data = NULL;
}

static void
play_cb(void *data) {

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
  return;
}

static void
in_cb(void *data, Window w) {

  Epplet_gadget_show(close_button);
  Epplet_gadget_show(zoom_button);
  Epplet_gadget_show(prev_button);
  Epplet_gadget_show(next_button);
  if (paused) {
    Epplet_gadget_show(play_button);
  } else {
    Epplet_gadget_show(pause_button);
  }
  return;
  data = NULL;
  w = (Window) 0;
}

static void
out_cb(void *data, Window w) {

  Epplet_gadget_hide(close_button);
  Epplet_gadget_hide(zoom_button);
  Epplet_gadget_hide(prev_button);
  Epplet_gadget_hide(next_button);
  Epplet_gadget_hide(play_button);
  Epplet_gadget_hide(pause_button);
  return;
  data = NULL;
  w = (Window) 0;
}

static void
parse_config(void) {

  char buff[1024], *s;

  path = Epplet_query_config("image_dir");
  if (path == NULL) {
    Esnprintf(buff, sizeof(buff), "%s/.enlightenment/backgrounds", getenv("HOME"));
    path = strdup(buff);
    Epplet_add_config("image_dir", buff);
  }
  s = Epplet_query_config("delay");
  if (s != NULL) {
    delay = atof(s);
  } else {
    Epplet_add_config("delay", "5.0");
  }
  zoom_cmd = Epplet_query_config_def("zoom_prog", "ee %s");
}

int
main(int argc, char **argv) {

  int prio, j = 0;

  prio = getpriority(PRIO_PROCESS, getpid());
  setpriority(PRIO_PROCESS, getpid(), prio + 10);
  atexit(Epplet_cleanup);

  for (j = 1; j < argc; j++) {
    if ((!strcmp("-w", argv[j])) && (argc - j > 1)) {
      w = atoi(argv[++j]);
      if (w < 3) {
        w = 3;
      }
    } else if ((!strcmp("-h", argv[j])) && (argc - j > 1)) {
      h = atoi(argv[++j]);
      if (h < 3) {
        h = 3;
      }
    }
  }

  Epplet_Init("E-Slides", "0.3", "Enlightenment Slideshow Epplet", w, h, argc, argv, 0);
  Epplet_load_config();
  parse_config();
  filenames = dirscan(path, &image_cnt);
  if (image_cnt == 0) {
    char err[255];

    Esnprintf(err, sizeof(err), "Unable to find any files in %s, nothing to do!", path);
    Epplet_dialog_ok(err);
    Esync();
    exit(-1);
  }
  chdir(path);
  
  close_button = Epplet_create_button(NULL, NULL, 3, 3, 0, 0, "CLOSE", 0, NULL, close_cb, NULL);
  zoom_button = Epplet_create_button(NULL, NULL, ((16 * w) - 15), 3, 0, 0, "EJECT", 0, NULL, zoom_cb, NULL);
  prev_button = Epplet_create_button(NULL, NULL, 3, ((16 * h) - 15), 0, 0, "PREVIOUS", 0, NULL, play_cb, (void *) (-1));
  play_button = Epplet_create_button(NULL, NULL, ((16 * w / 2) - 6), ((16 * h) - 15), 0, 0, "PLAY", 0, NULL, play_cb, (void *) (1));
  pause_button = Epplet_create_button(NULL, NULL, ((16 * w / 2) - 6), ((16 * h) - 15), 0, 0, "PAUSE", 0, NULL, play_cb, (void *) (0));
  next_button = Epplet_create_button(NULL, NULL, ((16 * w) - 15), ((16 * h) - 15), 0, 0, "NEXT", 0, NULL, play_cb, (void *) (2));
  picture = Epplet_create_image(3, 3, ((w * 16) - 6), ((h * 16) - 6), "/dev/null");
  Epplet_show();
  Epplet_register_focus_in_handler(in_cb, NULL);
  Epplet_register_focus_out_handler(out_cb, NULL);
  
  change_image(NULL);  /* Set everything up */
  Epplet_Loop();

  return 0;
}
