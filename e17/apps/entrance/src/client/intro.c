#include "entrance.h"
#include "entrance_config.h"

#define PERCENT_DESC_HORIZONTAL_PLACEMENT 0.5
#define PERCENT_DESC_VERTICAL_PLACEMENT 0.8

static Evas *evas = NULL;
static Evas_Object *_o_text_desc = NULL;
static Evas_Object *_o_pass_desc = NULL;
static Evas_Object *_o_text_entry = NULL;
static Evas_Object *_o_err_str = NULL;
static Entrance_Config config = NULL;
static Entrance_Theme theme;
static Entrance_Session s;

void
show_password_description(void)
{
   evas_object_show(_o_pass_desc);
}

void
hide_password_description(void)
{
   evas_object_hide(_o_pass_desc);
}

void
show_text_description(void)
{
   evas_object_show(_o_text_desc);
}

void
hide_text_description(void)
{
   evas_object_hide(_o_text_desc);
}

static void
error_die(int val, void *_data)
{
   evas_object_show(_o_text_desc);
   evas_object_hide(_o_err_str);
}

void
show_error_description(char *err_str)
{
   int w, h;
   double tw, th;
   Evas_Object *o = _o_err_str;

   if (err_str)
   {
      evas_output_size_get(evas, &w, &h);
      evas_object_text_text_set(_o_err_str, err_str);

      evas_object_geometry_get(o, NULL, NULL, &tw, &th);

      /* FIXME: This should have its own theme config as well, just to be
         fair */
      THEME_MOVE(_o_err_str, theme->welcome, s->geom);
      evas_object_hide(_o_pass_desc);
      evas_object_hide(_o_text_desc);
      evas_object_show(_o_err_str);
      ecore_add_event_timer("error_string", 1.6, error_die, 255, o);
   }
}

void
set_text_entry_text(int is_pass, char *txt)
{
   Evas_Object *o = _o_text_entry;
   char buf[PATH_MAX] = "";
   int i = 0;
   int w, margin;

   for (i = 0; i < PATH_MAX && i < strlen(txt); i++)
      strncat(buf, "*", 1);

   if (is_pass)
      if (config->passwd_echo)
         evas_object_text_text_set(o, buf);
      else
         evas_object_text_text_set(o, "");
   else
      evas_object_text_text_set(o, txt);

   margin =
      ((int) theme->entry.pos.x * s->geom.w + theme->entry.offset.x) -
      ((int) theme->entry.box.pos.x * s->geom.w + theme->entry.box.offset.x);
   w = theme->entry.box.size.w - (2 * margin);

   evas_object_resize(o, w, theme->entry.box.size.h);

   /* x = (((w / config->screens.w) - tw) *
      PERCENT_DESC_HORIZONTAL_PLACEMENT); y = (((h / config->screens.h) - th) 
      * PERCENT_DESC_VERTICAL_PLACEMENT + (config->welcome.font.size + 8)); */


}

void
intro_init(Entrance_Session e)
{
   Evas_Object *o;
   double tw, th;

   if (e->evas)
      evas = e->evas;
   if (evas)
   {
      if (_o_text_desc)
         evas_object_del(_o_text_desc);
      if (_o_pass_desc)
         evas_object_del(_o_pass_desc);
      if (_o_text_entry)
         evas_object_del(_o_text_entry);
      if (_o_err_str)
         evas_object_del(_o_err_str);
   }
   if (e)
      evas = e->evas;
   else
   {
      syslog(LOG_CRIT, "Evas is NULL yo.");
      exit(1);
   }
   if (!config)
      config = e->config;

   theme = e->theme;
   s = e;

   o = evas_object_text_add(evas);
   evas_object_text_text_set(o, config->welcome);
   THEME_SETFONT(o, e->theme->welcome);
   THEME_SETCOLOR(o, e->theme->welcome.color);
   evas_object_geometry_get(o, NULL, NULL, &tw, &th);
/*   x = (((e->geom.w / config->screens.w) -
         tw) * PERCENT_DESC_HORIZONTAL_PLACEMENT);
   y = (((e->geom.h / config->screens.h) -
         th) * PERCENT_DESC_VERTICAL_PLACEMENT);*/
   THEME_MOVE(o, e->theme->welcome, e->geom);
   evas_object_layer_set(o, 10);
   evas_object_show(o);
   _o_text_desc = o;

   /* password description */
   o = evas_object_text_add(evas);
   evas_object_text_text_set(o, config->passwd);
   THEME_SETFONT(o, e->theme->password);
   THEME_SETCOLOR(o, e->theme->password.color);
   evas_object_geometry_get(o, NULL, NULL, &tw, &th);
/*   x = (((e->geom.w / config->screens.w) -
         tw) * PERCENT_DESC_HORIZONTAL_PLACEMENT);
   y = (((e->geom.h / config->screens.h) -
         th) * PERCENT_DESC_VERTICAL_PLACEMENT);*/
   THEME_MOVE(o, e->theme->welcome, e->geom);
   evas_object_layer_set(o, 10);
   _o_pass_desc = o;

   /* text entry box */
   e->entrybox = evas_object_rectangle_add(evas);
   THEME_MOVE(e->entrybox, e->theme->entry.box, e->geom);
   THEME_SETCOLOR(e->entrybox, e->theme->entry.box.color);
   THEME_RESIZE(e->entrybox, e->theme->entry.box);
   evas_object_layer_set(e->entrybox, 12);
   evas_object_show(e->entrybox);

   /* o_text_entry */
   o = evas_object_text_add(evas);
   THEME_SETFONT(o, e->theme->entry);
   evas_object_text_text_set(o, "");
   THEME_SETCOLOR(o, e->theme->entry.color);
   THEME_MOVE(o, e->theme->entry, e->geom);
   evas_object_layer_set(o, 15);
   evas_object_show(o);
   _o_text_entry = o;

   /* _o_err_str is placed later */
   o = evas_object_text_add(evas);
   evas_object_text_text_set(o, "");
   THEME_SETFONT(o, e->theme->welcome);
   THEME_SETCOLOR(o, e->theme->welcome.color);
   evas_object_layer_set(o, 10);
   _o_err_str = o;
}
