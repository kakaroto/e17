#undef FNAME
#undef NAME
#undef ICON

/* metadata */
#define FNAME textblock_text_append_start
#define NAME "Textblock text_append"
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
      "This test just appends and removes text from different paragraphs, it's "
      "not a very visual test, it's included for benchmarking purposes."
"<ps>"
"Enlightenment is the flagship and original name bearer for this project. Once it was just a humble window manager for X11 that wanted to do things differently. To do them better, but it has expanded. This can be confusing so when we refer to Enlightenment, we may mean the project as a whole or just the window manager proper. The libraries behind Enlightenment are referred to as EFL collectively, each with a specific name and purpose."
"<ps>"
"The window manager is a lean, fast, modular and very extensible window manager for X11 and Linux. It is classed as a \"desktop shell\" providing the things you need to operate your desktop (or laptop), but is not a whole application suite. This covered launching applications, managing their windows and doing other system tasks like suspending, reboots, managing files etc.<br>"
"Platform Support<br>"
"Linux BSD Windows Apple"
"<ps>"
"Enlightenment and EFL support several platforms, though Linux is the primary platform of choice for our developers, some make efforts to make things work on FreeBSD and other BSD's, Solaris, MacOS X, Windows (XP, Vista, 7 etc.), Windows CE and more. Compatibility will vary, but most of core EFL support all Linuxes, BSD's, Solaris and other UNIX-like OS's. Mac support should work mostly thanks to the X11 support in OS X, and Windows support exists for most of the core libraries (XP, Vista, 7, CE)."
"<ps>"
"Please contact us for more information if you have problems with support in your platform. Just remember we have limited resources and most of them focus on the core open-source targets.<br>"
"Proven effectiveness"
"<ps>"
"Enlightenment libraries already power millions of systems, from mobile phones to set top boxes, desktops, laptops, game systems and more. It is only now being recognized for its forward-thinking approaches, as products and designers want to do more than the boring functional user experiences of the past. This is where EFL excels."
"<ps>"
"Free.fr is shipping millions of set top boxes in France, powered by EFL. The Openmoko Freerunner sold thousands of devices with EFL on them. Yellow Dog Linux for the Sony PS3 ships with Enlightenment as the default. EFL has been used on printers, netbooks and more."
"<ps>"
"Building Blocks"
"<ps>"
"Enlightenment, the window manager is built on top of building blocks known as EFL (the Enlightenment Foundation Libraries). There are more than can be sensibly put into the simple block diagram above, but this covers the essentials.<br>"
"Simple E stack"
"<ps>"
"Pretty much any application written using Core EFL libraries will use one or more of these depending on its needs. It may only need the lower level ones or use all of them to the top of the stack. Each library fulfills a purpose, so it may be skipped if not needed.<br>"
"Core<ps>Core EFL components are:<ps>    * Evas<br>"
"    * Eina<br>"
"    * Edje<br>"
"    * Eet<br>"
"    * Ecore<br>"
"    * Efreet<br>"
"    * E_Dbus<br>"
"    * Embryo<br>"
"    * Eeze<br>"
"    * Elementary (window manager will use this in 0.18)<ps>Binding support exists for several languages such as:<ps>    * Python<br>"
"    * Javascript<br>"
"    * Perl<br>"
"    * C++<br>"
"    * Ruby<ps>There are other libraries and applications which build on core EFL and function on other systems too, providing more functionality, examples, and utility:<ps>    * Emotion<br>"
"    * Ethumb<br>"
"    * Exquisite<br>"
"    * Eve<br>"
"    * Edje Editor<br>"
"    * E UPnP<br>"
"    * Enesim<br>"
"    * Expedite<br>"
"    * Epdf<br>"
"    * Exchange<br>"
"    * Eweather<br>"
"    * Rage<br>"
"    * Evil<br>"
"    * Exalt<ps>        <br>"
"Devices<br>"
"Laptop<ps>Enlightenment and EFL use desktop Linux systems as a primary method of development because it is fast and simple to do so, but all of it is written with the express goal in mind of also working on devices from Mobile Phones, to Televisions, Netbooks and more.<br>"
"Phone<ps>We have run and tested on x86-32, x86-64, Atom, Power-PC, ARM (ARM9, ARM11, Cortex-A8 and more), MIPS, Sparc, and many other architectures. The suggested minimum RAM required for a full Linux system + EFL application is 16MB, but you may be able to get by on 8MB. For full functionality 64MB or more is suggested. As little as a 200Mhz ARM core will provide sufficient processing power (depending on needs).<br>"
"Various Processors<ps>Screens from even less than QVGA (320x240 or 240x320) screens all the way up to and beyond full-HD (1920x1080) are covered by EFL. It has the ability to scale user interfaces to almost any sane resolution, as well as adapt to differing input device resolutions, from mouse and stylus to fat fingers. It can draw displays from e-paper through 8-bit paletted displays, 16bit beautifully dithered ones all the way to full 24/32bit OLED beauties.<br>"
"Graphics"
"<ps>"
"Enlightenment is built by designers and programmers who want others to be able to do more with less. Some of Enlightenment's libraries do not do anything with graphics at all, but it is the ones that do that are the shining stars of the Enlightenment world.<ps>Evas is the canvas layer. It is not a drawing library. It is not like OpenGL, Cairo, XRender, GDI, DirectFB etc. It is a scene graph library that retains state of all objects in it. They are created then manipulated until they are no longer needed, at which point they are deleted. This allows the programmer to work in terms that a designer thinks of. It is direct mapping, as opposed to having to convert the concepts into drawing commands in the right order, calculate minimum drawing calls needed to get the job done etc.<ps>Evas also handles abstracting the rendering mechanism. With zero changes the same application can move from software to OpenGL rendering, as they all use an abstracted scene graph to describe the world (canvas) to Evas. Evas supports multiple targets, but the most useful are the high-speed software rendering engines and OpenGL (as well as OpenGL-ES 2.0).<ps>Evas not only does quality rendering and compositing, but also can scale, rotate and fully 3D transform objects, allowing for sought-after 3D effects in your interfaces. It supplies these abilities in both software and OpenGL rendering, so you are never caught with unexpected loss of features. The software rendering is even fast enough to provide the 3D without any acceleration on devices for simple uses.<ps>Edje is a meta-object design library that is somewhere between Flash, PSD, SVG and HTML+CSS. It separates design out from code and into a dynamically loaded data file. This file is compressed and loaded very quickly, along with being cached and shared betweeen instances.<ps>This allows design to be provided at runtime by different design (EDJ) files, leaving the programmer to worry about overall application implementation and coarse grained UI as opposed to needing to worry about all the little details that the artists may vary even until the day before shipping the product.<br>"
      );
   evas_object_move(o_text, 0, 0);
   evas_object_resize(o_text, win_w, win_h);

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
   Evas_Textblock_Cursor *cur;
   static Evas_Textblock_Cursor *cur2;
   cur = (Evas_Textblock_Cursor *) evas_object_textblock_cursor_get(o_text);
   evas_textblock_cursor_text_append(cur, "*");
   evas_textblock_cursor_char_delete(cur);

   evas_textblock_cursor_paragraph_char_first(cur);
   if (!cur2)
     {
        cur2 = evas_object_textblock_cursor_new(o_text);
        evas_textblock_cursor_paragraph_last(cur2);
        evas_textblock_cursor_paragraph_char_first(cur2);
     }
   if (!evas_textblock_cursor_compare(cur, cur2))
     evas_textblock_cursor_paragraph_first(cur);
   else
     evas_textblock_cursor_paragraph_next(cur);

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
