#include <Elementary.h>
#include "elm_priv.h"

static void
_elm_win_name_set(Elm_Win *win, const char *name)
{
   if (win->name) evas_stringshare_del(win->name);
   win->name = evas_stringshare_add(win->name);
}

static void
_elm_win_title_set(Elm_Win *win, const char *title)
{
   if (win->title) evas_stringshare_del(win->title);
   win->title = evas_stringshare_add(title);
   if (win->ee) ecore_evas_title_set(win->ee, win->title);
}

static void
_elm_win_show(Elm_Win *win)
{
   ecore_evas_title_set(win->ee, win->title);
   ecore_evas_name_class_set(win->ee, win->name, "Elementary");
   ecore_evas_show(win->ee);
}

static void
_elm_win_hide(Elm_Win *win)
{
   ecore_evas_hide(win->ee);
}

EAPI Elm_Win *
elm_win_new(void)
{
   Elm_Win *win;
   
   win = ELM_NEW(Elm_Win);
   
   _elm_obj_init(win);
   
   // FIXME: override show and hide and del - need to del title/name
   
   win->name_set = _elm_win_name_set;
   win->title_set = _elm_win_title_set;
   win->show = _elm_win_show;
   win->hide = _elm_win_hide;
   
   win->type = ELM_WIN_BASIC;
   win->name = evas_stringshare_add("default"); 
   win->title = evas_stringshare_add("Elementary Window");

   // FIXME: create real window
   
   switch (_elm_engine)
     {
      case ELM_SOFTWARE_X11:
	win->ee = ecore_evas_software_x11_new(NULL, 0, 0, 0, 100, 100);
	break;
      case ELM_SOFTWARE_FB:
	win->ee = ecore_evas_fb_new(NULL, 0, 100, 100);
        ecore_evas_fullscreen_set(win->ee, 1);
	break;
      case ELM_SOFTWARE_16_X11:
	win->ee = ecore_evas_software_x11_16_new(NULL, 0, 0, 0, 100, 100);
	break;
      case ELM_XRENDER_X11:
	win->ee = ecore_evas_xrender_x11_new(NULL, 0, 0, 0, 100, 100);
	break;
      case ELM_OPENGL_X11:
	win->ee = ecore_evas_gl_x11_new(NULL, 0, 0, 0, 100, 100);
	break;
      default:
	break;
     }
   if (!win->ee)
     {
	// FIXME: dont return - clean up and unwind
	return win;
     }
   ecore_evas_title_set(win->ee, win->title);
   ecore_evas_name_class_set(win->ee, win->name, "Elementary");
   // FIXME: resize and delete callbacks
   win->evas = ecore_evas_get(win->ee);
   // FIXME: use elm config for this
   evas_image_cache_set(win->evas, 4096 * 1024);
   evas_font_cache_set(win->evas, 512 * 1024);
   evas_font_path_append(win->evas, "fonts");
   edje_frametime_set(1.0 / 30.0);
   
   return win;
}
