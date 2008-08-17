#undef FNAME
#undef NAME
#undef ICON

/* metadata */
#define FNAME textblock_basic_start
#define NAME "Textblock Basic"
#define ICON "mushroom.png"

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
      "DEFAULT='font=Vera,Kochi font_size=8 align=left color=#000000 wrap=word'"
      "center='+ font=Vera,Kochi font_size=10 align=center'"
      "/center='- \n'"
      "right='+ font=Vera,Kochi font_size=10 align=right'"
      "/right='- \n'"
      "blockquote='+ left_margin=+24 right_margin=+24 font=Vera,Kochi font_size=10 align=left'"
      "h1='+ font_size=20'"
      "red='+ color=#ff0000'"
      "p='+ font=Vera,Kochi font_size=10 align=left'"
      "/p='- \n'"
      "br='\n'"
      "tab='\t'"
      );
   evas_object_textblock_style_set(o, st);
   evas_textblock_style_free(st);
   evas_object_textblock_clear(o);

   evas_object_textblock_text_markup_set
     (o,
           "<center><h1>Title</h1></center><br>"
      "<p><tab>A pragraph here <red>red text</red> and stuff.</p>"
      "<p>And escaping &lt; and &gt; as well as &amp; as <h1>normal.</h1></p>"
      "<p>If you want a newline use &lt;br&gt;<br>woo a new line!</p>"
      "<right>Right "
      "<style=outline color=#fff outline_color=#000>aligned</> "
      "<style=shadow shadow_color=#fff8>text</> "
      "<style=soft_shadow shadow_color=#0002>should</> "
      "<style=glow color=#fff glow2_color=#fe87 glow_color=#f214 >go here</> "
      "<style=far_shadow shadow_color=#0005>as it is</> "
      "<style=outline_shadow color=#fff outline_color=#8228 shadow_color=#005>within</> "
      "<style=outline_soft_shadow color=#fff outline_color=#8228 shadow_color=#0002>right tags</> "
      "<style=far_soft_shadow color=#fff shadow_color=#0002>to make it align to the</> "
      "<underline=on underline_color=#00f>right hand</> "
      "<backing=on backing_color=#fff8>side </><backing_color=#ff08>of</><backing_color=#0f08> </>"
      "<strikethrough=on strikethrough_color=#f0f8>the textblock</>.</right>"
      "<p>And "
      "<underline=double underline_color=#f00 underline2_color=#00f>now we need</> "
      "to test some <color=#f00 font_size=8>C</><color=#0f0 font_size=10>O</>"
      "<color=#00f font_size=12>L</><color=#fff font_size=14>O</>"
      "<color=#ff0 font_size=16>R</><color=#0ff font_size=18> Bla Rai</>"
      "<color=#f0f font_size=20> Stuff</>.</p>"
      "<blockquote>"
      "(日本語 カタカナ ひらがな) "
      "<style=outline color=#fff outline_color=#000>Round about the cauldron go;</> "
      "In the poison'd entrails throw. "
      "<style=shadow shadow_color=#fff8>Toad, that under cold stone</> "
      "Days and nights has thirty-one "
      "<style=soft_shadow shadow_color=#0002>Swelter'd venom sleeping got,</> "
      "<style=glow color=#fff glow2_color=#fe87 glow_color=#f214 >Boil thou first i' the charmed pot.</> "
      "Double, double toil and trouble; "
      "Fire burn, and cauldron bubble. "
      "<style=far_shadow shadow_color=#0005>Fillet of a fenny snake,</> "
      "In the cauldron boil and bake; "
      "<style=outline_shadow color=#fff outline_color=#8228 shadow_color=#005>Eye of newt and toe of frog,</> "
      "<underline=on underline_color=#00f>Wool of bat and tongue of dog,</> "
      "<backing=on backing_color=#fff8>Adder's fork and blind-worm's sting,</> "
      "<underline=double underline_color=#f00 underline2_color=#00f>Lizard's leg and owlet's wing,</> "
      "<color=#808 font_size=20>For a charm of powerful trouble, "
      "Like a hell-broth boil and bubble. "
      "Double, double toil and trouble;</> "
      "Fire burn and cauldron bubble. "
      "Scale of dragon, tooth of wolf, "
      "Witches' mummy, maw and gulf "
      "Of the ravin'd salt-sea shark, "
      "Root of hemlock digg'd i' the dark, "
      "Liver of blaspheming Jew, "
      "Gall of goat, and slips of yew "
      "Silver'd in the moon's eclipse, "
      "Nose of Turk and Tartar's lips, "
      "Finger of birth-strangled babe "
      "Ditch-deliver'd by a drab, "
      "Make the gruel thick and slab: "
      "Add thereto a tiger's chaudron, "
      "For the ingredients of our cauldron. "
      "Double, double toil and trouble; "
      "Fire burn and cauldron bubble. "
      "Cool it with a baboon's blood, "
      "Then the charm is firm and good. "
      "Heizölrückstoßabdämpfung fløde pingüino kilómetros cœur déçu l'âme "
      "plutôt naïve Louÿs rêva crapaüter Íosa Úrmhac Óighe pór Éava Ádhaim"
      "</blockquote>"
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

   w0 = 160;
   h0 = 120;
   w = 150 + ((1.0 + cos((double)(f + (i * 10)) / (37.4 * SLOW) )) * w0 * 2);
   h = 50 + ((1.0 + sin((double)(f + (i * 19)) / (52.6 * SLOW) )) * h0 * 2);
   x = (win_w / 2) - (w / 2);
   x += sin((double)(f + (i * 13)) / (86.7 * SLOW)) * (w0 / 2);
   y = (win_h / 2) - (h / 2);
   y += cos((double)(f + (i * 28)) / (93.8 * SLOW)) * (h0 / 2);
   evas_object_move(o_text, x, y);
   evas_object_resize(o_text, w, 5000);

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
