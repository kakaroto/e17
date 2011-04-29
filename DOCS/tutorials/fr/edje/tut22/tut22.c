/* gcc -o test tut13.c `pkg-config elementary --cflags --libs` */

#include <Elementary.h>

static Evas_Object *win;
static Evas_Object *layout;
static Evas_Object *table;
static Evas_Object *panel;

static void
_win_del(void *data, Evas_Object *obj, void *event_info)
{
   elm_exit();
}

static void
_edje_signal_textblock_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
    //printf("Textblock Emission : %s - Source : %s\n", emission, source);

   if (!strncmp(emission, "anchor,mouse,up,1,", strlen("anchor,mouse,up,1,")))
     {
	const char *href = emission + strlen("anchor,mouse,up,1,");
	printf("href = %s\n", href);
	if (!strcmp(href, "\"evas_map\""))
	  {
              Evas_Object *edje;
	     printf("action\n");
             edje = elm_layout_edje_get(layout);
             edje_object_signal_emit(edje, "show,table,anim", "tuto");
	  }
     }

}

static void
_edje_signal_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
    //printf("Emission : %s - Source : %s\n", emission, source);

   if (!strcmp(emission, "info,clicked"))
     {
	Evas_Object *textblock;

	Evas_Object *edje;

	printf("info clicked \n");

	textblock = elm_layout_add(win);
	elm_layout_file_set(textblock, "tut22.edj", "infos_panel");
	evas_object_show(textblock);
	evas_object_size_hint_weight_set(textblock, 1, 1);
	elm_layout_content_set(layout, "panel_swallow", textblock);
	evas_object_show(textblock);
	elm_layout_text_set(textblock, "textblock", "<h1>What is Enlightenment?</h1><br>"
			    "<tab><h4>Enlightenment is not just a window manager for <hilight>Linux/X11</hilight> and others, but also a whole suite of libraries to help you create beautiful user interfaces with much less work than doing it the old fashioned way and fighting with traditional toolkits, not to mention a traditional window manager. It covers uses from small mobile devices like phones all the way to powerful multi-core desktops (which are the primary development environment). </h4><br>"
			    "<br><h1>Enlightenment Foundation Libraries (EFL)</h1> <br>"
			    "<tab>These provide both a semi-traditional toolkit set in <rhinoceros>Elementary</rhinoceros> as well as the object canvas <h2>(Evas)</h2> and powerful abstracted objects (Edje) that you can combine, mix and match, even layer on top of each other with alpha channels and events in-tact. It has <link><a href=\"evas_map\">3D transformations</a></link> for all objects and more. )");

	edje = elm_layout_edje_get(textblock);

	edje_object_signal_callback_add(edje, "*", "*", _edje_signal_textblock_cb, NULL);

        edje = elm_layout_edje_get(layout);
        edje_object_signal_emit(edje, "hide,table,anim", "tuto");

     }

}

int main(int argc, char **argv)
{

   Evas_Object *edje;

   elm_init(argc, argv);

   win = elm_win_add(NULL, "tuto", ELM_WIN_BASIC);
   elm_win_title_set(win, "Edje Tutorial");
   evas_object_smart_callback_add(win, "delete,request", _win_del, NULL);


   layout = elm_layout_add(win);
   elm_layout_file_set(layout, "tut22.edj", "interface");
   evas_object_show(layout);
   elm_win_resize_object_add(win, layout);

   table = elm_layout_add(layout);
   elm_layout_file_set(table, "tut22.edj", "table");
   evas_object_show(table);

   evas_object_size_hint_weight_set(layout, 1, 1);

   elm_layout_content_set(layout, "table_swallow", table);

   edje = elm_layout_edje_get(table);
   edje_object_signal_callback_add(edje, "*", "*", _edje_signal_cb, NULL);

   evas_object_resize(win, 800, 480);
   evas_object_show(win);

   elm_run();
   elm_shutdown();
}
