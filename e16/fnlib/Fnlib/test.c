#include <Imlib.h>
#include <Fnlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>
#include <sys/time.h>
#include <time.h>

int
main(int argc, char **argv)
{
  Display            *disp;
  ImlibData          *id;
  FnlibData          *fd;
  FnlibFont          *fn;
  FnlibStyle          fs;
  Window              win;
  Pixmap              p;
  Pixmap              m;
  ImlibImage         *im;
  ImlibColorModifier  mod;
  int                 w, h, i, j, k, n, o, sz;
  double              t1, t2, total;
  struct timeval      timev;

  disp = XOpenDisplay(NULL);
  id = Imlib_init(disp);
  fd = Fnlib_init(id);
  if (!fd)
    {
      printf("Cannot init Fnlib\n");
      exit(1);
    }
  if (argc <= 2)
    {
      printf("Usage:\n test font_name pixel_size\n");
      exit(1);
    }
  sz = atoi(argv[2]);
  fn = Fnlib_load_font(fd, argv[1]);
  if (!fn)
    {
      printf("Cannot find font %s\n", argv[1]);
      exit(1);
    }
  win = XCreateSimpleWindow(disp, DefaultRootWindow(disp), 0, 0, 640, 480, 0, 0, 0);
  XClearWindow(disp, win);
  XMapWindow(disp, win);
  XSync(disp, False);
  fs.mode = MODE_WRAP_WORD;
  fs.orientation = FONT_TO_RIGHT;
  fs.justification = 0;
  fs.spacing = 0;

  for (i = 0; i < 1; i += 1)
    {
      usleep(100000);
      XClearWindow(disp, win);
      Fnlib_draw(fd, fn, win, 0, 20, 30, 600, 400, i, i / 2, sz, &fs,
		 "T11esting1 1 1   0123456789! \n"
		 "1234567890-=[]\\;',./~!@#$%^&*()_+{}|:\"<>?\n"
		 "My bum is bigger than yours because you are a complete whimp and so there bum you.\n"
		 "Pummel pummel. fuck me DEAD!@#%\n"
		 "Said 4-skin NED NED. So this is a test string to see how well this word and character wrapping works.\n"
		 "Blum\n"
		 "Shubbdhjfbhedfhjewfdwsdkbjnbgdskfvbvhjdshjbsdfdsfhgwerewuygfrhwfewfhewfnwdfhjnfjneygfnjegnfunfgfewuyuxe\n"
		 "Shub Brumby\n"
		 "Nim Nim Nim\n"
		 "Blumfrub\n");
      XSync(disp, False);
    }
  sleep(80);
  return 0;
}
