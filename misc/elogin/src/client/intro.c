#include "elogin.h"
#include "e_login_config.h"

#define PERCENT_DESC_HORIZONTAL_PLACEMENT 0.5
#define PERCENT_DESC_VERTICAL_PLACEMENT 0.8

static Evas *evas = NULL;
static Evas_Object *_o_text_desc = NULL;
static Evas_Object *_o_pass_desc = NULL;
static Evas_Object *_o_text_entry = NULL;
static Evas_Object *_o_err_str = NULL;
static E_Login_Config config = NULL;

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
   int x, y;
   int w, h;
   double tw, th;
   Evas_Object *o = _o_err_str;

   if (err_str)
   {
      evas_output_size_get(evas, &w, &h);
      evas_object_text_text_set(_o_err_str, err_str);

      evas_object_geometry_get(o, NULL, NULL, &tw, &th);

      x = (((w / config->screens.w) -
            tw) * PERCENT_DESC_HORIZONTAL_PLACEMENT);
      y = (((h / config->screens.h) - th) * PERCENT_DESC_VERTICAL_PLACEMENT);

      evas_object_move(o, x, y);

      evas_object_hide(_o_pass_desc);
      evas_object_hide(_o_text_desc);
      evas_object_show(_o_err_str);
      ecore_add_event_timer("error_string", 1.5, error_die, 255, o);
   }
}

void
set_text_entry_text(int is_pass, char *txt)
{
   Evas_Object *o = _o_text_entry;

   double tw = 0.0, th = 0.0, x, y;
   int w, h;

   if (is_pass)
      evas_object_text_text_set(o, "");
   else
      evas_object_text_text_set(o, txt);

   evas_output_size_get(evas, &w, &h);
   evas_object_geometry_get(o, NULL, NULL, &tw, &th);

   x = (((w / config->screens.w) - tw) * PERCENT_DESC_HORIZONTAL_PLACEMENT);
   y = (((h / config->screens.h) - th) * PERCENT_DESC_VERTICAL_PLACEMENT +
        (config->welcome.font.size + 8));

   evas_object_move(o, x, y);
}

void
intro_init(E_Login_Session e)
{
   Evas_Object *o;
   double tw, th;
   double x, y;

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
      fprintf(stderr, "Evas is NULL yo\n");
      exit(1);
   }
   if (!config)
      config = e->config;


   o = evas_object_text_add(evas);
   evas_object_text_font_set(o, config->welcome.font.name,
                             config->welcome.font.size);
   evas_object_text_text_set(o, config->welcome.mess);
   evas_object_color_set(o, config->welcome.font.r, config->welcome.font.g,
                         config->welcome.font.b, config->welcome.font.a);
   evas_object_geometry_get(o, NULL, NULL, &tw, &th);
   x = (((e->geom.w / config->screens.w) -
         tw) * PERCENT_DESC_HORIZONTAL_PLACEMENT);
   y = (((e->geom.h / config->screens.h) -
         th) * PERCENT_DESC_VERTICAL_PLACEMENT);
   evas_object_move(o, x, y);
   evas_object_layer_set(o, 5);
   evas_object_show(o);
   _o_text_desc = o;

   /* password description */
   o = evas_object_text_add(evas);
   evas_object_text_font_set(o, config->passwd.font.name,
                             config->passwd.font.size);
   evas_object_text_text_set(o, config->passwd.mess);
   evas_object_color_set(o, config->passwd.font.r, config->passwd.font.g,
                         config->passwd.font.b, config->passwd.font.a);
   evas_object_geometry_get(o, NULL, NULL, &tw, &th);
   x = (((e->geom.w / config->screens.w) -
         tw) * PERCENT_DESC_HORIZONTAL_PLACEMENT);
   y = (((e->geom.h / config->screens.h) -
         th) * PERCENT_DESC_VERTICAL_PLACEMENT);
   evas_object_move(o, x, y);
   evas_object_layer_set(o, 5);
   _o_pass_desc = o;

   /* o_text_entry */
   o = evas_object_text_add(evas);
   evas_object_text_font_set(o, config->welcome.font.name,
                             config->welcome.font.size);
   evas_object_text_text_set(o, "");
   evas_object_color_set(o, config->welcome.font.r, config->welcome.font.g,
                         config->welcome.font.b, config->welcome.font.a);
   evas_object_layer_set(o, 5);
   evas_object_move(o, -999999, -9999999);
   evas_object_show(o);
   _o_text_entry = o;

   /* _o_err_str is placed later */
   o = evas_object_text_add(evas);
   evas_object_text_font_set(o, config->welcome.font.name,
                             config->welcome.font.size);
   evas_object_text_text_set(o, "");
   evas_object_color_set(o, config->welcome.font.r, config->welcome.font.g,
                         config->welcome.font.b, config->welcome.font.a);
   evas_object_layer_set(o, 10);
   _o_err_str = o;
}
