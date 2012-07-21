
#include <Exquisite.h>

#include "main.h"

static Evas_Object *o_bg = NULL;
static int last_text_id = -1;

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
     snprintf(buf, sizeof(buf), "%s/%s.edj", eina_prefix_data_get(pfx), theme);
   else
     snprintf(buf, sizeof(buf), "%s", theme);
   o = exquisite_object_add (evas, buf);
   evas_object_move(o, 0, 0);
   evas_object_resize(o, scr_w, scr_h);
   evas_object_show(o);
   o_bg = o;
}

void
theme_shutdown(void)
{
   evas_object_del(o_bg);
   o_bg = NULL;
}

void
theme_resize(void)
{
   evas_object_resize(o_bg, scr_w, scr_h);
}

/* emit to theme to shut down - when done call callback func */
void
theme_exit(void (*func) (void *data), void *data)
{
   quitting = 1;
   exquisite_object_exit_callback_set (o_bg, func, data);
   exquisite_object_exit (o_bg);
}

/* if theme_exit() was called - this aborts calling the callback at the end */
void
theme_exit_abort(void)
{
  exquisite_object_exit_callback_set(o_bg, NULL, NULL);
}

/* set theme title text */
void
theme_title_set(const char *txt)
{
  exquisite_object_title_set(o_bg, txt);
}

/* set theme title text */
void
theme_message_set(const char *txt)
{
  exquisite_object_message_set(o_bg, txt);
}

/* set progress position 0.0 -> 1.0 */
void
theme_progress_set(double val)
{
  exquisite_object_progress_set(o_bg, val);
}

/* input a tick into the theme (like a heartbeat - are things alive */
void
theme_tick(void)
{
  exquisite_object_tick(o_bg);
}

/* Put into pulsate mode, Usplash command */
void
theme_pulsate(void)
{
  exquisite_object_pulsate(o_bg);
}

void
theme_text_add(const char *txt)
{
  last_text_id = exquisite_object_text_add(o_bg, txt);
}

void
theme_status_set(const char *txt, int type)
{
  exquisite_object_status_set(o_bg, last_text_id, txt, type);
}

void
theme_text_clear()
{
  exquisite_object_text_clear(o_bg);
}
