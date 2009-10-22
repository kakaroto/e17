#include "stickies.h"

typedef struct _Pginfo Pginfo;

struct _Pginfo
{
   Evas_Object *pager, *page_about, *page_authors, *page_help;
};

/* INTERNAL FUNCTIONS, USUALLY CALLBACKS PROTOTYPES.
 */
static void _about_page_about(void *data, Evas_Object *obj, void *event_info);
static void _about_page_authors(void *data, Evas_Object *obj, void *event_info);
static void _about_page_help(void *data, Evas_Object *obj, void *event_info);
static void _about_destroy_cb(void *data, Evas_Object *obj, void *event_info);

/* FUNCTIONS WHICH HAVE PROTOTYPES DEFINED IN STICKIES.H.
 */
ESAPI void
_e_about_show(void)
{
   // defines
   Evas_Object *win, *background, *vbox, *logo, *pager, *pgbox, *hbox, *scroller, *desctext, *abouttext, *helptext, *close_button, *pgbt;
   char buf[PATH_MAX];
   static Pginfo info;
   int w, h;
   //////////
   win = elm_win_add(NULL, "estickies-about", ELM_WIN_BASIC);
   elm_win_title_set(win, "E Stickies "VERSION);
   evas_object_smart_callback_add(win, "delete,request", _about_destroy_cb, NULL);
   evas_object_show(win);
   //////////
   background = elm_bg_add(win);
   evas_object_size_hint_weight_set(background, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, background);
   evas_object_show(background);
   //////////
   vbox = elm_box_add(win);
   elm_box_horizontal_set(vbox, 0);
   evas_object_size_hint_weight_set(vbox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(vbox, 0.0, 0.0);
   elm_win_resize_object_add(win, vbox);
   evas_object_show(vbox);
   ////////// LOGO OF ESTICKIES
   logo = elm_image_add(win);
   snprintf(buf, sizeof(buf), "%s/images/estickies.png", PACKAGE_DATA_DIR);
   elm_image_file_set(logo, buf, NULL);
   elm_image_scale_set(logo, 0, 0);
   elm_box_pack_end(vbox, logo);
   evas_object_show(logo);
   //////////
   pager = elm_pager_add(win);
   evas_object_size_hint_weight_set(pager, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(pager, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(vbox, pager);
   evas_object_show(pager);

   info.pager = pager;
   //////////
   pgbox = elm_box_add(win);
   evas_object_size_hint_weight_set(pgbox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(pgbox, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(pgbox);
   //////////
   scroller = elm_scroller_add(win);
   evas_object_size_hint_weight_set(scroller, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(scroller, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(pgbox, scroller);
   evas_object_show(scroller);
   //////////
   desctext = elm_entry_add(win);
   elm_entry_editable_set(desctext, 0);
   evas_object_size_hint_weight_set(desctext, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(desctext, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_entry_entry_set(desctext,
			  _("E Stickies is a sticky notes application that uses Elementary. It uses Elementary's runtime theming support to change the look and feel of the windows and buttons.<br>"));
   elm_scroller_content_set(scroller, desctext);
   evas_object_show(desctext);

   elm_pager_content_push(pager, pgbox);
   info.page_about = pgbox;

   //////////
   //////////
   //////////

   pgbox = elm_box_add(win);
   evas_object_size_hint_weight_set(pgbox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(pgbox, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(pgbox);
   //////////
   scroller = elm_scroller_add(win);
   evas_object_size_hint_weight_set(scroller, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(scroller, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(pgbox, scroller);
   evas_object_show(scroller);
   //////////
   abouttext = elm_entry_add(win);
   elm_entry_editable_set(abouttext, 0);
   evas_object_size_hint_weight_set(abouttext, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(abouttext, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_entry_entry_set(abouttext,
			  _("<b>Code:</b><br>Daniel '<b>quaker</b>' Kolesa<br><b>Old Etk version:</b><br>Hisham '<b>CodeWarrior</b>' Mardam Bey<br><br><b>Themes:</b><br>Brian 'morlenxus' Miculcy"));
   elm_scroller_content_set(scroller, abouttext);
   evas_object_show(abouttext);

   elm_pager_content_push(pager, pgbox);
   info.page_authors = pgbox;

   //////////
   //////////
   //////////

   pgbox = elm_box_add(win);
   evas_object_size_hint_weight_set(pgbox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(pgbox, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(pgbox);
   //////////
   scroller = elm_scroller_add(win);
   evas_object_size_hint_weight_set(scroller, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(scroller, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(pgbox, scroller);
   evas_object_show(scroller);
   //////////
   helptext = elm_entry_add(win);
   elm_entry_editable_set(helptext, 0);
   evas_object_size_hint_weight_set(helptext, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(helptext, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_entry_entry_set(helptext,
			  _("<b>control-n:</b> create a new note<br><b>control-q:</b> quit application<br><b>control-c:</b> copy selected text<br><b>control-x:</b> cut selected text<br><b>control-v:</b> paste text from clipboard<br>"));
   elm_scroller_content_set(scroller, helptext);
   evas_object_show(helptext);

   elm_pager_content_push(pager, pgbox);
   info.page_help = pgbox;
   //////////
   hbox = elm_box_add(win);
   elm_box_horizontal_set(hbox, 1);
   evas_object_size_hint_weight_set(hbox, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(hbox, EVAS_HINT_FILL, 0.0);
   elm_box_pack_end(vbox, hbox);
   evas_object_show(hbox);
   //////////
   pgbt = elm_button_add(win);
   elm_button_label_set(pgbt, _("About E Stickies"));
   evas_object_smart_callback_add(pgbt, "clicked", _about_page_about, &info);
   elm_box_pack_end(hbox, pgbt);
   evas_object_show(pgbt);
   //////////
   pgbt = elm_button_add(win);
   elm_button_label_set(pgbt, _("Authors, credits"));
   evas_object_smart_callback_add(pgbt, "clicked", _about_page_authors, &info);
   elm_box_pack_end(hbox, pgbt);
   evas_object_show(pgbt);
   //////////
   pgbt = elm_button_add(win);
   elm_button_label_set(pgbt, _("E Stickies help"));
   evas_object_smart_callback_add(pgbt, "clicked", _about_page_help, &info);
   elm_box_pack_end(hbox, pgbt);
   evas_object_show(pgbt);
   //////////
   close_button = elm_button_add(win);
   elm_button_label_set(close_button, _("Close"));
   evas_object_smart_callback_add(close_button, "clicked", _about_destroy_cb, win);
   elm_box_pack_end(vbox, close_button);
   evas_object_show(close_button);

   elm_pager_content_promote(pager, info.page_about);
   evas_object_geometry_get(win, NULL, NULL, &w, &h);
   evas_object_resize(win, w, h + 290);
}

/* CALLBACKS THEMSELVES
 */
static void _about_destroy_cb(void *data, Evas_Object *obj, void *event_info)
{
   evas_object_del(data);
}

static void _about_page_about(void *data, Evas_Object *obj, void *event_info)
{
   Pginfo *info = data;
   elm_pager_content_promote(info->pager, info->page_about);
}

static void _about_page_authors(void *data, Evas_Object *obj, void *event_info)
{
   Pginfo *info = data;
   elm_pager_content_promote(info->pager, info->page_authors);
}

static void _about_page_help(void *data, Evas_Object *obj, void *event_info)
{
   Pginfo *info = data;
   elm_pager_content_promote(info->pager, info->page_help);
}
