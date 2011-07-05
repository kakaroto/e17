#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <Emotion.h>

#include "emage_private.h"

static char *
_emage_emotion_item_label_get(void *data, Evas_Object *obj __UNUSED__, const char *part __UNUSED__)
{
   Emage_File_Info *info = data;

   return strdup(info->info.path + info->info.name_start);
}

static void
_emage_emotion_edje_mouve_in(void *data, Evas_Object *obj __UNUSED__, const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Evas_Object *ic = data;

   if (!crazy_option) elm_video_play(ic);
   elm_video_audio_mute_set(ic, EINA_FALSE);
}

static void
_emage_emotion_edje_mouve_out(void *data, Evas_Object *obj __UNUSED__, const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Evas_Object *ic = data;

   if (!crazy_option) elm_video_pause(ic);
   elm_video_audio_mute_set(ic, EINA_TRUE);
}

static Evas_Object *
_emage_emotion_item_object_get(void *data, Evas_Object *obj, const char *part __UNUSED__)
{
   Emage_File_Info *info = data;
   Evas_Object *ic;

   ic = elm_video_add(obj);
   elm_video_file_set(ic, info->info.path);
   if (crazy_option) elm_video_play(ic);
   elm_video_audio_mute_set(ic, EINA_TRUE);

   edje_object_signal_callback_add((Evas_Object *) elm_gengrid_item_object_get(info->item), "mouse,in", "*", _emage_emotion_edje_mouve_in, ic);
   edje_object_signal_callback_add((Evas_Object *) elm_gengrid_item_object_get(info->item), "mouse,out", "*", _emage_emotion_edje_mouve_out, ic);

   return ic;
}

static void
_emage_emotion_item_object_del(void *data, Evas_Object *obj __UNUSED__)
{
   /* FIXME: implement a cache of object */
   free(data);
}

static Eina_Bool
_emage_is_video(Eio_File *handler __UNUSED__, const Eina_File_Direct_Info *info)
{
   return emotion_object_extension_may_play_get(info->path + info->name_start);
}

static Evas_Object *
_emage_emotion_display_object(Evas_Object *parent)
{
   Evas_Object *result;

   result = elm_video_add(parent);
   evas_object_data_set(result, "emage/parent", parent);
   elm_video_remember_position_set(result, EINA_TRUE);
   return result;
}

static void
_emage_emotion_display_clear(Evas_Object *display)
{
   elm_video_file_set(display, NULL);
}

static Emage_File_Object *
_emage_emotion_file_insert(Evas_Object *display __UNUSED__, const Emage_File_Info *info)
{
   return (Emage_File_Object *) info;
}

static void
_emage_emotion_file_del(Evas_Object *display __UNUSED__, Emage_File_Object *object __UNUSED__)
{
}

static void
_notify_show(void *data, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   evas_object_show(data);
}

static void
_notify_block(void *data, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
  elm_notify_timeout_set(data, 0);
  evas_object_show(data);
}

static void
_notify_unblock(void *data, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
  elm_notify_timeout_set(data, 3.0);
  evas_object_show(data);
}

static void
_stop(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Evas_Object *display = data;
   Evas_Object *layout = evas_object_data_get(display, "emage/parent");

   elm_video_stop(display);
   elm_video_file_set(display, NULL);
   evas_object_del(evas_object_data_get(layout, "emage/notify"));
   evas_object_data_set(layout, "emage/notify", NULL);
   edje_object_signal_emit(elm_layout_edje_get(layout), "hide,content", "code");
}

static void
_emage_emotion_action(Evas_Object *display, Emage_File_Object *object)
{
   Emage_File_Info *info = (Emage_File_Info *) object;
   Evas_Object *layout;
   Evas_Object *notify;
   Evas_Object *player;

   layout = evas_object_data_get(display, "emage/parent");

   if (evas_object_data_get(layout, "emage/notify")) return ;

   elm_video_file_set(display, info->info.path);
   elm_video_play(display);

   notify = elm_notify_add(layout);
   elm_notify_orient_set(notify, ELM_NOTIFY_ORIENT_BOTTOM);
   elm_notify_timeout_set(notify, 3.0);
   evas_object_data_set(layout, "emage/notify", notify);

   player = elm_player_add(layout);
   elm_player_video_set(player, display);
   elm_notify_content_set(notify, player);
   evas_object_show(player);

   evas_object_event_callback_add(display, EVAS_CALLBACK_MOUSE_UP, _notify_show, notify);
   evas_object_event_callback_add(display, EVAS_CALLBACK_MOUSE_MOVE, _notify_show, notify);
   evas_object_event_callback_add(player, EVAS_CALLBACK_MOUSE_DOWN, _notify_block, notify);
   evas_object_event_callback_add(player, EVAS_CALLBACK_MOUSE_MOVE, _notify_show, notify);
   evas_object_event_callback_add(player, EVAS_CALLBACK_MOUSE_UP, _notify_unblock, notify);
   evas_object_smart_callback_add(player, "stop,clicked", _stop, display);

   evas_object_show(notify);
}

static const Elm_Gengrid_Item_Class _emage_emotion_class = {
  "emage-emotion",
  {
    _emage_emotion_item_label_get,
    _emage_emotion_item_object_get,
    NULL,
    _emage_emotion_item_object_del,
  }
};

static const Emage_Mapping _emage_emotion_mapping = {
  "video", &_emage_emotion_class,
  {
    _emage_is_video,
    _emage_emotion_display_object,
    _emage_emotion_display_clear,
    _emage_emotion_file_insert,
    _emage_emotion_file_del,
    _emage_emotion_action
  }
};

Eina_Bool
emage_show_emotion_init(void)
{
  return emage_browse_register(&_emage_emotion_mapping);
}

Eina_Bool
emage_show_emotion_shutdown(void)
{
  return EINA_TRUE;
}
