#undef FNAME
#undef NAME
#undef ICON

/* metadata */
#define FNAME textblock_intl_start
#define NAME "Textblock Intl"
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
      "DEFAULT='font=Sans font_size=10 align=left color=#000000 wrap=word'"
      "br='\n'"
      );
   evas_object_textblock_style_set(o, st);
   evas_textblock_style_free(st);
   evas_object_textblock_clear(o);

   evas_object_textblock_text_markup_set
     (o,
      "This is a test of International test rendering in Evas<br>"
      "<br>"
      "Danish: 'Quizdeltagerne spiste jordbær med fløde, mens cirkusklovnen'<br>"
      "German: 'Heizölrückstoßabdämpfung'<br>"
      "Spanish: 'El pingüino Wenceslao hizo kilómetros bajo exhaustiva lluvia y'<br>"
      "French: 'Le cœur déçu mais l'âme plutôt naïve, Louÿs rêva de crapaüter en'<br>"
      "Irish Gaelic: 'D'fhuascail Íosa, Úrmhac na hÓighe Beannaithe, pór Éava agus Ádhaimh'<br>"
      "Hungarian: 'Árvíztűrő tükörfúrógép'<br>"
      "Icelandic: 'Kæmi ný öxi hér ykist þjófum nú bæði víl og ádrepa'<br>"
      "Japanese (hiragana): 'いろはにほへとちりぬるを'<br>"
      "Japanese (katakana): 'イロハニホヘト チリヌルヲ ワカヨタレソ ツネナラム'<br>"
      "Hebrew: '? דג סקרן שט בים מאוכזב ולפתע מצא לו חברה איך הקליטה'<br>"
      "Polish: 'Pchnąć w tę łódź jeża lub ośm skrzyń fig'<br>"
      "Russian: 'В чащах юга жил бы цитрус? Да, но фальшивый экземпляр!'<br>"
      "IPA: 'ˈjunɪˌkoːd'<br>"
      "American Dictionary: 'Ūnĭcōde̽'<br>"
      "Anglo-saxon: 'ᛡᚢᚾᛁᚳᚩᛞ'<br>"
      "Arabic: 'يونِكود'<br>"
      "Armenian: 'Յունիկօդ'<br>"
      "Bengali: 'য়ূনিকোড'<br>"
      "Bopomofo: 'ㄊㄨㄥ˅ ㄧˋ ㄇㄚ˅'<br>"
      "Canadian Syllabics: 'ᔫᗂᑰᑦ'<br>"
      "Cherokee: 'ᏳᏂᎪᏛ'<br>"
      "Chinese: '萬國碼'<br>"
      "Ethiopic: 'ዩኒኮድ'<br>"
      "Georgian: 'უნიკოდი'<br>"
      "Greek: 'Γιούνικοντ'<br>"

      /* also test the html entity stuff a bit */
      "Greek continued: '&tau;&upsilon;&lambda;&theta;'<br>"

      "Gujarati: 'યૂનિકોડ'<br>"
      "Gurmukhi: 'ਯੂਨਿਕੋਡ'<br>"
      "Hindi: 'यूनिकोड'<br>"
      "Kannada: 'ಯೂನಿಕೋಡ್'<br>"
      "Khmer: 'យូនីគោដ'<br>"
      "Korean: '유니코드'<br>"
      "Malayalam: 'യൂനികോഡ്'<br>"
      "Ogham: 'ᚔᚒᚅᚔᚉᚑᚇ'<br>"
      "Oriya: 'ୟୂନିକୋଡ'<br>"
      "Persian: 'یونی‌کُد'<br>"
      "Sinhala: 'යණනිකෞද්'<br>"
      "Syriac: 'ܝܘܢܝܩܘܕ'<br>"
      "Tamil:'யூனிகோட்'<br>"
      "Telugu: 'యూనికోడ్'<br>"
      "Thai: 'ยูนืโคด'<br>"
      "Tibetan: 'ཨུ་ནི་ཀོཌྲ།'<br>"
      "Yiddish: 'יוניקאָד'<br>"
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
   x = (win_w / 2) - (w / 2);
   x += sin((double)(f + (i * 13)) / (31.1 * SLOW)) * (w0 / (2 * 2));
   y = (win_h / 2) - (h / 2);
   y += cos((double)(f + (i * 28)) / (19.6 * SLOW)) * (h0 / (2 * 2));
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
