#include "elogin.h"

struct _Elogin_Config
{
   char *welcome;
   char *password;
   char *bg;

   struct
   {
      int size;
      int r, g, b, a;
      char *name;
   }
   font;

   struct
   {
      int total, placed;
   }
   xinerama;

   struct
   {
      char *file;
      int h, v;
   }
   logo;
};

typedef struct _Elogin_Config Elogin_Config;

#define WELCOME_STRING "Enter your username"
#define PASSWORD_STRING "Enter your password..."

#define TEXT_DESC_FONTSIZE 22
#define TEXT_ENTRY_FONTSIZE 20
#define TEXT_ERR_FONTSIZE 22

#define ENTRY_OFFSET 30

#define XINERAMA_HEADS 2

#define PERCENT_LOGO_HORIZONTAL_PLACEMENT 0.5
#define PERCENT_LOGO_VERTICAL_PLACEMENT 0.5

#define PERCENT_DESC_HORIZONTAL_PLACEMENT 0.5
#define PERCENT_DESC_VERTICAL_PLACEMENT 0.8

#define FONTNAME "notepad.ttf"
#define FONT_R 192
#define FONT_G 192
#define FONT_B 192
#define FONT_A 210

static Evas evas = NULL;
static Evas_Object _o_logo = NULL;
static Evas_Object _o_text_desc = NULL;
static Evas_Object _o_pass_desc = NULL;
static Evas_Object _o_text_entry = NULL;
static Evas_Object _o_err_str = NULL;

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

      x = (((w / XINERAMA_HEADS) - tw) * PERCENT_DESC_HORIZONTAL_PLACEMENT);
      y = ((h - th) * PERCENT_DESC_VERTICAL_PLACEMENT);

      evas_move(evas, o, x, y);

      evas_hide(evas, _o_pass_desc);
      evas_hide(evas, _o_text_desc);
      evas_show(evas, _o_err_str);
      ecore_add_event_timer("error_string", 1, error_die, 255, o);
   }
}

#define STAR 42                 /* '*' */
#define PASSWORD(str, size) { memset(&str, STAR, size); str[size + 1] = 0; }

void
set_text_entry_text(int is_pass, char *txt)
{
   Evas_Object o = _o_text_entry;

   char buf[PATH_MAX];
   double tw, th, x, y;
   int w, h;

   memset(&buf, 0, PATH_MAX);
   if (is_pass)                 /* should only be ***** */
   {
      PASSWORD(buf, strlen(txt)) evas_set_text(evas, o, buf);
   }
   else
   {
      evas_set_text(evas, o, txt);
   }

   evas_get_drawable_size(evas, &w, &h);
   tw = evas_get_text_width(evas, o);
   th = evas_get_text_height(evas, o);

   x = (((w / XINERAMA_HEADS) - tw) * PERCENT_DESC_HORIZONTAL_PLACEMENT);
   y = ((h - th) * PERCENT_DESC_VERTICAL_PLACEMENT + ENTRY_OFFSET);

   evas_move(evas, o, x, y);
}

void
intro_init(E_Login_Session e)
{
   Evas_Object o;
   int iw, ih;
   double tw, th;
   double x, y;

   if (evas)
   {
      if (_o_logo)
         evas_del_object(evas, _o_logo);
      if (_o_text_desc)
         evas_del_object(evas, _o_text_desc);
      if (_o_pass_desc)
         evas_del_object(evas, _o_pass_desc);
      if (_o_text_entry)
         evas_del_object(evas, _o_text_entry);
      if (_o_err_str)
         evas_del_object(evas, _o_err_str);
   }
   else
   {
      if (e)
         evas = e->evas;
   }

   o = evas_add_image_from_file(evas,
                                PACKAGE_DATA_DIR "/data/images/logo.png");
   evas_get_image_size(evas, o, &iw, &ih);
   evas_resize(evas, o, iw, ih);
   evas_set_image_fill(evas, o, 0.0, 0.0, (float) iw, (float) ih);
   x = (((e->geom.w / XINERAMA_HEADS) -
         iw) * PERCENT_LOGO_HORIZONTAL_PLACEMENT);
   y = ((e->geom.h - ih) * PERCENT_LOGO_VERTICAL_PLACEMENT);
   evas_set_color(evas, o, 39, 196, 255, 255);
   evas_move(evas, o, x, y);
   evas_show(evas, o);
   _o_logo = o;

   o = evas_add_text(evas, FONTNAME, TEXT_DESC_FONTSIZE, WELCOME_STRING);
   tw = evas_get_text_width(evas, o);
   th = evas_get_text_height(evas, o);
   x = (((e->geom.w / XINERAMA_HEADS) -
         tw) * PERCENT_DESC_HORIZONTAL_PLACEMENT);
   y = ((e->geom.h - th) * PERCENT_DESC_VERTICAL_PLACEMENT);
   evas_set_color(evas, o, FONT_R, FONT_G, FONT_B, FONT_A);
   evas_move(evas, o, x, y);
   evas_set_layer(evas, o, 5);
   evas_show(evas, o);
   _o_text_desc = o;

   o = evas_add_text(evas, FONTNAME, TEXT_DESC_FONTSIZE, PASSWORD_STRING);
   tw = evas_get_text_width(evas, o);
   th = evas_get_text_height(evas, o);
   x = (((e->geom.w / XINERAMA_HEADS) -
         tw) * PERCENT_DESC_HORIZONTAL_PLACEMENT);
   y = ((e->geom.h - th) * PERCENT_DESC_VERTICAL_PLACEMENT);
   evas_set_color(evas, o, FONT_R, FONT_G, FONT_B, FONT_A);
   evas_move(evas, o, x, y);
   evas_set_layer(evas, o, 5);
   _o_pass_desc = o;

   o = evas_add_text(evas, FONTNAME, TEXT_DESC_FONTSIZE, "");
   evas_set_color(evas, o, FONT_R, FONT_G, FONT_B, FONT_A);
   evas_set_layer(evas, o, 5);
   evas_move(evas, o, -999999, -9999999);
   evas_show(evas, o);
   _o_text_entry = o;

   /* _o_err_str is placed later */
   o = evas_add_text(evas, FONTNAME, TEXT_ERR_FONTSIZE, "");
   evas_set_color(evas, o, FONT_R, FONT_G, FONT_B, FONT_A);
   evas_set_layer(evas, o, 10);
   _o_err_str = o;

}
