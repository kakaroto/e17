/*
 * Copyright (C) 1999-2000, Wayde Milas
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <dirent.h>
#include <unistd.h>
#include "config.h"
#include "epplet.h"

Epplet_gadget da, b_close, b_help;
Window	win;
Display *dpy;
static GLfloat spin = 0.0;

static void cb_in(void *data, Window w);
static void cb_out(void *data, Window w);
static void cb_timer(void *data);
static void cb_close(void *data);
static void cb_help(void *data);
static void raw_rotating_square(void);

#define DEBUG 0
static void
draw_rotating_square(void)
{
	if(!DEBUG) { 
	glClear(GL_COLOR_BUFFER_BIT);
	glPushMatrix();
	glRotatef(spin, 0.0, 0.0, 1.0);
	glColor3f(1.0, 1.0, 1.0);
	glRectf(-25.0, -25.0, 25.0, 25.0);
	glPopMatrix();
	glXSwapBuffers(dpy,win);
	}
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
	data = NULL;
	exit(0);
}

static void
cb_in(void *data, Window w)
{
	if (w == Epplet_get_main_window()) {
		Epplet_gadget_show(b_close);
		Epplet_gadget_show(b_help);
	}
	data = NULL;
}

static void
cb_out(void *data, Window w)
{
	if (w == Epplet_get_main_window()) {
		Epplet_gadget_hide(b_close);
		Epplet_gadget_hide(b_help);
	}
	data = NULL;
}

static void
cb_help(void *data)
{
	data = NULL;
	Epplet_show_about("E-OpenGL-Demo");
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
		4, 4, argc, argv, 0);
	Epplet_timer(cb_timer, NULL, 0, "TIMER");
	Epplet_gadget_show(da = Epplet_create_drawingarea(2, 2, 60, 60));

	/* We need win and dpy for the glXSwapBuffer */
	win = Epplet_get_drawingarea_window(da);
	dpy=Epplet_get_display();

	b_close = Epplet_create_button(NULL, NULL, 0, 0, 0, 0, "CLOSE", win, NULL,
    cb_close, NULL);
	b_help = Epplet_create_button(NULL, NULL, 14, 0, 0, 0, "HELP", win, NULL,
		cb_help, NULL);
  Epplet_register_focus_in_handler(cb_in, NULL);
  Epplet_register_focus_out_handler(cb_out, NULL);

	cx = Epplet_bind_double_GL(da);

	glViewport (0, 0, (GLsizei) 60, (GLsizei) 60);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-50.0, 50.0, -50.0, 50.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
		
	glClearColor(0,0,0,0);
	glShadeModel(GL_FLAT);

	Epplet_show();
	draw_rotating_square();
	
	Epplet_Loop();
	return 0;
}
