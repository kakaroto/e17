#include "elogin.h"
#include "e_login_config.h"

#define PERCENT_DESC_HORIZONTAL_PLACEMENT 0.5
#define PERCENT_DESC_VERTICAL_PLACEMENT 0.8

static Evas evas = NULL;
static Evas_Object _o_text_desc = NULL;
static Evas_Object _o_pass_desc = NULL;
static Evas_Object _o_text_entry = NULL;
static Evas_Object _o_err_str = NULL;
static E_Login_Config config = NULL;

void
show_password_description(void)
{
   evas_show(evas, _o_pass_desc);
}

void
hide_password_description(void)
{
   evas_hide(evas, _o_pass_desc);
}

void
show_text_description(void)
{
   evas_show(evas, _o_text_desc);
}

void
hide_text_description(void)
{
   evas_hide(evas, _o_text_desc);
}

static void
error_die(int val, void *_data)
{
   evas_show(evas, _o_text_desc);
   evas_hide(evas, _o_err_str);
}

void
show_error_description(char *err_str)
{
   int x, y;
   int w, h;
   double tw, th;
   Evas_Object o = _o_err_str;

   if (err_str)
   {
      evas_get_drawable_size(evas, &w, &h);
      evas_set_text(evas, _o_err_str, err_str);

      tw = evas_get_text_width(evas, o);
      th = evas_get_text_height(evas, o);

      x = (((w / config->screens.w) -
            tw) * PERCENT_DESC_HORIZONTAL_PLACEMENT);
      y = (((h / config->screens.h) - th) * PERCENT_DESC_VERTICAL_PLACEMENT);

      evas_move(evas, o, x, y);

      evas_hide(evas, _o_pass_desc);
      evas_hide(evas, _o_text_desc);
      evas_show(evas, _o_err_str);
      ecore_add_event_timer("error_string", 1.5, error_die, 255, o);
   }
}

void
set_text_entry_text(int is_pass, char *txt)
{
   Evas_Object o = _o_text_entry;

   double tw, th, x, y;
   int w, h;

   if (is_pass)                 /* should only be ***** */
   {
	evas_set_text(evas, o, ""); 
   }
   else
   {
      evas_set_text(evas, o, txt);
   }

   evas_get_drawable_size(evas, &w, &h);
   tw = evas_get_text_width(evas, o);
   th = evas_get_text_height(evas, o);

   x = (((w / config->screens.w) - tw) * PERCENT_DESC_HORIZONTAL_PLACEMENT);
   y = (((h / config->screens.h) - th) * PERCENT_DESC_VERTICAL_PLACEMENT +
        (config->welcome.font.size + 8));

#if 0
   fprintf(stderr, "%0.2f,%0.2f is x,y\n", x, y);
#endif

   evas_move(evas, o, x, y);
}

void
intro_init(E_Login_Session e)
{
   Evas_Object o;
   double tw, th;
   double x, y;

   if (evas)
   {
      if (_o_text_desc)
         evas_del_object(evas, _o_text_desc);
      if (_o_pass_desc)
         evas_del_object(evas, _o_pass_desc);
      if (_o_text_entry)
         evas_del_object(evas, _o_text_entry);
      if (_o_err_str)
         evas_del_object(evas, _o_err_str);
   }
   if (e)
      evas = e->evas;
   if (!config)
      config = e->config;


   o = evas_add_text(evas, config->welcome.font.name,
                     config->welcome.font.size, config->welcome.mess);
   tw = evas_get_text_width(evas, o);
   th = evas_get_text_height(evas, o);
   x = (((e->geom.w / config->screens.w) -
         tw) * PERCENT_DESC_HORIZONTAL_PLACEMENT);
   y = (((e->geom.h / config->screens.h) -
         th) * PERCENT_DESC_VERTICAL_PLACEMENT);
   evas_set_color(evas, o, config->welcome.font.r, config->welcome.font.g,
                  config->welcome.font.b, config->welcome.font.a);
   evas_move(evas, o, x, y);
   evas_set_layer(evas, o, 5);
   evas_show(evas, o);
   _o_text_desc = o;

   o = evas_add_text(evas, config->passwd.font.name, config->passwd.font.size,
                     config->passwd.mess);
   tw = evas_get_text_width(evas, o);
   th = evas_get_text_height(evas, o);
   x = (((e->geom.w / config->screens.w) -
         tw) * PERCENT_DESC_HORIZONTAL_PLACEMENT);
   y = (((e->geom.h / config->screens.h) -
         th) * PERCENT_DESC_VERTICAL_PLACEMENT);
   evas_set_color(evas, o, config->passwd.font.r, config->passwd.font.g,
                  config->passwd.font.b, config->passwd.font.a);
   evas_move(evas, o, x, y);
   evas_set_layer(evas, o, 5);
   _o_pass_desc = o;

   o = evas_add_text(evas, config->welcome.font.name,
                     config->welcome.font.size, "");
   evas_set_color(evas, o, config->welcome.font.r, config->welcome.font.g,
                  config->welcome.font.b, config->welcome.font.a);
   evas_set_layer(evas, o, 5);
   evas_move(evas, o, -999999, -9999999);
   evas_show(evas, o);
   _o_text_entry = o;

   /* _o_err_str is placed later */
   o = evas_add_text(evas, config->welcome.font.name,
                     config->welcome.font.size, "");
   evas_set_color(evas, o, config->welcome.font.r, config->welcome.font.g,
                  config->welcome.font.b, config->welcome.font.a);
   evas_set_layer(evas, o, 10);
   _o_err_str = o;

}
