#include "elogin.h"

#define WELCOME_STRING "Enter your username"
#define PASSWORD_STRING "Enter your password..."

#define TEXT_DESC_FONTSIZE 22
#define TEXT_ENTRY_FONTSIZE 20

#define ENTRY_OFFSET 30

#define PERCENT_LOGO_HORIZONTAL_PLACEMENT 0.5
#define PERCENT_LOGO_VERTICAL_PLACEMENT 0.5

#define PERCENT_DESC_HORIZONTAL_PLACEMENT 0.5
#define PERCENT_DESC_VERTICAL_PLACEMENT 0.8

#define DESC_FADE_IN_TO 0.05
#define DESC_FADE_OUT_TO 0.1

#define FONT_R 0
#define FONT_G 0
#define FONT_B 0
#define FONT_A 0

static Evas evas = NULL;
static Evas_Object _o_logo = NULL;
static Evas_Object _o_text_desc = NULL;
static Evas_Object _o_pass_desc = NULL;
static Evas_Object _o_text_entry = NULL;

void intro_fade_in_text(int val, void *_data);
void intro_fade_out_text(int val, void *_data);

void
intro_fade_out_text(int val, void *_data)
{
   val -= 2;

   if (val > 160)
   {
      evas_set_color(evas, _o_text_desc, FONT_R, FONT_G, FONT_B, val);
      evas_set_color(evas, _o_pass_desc, FONT_R, FONT_G, FONT_B, val);
      evas_set_color(evas, _o_text_entry, FONT_R, FONT_G, FONT_B, val);
      ecore_add_event_timer("fade_out", DESC_FADE_OUT_TO, intro_fade_out_text,
                            val, _data);
   }
   else
   {
      ecore_add_event_timer("fade_in", DESC_FADE_IN_TO, intro_fade_in_text,
                            val, _data);
   }

}
void
intro_fade_in_text(int val, void *_data)
{
   val += 3;

   if (val < 255)
   {
      evas_set_color(evas, _o_text_desc, FONT_R, FONT_G, FONT_B, val);
      evas_set_color(evas, _o_pass_desc, FONT_R, FONT_G, FONT_B, val);
      evas_set_color(evas, _o_text_entry, FONT_R, FONT_G, FONT_B, val);
      ecore_add_event_timer("fade_in", DESC_FADE_IN_TO, intro_fade_in_text,
                            val, _data);
   }
   else
   {
      ecore_add_event_timer("fade_out", DESC_FADE_OUT_TO, intro_fade_out_text,
                            val, _data);
   }
}
void
show_password_description(void)
{
   evas_show(evas, _o_pass_desc);
   evas_hide(evas, _o_text_desc);
}

void
show_text_description(void)
{
   evas_show(evas, _o_text_desc);
   evas_hide(evas, _o_pass_desc);
}

#define PASSWORD(str, size) { int i; memset(&str, 0, size+1); for(i = 0; i < size; i++) str[i] = '*'; }

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

   x = ((w - tw) * PERCENT_DESC_HORIZONTAL_PLACEMENT);
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
   }
   else
   {
      if (e)
         evas = e->evas;
      else
      {
         fprintf(stderr, "intro_init with null evas\n");
         exit(0);
      }
   }

   o = evas_add_image_from_file(evas, PACKAGE_DATA_DIR "/data/images/logo.png");
   evas_get_image_size(evas, o, &iw, &ih);
   evas_resize(evas, o, iw, ih);
   evas_set_image_fill(evas, o, 0.0, 0.0, (float) iw, (float) ih);
   x = ((e->geom.w - iw) * PERCENT_LOGO_HORIZONTAL_PLACEMENT);
   y = ((e->geom.h - ih) * PERCENT_LOGO_VERTICAL_PLACEMENT);
   evas_set_color(evas, o, 39, 196, 255, 255);
   evas_move(evas, o, x, y);
   evas_show(evas, o);
   _o_logo = o;

   o = evas_add_text(evas, "notepad.ttf", TEXT_DESC_FONTSIZE, WELCOME_STRING);
   tw = evas_get_text_width(evas, o);
   th = evas_get_text_height(evas, o);
   x = ((e->geom.w - tw) * PERCENT_DESC_HORIZONTAL_PLACEMENT);
   y = ((e->geom.h - th) * PERCENT_DESC_VERTICAL_PLACEMENT);
   evas_set_color(evas, o, FONT_R, FONT_G, FONT_B, FONT_A);
   evas_move(evas, o, x, y);
   evas_set_layer(evas, o, 5);
   evas_show(evas, o);
   _o_text_desc = o;

   o = evas_add_text(evas, "notepad.ttf", TEXT_DESC_FONTSIZE,
                     PASSWORD_STRING);
   tw = evas_get_text_width(evas, o);
   th = evas_get_text_height(evas, o);
   x = ((e->geom.w - tw) * PERCENT_DESC_HORIZONTAL_PLACEMENT);
   y = ((e->geom.h - th) * PERCENT_DESC_VERTICAL_PLACEMENT);
   evas_set_color(evas, o, FONT_R, FONT_G, FONT_B, FONT_A);
   evas_move(evas, o, x, y);
   evas_set_layer(evas, o, 5);
   _o_pass_desc = o;

   o = evas_add_text(evas, "notepad.ttf", TEXT_DESC_FONTSIZE, "");
   tw = evas_get_text_width(evas, o);
   th = evas_get_text_height(evas, o);
   x = ((e->geom.w - tw) * PERCENT_DESC_HORIZONTAL_PLACEMENT);
   y = ((e->geom.h - th) * PERCENT_DESC_VERTICAL_PLACEMENT + ENTRY_OFFSET);
   evas_set_color(evas, o, FONT_R, FONT_G, FONT_B, FONT_A);
   evas_move(evas, o, x, y);
   evas_set_layer(evas, o, 5);
   evas_show(evas, o);
   _o_text_entry = o;

   ecore_add_event_timer("intro_init", DESC_FADE_IN_TO, intro_fade_in_text, 0,
                         e);
}
