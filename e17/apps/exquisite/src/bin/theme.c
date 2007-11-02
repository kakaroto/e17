#include "main.h"

static void _theme_cb_exit_done(void *data, Evas_Object *obj, const char *emission, const char *source);

static Evas_Object *o_bg = NULL;

void
theme_init(const char *theme)
{
   Evas_Object *o;
   char buf[4096];
   
   if (!theme) theme = "default";
   o = edje_object_add(evas);
   if ((theme) && 
       (theme[0] != '/') && 
       (!!strncmp(theme, "./", 2)) &&
	(!!strncmp(theme, "../", 3)))
     snprintf(buf, sizeof(buf), THEMES"/%s.edj", theme);
   else
     snprintf(buf, sizeof(buf), "%s", theme);
   edje_object_file_set(o, buf, "exquisite/theme");
   evas_object_move(o, 0, 0);
   evas_object_resize(o, scr_w, scr_h);
   edje_object_signal_callback_add(o, "exquisite", "done", _theme_cb_exit_done, NULL);
   evas_object_show(o);
   o_bg = o;
}

void
theme_shutdown(void)
{
   evas_object_del(o_bg);
   o_bg = NULL;
}

/* emit to theme to shut down - when done call callback func */
static void (*_theme_exit_func) (void *data) = NULL;
static void *theme_exit_data = NULL;
void
theme_exit(void (*func) (void *data), void *data)
{
   edje_object_signal_emit(o_bg, "exquisite", "exit");
   _theme_exit_func = func;
   theme_exit_data = data;
}

/* if theme_exit() was called - this aborts calling the callback at the end */
void
theme_exit_abort(void)
{
   _theme_exit_func = NULL;
   theme_exit_data = NULL;
}

/* set theme title text */
void
theme_title_set(const char *txt)
{
   if (edje_object_part_exists(o_bg, "exquisite.title"))
     {
	edje_object_part_text_set(o_bg, "exquisite.title", txt);
	edje_object_signal_emit(o_bg, "exquisite", "title");
     }
   else
     {
	Edje_Message_String m;
	m.str = txt;
	edje_object_message_send(o_bg, EDJE_MESSAGE_STRING, 0, &m);
     }
}

/* set theme title text */
void
theme_message_set(const char *txt)
{
   if (edje_object_part_exists(o_bg, "exquisite.message"))
     {
	edje_object_part_text_set(o_bg, "exquisite.message", txt);
	edje_object_signal_emit(o_bg, "exquisite", "message");
     }
   else
     {
	Edje_Message_String m;
	m.str = txt;
	edje_object_message_send(o_bg, EDJE_MESSAGE_STRING, 1, &m);
     }
}

/* set progress position 0.0 -> 1.0 */
void
theme_progress_set(double val)
{	
   if (edje_object_part_exists(o_bg, "exquisite.progress"))
     {
	edje_object_part_drag_value_set(o_bg, "exquisite.progress", val, val);
	edje_object_signal_emit(o_bg, "exquisite", "progress");
     }
   else
     {
	Edje_Message_Float m;
	m.val = val;
	edje_object_message_send(o_bg, EDJE_MESSAGE_FLOAT, 2, &m);
     }
}

/* input a tick into the theme (like a heartbeat - are things alive */
void
theme_tick(void)
{
   edje_object_signal_emit(o_bg, "exquisite", "tick");
}

/* Put into pulsate mode, Usplash command */
void
theme_pulsate(void)
{
   edje_object_signal_emit(o_bg, "exquisite", "pulsate");
}

static void
_theme_cb_exit_done(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   if (_theme_exit_func)
     {
	_theme_exit_func(theme_exit_data);
	_theme_exit_func = NULL;
	theme_exit_data = NULL;
     }
}
