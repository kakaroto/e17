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

Epplet_gadget da, b_close, b_help, b_config, pop;
Window	win;
Display *dpy;
static GLfloat spin = 0.0;
GLuint squareList, cubeList;
int	whichRotate=1;

static void cb_in(void *data, Window w);
static void cb_out(void *data, Window w);
static void cb_timer(void *data);
static void cb_close(void *data);
static void cb_help(void *data);
static void cb_config(void *data);
static void cb_set_object(void *data);
static void setup_rotating_square(void);
static void setup_rotating_cube(void);
static void draw_rotating(void);

#define DEBUG 0
#define SQUARE 1
#define CUBE 2

static int object_type_table[] =
{
	1, 2
};

/* All setup_rotating_* fucntions compile our display lists. Since most
   of the data is static, there is no reason we need to redraw it over
	 and over */

static void
setup_rotating_square(void)
{
	GLfloat x=40.0;

	squareList = glGenLists(1);
	glNewList(squareList, GL_COMPILE);
		glBegin(GL_QUADS);
			glColor3f(1.0, 0, 0);
			glVertex3f(-x, -x, 0);
			glColor3f(0, 1.0, 0);
			glVertex3f(-x, x, 0);
			glColor3f(0, 0, 1.0);
			glVertex3f(x, x, 0);
			glColor3f(1, 0, 1);
			glVertex3f(x, -x, 0);
		glEnd();
		glPopMatrix();
	glEndList();
}

static void
setup_rotating_cube(void)
{
  GLfloat x=30.0;
	GLfloat y=30.0;
	GLfloat z=30.0;

	cubeList = glGenLists(1);
	glNewList(cubeList, GL_COMPILE);
	glColor3f(.447, .243, .678);
	glBegin(GL_QUADS);
	// Face 1 bottom
		glVertex3f(-x, -y, -z);
		glVertex3f(-x, y, -z);
		glVertex3f(x, y, -z);
    glVertex3f(x, -y, -z);

	// Face 2 top
    glVertex3f(-x, -y, z);
    glVertex3f(-x, y, z);
    glVertex3f(x, y, z);
    glVertex3f(x, -y, z);

	// Face 3 back
    glVertex3f(-x, y, z);
    glVertex3f(x, y, z);
    glVertex3f(x, y, -z);
    glVertex3f(-x, y, -z);

	// Face 4 front
    glVertex3f(-x, -y, z);
    glVertex3f(x, -y, z);
    glVertex3f(x, -y, -z);
    glVertex3f(-x, -y, -z);

	// Face 5 left
    glVertex3f(-x, -y, z);
    glVertex3f(-x, y, z);
    glVertex3f(-x, y, -z);
    glVertex3f(-x, -y, -z);

	// Face 6 right
    glVertex3f(x, -y, z);
    glVertex3f(x, y, z);
    glVertex3f(x, y, -z);
    glVertex3f(x, -y, -z);

	glEnd();
	glPopMatrix();
	glEndList();
}

/* These are our actual drawing functions */
static void
draw_rotating(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glPushMatrix();
	glRotatef(spin, .5, 1, .5);
	switch(whichRotate) {
	case SQUARE:
		glCallList(squareList);
		break;
	case CUBE:
		glCallList(cubeList);
		break;
	}
  glXSwapBuffers(dpy,win);
}

static void
cb_timer(void *data)
{
	spin = spin +.5;
	if (spin > 360.0)
		spin = spin - 360.0;

	draw_rotating();
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
		Epplet_gadget_show(b_config);
	}
	data = NULL;
}

static void
cb_out(void *data, Window w)
{
	if (w == Epplet_get_main_window()) {
		Epplet_gadget_hide(b_close);
		Epplet_gadget_hide(b_help);
		Epplet_gadget_hide(b_config);
	}
	data = NULL;
}

static void
cb_help(void *data)
{
	data = NULL;
	Epplet_show_about("E-OpenGL-Demo");
}

static void
cb_config(void *data)
{
	data = NULL;
	Epplet_gadget_show(pop);
}

static void
cb_set_object(void *data)
{
	int *d;

	d = (int *)data;
	whichRotate = *d;
	Epplet_gadget_hide(pop);
}

int
main(int argc, char **argv)
{
	GLXContext cx;
	int prio, p_type;
	Epplet_gadget p;

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
	b_config = Epplet_create_button(NULL, NULL, 28, 0, 0, 0, "CONFIGURE",
		win, NULL, cb_config, NULL);

	p = Epplet_create_popup();
	Epplet_add_popup_entry(p, "Square", NULL, cb_set_object, 
		(void *)(&(object_type_table[0])));
	Epplet_add_popup_entry(p, "Cube", NULL, cb_set_object,
		(void *)(&(object_type_table[1])));
	pop = Epplet_create_popupbutton("Objects", NULL, 6, 24, 36, 12, NULL, p);
  Epplet_register_focus_in_handler(cb_in, NULL);
  Epplet_register_focus_out_handler(cb_out, NULL);

	cx = Epplet_bind_double_GL(da);

	/* To properly center the viewport, -2, -2 isntead of 0, 0 must be used.
	Why? I have no freak'n idea. For some reason in Ortho everything is
	coming ot 2 pixes shifted right and up. I'll look into it when I'm bored
	or something */
	glViewport (-2, -2, 60, 60);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-60.0, 60.0, -60.0, 60.0, -60.0, 60.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
		
	glClearColor(0,0,0,0);
	glShadeModel(GL_SMOOTH);

	Epplet_show();

	/* Compile the display lists */
	setup_rotating_square();
	setup_rotating_cube();
	
	Epplet_Loop();
	return 0;
}
