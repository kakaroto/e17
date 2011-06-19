#include "main.h"

static Evas_Object *menu = NULL;

struct set_account
{
   Evas_Object *inwin;
   Evas_Object *entry;
};

static struct set_account set_account;
static Evas_Object *ic = NULL;

static void
_menu_open_cb(void *data, Evas_Object *obj, void *event_info);
static void
_account_set_cb(void *data, Evas_Object *obj, void *event_info);
static void
_account_set_apply_cb(void *data, Evas_Object *obj, void *event_info);
static void
_account_set_cancel_cb(void *data, Evas_Object *obj, void *event_info);
static void
_account_set_entry_changed_cb(void *data, Evas_Object *obj, void *event_info);

Evas_Object *
flickr_menu_new(Evas_Object *edje)
{
   Evas_Object *bt =
            (Evas_Object *) edje_object_part_object_get(edje, "object.flickr");
   evas_object_smart_callback_add(bt, "clicked", _menu_open_cb, edje);

   return bt;
}

void
flickr_menu_animated_set(Eina_Bool animated)
{
   if (animated)
      edje_object_signal_emit(ic, "animated", "");
   else
      edje_object_signal_emit(ic, "not_animated", "");
}

static void
_menu_open_cb(void *data, Evas_Object *obj, void *event_info)
{
   Evas_Coord x, y;
   char buf[PATH_MAX];

   if (!enlil_data->library) return;

   if (menu) evas_object_del(menu);

   menu = elm_menu_add(enlil_data->win->win);
   if (!enlil_library_netsync_account_get(enlil_data->library))
   {
      elm_menu_item_add(menu, NULL, NULL, D_("Set the Flickr account"),
                        _account_set_cb, NULL);
   }
   else
   {
      snprintf(buf, PATH_MAX, D_("Flickr account : %s"),
               enlil_library_netsync_account_get(enlil_data->library));
      elm_menu_item_add(menu, NULL, NULL, buf, NULL, NULL);
      elm_menu_item_add(menu, NULL, NULL, D_("Change the Flickr account"),
                        _account_set_cb, NULL);
   }

   evas_object_show(menu);
   evas_pointer_output_xy_get(evas_object_evas_get(obj), &x, &y);
   elm_menu_move(menu, x, y);
}

static void
_account_set_cb(void *data, Evas_Object *obj, void *event_info)
{
   Evas_Object *fr, *tb, *inwin, *lbl, *sc, *text, *bt, *hbox, *img, *entry;
   char buf[PATH_MAX];

   inwin = elm_win_inwin_add(enlil_data->win->win);
   set_account.inwin = inwin;
   evas_object_show(inwin);

   fr = elm_frame_add(enlil_data->win->win);
   elm_frame_label_set(fr, D_("Set the Flick account."));
   evas_object_size_hint_weight_set(fr, 1.0, 1.0);
   evas_object_size_hint_align_set(fr, 0.0, 0.0);
   evas_object_show(fr);

   tb = elm_table_add(enlil_data->win->win);
   elm_table_padding_set(tb, 10, 10);
   evas_object_size_hint_weight_set(tb, 1.0, 1.0);
   evas_object_size_hint_align_set(tb, -1.0, -1.0);
   evas_object_show(tb);

   //steps
   lbl = elm_label_add(enlil_data->win->win);
   elm_label_label_set(
                       lbl,
                       D_("<b>STEP 1 :</b> Open the web page in your favorite web browser."));
   evas_object_size_hint_weight_set(lbl, 0.0, 0.0);
   evas_object_size_hint_align_set(lbl, -1.0, 0.0);
   evas_object_show(lbl);
   elm_table_pack(tb, lbl, 0, 0, 2, 1);

   entry = elm_scrolled_entry_add(enlil_data->win->win);
   elm_scrolled_entry_single_line_set(entry, EINA_TRUE);
   elm_scrolled_entry_entry_set(entry, enlil_flickr_auth_url_get());
   evas_object_size_hint_weight_set(entry, 1.0, 0.0);
   evas_object_size_hint_align_set(entry, -1.0, 0.0);
   evas_object_show(entry);
   elm_table_pack(tb, entry, 0, 1, 2, 1);

   img = elm_image_add(obj);
   elm_image_file_set(img, Theme, "flickr/set_account_0");
   evas_object_size_hint_weight_set(img, 1.0, 1.0);
   evas_object_size_hint_align_set(img, -1.0, -1.0);
   evas_object_show(img);
   elm_table_pack(tb, img, 0, 2, 2, 1);

   snprintf(
            buf,
            PATH_MAX,
            D_("<b>STEP 2 :</b> Authorize Enki to access to your account."));
   lbl = elm_label_add(enlil_data->win->win);
   elm_label_label_set(lbl, buf);
   evas_object_size_hint_weight_set(lbl, 0.0, 0.0);
   evas_object_size_hint_align_set(lbl, -1.0, 0.0);
   evas_object_show(lbl);
   elm_table_pack(tb, lbl, 0, 3, 2, 1);

   img = elm_image_add(obj);
   elm_image_file_set(img, Theme, "flickr/set_account_1");
   evas_object_size_hint_weight_set(img, 0.0, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(img, -1.0, -1.0);
   evas_object_show(img);
   elm_table_pack(tb, img, 0, 4, 2, 1);

   snprintf(
            buf,
            PATH_MAX,
            D_("<b>STEP 3 :</b> Copy the code in the followed text area."));
   lbl = elm_label_add(enlil_data->win->win);
   elm_label_label_set(lbl, buf);
   evas_object_size_hint_weight_set(lbl, 0.0, 0.0);
   evas_object_size_hint_align_set(lbl, -1.0, 0.0);
   evas_object_show(lbl);
   elm_table_pack(tb, lbl, 0, 5, 2, 1);
   //

   lbl = elm_label_add(enlil_data->win->win);
   elm_label_label_set(lbl, D_("The 9-digit code : "));
   evas_object_size_hint_weight_set(lbl, 0.0, 0.0);
   evas_object_size_hint_align_set(lbl, -1.0, 0.0);
   evas_object_show(lbl);
   elm_table_pack(tb, lbl, 0, 6, 1, 1);

   sc = elm_scroller_add(enlil_data->win->win);
   evas_object_size_hint_weight_set(sc, 1.0, 0.0);
   evas_object_size_hint_align_set(sc, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_scroller_content_min_limit(sc, EINA_FALSE, EINA_TRUE);
   elm_scroller_policy_set(sc, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
   elm_scroller_bounce_set(sc, 0, 0);
   evas_object_show(sc);
   elm_table_pack(tb, sc, 1, 6, 1, 1);

   text = elm_entry_add(enlil_data->win->win);
   set_account.entry = text;
   elm_entry_entry_set(text, "");
   elm_entry_single_line_set(text, 1);
   evas_object_size_hint_weight_set(text, EVAS_HINT_EXPAND, 1.0);
   evas_object_size_hint_align_set(text, 1.0, 0.5);
   elm_scroller_content_set(sc, text);
   evas_object_show(text);

   hbox = elm_box_add(enlil_data->win->win);
   elm_box_horizontal_set(hbox, 1);
   evas_object_size_hint_weight_set(hbox, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(hbox, 1.0, 0.0);
   evas_object_show(hbox);
   elm_table_pack(tb, hbox, 1, 7, 1, 1);

   bt = elm_button_add(enlil_data->win->win);
   elm_button_label_set(bt, D_("Cancel"));
   evas_object_smart_callback_add(bt, "clicked", _account_set_cancel_cb, NULL);
   evas_object_size_hint_align_set(bt, 1.0, 0.0);
   evas_object_show(bt);
   elm_box_pack_end(hbox, bt);

   bt = elm_button_add(enlil_data->win->win);
   elm_button_label_set(bt, D_("Apply"));
   evas_object_smart_callback_add(bt, "clicked", _account_set_apply_cb, text);
   evas_object_size_hint_align_set(bt, 1.0, 0.0);
   evas_object_show(bt);
   elm_box_pack_end(hbox, bt);
   elm_object_disabled_set(bt, EINA_TRUE);

   evas_object_smart_callback_add(text, "changed",
                                  _account_set_entry_changed_cb, bt);

   elm_frame_content_set(fr, tb);
   elm_win_inwin_content_set(inwin, fr);
}

static void
_account_set_apply_cb(void *data, Evas_Object *obj, void *event_info)
{
   if (enlil_data->library)
   {
      enlil_library_netsync_account_set(enlil_data->library,
                                        elm_entry_entry_get(data));
      //enlil_library_flickr_auth_token_set(enlil_data->library, NULL);
      enlil_flickr_job_reinit_prepend(enlil_data->library);
   }
   evas_object_del(set_account.inwin);
}

static void
_account_set_cancel_cb(void *data, Evas_Object *obj, void *event_info)
{
   evas_object_del(set_account.inwin);
}

static void
_account_set_entry_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
   const char *s = elm_entry_entry_get(obj);

   if (strlen(s) == 11 && s[0] >= 48 && s[0] <= 57 && s[1] >= 48 && s[1] <= 57
            && s[2] >= 48 && s[2] <= 57 && s[4] >= 48 && s[4] <= 57 && s[5]
            >= 48 && s[5] <= 57 && s[6] >= 48 && s[6] <= 57 && s[8] >= 48
            && s[8] <= 57 && s[9] >= 48 && s[9] <= 57 && s[10] >= 48 && s[10]
            <= 57 && s[3] == '-' && s[7] == '-')
   {
      elm_object_disabled_set(data, EINA_FALSE);
   }
   else
      elm_object_disabled_set(data, EINA_TRUE);
}

