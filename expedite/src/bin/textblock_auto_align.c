#undef FNAME
#undef NAME
#undef ICON

/* metadata */
#define FNAME textblock_auto_align_start
#define NAME "Textblock auto align"
#define ICON "text.png"

#ifndef PROTO
# ifndef UI
#  include "main.h"

/* standard var */
static int done = 0;

/* private data */
static Evas_Object *o_text;

/* setup */
static void _setup(void)
{
   Evas_Object *o;
   Evas_Textblock_Style *st;

   o = evas_object_textblock_add(evas);
   o_text = o;
   st = evas_textblock_style_new();
   evas_textblock_style_set
     (st,
      "DEFAULT='font=Sans font_size=10 color=#000000 wrap=word'"
      "br='\n'"
      "ps='ps'"
      );
   evas_object_textblock_style_set(o, st);
   evas_textblock_style_free(st);
   evas_object_textblock_clear(o);

   evas_object_textblock_text_markup_set
     (o,
      "This is a test of auto alignment in Evas<ps>"
      "<br>"
      "This text should be left aligned<ps>"
      "\xE2\x80\x8FThis text should be right aligned<ps>"
      "\xE2\x80\x8FThis text should be right aligned<br>"
      "Same as this text.<ps>"
      "This text should be left aligned<br>"
      "\xE2\x80\x8FSame as this text."
      "<br><br><ps>"
      "The word 'זה' should be the rightmost, 'טקסט' after, and 'בעברית' last<br>"
      "in the following text:<ps>"
      "זה טקסט בעברית<ps>"
      "<br>"
      "In the next text, the paragraph should be right aligned and the<br>"
      "words should appear in the following order: 'דוגמה' first, 'of' second,<br>"
      "‎'טקסט' third, 'english' fourth and 'in' fifth, counting from right to left<ps>"
      "דוגמה of טקסט in english."
      );

   evas_object_show(o);

   done = 0;
}

/* cleanup */
static void _cleanup(void)
{
   evas_object_del(o_text);
}

/* loop - do things */
static void _loop(double t, int f)
{
   Evas_Coord x, y, w, h, w0, h0;
   int i = 0;

   evas_object_textblock_size_native_get(o_text, &w0, &h0);
   w = w0;
   h = h0;
   w += fabs(sin((double)(f + (i * 13)) / (31.1 * SLOW))) * (w0);
   x = (win_w / 2) - (w / 2);
   y = (win_h / 2) - (h0 / 2);
   evas_object_move(o_text, x, y);
   evas_object_resize(o_text, w, h);

   FPS_STD(NAME);
}

/* prepend special key handlers if interactive (before STD) */
static void _key(char *key)
{
   KEY_STD;
}












/* template stuff - ignore */
# endif
#endif

#ifdef UI
_ui_menu_item_add(ICON, NAME, FNAME);
#endif

#ifdef PROTO
void FNAME(void);
#endif

#ifndef PROTO
# ifndef UI
void FNAME(void)
{
   ui_func_set(_key, _loop);
   _setup();
}
# endif
#endif
#undef FNAME
#undef NAME
#undef ICON
