/* A very eraly demo that outlines how to use OpenGL in Epplets */
#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <dirent.h>
#include <unistd.h>
#include "config.h"
#include "epplet.h"
#include <GL/glut.h>
#include <stdlib.h>
#include <GL/glx.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

Epplet_gadget da;
Window	win;
Display *dpy;
static GLfloat spin = 0.0;

static void cb_timer(void *data);
static void cb_close(void *data);

static void
draw_rotating_square()
{
  glClear(GL_COLOR_BUFFER_BIT);
  glPushMatrix();
  glRotatef(spin, 0.0, 0.0, 1.0);
  glColor3f(1.0, 1.0, 1.0);
  glRectf(-25.0, -25.0, 25.0, 25.0);
  glPopMatrix();
  glXSwapBuffers(dpy,win);
}

static void
cb_timer(void *data)
{
	 spin = spin + 10.0;
	 if (spin > 360.0)
	 	spin = spin - 360.0;
	
	 draw_rotating_square();
   Epplet_timer(cb_timer, NULL, 0, "TIMER");   
   data = NULL;
}

static void
cb_close(void *data)
{
   Epplet_unremember();
   Esync();
   exit(0);
   data = NULL;
}

int
main(int argc, char **argv)
{
	 GLXContext cx;
   int prio;

   prio = getpriority(PRIO_PROCESS, getpid());
   setpriority(PRIO_PROCESS, getpid(), prio + 10);
   atexit(Epplet_cleanup);
   
   Epplet_Init("E-OpenGL-Demo", "0.1", "Enlightenment OpenGL Demo",
	       4, 5, argc, argv, 0);
   Epplet_timer(cb_timer, NULL, 0, "TIMER");

	 Epplet_gadget_show(da = Epplet_create_drawingarea(2, 2, 60, 65));

	 /* We need win and dpy for the glXSwapBuffer */
	 win = Epplet_get_drawingarea_window(da);
	 dpy=Epplet_get_display();

	 cx = Epplet_bind_double_GL(da);

	 glViewport (0, 0, (GLsizei) 60, (GLsizei) 65);
	 glMatrixMode(GL_PROJECTION);
	 glLoadIdentity();
	 glOrtho(-50.0, 50.0, -50.0, 50.0, -1.0, 1.0);
	 glMatrixMode(GL_MODELVIEW);
	 glLoadIdentity();
									
	 glClearColor(0,0,0,0);
	 glShadeModel(GL_FLAT);

	 Epplet_show();
	 draw_rotating_square();
	
	 /* sleep(20); */
   Epplet_Loop();
   return 0;
}
