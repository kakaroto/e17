#include "main.h"

static Evas_Object *o_text = NULL;

static void
_setup(void)
{
   Evas_Object *o;
   Evas_Textblock_Style *st;

   o = evas_object_textblock_add(evas);
   evas_object_move(o, 10, 40);
   evas_object_resize(o, win_w - 20, win_h - 50);
   evas_object_show(o);
   st = evas_textblock_style_new();
   evas_textblock_style_set
     (st,
      "DEFAULT='font=Vera font_size=8 align=left color=#000 wrap=word'"
      "center='+ font=Vera font_size=10 align=center'"
      "/center='- \n \n'"
      "p='+ font=Vera font_size=10 align=left'"
      "/p='- \n \n'"
      );
   evas_object_textblock_style_set(o, st);
   evas_textblock_style_free(st);
   evas_object_textblock_clear(o);
   evas_object_textblock_text_markup_set
     (o,
      "<center>"
      "Enlightenment used to be a window manager project, but "
      "since has changed a lot to become a miniature desktop and mobile "
      "device environment all of its own. It is now made up of many "
      "components (libraries and applications) that have specific uses. "
      "It is very large, and so requires more testing and demonstration."
      "</center>"

      "<center>"
      "Expedite is a full test suite for Evas, which is one of the "
      "core components of the Enlightenment Foundation Libraries. Evas "
      "handles the realtime display canvas used by EFL applications to "
      "render to many targets, including framebuffer, X11, OpenGL, memory, "
      "DirectFB and other targets to boot. It handles high level graphic "
      "layout descriptions that applications set up, dealing with the hard "
      "work of doing the drawing for them."
      "</center>"
      );
   o_text = o;
   ui_fps(0.0);
}

static void
_cleanup(void)
{
   evas_object_del(o_text);
   o_text = NULL;
}

static void
_loop(double t, int f)
{
}

static void
_key(char *key)
{
   if ((!strcmp(key, "Escape")) || (!strcmp(key, "q")) || (!strcmp(key, "Q")))
     {
	_cleanup();
	ui_menu();
     }
}

void
about_start(void)
{
   ui_func_set(_key, _loop);
   _setup();
}
