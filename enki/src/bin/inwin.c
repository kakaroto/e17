#include "main.h"

static void _bt_tag_new_cancel_cb(void *data, Evas_Object *obj, void *event_info);
static void _bt_tag_new_apply_cb(void *data, Evas_Object *obj, void *event_info);
static void _bt_album_tag_new_apply_cb(void *data, Evas_Object *obj, void *event_info);

static void _bt_collection_new_cancel_cb(void *data, Evas_Object *obj, void *event_info);
static void _bt_collection_new_apply_cb(void *data, Evas_Object *obj, void *event_info);

static void _bt_album_new_cancel_cb(void *data, Evas_Object *obj, void *event_info);
static void _bt_album_new_apply_cb(void *data, Evas_Object *obj, void *event_info);

static void _bt_photo_delete_cancel_cb(void *data, Evas_Object *obj, void *event_info);
static void _bt_photo_delete_apply_cb(void *data, Evas_Object *obj, void *event_info);

static void _bt_album_delete_cancel_cb(void *data, Evas_Object *obj, void *event_info);
static void _bt_album_delete_apply_cb(void *data, Evas_Object *obj, void *event_info);

static Elm_Genlist_Item_Class itc_album;
static char *_gl_album_label_get(const void *data, Evas_Object *obj, const char *part);
static void _gl_album_sel(void *data, Evas_Object *obj, void *event_info);

static void _bt_photo_move_album_cancel_cb(void *data, Evas_Object *obj, void *event_info);
static void _bt_photo_move_album_apply_cb(void *data, Evas_Object *obj, void *event_info);

static void _bt_photo_save_dont_cb(void *data, Evas_Object *obj, void *event_info);
static void _bt_photo_save_cancel_cb(void *data, Evas_Object *obj, void *event_info);
static void _bt_photo_save_save_cb(void *data, Evas_Object *obj, void *event_info);

static void _bt_album_rename_cancel_cb(void *data, Evas_Object *obj, void *event_info);
static void _bt_album_rename_apply_cb(void *data, Evas_Object *obj, void *event_info);

static void _bt_save_as_file_exists_cancel_cb(void *data, Evas_Object *obj, void *event_info);
static void _bt_save_as_file_exists_apply_cb(void *data, Evas_Object *obj, void *event_info);

static void _bt_preferences_cancel_cb(void *data, Evas_Object *obj, void *event_info);
static void _bt_preferences_apply_cb(void *data, Evas_Object *obj, void *event_info);

Inwin *inwin_tag_new_new(Inwin_Del del_cb, void *data, Eina_List *photos)
{
   Evas_Object *hbox, *fr, *fr2, *lbl, *text, *sc, *bt, *tb;

   ASSERT_RETURN(del_cb != NULL);
   ASSERT_RETURN(data != NULL);
   ASSERT_RETURN(photos != NULL);

   Inwin *inwin = calloc(1, sizeof(Inwin));
   inwin->type = INWIN_TAG_NEW;
   inwin->del_cb = del_cb;
   inwin->data = data;
   inwin->photos = photos;

   inwin->inwin = elm_notify_add(enlil_data->win->win);
   elm_notify_repeat_events_set(inwin->inwin, EINA_FALSE);
   elm_notify_orient_set(inwin->inwin, ELM_NOTIFY_ORIENT_TOP);
   evas_object_show(inwin->inwin);

   fr = elm_frame_add(enlil_data->win->win);
   elm_frame_label_set(fr,D_("Create a new tag"));
   evas_object_size_hint_weight_set(fr, 1.0, 1.0);
   evas_object_size_hint_align_set(fr, 0.0, 0.0);
   evas_object_show(fr);

   tb = elm_table_add(enlil_data->win->win);
   elm_table_padding_set(tb, 10, 10);
   evas_object_size_hint_weight_set(tb, 1.0, 1.0);
   evas_object_size_hint_align_set(tb, -1.0, 0.0);
   evas_object_show(tb);

   fr2 = elm_frame_add(enlil_data->win->win);
   elm_object_style_set(fr2, "outdent_top");
   elm_frame_label_set(fr2,"");
   evas_object_size_hint_weight_set(fr2, 1.0, 0.0);
   evas_object_size_hint_align_set(fr2, 0.5, 0.0);
   evas_object_show(fr2);
   elm_table_pack(tb, fr2, 0, 0, 2, 1);


   lbl = elm_label_add(enlil_data->win->win);
   elm_label_label_set(lbl, D_("A tag is associated to a list of photos. <br>For example you can create a tag \"My dog\" and <br>associate this tag to all the photos where your dog is. <br>Then if you want display the photos with your dog <br>you can use the tag."));
   evas_object_size_hint_weight_set(lbl, 1.0, 1.0);
   evas_object_size_hint_align_set(lbl, -1.0, 0.0);
   //elm_label_line_wrap_set(lbl, 1);
   evas_object_show(lbl);
   elm_frame_content_set(fr2, lbl);


   lbl = elm_label_add(enlil_data->win->win);
   elm_label_label_set(lbl, D_("The name : "));
   evas_object_size_hint_weight_set(lbl, 0.0, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(lbl, -1.0, -1.0);
   evas_object_show(lbl);
   elm_table_pack(tb, lbl, 0, 1, 1, 1);

   sc = elm_scroller_add(enlil_data->win->win);
   evas_object_size_hint_weight_set(sc, 1.0, 1.0);
   evas_object_size_hint_align_set(sc, -1.0, -1.0);
   elm_scroller_policy_set(sc, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
   elm_scroller_bounce_set(sc, 0, 0);
   evas_object_show(sc);
   elm_table_pack(tb, sc, 1, 1, 1, 1);

   text = elm_entry_add(enlil_data->win->win);
   inwin->entry = text;
   elm_entry_entry_set(text, "");
   elm_entry_single_line_set(text, 1);
   evas_object_size_hint_weight_set(text, EVAS_HINT_EXPAND, 0.0);
   elm_scroller_content_set(sc,text);
   evas_object_show(text);

   hbox = elm_box_add(enlil_data->win->win);
   elm_box_horizontal_set(hbox, 1);
   evas_object_size_hint_weight_set(hbox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(hbox, 1.0, 0.0);
   evas_object_show(hbox);
   elm_table_pack(tb, hbox, 1, 2, 1, 1);

   bt = elm_button_add(enlil_data->win->win);
   elm_button_label_set(bt, D_("Cancel"));
   evas_object_smart_callback_add(bt, "clicked", _bt_tag_new_cancel_cb, inwin);
   evas_object_size_hint_align_set(bt, 1.0, 0.0);
   evas_object_show(bt);
   elm_box_pack_end(hbox, bt);

   bt = elm_button_add(enlil_data->win->win);
   elm_button_label_set(bt, D_("Apply"));
   evas_object_smart_callback_add(bt, "clicked", _bt_tag_new_apply_cb, inwin);
   evas_object_size_hint_align_set(bt, 1.0, 0.0);
   evas_object_show(bt);
   elm_box_pack_end(hbox, bt);

   elm_frame_content_set(fr, tb);
   elm_notify_content_set(inwin->inwin, fr);

   return inwin;
}

Inwin *inwin_album_tag_new_new(Inwin_Del del_cb, void *data, Enlil_Album *album)
{
   Evas_Object *hbox, *fr, *fr2, *lbl, *text, *sc, *bt, *tb;

   ASSERT_RETURN(del_cb != NULL);
   ASSERT_RETURN(data != NULL);
   ASSERT_RETURN(album != NULL);

   Inwin *inwin = calloc(1, sizeof(Inwin));
   inwin->type = INWIN_ALBUM_TAG_NEW;
   inwin->del_cb = del_cb;
   inwin->data = data;
   inwin->album = album;

   inwin->inwin = elm_notify_add(enlil_data->win->win);
   elm_notify_repeat_events_set(inwin->inwin, EINA_FALSE);
   elm_notify_orient_set(inwin->inwin, ELM_NOTIFY_ORIENT_TOP);
   evas_object_show(inwin->inwin);

   fr = elm_frame_add(enlil_data->win->win);
   elm_frame_label_set(fr,D_("Create a new tag"));
   evas_object_size_hint_weight_set(fr, 1.0, 1.0);
   evas_object_size_hint_align_set(fr, 0.0, 0.0);
   evas_object_show(fr);

   tb = elm_table_add(enlil_data->win->win);
   elm_table_padding_set(tb, 10, 10);
   evas_object_size_hint_weight_set(tb, 1.0, 1.0);
   evas_object_size_hint_align_set(tb, -1.0, 0.0);
   evas_object_show(tb);

   fr2 = elm_frame_add(enlil_data->win->win);
   elm_object_style_set(fr2, "outdent_top");
   elm_frame_label_set(fr2,"");
   evas_object_size_hint_weight_set(fr2, 1.0, 0.0);
   evas_object_size_hint_align_set(fr2, 0.5, 0.0);
   evas_object_show(fr2);
   elm_table_pack(tb, fr2, 0, 0, 2, 1);


   lbl = elm_label_add(enlil_data->win->win);
   elm_label_label_set(lbl, D_("A tag is associated to a list of photos. <br>For example you can create a tag \"My dog\" and <br>associate this tag to all the photos where your dog is. <br>Then if you want display the photos with your dog <br>you can use the tag."));
   evas_object_size_hint_weight_set(lbl, 1.0, 1.0);
   evas_object_size_hint_align_set(lbl, -1.0, 0.0);
   //elm_label_line_wrap_set(lbl, 1);
   evas_object_show(lbl);
   elm_frame_content_set(fr2, lbl);


   lbl = elm_label_add(enlil_data->win->win);
   elm_label_label_set(lbl, D_("The name : "));
   evas_object_size_hint_weight_set(lbl, 0.0, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(lbl, -1.0, -1.0);
   evas_object_show(lbl);
   elm_table_pack(tb, lbl, 0, 1, 1, 1);

   sc = elm_scroller_add(enlil_data->win->win);
   evas_object_size_hint_weight_set(sc, 1.0, 1.0);
   evas_object_size_hint_align_set(sc, -1.0, -1.0);
   elm_scroller_policy_set(sc, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
   elm_scroller_bounce_set(sc, 0, 0);
   evas_object_show(sc);
   elm_table_pack(tb, sc, 1, 1, 1, 1);

   text = elm_entry_add(enlil_data->win->win);
   inwin->entry = text;
   elm_entry_entry_set(text, "");
   elm_entry_single_line_set(text, 1);
   evas_object_size_hint_weight_set(text, EVAS_HINT_EXPAND, 0.0);
   elm_scroller_content_set(sc,text);
   evas_object_show(text);

   hbox = elm_box_add(enlil_data->win->win);
   elm_box_horizontal_set(hbox, 1);
   evas_object_size_hint_weight_set(hbox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(hbox, 1.0, 0.0);
   evas_object_show(hbox);
   elm_table_pack(tb, hbox, 1, 2, 1, 1);

   bt = elm_button_add(enlil_data->win->win);
   elm_button_label_set(bt, D_("Cancel"));
   evas_object_smart_callback_add(bt, "clicked", _bt_tag_new_cancel_cb, inwin);
   evas_object_size_hint_align_set(bt, 1.0, 0.0);
   evas_object_show(bt);
   elm_box_pack_end(hbox, bt);

   bt = elm_button_add(enlil_data->win->win);
   elm_button_label_set(bt, D_("Apply"));
   evas_object_smart_callback_add(bt, "clicked", _bt_album_tag_new_apply_cb, inwin);
   evas_object_size_hint_align_set(bt, 1.0, 0.0);
   evas_object_show(bt);
   elm_box_pack_end(hbox, bt);

   elm_frame_content_set(fr, tb);
   elm_notify_content_set(inwin->inwin, fr);

   return inwin;
}

Inwin *inwin_collection_new_new(Inwin_Del del_cb, void *data, Enlil_Album *album)
{
   Evas_Object *hbox, *fr, *fr2, *lbl, *sc, *bt, *tb, *text;

   ASSERT_RETURN(del_cb != NULL);
   ASSERT_RETURN(data != NULL);
   ASSERT_RETURN(album != NULL);

   Inwin *inwin = calloc(1, sizeof(Inwin));
   inwin->type = INWIN_COLLECTION_NEW;
   inwin->del_cb = del_cb;
   inwin->data = data;
   inwin->album = album;

   //create inwin & file selector
   inwin->inwin = elm_notify_add(enlil_data->win->win);
   elm_notify_repeat_events_set(inwin->inwin, EINA_FALSE);
   elm_notify_orient_set(inwin->inwin, ELM_NOTIFY_ORIENT_TOP);
   evas_object_show(inwin->inwin);

   fr = elm_frame_add(enlil_data->win->win);
   elm_frame_label_set(fr,D_("Create a new collection"));
   evas_object_size_hint_weight_set(fr, 1.0, 1.0);
   evas_object_size_hint_align_set(fr, 0.0, 0.0);
   evas_object_show(fr);

   tb = elm_table_add(enlil_data->win->win);
   elm_table_padding_set(tb, 10, 10);
   evas_object_size_hint_weight_set(tb, 1.0, 1.0);
   evas_object_size_hint_align_set(tb, -1.0, 0.0);
   evas_object_show(tb);

   fr2 = elm_frame_add(enlil_data->win->win);
   elm_object_style_set(fr2, "outdent_top");
   elm_frame_label_set(fr2,"");
   evas_object_size_hint_weight_set(fr2, 1.0, 0.0);
   evas_object_size_hint_align_set(fr2, 0.5, 0.0);
   evas_object_show(fr2);
   elm_table_pack(tb, fr2, 0, 0, 2, 1);


   lbl = elm_label_add(enlil_data->win->win);
   elm_label_label_set(lbl, D_("A collection is a set of albums. For example if <br>you had visited some cities in Europa you <br>can create one album per city ( Paris, London, <br>Berlin ... ), a collection named \"Europa\" and puts <br>theses albums inside."));
   evas_object_size_hint_weight_set(lbl, 1.0, 1.0);
   evas_object_size_hint_align_set(lbl, -1.0, 0.0);
   //elm_label_line_wrap_set(lbl, 1);
   evas_object_show(lbl);
   elm_frame_content_set(fr2, lbl);


   lbl = elm_label_add(enlil_data->win->win);
   elm_label_label_set(lbl, D_("The name : "));
   evas_object_size_hint_weight_set(lbl, 0.0, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(lbl, -1.0, -1.0);
   evas_object_show(lbl);
   elm_table_pack(tb, lbl, 0, 1, 1, 1);

   sc = elm_scroller_add(enlil_data->win->win);
   evas_object_size_hint_weight_set(sc, 1.0, 1.0);
   evas_object_size_hint_align_set(sc, -1.0, -1.0);
   elm_scroller_policy_set(sc, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
   elm_scroller_bounce_set(sc, 0, 0);
   evas_object_show(sc);
   elm_table_pack(tb, sc, 1, 1, 1, 1);

   text = elm_entry_add(enlil_data->win->win);
   inwin->entry = text;
   elm_entry_entry_set(text, "");
   elm_entry_single_line_set(text, 1);
   evas_object_size_hint_weight_set(text, EVAS_HINT_EXPAND, 0.0);
   elm_scroller_content_set(sc,text);
   evas_object_show(text);

   hbox = elm_box_add(enlil_data->win->win);
   elm_box_horizontal_set(hbox, 1);
   evas_object_size_hint_weight_set(hbox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(hbox, 1.0, 0.0);
   evas_object_show(hbox);
   elm_table_pack(tb, hbox, 1, 2, 1, 1);

   bt = elm_button_add(enlil_data->win->win);
   elm_button_label_set(bt, D_("Cancel"));
   evas_object_smart_callback_add(bt, "clicked", _bt_collection_new_cancel_cb, inwin);
   evas_object_size_hint_align_set(bt, 1.0, 0.0);
   evas_object_show(bt);
   elm_box_pack_end(hbox, bt);

   bt = elm_button_add(enlil_data->win->win);
   elm_button_label_set(bt, D_("Apply"));
   evas_object_smart_callback_add(bt, "clicked", _bt_collection_new_apply_cb, inwin);
   evas_object_size_hint_align_set(bt, 1.0, 0.0);
   evas_object_show(bt);
   elm_box_pack_end(hbox, bt);

   elm_frame_content_set(fr, tb);
   elm_notify_content_set(inwin->inwin, fr);

   return inwin;
}

Inwin *inwin_album_new_new(Inwin_Del del_cb, void *data)
{
   Evas_Object *hbox, *fr, *lbl, *text, *sc, *bt, *tb;
   Inwin *inwin = calloc(1, sizeof(Inwin));
   inwin->type = INWIN_ALBUM_NEW;
   inwin->del_cb = del_cb;
   inwin->data = data;

   //create inwin & file selector
   inwin->inwin = elm_notify_add(enlil_data->win->win);
   elm_notify_repeat_events_set(inwin->inwin, EINA_FALSE);
   elm_notify_orient_set(inwin->inwin, ELM_NOTIFY_ORIENT_TOP);
   evas_object_show(inwin->inwin);

   fr = elm_frame_add(enlil_data->win->win);
   elm_frame_label_set(fr,D_("Create a new album"));
   evas_object_size_hint_weight_set(fr, 1.0, 1.0);
   evas_object_size_hint_align_set(fr, 0.0, 0.0);
   evas_object_show(fr);

   tb = elm_table_add(enlil_data->win->win);
   elm_table_padding_set(tb, 10, 10);
   evas_object_size_hint_weight_set(tb, 1.0, 1.0);
   evas_object_size_hint_align_set(tb, 0.0, 0.0);
   evas_object_show(tb);

   lbl = elm_label_add(enlil_data->win->win);
   elm_label_label_set(lbl, D_("The name : "));
   evas_object_size_hint_weight_set(lbl, 0.0, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(lbl, -1.0, -1.0);
   evas_object_show(lbl);
   elm_table_pack(tb, lbl, 0, 0, 1, 1);

   sc = elm_scroller_add(enlil_data->win->win);
   evas_object_size_hint_weight_set(sc, 1.0, 1.0);
   evas_object_size_hint_align_set(sc, -1.0, -1.0);
   elm_scroller_policy_set(sc, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
   elm_scroller_bounce_set(sc, 0, 0);
   evas_object_show(sc);
   elm_table_pack(tb, sc, 1, 0, 1, 1);

   text = elm_entry_add(enlil_data->win->win);
   inwin->entry = text;
   elm_entry_entry_set(text, "");
   elm_entry_single_line_set(text, 1);
   evas_object_size_hint_weight_set(text, EVAS_HINT_EXPAND, 0.0);
   elm_scroller_content_set(sc,text);
   evas_object_show(text);

   hbox = elm_box_add(enlil_data->win->win);
   elm_box_horizontal_set(hbox, 1);
   evas_object_size_hint_weight_set(hbox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(hbox, 1.0, 0.0);
   evas_object_show(hbox);
   elm_table_pack(tb, hbox, 1, 1, 1, 1);

   bt = elm_button_add(enlil_data->win->win);
   elm_button_label_set(bt, D_("Cancel"));
   evas_object_smart_callback_add(bt, "clicked", _bt_album_new_cancel_cb, inwin);
   evas_object_size_hint_align_set(bt, 1.0, 0.0);
   evas_object_show(bt);
   elm_box_pack_end(hbox, bt);

   bt = elm_button_add(enlil_data->win->win);
   elm_button_label_set(bt, D_("Apply"));
   evas_object_smart_callback_add(bt, "clicked", _bt_album_new_apply_cb, inwin);
   evas_object_size_hint_align_set(bt, 1.0, 0.0);
   evas_object_show(bt);
   elm_box_pack_end(hbox, bt);

   elm_frame_content_set(fr, tb);
   elm_notify_content_set(inwin->inwin, fr);

   return inwin;
}


Inwin *inwin_photo_delete_new(Evas_Object *win, Inwin_Del del_cb, void *data, Eina_List *photos)
{
   Evas_Object *hbox, *fr, *bt, *tb, *lbl;
   char buf[PATH_MAX], buf2[PATH_MAX], buf3[PATH_MAX];
   Eina_List *l;
   Enlil_Photo *photo;

   ASSERT_RETURN(photos != NULL);

   Inwin *inwin = calloc(1, sizeof(Inwin));
   inwin->type = INWIN_PHOTO_DELETE;
   inwin->del_cb = del_cb;
   inwin->data = data;
   inwin->photos = photos;

   inwin->inwin = elm_notify_add(win);
   elm_notify_repeat_events_set(inwin->inwin, EINA_FALSE);
   elm_notify_orient_set(inwin->inwin, ELM_NOTIFY_ORIENT_TOP);
   evas_object_show(inwin->inwin);

   fr = elm_frame_add(win);
   elm_frame_label_set(fr,D_("Delete a photo"));
   evas_object_size_hint_weight_set(fr, 1.0, 1.0);
   evas_object_size_hint_align_set(fr, 0.0, 0.0);
   evas_object_show(fr);

   tb = elm_table_add(win);
   elm_table_padding_set(tb, 10, 10);
   evas_object_size_hint_weight_set(tb, 1.0, 1.0);
   evas_object_size_hint_align_set(tb, 0.0, 0.0);
   evas_object_show(tb);

   *buf3 = '\0';
   EINA_LIST_FOREACH(photos, l, photo)
     {
	snprintf(buf2, PATH_MAX, "%s<br>%s", buf3, enlil_photo_name_get(photo));
	strncpy(buf3, buf2, PATH_MAX);
     }

   snprintf(buf, PATH_MAX,
	 D_("You are about to delete %d photo(s) : <br>%s <br><br>Do you want to continue ?"),
	 eina_list_count(inwin->photos), buf2);
   lbl = elm_label_add(win);
   elm_label_label_set(lbl, buf);
   evas_object_size_hint_weight_set(lbl, 0.0, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(lbl, -1.0, -1.0);
   evas_object_show(lbl);
   elm_table_pack(tb, lbl, 0, 0, 2, 1);

   hbox = elm_box_add(win);
   elm_box_horizontal_set(hbox, 1);
   evas_object_size_hint_weight_set(hbox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(hbox, 1.0, 0.0);
   evas_object_show(hbox);
   elm_table_pack(tb, hbox, 1, 1, 1, 1);

   bt = elm_button_add(win);
   elm_button_label_set(bt, D_("Cancel"));
   evas_object_smart_callback_add(bt, "clicked", _bt_photo_delete_cancel_cb, inwin);
   evas_object_size_hint_align_set(bt, 1.0, 0.0);
   evas_object_show(bt);
   elm_box_pack_end(hbox, bt);

   bt = elm_button_add(win);
   elm_button_label_set(bt, D_("Apply"));
   evas_object_smart_callback_add(bt, "clicked", _bt_photo_delete_apply_cb, inwin);
   evas_object_size_hint_align_set(bt, 1.0, 0.0);
   evas_object_show(bt);
   elm_box_pack_end(hbox, bt);

   elm_frame_content_set(fr, tb);
   elm_notify_content_set(inwin->inwin, fr);

   return inwin;
}

Inwin *inwin_save_as_file_exists_new(Inwin_Del del_cb, Inwin_Apply apply_cb, void *data, const char *file)
{
   Evas_Object *hbox, *fr, *bt, *tb, *lbl;
   char buf[PATH_MAX];

   ASSERT_RETURN(file != NULL);

   Inwin *inwin = calloc(1, sizeof(Inwin));
   inwin->type = INWIN_SAVE_AS_FILE_EXISTS;
   inwin->del_cb = del_cb;
   inwin->apply_cb = apply_cb;
   inwin->data = data;
   inwin->file = eina_stringshare_add(buf);

   inwin->inwin = elm_notify_add(enlil_data->win->win);
   elm_notify_repeat_events_set(inwin->inwin, EINA_FALSE);
   elm_notify_orient_set(inwin->inwin, ELM_NOTIFY_ORIENT_TOP);
   evas_object_show(inwin->inwin);

   fr = elm_frame_add(enlil_data->win->win);
   elm_frame_label_set(fr,D_("Erase a photo"));
   evas_object_size_hint_weight_set(fr, 1.0, 1.0);
   evas_object_size_hint_align_set(fr, 0.0, 0.0);
   evas_object_show(fr);

   tb = elm_table_add(enlil_data->win->win);
   elm_table_padding_set(tb, 10, 10);
   evas_object_size_hint_weight_set(tb, 1.0, 1.0);
   evas_object_size_hint_align_set(tb, 0.0, 0.0);
   evas_object_show(tb);

   snprintf(buf, PATH_MAX,
	 D_("You are about to erase the photo <br>%s. Do you want to continue ?"), file);
   lbl = elm_label_add(enlil_data->win->win);
   elm_label_label_set(lbl, buf);
   evas_object_size_hint_weight_set(lbl, 0.0, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(lbl, -1.0, -1.0);
   evas_object_show(lbl);
   elm_table_pack(tb, lbl, 0, 0, 2, 1);

   hbox = elm_box_add(enlil_data->win->win);
   elm_box_horizontal_set(hbox, 1);
   evas_object_size_hint_weight_set(hbox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(hbox, 1.0, 0.0);
   evas_object_show(hbox);
   elm_table_pack(tb, hbox, 1, 1, 1, 1);

   bt = elm_button_add(enlil_data->win->win);
   elm_button_label_set(bt, D_("Cancel"));
   evas_object_smart_callback_add(bt, "clicked", _bt_save_as_file_exists_cancel_cb, inwin);
   evas_object_size_hint_align_set(bt, 1.0, 0.0);
   evas_object_show(bt);
   elm_box_pack_end(hbox, bt);

   bt = elm_button_add(enlil_data->win->win);
   elm_button_label_set(bt, D_("Apply"));
   evas_object_smart_callback_add(bt, "clicked", _bt_save_as_file_exists_apply_cb, inwin);
   evas_object_size_hint_align_set(bt, 1.0, 0.0);
   evas_object_show(bt);
   elm_box_pack_end(hbox, bt);

   elm_frame_content_set(fr, tb);
   elm_notify_content_set(inwin->inwin, fr);

   return inwin;
}



Inwin *inwin_album_delete_new(Inwin_Del del_cb, void *data, Enlil_Album *album)
{
   Evas_Object *hbox, *fr, *bt, *tb, *lbl;
   char buf[PATH_MAX];

   ASSERT_RETURN(album != NULL);

   Inwin *inwin = calloc(1, sizeof(Inwin));
   inwin->type = INWIN_ALBUM_DELETE;
   inwin->del_cb = del_cb;
   inwin->data = data;
   inwin->album = album;

   inwin->inwin = elm_notify_add(enlil_data->win->win);
   elm_notify_repeat_events_set(inwin->inwin, EINA_FALSE);
   elm_notify_orient_set(inwin->inwin, ELM_NOTIFY_ORIENT_TOP);
   evas_object_show(inwin->inwin);

   fr = elm_frame_add(enlil_data->win->win);
   elm_frame_label_set(fr,D_("Delete an album"));
   evas_object_size_hint_weight_set(fr, 1.0, 1.0);
   evas_object_size_hint_align_set(fr, 0.0, 0.0);
   evas_object_show(fr);

   tb = elm_table_add(enlil_data->win->win);
   elm_table_padding_set(tb, 10, 10);
   evas_object_size_hint_weight_set(tb, 1.0, 1.0);
   evas_object_size_hint_align_set(tb, 0.0, 0.0);
   evas_object_show(tb);

   snprintf(buf, PATH_MAX,
	 D_("You are about to delete the album <br>%s and its content. <br>Do you want to continue ?"),
	 enlil_album_name_get(album));
   lbl = elm_label_add(enlil_data->win->win);
   elm_label_label_set(lbl, buf);
   evas_object_size_hint_weight_set(lbl, 0.0, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(lbl, -1.0, -1.0);
   evas_object_show(lbl);
   elm_table_pack(tb, lbl, 0, 0, 2, 1);

   hbox = elm_box_add(enlil_data->win->win);
   elm_box_horizontal_set(hbox, 1);
   evas_object_size_hint_weight_set(hbox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(hbox, 1.0, 0.0);
   evas_object_show(hbox);
   elm_table_pack(tb, hbox, 1, 1, 1, 1);

   bt = elm_button_add(enlil_data->win->win);
   elm_button_label_set(bt, D_("Cancel"));
   evas_object_smart_callback_add(bt, "clicked", _bt_album_delete_cancel_cb, inwin);
   evas_object_size_hint_align_set(bt, 1.0, 0.0);
   evas_object_show(bt);
   elm_box_pack_end(hbox, bt);

   bt = elm_button_add(enlil_data->win->win);
   elm_button_label_set(bt, D_("Apply"));
   evas_object_smart_callback_add(bt, "clicked", _bt_album_delete_apply_cb, inwin);
   evas_object_size_hint_align_set(bt, 1.0, 0.0);
   evas_object_show(bt);
   elm_box_pack_end(hbox, bt);

   elm_frame_content_set(fr, tb);
   elm_notify_content_set(inwin->inwin, fr);

   return inwin;
}

Inwin *inwin_album_rename_new(Inwin_Del del_cb, void *data, Enlil_Album *album)
{
   Evas_Object *hbox, *fr, *fr2, *lbl, *text, *sc, *bt, *tb;
   char buf[PATH_MAX];

   ASSERT_RETURN(del_cb != NULL);
   ASSERT_RETURN(data != NULL);
   ASSERT_RETURN(album != NULL);

   Inwin *inwin = calloc(1, sizeof(Inwin));
   inwin->type = INWIN_ALBUM_RENAME;
   inwin->del_cb = del_cb;
   inwin->data = data;
   inwin->album = album;

   inwin->inwin = elm_notify_add(enlil_data->win->win);
   elm_notify_repeat_events_set(inwin->inwin, EINA_FALSE);
   elm_notify_orient_set(inwin->inwin, ELM_NOTIFY_ORIENT_TOP);
   evas_object_show(inwin->inwin);

   fr = elm_frame_add(enlil_data->win->win);
   elm_frame_label_set(fr,D_("Rename the album"));
   evas_object_size_hint_weight_set(fr, 1.0, 1.0);
   evas_object_size_hint_align_set(fr, 0.0, 0.0);
   evas_object_show(fr);

   tb = elm_table_add(enlil_data->win->win);
   elm_table_padding_set(tb, 10, 10);
   evas_object_size_hint_weight_set(tb, 1.0, 1.0);
   evas_object_size_hint_align_set(tb, -1.0, 0.0);
   evas_object_show(tb);

   fr2 = elm_frame_add(enlil_data->win->win);
   elm_object_style_set(fr2, "outdent_top");
   elm_frame_label_set(fr2,"");
   evas_object_size_hint_weight_set(fr2, 1.0, 0.0);
   evas_object_size_hint_align_set(fr2, 0.5, 0.0);
   evas_object_show(fr2);
   elm_table_pack(tb, fr2, 0, 0, 2, 1);


   snprintf(buf, PATH_MAX, D_("The current name of the album is <br>      \"%s\""), enlil_album_name_get(album));
   lbl = elm_label_add(enlil_data->win->win);
   elm_label_label_set(lbl, buf);
   evas_object_size_hint_weight_set(lbl, 1.0, 1.0);
   evas_object_size_hint_align_set(lbl, -1.0, 0.0);
   //elm_label_line_wrap_set(lbl, 1);
   evas_object_show(lbl);
   elm_frame_content_set(fr2, lbl);


   lbl = elm_label_add(enlil_data->win->win);
   elm_label_label_set(lbl, D_("The new name : "));
   evas_object_size_hint_weight_set(lbl, 0.0, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(lbl, -1.0, -1.0);
   evas_object_show(lbl);
   elm_table_pack(tb, lbl, 0, 1, 1, 1);

   sc = elm_scroller_add(enlil_data->win->win);
   evas_object_size_hint_weight_set(sc, 1.0, 1.0);
   evas_object_size_hint_align_set(sc, -1.0, -1.0);
   elm_scroller_policy_set(sc, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
   elm_scroller_bounce_set(sc, 0, 0);
   evas_object_show(sc);
   elm_table_pack(tb, sc, 1, 1, 1, 1);

   text = elm_entry_add(enlil_data->win->win);
   inwin->entry = text;
   elm_entry_entry_set(text, "");
   elm_entry_single_line_set(text, 1);
   evas_object_size_hint_weight_set(text, EVAS_HINT_EXPAND, 0.0);
   elm_scroller_content_set(sc,text);
   evas_object_show(text);

   hbox = elm_box_add(enlil_data->win->win);
   elm_box_horizontal_set(hbox, 1);
   evas_object_size_hint_weight_set(hbox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(hbox, 1.0, 0.0);
   evas_object_show(hbox);
   elm_table_pack(tb, hbox, 1, 2, 1, 1);

   bt = elm_button_add(enlil_data->win->win);
   elm_button_label_set(bt, D_("Cancel"));
   evas_object_smart_callback_add(bt, "clicked", _bt_album_rename_cancel_cb, inwin);
   evas_object_size_hint_align_set(bt, 1.0, 0.0);
   evas_object_show(bt);
   elm_box_pack_end(hbox, bt);

   bt = elm_button_add(enlil_data->win->win);
   elm_button_label_set(bt, D_("Apply"));
   evas_object_smart_callback_add(bt, "clicked", _bt_album_rename_apply_cb, inwin);
   evas_object_size_hint_align_set(bt, 1.0, 0.0);
   evas_object_show(bt);
   elm_box_pack_end(hbox, bt);

   elm_frame_content_set(fr, tb);
   elm_notify_content_set(inwin->inwin, fr);

   return inwin;
}

Inwin *inwin_photo_move_album_new(Inwin_Del del_cb, void *data, Eina_List *photos)
{
   Evas_Object *hbox, *fr, *fr2, *lbl, *bt, *tb, *gl;
   char buf[PATH_MAX], buf2[PATH_MAX], buf3[PATH_MAX];
   const Eina_List *l;
   Enlil_Album *album;
   Enlil_Photo *photo;

   ASSERT_RETURN(del_cb != NULL);
   ASSERT_RETURN(data != NULL);
   ASSERT_RETURN(photos != NULL);

   Inwin *inwin = calloc(1, sizeof(Inwin));
   inwin->type = INWIN_PHOTO_MOVE_ALBUM;
   inwin->del_cb = del_cb;
   inwin->data = data;
   inwin->photos = photos;

   inwin->inwin = elm_win_inwin_add(enlil_data->win->win);
   evas_object_show(inwin->inwin);

   fr = elm_frame_add(enlil_data->win->win);
   elm_frame_label_set(fr,D_("Move the photo into an album"));
   evas_object_size_hint_weight_set(fr, 1.0, 1.0);
   evas_object_size_hint_align_set(fr, 0.0, 0.0);
   evas_object_show(fr);

   tb = elm_table_add(enlil_data->win->win);
   elm_table_padding_set(tb, 10, 10);
   evas_object_size_hint_weight_set(tb, 1.0, 1.0);
   evas_object_size_hint_align_set(tb, -1.0, 0.0);
   evas_object_show(tb);

   *buf3 = '\0';
   EINA_LIST_FOREACH(photos, l, photo)
     {
	snprintf(buf2, PATH_MAX, "%s<br>%s", buf3, enlil_photo_name_get(photo));
	strncpy(buf3, buf2, PATH_MAX);
     }

   snprintf(buf, PATH_MAX, D_("Move %d photo(s): <br> %s <br><br>into an new album"), eina_list_count(photos), buf3);
   fr2 = elm_frame_add(enlil_data->win->win);
   elm_object_style_set(fr2, "outdent_top");
   elm_frame_label_set(fr2,"");
   evas_object_size_hint_weight_set(fr2, 1.0, 0.0);
   evas_object_size_hint_align_set(fr2, 0.5, 0.0);
   evas_object_show(fr2);
   elm_table_pack(tb, fr2, 0, 0, 2, 1);


   lbl = elm_label_add(enlil_data->win->win);
   elm_label_label_set(lbl, buf);
   evas_object_size_hint_weight_set(lbl, 1.0, 1.0);
   evas_object_size_hint_align_set(lbl, -1.0, 0.0);
   //elm_label_line_wrap_set(lbl, 1);
   evas_object_show(lbl);
   elm_frame_content_set(fr2, lbl);

   gl = elm_genlist_add(enlil_data->win->win);
   inwin->gl = gl ;
   elm_genlist_horizontal_mode_set(gl, ELM_LIST_SCROLL);
   evas_object_size_hint_weight_set(gl, 1.0, 1.0);
   evas_object_size_hint_align_set(gl, -1.0, -1.0);
   evas_object_show(gl);
   elm_table_pack(tb, gl, 0, 1, 2, 1);

   itc_album.item_style     = "default";
   itc_album.func.label_get = _gl_album_label_get;
   itc_album.func.icon_get  = NULL;
   itc_album.func.state_get = NULL;
   itc_album.func.del       = NULL;

   EINA_LIST_FOREACH(enlil_root_albums_get(enlil_data->root), l, album)
     {
	Enlil_Album_Data *album_data = enlil_album_user_data_get(album);
	album_data->inwin_photo_move_album = inwin;

	album_data->photo_move_album_list_album_item =
	   elm_genlist_item_append(gl, &itc_album,
		 album, NULL, ELM_GENLIST_ITEM_NONE, _gl_album_sel,
		 album);
     }

   hbox = elm_box_add(enlil_data->win->win);
   elm_box_horizontal_set(hbox, 1);
   evas_object_size_hint_weight_set(hbox, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(hbox, 1.0, 0.0);
   evas_object_show(hbox);
   elm_table_pack(tb, hbox, 1, 2, 1, 1);

   bt = elm_button_add(enlil_data->win->win);
   elm_button_label_set(bt, D_("Cancel"));
   evas_object_smart_callback_add(bt, "clicked", _bt_photo_move_album_cancel_cb, inwin);
   evas_object_size_hint_align_set(bt, 1.0, 0.0);
   evas_object_show(bt);
   elm_box_pack_end(hbox, bt);

   bt = elm_button_add(enlil_data->win->win);
   elm_object_disabled_set(bt, 1);
   elm_button_label_set(bt, D_("Apply"));
   inwin->bt_apply = bt;
   evas_object_smart_callback_add(bt, "clicked", _bt_photo_move_album_apply_cb, inwin);
   evas_object_size_hint_align_set(bt, 1.0, 0.0);
   evas_object_show(bt);
   elm_box_pack_end(hbox, bt);

   elm_frame_content_set(fr, tb);
   elm_win_inwin_content_set(inwin->inwin, fr);

   return inwin;
}

Inwin *inwin_preferences_new()
{
   Evas_Object *hbox, *fr, *lbl, *bt, *tb, *sc, *text;

   Inwin *inwin = calloc(1, sizeof(Inwin));
   inwin->type = INWIN_PREFERENCES;

   inwin->inwin = elm_win_inwin_add(enlil_data->win->win);
   evas_object_show(inwin->inwin);

   fr = elm_frame_add(enlil_data->win->win);
   elm_frame_label_set(fr,D_("Preferences"));
   evas_object_size_hint_weight_set(fr, 1.0, 1.0);
   evas_object_size_hint_align_set(fr, 0.0, 0.0);
   evas_object_show(fr);

   tb = elm_table_add(enlil_data->win->win);
   elm_table_padding_set(tb, 10, 10);
   evas_object_size_hint_weight_set(tb, 1.0, 1.0);
   evas_object_size_hint_align_set(tb, -1.0, 0.0);
   evas_object_show(tb);

   lbl = elm_label_add(enlil_data->win->win);
   elm_label_label_set(lbl, D_("Video software : "));
   evas_object_size_hint_weight_set(lbl, 0.0, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(lbl, -1.0, -1.0);
   evas_object_show(lbl);
   elm_table_pack(tb, lbl, 0, 1, 1, 1);

   sc = elm_scroller_add(enlil_data->win->win);
   evas_object_size_hint_weight_set(sc, 1.0, 1.0);
   evas_object_size_hint_align_set(sc, -1.0, -1.0);
   elm_scroller_policy_set(sc, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
   elm_scroller_bounce_set(sc, 0, 0);
   evas_object_show(sc);
   elm_table_pack(tb, sc, 1, 1, 1, 1);

   text = elm_entry_add(enlil_data->win->win);
   inwin->entry = text;
   elm_entry_entry_set(text, media_player);
   elm_entry_single_line_set(text, 1);
   evas_object_size_hint_weight_set(text, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(text, -1.0, 0.0);
   elm_scroller_content_set(sc,text);
   evas_object_show(text);

   hbox = elm_box_add(enlil_data->win->win);
   elm_box_horizontal_set(hbox, 1);
   evas_object_size_hint_weight_set(hbox, -1.0, 1.0);
   evas_object_size_hint_align_set(hbox, -1.0, -1.0);
   evas_object_show(hbox);
   elm_table_pack(tb, hbox, 1, 2, 1, 1);

   bt = elm_button_add(enlil_data->win->win);
   elm_button_label_set(bt, D_("Cancel"));
   evas_object_smart_callback_add(bt, "clicked", _bt_preferences_cancel_cb, inwin);
   evas_object_size_hint_align_set(bt, 1.0, 0.0);
   evas_object_show(bt);
   elm_box_pack_end(hbox, bt);

   bt = elm_button_add(enlil_data->win->win);
   elm_button_label_set(bt, D_("Apply"));
   inwin->bt_apply = bt;
   evas_object_smart_callback_add(bt, "clicked", _bt_preferences_apply_cb, inwin);
   evas_object_size_hint_align_set(bt, 1.0, 0.0);
   evas_object_show(bt);
   elm_box_pack_end(hbox, bt);

   elm_frame_content_set(fr, tb);
   elm_win_inwin_content_set(inwin->inwin, fr);

   return inwin;
}

Inwin *inwin_photo_save_new(Inwin_Del del_cb, Inwin_Apply apply_cb, Inwin_Close close_cb,
      void *data, Enlil_Photo *photo)
{
   Evas_Object *hbox, *fr, *fr2, *lbl, *bt, *tb;
   char buf[PATH_MAX];

   ASSERT_RETURN(apply_cb != NULL);
   ASSERT_RETURN(close_cb != NULL);
   ASSERT_RETURN(data != NULL);
   ASSERT_RETURN(photo != NULL);

   Inwin *inwin = calloc(1, sizeof(Inwin));
   inwin->type = INWIN_PHOTO_SAVE;
   inwin->del_cb = del_cb;
   inwin->apply_cb = apply_cb;
   inwin->close_cb = close_cb;
   inwin->data = data;
   inwin->photo = photo;

   inwin->inwin = elm_notify_add(enlil_data->win->win);
   elm_notify_repeat_events_set(inwin->inwin, EINA_FALSE);
   elm_notify_orient_set(inwin->inwin, ELM_NOTIFY_ORIENT_TOP);
   evas_object_show(inwin->inwin);

   fr = elm_frame_add(enlil_data->win->win);
   elm_frame_label_set(fr,D_("Close a photo"));
   evas_object_size_hint_weight_set(fr, 1.0, 1.0);
   evas_object_size_hint_align_set(fr, 0.0, 0.0);
   evas_object_show(fr);

   tb = elm_table_add(enlil_data->win->win);
   elm_table_padding_set(tb, 10, 10);
   evas_object_size_hint_weight_set(tb, 1.0, 1.0);
   evas_object_size_hint_align_set(tb, -1.0, 0.0);
   evas_object_show(tb);

   fr2 = elm_frame_add(enlil_data->win->win);
   elm_object_style_set(fr2, "outdent_top");
   elm_frame_label_set(fr2,"");
   evas_object_size_hint_weight_set(fr2, 1.0, 0.0);
   evas_object_size_hint_align_set(fr2, 0.5, 0.0);
   evas_object_show(fr2);
   elm_table_pack(tb, fr2, 0, 0, 2, 1);


   snprintf(buf, PATH_MAX, D_("Save the changes to image %s before closing?"), enlil_photo_file_name_get(photo));
   lbl = elm_label_add(enlil_data->win->win);
   elm_label_label_set(lbl, buf);
   evas_object_size_hint_weight_set(lbl, 1.0, 1.0);
   evas_object_size_hint_align_set(lbl, -1.0, 0.0);
   //elm_label_line_wrap_set(lbl, 1);
   evas_object_show(lbl);
   elm_frame_content_set(fr2, lbl);


   hbox = elm_box_add(enlil_data->win->win);
   elm_box_horizontal_set(hbox, 1);
   evas_object_size_hint_weight_set(hbox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(hbox, 1.0, 0.0);
   evas_object_show(hbox);
   elm_table_pack(tb, hbox, 1, 2, 1, 1);

   bt = elm_button_add(enlil_data->win->win);
   elm_button_label_set(bt, D_("Don't save"));
   evas_object_smart_callback_add(bt, "clicked", _bt_photo_save_dont_cb, inwin);
   evas_object_size_hint_align_set(bt, 1.0, 0.0);
   evas_object_show(bt);
   elm_box_pack_end(hbox, bt);

   bt = elm_button_add(enlil_data->win->win);
   elm_button_label_set(bt, D_("Cancel"));
   evas_object_smart_callback_add(bt, "clicked", _bt_photo_save_cancel_cb, inwin);
   evas_object_size_hint_align_set(bt, 1.0, 0.0);
   evas_object_show(bt);
   elm_box_pack_end(hbox, bt);

   bt = elm_button_add(enlil_data->win->win);
   elm_button_label_set(bt, D_("Save"));
   evas_object_smart_callback_add(bt, "clicked", _bt_photo_save_save_cb, inwin);
   evas_object_size_hint_align_set(bt, 1.0, 0.0);
   evas_object_show(bt);
   elm_box_pack_end(hbox, bt);

   elm_frame_content_set(fr, tb);
   elm_notify_content_set(inwin->inwin, fr);

   return inwin;
}

static char *_gl_album_label_get(const void *data, Evas_Object *obj, const char *part)
{
   Enlil_Album *album = (Enlil_Album *)data;

   return strdup(enlil_album_name_get(album));
}

static void _gl_album_sel(void *data, Evas_Object *obj, void *event_info)
{
   Enlil_Album *album = (Enlil_Album *)data;
   Enlil_Album_Data *album_data = enlil_album_user_data_get(album);

   if(album_data->inwin_photo_move_album)
     elm_object_disabled_set(album_data->inwin_photo_move_album->bt_apply, 0);
}

static void _bt_tag_new_cancel_cb(void *data, Evas_Object *obj, void *event_info)
{
   Inwin *inwin = data;
   inwin_free(inwin);
}

static void _bt_tag_new_apply_cb(void *data, Evas_Object *obj, void *event_info)
{
   Inwin *inwin = data;

   const char *s = elm_entry_entry_get(inwin->entry);
   if(s && strlen(s)>0)
     {
	Eina_List *l;
	Enlil_Photo *photo;
	EINA_LIST_FOREACH(inwin->photos, l, photo)
	   enlil_photo_tag_add(photo, s);
     }

   inwin_free(inwin);
}

static void _bt_album_tag_new_apply_cb(void *data, Evas_Object *obj, void *event_info)
{
   Inwin *inwin = data;

   const char *s = elm_entry_entry_get(inwin->entry);
   const char *__s = eina_stringshare_add(s);
   if(s && strlen(s)>0)
     {
	Enlil_Photo *photo;
	Enlil_Photo_Tag *photo_tag;
	Eina_List *l;
	const Eina_List *_l;
	EINA_LIST_FOREACH(enlil_album_photos_get(inwin->album), l, photo)
	  {
	     EINA_LIST_FOREACH(enlil_photo_tags_get(photo), _l, photo_tag)
	       {
		  if(photo_tag->name == __s)
		    break ;
	       }

	     if(!photo_tag)
	       enlil_photo_tag_add(photo, __s);
	  }
     }

   eina_stringshare_del(__s);
   inwin_free(inwin);
}

static void _bt_collection_new_cancel_cb(void *data, Evas_Object *obj, void *event_info)
{
   Inwin *inwin = data;
   inwin_free(inwin);
}


static void _bt_collection_new_apply_cb(void *data, Evas_Object *obj, void *event_info)
{
   Inwin *inwin = data;

   const char *s = elm_entry_entry_get(inwin->entry);
   if(s && strlen(s)>0)
     enlil_album_collection_add(inwin->album, s);

   inwin_free(inwin);
}

static void _bt_album_new_cancel_cb(void *data, Evas_Object *obj, void *event_info)
{
   Inwin *inwin = data;
   inwin_free(inwin);
}


static void _bt_album_new_apply_cb(void *data, Evas_Object *obj, void *event_info)
{
   Inwin *inwin = data;

   const char *s = elm_entry_entry_get(inwin->entry);
   if(s && strlen(s)>0)
     {
	 char buf[PATH_MAX];
	 snprintf(buf, PATH_MAX, "%s/%s", enlil_root_path_get(enlil_data->root), s);
	 ecore_file_mkdir(buf);
     }

   inwin_free(inwin);
}

static void _bt_preferences_cancel_cb(void *data, Evas_Object *obj, void *event_info)
{
   Inwin *inwin = data;
   inwin_free(inwin);
}


static void _bt_preferences_apply_cb(void *data, Evas_Object *obj, void *event_info)
{
   Inwin *inwin = data;

   const char *s = elm_entry_entry_get(inwin->entry);
   if(s && strlen(s)>0)
     {
	Eet_Data_Descriptor *edd;
	Enlil_String string;

	EINA_STRINGSHARE_DEL(media_player);
	media_player = eina_stringshare_add(s);

	string.string = eina_stringshare_add(s);
	edd = enlil_string_edd_new();

	enlil_eet_app_data_save(edd, APP_NAME" media_player", &string);
	eet_data_descriptor_free(edd);
	eina_stringshare_del(string.string);
     }

   inwin_free(inwin);
}

static void _bt_photo_delete_cancel_cb(void *data, Evas_Object *obj, void *event_info)
{
   Inwin *inwin = data;
   inwin_free(inwin);
}


static void _bt_photo_delete_apply_cb(void *data, Evas_Object *obj, void *event_info)
{
   Inwin *inwin = data;
   char buf[PATH_MAX];
   Eina_List *l;
   Enlil_Photo *photo;

   EINA_LIST_FOREACH(inwin->photos, l, photo)
     {
	snprintf(buf, PATH_MAX, "%s/%s", enlil_photo_path_get(photo),
	      enlil_photo_file_name_get(photo));
	LOG_INFO("Delete the photo %s", buf);
	remove(buf);
     }

   inwin_free(inwin);
}

static void _bt_album_delete_cancel_cb(void *data, Evas_Object *obj, void *event_info)
{
   Inwin *inwin = data;
   inwin_free(inwin);
}


static void _bt_album_delete_apply_cb(void *data, Evas_Object *obj, void *event_info)
{
   Inwin *inwin = data;
   char buf[PATH_MAX];

   snprintf(buf, PATH_MAX, "%s/%s", enlil_album_path_get(inwin->album),
	 enlil_album_file_name_get(inwin->album));
   LOG_INFO("Delete the album %s", buf);
   ecore_file_recursive_rm(buf);

   inwin_free(inwin);
}

static void _bt_album_rename_cancel_cb(void *data, Evas_Object *obj, void *event_info)
{
   Inwin *inwin = data;
   inwin_free(inwin);
}

static void _bt_album_rename_apply_cb(void *data, Evas_Object *obj, void *event_info)
{
   Inwin *inwin = data;
   Enlil_Album *album = inwin->album;
   Enlil_Album_Data *album_data = enlil_album_user_data_get(album);

   const char *s = elm_entry_entry_get(inwin->entry);
   enlil_album_name_set(album, s);
   enlil_album_eet_header_save(album);

   elm_genlist_item_del(album_data->list_album_item);
   album_data->list_album_item = NULL;

   photos_list_object_header_update(album_data->list_photo_item);

   Enlil_Album *album_prev = enlil_root_album_prev_get(enlil_album_root_get(album), album);
   if(!album_prev)
     {
	list_left_append_relative(enlil_data->list_left, album, NULL);
	photos_list_object_header_move_after(album_data->list_photo_item, NULL);
     }
   else
     {
	Enlil_Album_Data *album_data_prev = enlil_album_user_data_get(album_prev);
        list_left_append_relative(enlil_data->list_left, album, album_data_prev->list_album_item);
	photos_list_object_header_move_after(album_data->list_photo_item, album_data_prev->list_photo_item);
     }

   enlil_flickr_job_sync_album_header_append(album, flickr_album_new_cb, 
		  flickr_album_notinflickr_cb, flickr_album_notuptodate_cb, 
		  flickr_album_flickrnotuptodate_cb, flickr_album_uptodate_cb, 
		  flickr_error_cb, enlil_data);

   inwin_free(inwin);
}


static void _bt_photo_move_album_cancel_cb(void *data, Evas_Object *obj, void *event_info)
{
   Inwin *inwin = data;
   Eina_List *l;
   Enlil_Album *album;

   EINA_LIST_FOREACH(enlil_root_albums_get(enlil_data->root), l, album)
     {
	Enlil_Album_Data *album_data = enlil_album_user_data_get(album);
	album_data->inwin_photo_move_album = NULL;
	album_data->photo_move_album_list_album_item = NULL;
     }

   inwin_free(inwin);
}

static void _bt_photo_move_album_apply_cb(void *data, Evas_Object *obj, void *event_info)
{
   Inwin *inwin = data;
   Elm_Genlist_Item *item = elm_genlist_selected_item_get(inwin->gl);
   char buf[PATH_MAX], buf2[PATH_MAX];
   Eina_List *l;
   Enlil_Photo *photo;
   if(!item) return ;
   Enlil_Album *album = (Enlil_Album *)elm_genlist_item_data_get(item);
   ASSERT_RETURN_VOID(album != NULL);

   EINA_LIST_FOREACH(inwin->photos, l, photo)
     {
	snprintf(buf, PATH_MAX, "%s/%s", enlil_photo_path_get(photo),
	      enlil_photo_file_name_get(photo));

	snprintf(buf2, PATH_MAX, "%s/%s/%s", enlil_album_path_get(album), enlil_album_file_name_get(album),
	      enlil_photo_file_name_get(photo));

	ecore_file_mv(buf, buf2);
     }

   _bt_photo_move_album_cancel_cb(inwin, NULL, NULL);
}

static void _bt_photo_save_cancel_cb(void *data, Evas_Object *obj, void *event_info)
{
   Inwin *inwin = data;

   if(inwin->del_cb)
     inwin->del_cb(inwin->data);

   inwin_free(inwin);
}

static void _bt_photo_save_dont_cb(void *data, Evas_Object *obj, void *event_info)
{
   Inwin *inwin = data;

   if(inwin->close_cb)
     inwin->close_cb(inwin->data);

   if(inwin->del_cb)
     inwin->del_cb(inwin->data);

   inwin_free(inwin);
}

static void _bt_photo_save_save_cb(void *data, Evas_Object *obj, void *event_info)
{
   Inwin *inwin = data;

   if(inwin->apply_cb)
     inwin->apply_cb(inwin->data);

   if(inwin->del_cb)
     inwin->del_cb(inwin->data);

   inwin_free(inwin);
}

static void _bt_save_as_file_exists_cancel_cb(void *data, Evas_Object *obj, void *event_info)
{
   Inwin *inwin = data;

   if(inwin->del_cb)
     inwin->del_cb(inwin->data);

   inwin_free(inwin);
}

static void _bt_save_as_file_exists_apply_cb(void *data, Evas_Object *obj, void *event_info)
{
   Inwin *inwin = data;

   if(inwin->apply_cb)
     inwin->apply_cb(inwin->data);

   if(inwin->del_cb)
     inwin->del_cb(inwin->data);

   inwin_free(inwin);
}

void inwin_free(Inwin *inwin)
{
   ASSERT_RETURN_VOID(inwin != NULL);

   if(inwin->del_cb)
     inwin->del_cb(inwin->data);

   EINA_STRINGSHARE_DEL(inwin->file);
   evas_object_del(inwin->inwin);
   FREE(inwin);
}


