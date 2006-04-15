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

Epplet_gadget       da, b_close, b_help, b_config;
Epplet_gadget       gObjectPopupButton, gLightingPopupButton,
   gTexturingPopupButton;
Window              win;
Display            *dpy;

static GLfloat      gSpin = 0.0;
static GLuint       gObjectList, gTextureObject;
int                 gWhichRotate = 2, gLighting = 0, gTexturing = 0;

static void         cb_in(void *data, Window w);
static void         cb_out(void *data, Window w);
static void         cb_timer(void *data);
static void         cb_close(void *data);
static void         cb_help(void *data);
static void         cb_config(void *data);
static void         cb_set_object(void *data);
static void         cb_set_lighting(void *data);
static void         cb_set_texturing(void *data);
static void         save_conf(void);
static void         load_conf(void);
static void         setup_rotating_square(void);
static void         setup_rotating_cube(void);
static void         enable_lighting(void);
static void         disable_lighting(void);
static void         draw_rotating(void);

#define DEBUG 0
#define SQUARE 1
#define CUBE 2

#define OFF 0
#define ON 1

static int          object_type_table[] = {
   SQUARE, CUBE
};

static int          lighting_type_table[] = {
   OFF, ON
};

/* All setup_rotating_* fucntions compile our display lists. Since most
   of the data is static, there is no reason we need to redraw it over
	 and over */

static void
setup_rotating_square(void)
{
   GLfloat             x = 40.0;

   gObjectList = glGenLists(1);
   glNewList(gObjectList, GL_COMPILE);
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
   /* coordinates for out cube... since im using + and -, the cube
    * is actually 2*coords */
   static GLfloat      x = 32.0;
   static GLfloat      y = 32.0;
   static GLfloat      z = 32.0;

   /* These are our 6 faces * 4 vertexes per face = 24 vertex
    * coordinates. Its a bit messy, but its a hell of alot
    * better than 24 glVertex3f() commands. */
   GLfloat             cubeVerts[] = {
      -x, -y, z, -x, y, z, -x, y, -z, -x, -y, -z,	/* left */
      x, -y, z, x, -y, -z, x, y, -z, x, y, z,	/* right */
      -x, -y, z, x, -y, z, x, y, z, -x, y, z,	/* front */
      -x, -y, -z, -x, y, -z, x, y, -z, x, -y, -z,	/* back */
      -x, y, z, x, y, z, x, y, -z, -x, y, -z,	/* top */
      -x, -y, z, -x, -y, -z, x, -y, -z, x, -y, z
   };				/* bottom */

   static GLfloat      cubeNormals[] = {
      -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0,
      1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0,
      0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1,
      0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1,
      0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0,
      0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0
   };

   static GLfloat      cubeColors[] = {
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0,
      0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0,
      0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1,
      1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0,
      0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1
   };

   static GLfloat      cubeTexCords[] = {
      0, 0, 0, 1, 1, 1, 1, 0,
      0, 0, 0, 1, 1, 1, 1, 0,
      0, 0, 0, 1, 1, 1, 1, 0,
      0, 0, 0, 1, 1, 1, 1, 0,
      0, 0, 0, 1, 1, 1, 1, 0,
      0, 0, 0, 1, 1, 1, 1, 0
   };

   gObjectList = glGenLists(1);
   glNewList(gObjectList, GL_COMPILE);

   if (gTexturing == 1)
      glEnable(GL_TEXTURE_2D);
   else
      glDisable(GL_TEXTURE_2D);

   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
   glBindTexture(GL_TEXTURE_2D, gTextureObject);
   glEnable(GL_COLOR_MATERIAL);
   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_NORMAL_ARRAY);
   glEnableClientState(GL_COLOR_ARRAY);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   glVertexPointer(3, GL_FLOAT, 0, cubeVerts);
   glNormalPointer(GL_FLOAT, 0, cubeNormals);
   glColorPointer(3, GL_FLOAT, 0, cubeColors);
   glTexCoordPointer(2, GL_FLOAT, 0, cubeTexCords);
   glDrawArrays(GL_QUADS, 0, 24);
   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_NORMAL_ARRAY);
   glDisableClientState(GL_COLOR_ARRAY);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);

   glEndList();
}

static void
enable_lighting(void)
{
   GLfloat             mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat             mat_amb_diff[] = { .447, .243, .678 };
   GLfloat             mat_shininess[] = { 100.0 };
#if 0
   GLfloat             light_position[] = { 0.0, 0.0, 1.0, 0.0 };
#endif /* Unused */
   GLfloat             white_light[] = { 1.0, 1.0, 1.0, 1.0 };

   glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff);
   glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
   glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
   glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);
   glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);

   /* We enable the lights, but notice we dont set the position till later */
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
}

static void
disable_lighting(void)
{
   glDisable(GL_LIGHTING);
   glDisable(GL_LIGHT0);
}

/* These are our actual drawing functions */
static void
draw_rotating(void)
{
   GLfloat             light_position[] = { 0.0, 20.0, 200.0, 1.0 };

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glLoadIdentity();

   /* If we have lights on, set the light position */
   if (gLighting)
      glLightfv(GL_LIGHT0, GL_POSITION, light_position);

   glPushMatrix();
   glRotatef(gSpin, 1, 1, .1);
   glCallList(gObjectList);
   glPopMatrix();

   glXSwapBuffers(dpy, win);
}

static void
cb_timer(void *data)
{
   gSpin = gSpin + .5;
   if (gSpin > 360.0)
      gSpin = gSpin - 360.0;

   draw_rotating();
   Epplet_timer(cb_timer, NULL, 0, "TIMER");
   data = NULL;
}

static void
cb_close(void *data)
{
   GLXContext         *cx;

   cx = (GLXContext *) data;
   Epplet_unremember();
   Esync();
   if (cx != NULL)
      Epplet_unbind_GL(*cx);
   exit(0);
}

static void
cb_in(void *data, Window w)
{
   if (w == Epplet_get_main_window())
     {
	Epplet_gadget_show(b_close);
	Epplet_gadget_show(b_help);
	Epplet_gadget_show(b_config);
     }
   data = NULL;
}

static void
cb_out(void *data, Window w)
{
   if (w == Epplet_get_main_window())
     {
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
   Epplet_gadget_show(gObjectPopupButton);
   Epplet_gadget_show(gLightingPopupButton);
   Epplet_gadget_show(gTexturingPopupButton);
}

static void
cb_set_object(void *data)
{
   int                *newObject;

   glDeleteLists(gObjectList, 1);
   newObject = (int *)data;
   gWhichRotate = *newObject;
   save_conf();
   Epplet_gadget_hide(gObjectPopupButton);
   Epplet_gadget_hide(gLightingPopupButton);
   Epplet_gadget_hide(gTexturingPopupButton);

   if (gWhichRotate == SQUARE)
     {
	if (gLighting)
	  {
	     gLighting = OFF;
	     disable_lighting();
	  }
	setup_rotating_square();
     }
   else
      setup_rotating_cube();
}

static void
cb_set_lighting(void *data)
{
   int                *newLighting;

   newLighting = (int *)data;

   if (gLighting && !*(newLighting))
     {				/* Lighting was on, turn it off */
	gLighting = OFF;
	disable_lighting();
     }
   else if (!gLighting && *newLighting && (gWhichRotate = CUBE))
     {				/* Lighting was off, turn it on */
	gLighting = ON;
	enable_lighting();
     }

   save_conf();
   Epplet_gadget_hide(gObjectPopupButton);
   Epplet_gadget_hide(gLightingPopupButton);
   Epplet_gadget_hide(gTexturingPopupButton);
}

static void
cb_set_texturing(void *data)
{
   int                *newTexturing;

   newTexturing = (int *)data;

   if (gTexturing && !*(newTexturing))
     {				/* Texturing was on, turn it off */
	gTexturing = OFF;
	if (gWhichRotate == CUBE)
	   setup_rotating_cube();
     }
   else if (!gTexturing && *newTexturing && (gWhichRotate == CUBE))
     {				/* Texturing  was off, turn it on */
	gTexturing = ON;
	setup_rotating_cube();
     }

   save_conf();
   Epplet_gadget_hide(gObjectPopupButton);
   Epplet_gadget_hide(gLightingPopupButton);
   Epplet_gadget_hide(gTexturingPopupButton);
}

static void
save_conf(void)
{
   char                s[8];

   Esnprintf(s, sizeof(s), "%i", gWhichRotate);
   Epplet_modify_config("gWhichRotate", s);
   Esnprintf(s, sizeof(s), "%i", gLighting);
   Epplet_modify_config("gLighting", s);
   Esnprintf(s, sizeof(s), "%i", gTexturing);
   Epplet_modify_config("gTexturing", s);

   Epplet_save_config();
}

static void
load_conf(void)
{
   char               *str;

   str = Epplet_query_config_def("gWhichRotate", "2");
   sscanf(str, "%i", &gWhichRotate);
   str = Epplet_query_config_def("gLighting", "0");
   sscanf(str, "%i", &gLighting);
   str = Epplet_query_config_def("gTexturing", "0");
   sscanf(str, "%i", &gTexturing);
}

int
main(int argc, char **argv)
{
   char                buf[1024];

   GLXContext          cx;
   int                 prio;
   Epplet_gadget       objectPopup, lightingPopup, texturePopup;
   FILE               *textureFile;

   /* This has to be an unsignned byte, otherwise you'll loose the 8th
    * bit to the sign since the texture is 8 bits per color component. */
   GLubyte             textureArray[3 * 64 * 64];

   prio = getpriority(PRIO_PROCESS, getpid());
   setpriority(PRIO_PROCESS, getpid(), prio + 10);
   atexit(Epplet_cleanup);

   Epplet_Init("E-OpenGL-Demo", "0.1", "Enlightenment OpenGL Demo",
	       4, 4, argc, argv, 0);
   Epplet_load_config();
   Epplet_timer(cb_timer, NULL, 0, "TIMER");
   Epplet_gadget_show(da = Epplet_create_drawingarea(2, 2, 60, 60));

   /* We need win and dpy for the glXSwapBuffer */
   win = Epplet_get_drawingarea_window(da);
   dpy = Epplet_get_display();

   b_close = Epplet_create_button(NULL, NULL, 0, 0, 0, 0, "CLOSE", win, NULL,
				  cb_close, (void *)(&cx));
   b_help = Epplet_create_button(NULL, NULL, 14, 0, 0, 0, "HELP", win, NULL,
				 cb_help, NULL);
   b_config = Epplet_create_button(NULL, NULL, 28, 0, 0, 0, "CONFIGURE",
				   win, NULL, cb_config, NULL);

   objectPopup = Epplet_create_popup();
   Epplet_add_popup_entry(objectPopup, "Square", NULL, cb_set_object,
			  (void *)(&(object_type_table[0])));
   Epplet_add_popup_entry(objectPopup, "Cube", NULL, cb_set_object,
			  (void *)(&(object_type_table[1])));
   gObjectPopupButton = Epplet_create_popupbutton("Objects", NULL,
						  9, 16, 46, 14, NULL,
						  objectPopup);

   lightingPopup = Epplet_create_popup();
   Epplet_add_popup_entry(lightingPopup, "Off", NULL, cb_set_lighting,
			  (void *)(&(lighting_type_table[0])));
   Epplet_add_popup_entry(lightingPopup, "On", NULL, cb_set_lighting,
			  (void *)(&(lighting_type_table[1])));
   gLightingPopupButton = Epplet_create_popupbutton("Lights", NULL,
						    13, 31, 38, 14, NULL,
						    lightingPopup);

   texturePopup = Epplet_create_popup();
   Epplet_add_popup_entry(texturePopup, "Off", NULL, cb_set_texturing,
			  (void *)(&(lighting_type_table[0])));
   Epplet_add_popup_entry(texturePopup, "On", NULL, cb_set_texturing,
			  (void *)(&(lighting_type_table[1])));
   gTexturingPopupButton = Epplet_create_popupbutton("Textures", NULL,
						     9, 46, 46, 14, NULL,
						     texturePopup);

   Epplet_register_focus_in_handler(cb_in, NULL);
   Epplet_register_focus_out_handler(cb_out, NULL);

   load_conf();

   cx = Epplet_default_bind_GL(da);
   /* This could also be done as: 
    * cx = Epplet_bind_double_GL(da, 1, 1, 1, 0, 0, 8, 0, 0, 0, 0, 0); */

   if (cx == NULL)
      cb_close(NULL);

   /* To properly center the viewport, -2, -2 isntead of 0, 0 must be used.
    * Why? I have no freak'n idea. For some reason in Ortho everything is
    * coming ot 2 pixes shifted right and up. I'll look into it when I'm bored
    * or something */
   glViewport(-2, -2, 60, 60);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(-60.0, 60.0, -60.0, 60.0, -60.0, 60.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   glClearColor(0, 0, 0, 0);
   glShadeModel(GL_SMOOTH);
   glEnable(GL_DEPTH_TEST);

   /* Lets load teh texture */
   Esnprintf(buf, sizeof(buf), "%s/cube_texture.RGB", Epplet_data_dir());
   if ((textureFile = fopen(buf, "rb")) == NULL)
     {
	printf("Failed to load the cube texture file!\n");
     }
   else
     {
	fread(textureArray, sizeof(GLubyte), 3 * 64 * 64, textureFile);
	fclose(textureFile);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &gTextureObject);
	glBindTexture(GL_TEXTURE_2D, gTextureObject);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 64, 64, 0, GL_RGB,
		     GL_UNSIGNED_BYTE, textureArray);
     }

   Epplet_show();

   /* Sstup the default lighting */

   if (gLighting && gWhichRotate != SQUARE)
      enable_lighting();

   /* Compile the display lists */
   if (gWhichRotate == SQUARE)
      setup_rotating_square();
   else
      setup_rotating_cube();

   Epplet_Loop();
   return 0;
}
