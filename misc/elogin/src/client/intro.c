#include "elogin.h"
#include "e_login_config.h"
#include "callbacks.h"
#include <Estyle.h>
#include "session_bar.h"

static Evas *evas = NULL;
static E_Login_Config config = NULL;
static Estyle *_o_err_str = NULL;
static Estyle *_o_text_desc = NULL;
static Estyle *_o_pass_desc = NULL;
static Estyle *_o_text_entry = NULL;

void
show_password_description(void)
{
   estyle_show(_o_pass_desc);
}

void
hide_password_description(void)
{
   estyle_hide(_o_pass_desc);
}

void
show_text_description(void)
{
   estyle_show(_o_text_desc);
}

void
hide_text_description(void)
{
   estyle_hide(_o_text_desc);
}

static void
error_die(int val, void *_data)
{
   estyle_show(_o_text_desc);
   estyle_hide(_o_err_str);
}

void
show_error_description(char *err_str)
{
   int x, y;
   int w, h;
   double tw, th;
   Estyle *o = _o_err_str;

   if (err_str)
   {
      evas_output_size_get(evas, &w, &h);
      estyle_set_text(o, err_str);

      estyle_geometry(o, NULL, NULL, (int *) &tw, (int *) &th);
      x = ((((w / config->screens.w) - tw) * config->welcome.pos.x) +
           config->welcome.pos.offset_x);
      y = ((((h / config->screens.h) - th) * config->welcome.pos.y) +
           config->welcome.pos.offset_y);

      estyle_move(o, x, y);
      estyle_hide(_o_pass_desc);
      estyle_hide(_o_text_desc);
      estyle_show(_o_err_str);
      ecore_add_event_timer("error_string", 1.5, error_die, 255, o);
   }
}

void
set_text_entry_text(int is_pass, char *txt)
{
   Estyle *o = _o_text_entry;

   int x = 0, y = 0;
   int w, h, tw = 0, th = 0;

   if (is_pass)
      estyle_set_text(o, "");
   else
      estyle_set_text(o, txt);

   evas_output_size_get(evas, &w, &h);
   estyle_geometry(o, NULL, NULL, &tw, &th);

   x = ((((w / config->screens.w) - tw) * config->passwd.pos.x) +
        config->passwd.pos.offset_x);
   y = ((((h / config->screens.h) - th) * config->passwd.pos.y) +
        config->passwd.pos.offset_y + th);
   estyle_move(o, x, y);
}

void
elogin_update_time(int val, void *data)
{
   char buf[PATH_MAX], timer[PATH_MAX];

   if (data)
   {
      struct tm *current;
      time_t _t = time(NULL);

      current = localtime(&_t);
      if (val)
         strftime(buf, PATH_MAX, "%l:%M %Z", current);
      else
         strftime(buf, PATH_MAX, "%b %e %Y", current);
      estyle_set_text((Estyle *) data, buf);
      snprintf(timer, PATH_MAX, "%s timer", buf);
      ecore_add_event_timer(timer, 1.0, elogin_update_time, val, data);
   }
}

void
intro_init(E_Login_Session e)
{
   Estyle *es;
   int tw = 0, th = 0;
   double x, y;

   if (e->evas)
      evas = e->evas;
   if (evas)
   {
      if (_o_text_desc)
         estyle_free(_o_text_desc);
      if (_o_pass_desc)
         estyle_free(_o_pass_desc);
      if (_o_text_entry)
         estyle_free(_o_text_entry);
      if (_o_err_str)
         estyle_free(_o_err_str);
   }
   if (e)
      evas = e->evas;
   else
   {
      fprintf(stderr, "Evas is NULL yo\n");
      exit(1);
   }
   if (!config)
      config = e->config;



   es = estyle_new(evas, config->welcome.mess, "raised");
   estyle_set_color(es, config->welcome.font.r, config->welcome.font.g,
                    config->welcome.font.b, config->welcome.font.a);
   estyle_set_font(es, config->welcome.font.name, config->welcome.font.size);
   estyle_geometry(es, NULL, NULL, &tw, &th);
   x = ((((e->geom.w / config->screens.w) - tw) * config->welcome.pos.x) +
        config->welcome.pos.offset_x);
   y = ((((e->geom.h / config->screens.h) - th) * config->welcome.pos.y) +
        config->welcome.pos.offset_y);
   estyle_move(es, x, y);
   estyle_set_layer(es, 5);
   estyle_show(es);
   _o_text_desc = es;

   /* password description */
   es = estyle_new(evas, config->passwd.mess, "raised");
   estyle_set_color(es, config->passwd.font.r, config->passwd.font.g,
                    config->passwd.font.b, config->passwd.font.a);
   estyle_set_font(es, config->passwd.font.name, config->passwd.font.size);
   estyle_geometry(es, NULL, NULL, &tw, &th);
   x = ((((e->geom.w / config->screens.w) - tw) * config->passwd.pos.x) +
        config->passwd.pos.offset_x);
   y = ((((e->geom.h / config->screens.h) - th) * config->passwd.pos.y) +
        config->passwd.pos.offset_y);
   estyle_move(es, x, y);
   estyle_set_layer(es, 5);
   estyle_hide(es);
   _o_pass_desc = es;

   /* o_text_entry */
   es = estyle_new(evas, "", "raised");
   estyle_set_color(es, config->welcome.font.r, config->welcome.font.g,
                    config->welcome.font.b, config->welcome.font.a);
   estyle_set_font(es, config->welcome.font.name, config->welcome.font.size);
   estyle_move(es, x, y);
   estyle_set_layer(es, 5);
   estyle_show(es);
   _o_text_entry = es;

   /* _o_err_str is placed later */
   es = estyle_new(evas, "", "raised");
   estyle_set_color(es, config->welcome.font.r, config->welcome.font.g,
                    config->welcome.font.b, config->welcome.font.a);
   estyle_set_font(es, config->welcome.font.name, config->welcome.font.size);
   estyle_move(es, x, y);
   estyle_hide(es);
   estyle_set_layer(es, 5);
   _o_err_str = es;

   /* Greeting Message */
   es = estyle_new(evas, config->greeting.mess, "raised");
   estyle_set_color(es, config->greeting.font.r, config->greeting.font.g,
                    config->greeting.font.b, config->greeting.font.a);
   estyle_set_font(es, config->greeting.font.name,
                   config->greeting.font.size + 8);
   estyle_geometry(es, NULL, NULL, &tw, &th);
   x = ((((e->geom.w / config->screens.w) - tw) * config->greeting.pos.x) +
        config->greeting.pos.offset_x);
   y = ((((e->geom.h / config->screens.h) - th) * config->greeting.pos.y) +
        config->greeting.pos.offset_y);
   estyle_move(es, x, y);
   y += th;
   estyle_show(es);

   /* Greeting Date */
   es = estyle_new(evas, "", "raised");
   estyle_set_color(es, config->greeting.font.r, config->greeting.font.g,
                    config->greeting.font.b, config->greeting.font.a);
   estyle_set_font(es, config->greeting.font.name,
                   config->greeting.font.size);
   estyle_move(es, x, y);
   y += config->greeting.font.size + 4;
   estyle_show(es);
   ecore_add_event_timer("timer", 1.0, elogin_update_time, 0, es);

   /* Greeting time */
   es = estyle_new(evas, "", "raised");
   estyle_set_color(es, config->greeting.font.r, config->greeting.font.g,
                    config->greeting.font.b, config->greeting.font.a);
   estyle_set_font(es, config->greeting.font.name,
                   config->greeting.font.size);
   estyle_move(es, x, y);
   estyle_show(es);
   ecore_add_event_timer("timer2", 1.0, elogin_update_time, 1, es);

   e_login_session_bar_init(e);
}
