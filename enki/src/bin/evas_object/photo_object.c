#include "photo_object.h"
#include <locale.h>

#include "../main.h"
#include "edje_mempool.h"

typedef struct _Smart_Data Smart_Data;

struct _Smart_Data
{
   const char *theme_file;
   const char *theme_group;
   const char *photo_file;
   const char *photo_group;

   Evas_Object *obj, *image;
   Evas_Coord iw, ih;
   Evas_Coord w, h;
   Evas_Coord x, y;

   int zoom;
   Eina_Bool camera;

   Eina_Bool progressbar;
   Evas_Object *o_progressbar;

   Evas_Object *netsync;

   Eina_Bool done :1;
   Eina_Bool preloading :1;
   Eina_Bool show :1;
   Eina_Bool gpx :1;

   Eina_Bool fill :1;

   Ecore_Job *job_update;
};

#define E_SMART_OBJ_GET_RETURN(smart, o, type, ret)  \
  {                                                  \
     char *_e_smart_str;                             \
                                                     \
     if (!o) return ret;                             \
     smart = evas_object_smart_data_get(o);          \
     if (!smart) return ret;                         \
     _e_smart_str = (char *)evas_object_type_get(o); \
     if (!_e_smart_str) return ret;                  \
     if (strcmp(_e_smart_str, type)) return ret;     \
  }

#define E_SMART_OBJ_GET(smart, o, type)              \
  {                                                  \
     char *_e_smart_str;                             \
                                                     \
     if (!o) return;                                 \
     smart = evas_object_smart_data_get(o);          \
     if (!smart) return;                             \
     _e_smart_str = (char *)evas_object_type_get(o); \
     if (!_e_smart_str) return;                      \
     if (strcmp(_e_smart_str, type)) return;         \
  }

#define E_OBJ_NAME "photo_object"
static Evas_Smart *smart = NULL;

static void
_smart_init(void);
static void
_smart_add(Evas_Object *obj);
static void
_smart_del(Evas_Object *obj);
static void
_smart_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y);
static void
_smart_resize(Evas_Object *obj, Evas_Coord w, Evas_Coord h);
static void
_smart_show(Evas_Object *obj);
static void
_smart_hide(Evas_Object *obj);
static void
_smart_clip_set(Evas_Object *obj, Evas_Object *clip);
static void
_smart_clip_unset(Evas_Object *obj);
static void
_preloaded(void *data, Evas *e, Evas_Object *obj, void *event);

static void
_update(void *data);

static void
_clicked_menu(void *data, Evas_Object *obj, const char *signal,
              const char *source)
{
   Evas_Object *o = data;
   evas_object_smart_callback_call(o, "clicked,menu", NULL);
}

Evas_Object *
photo_object_add(Evas_Object *obj)
{
   _smart_init();
   return evas_object_smart_add(evas_object_evas_get(obj), smart);
}

void
photo_object_theme_file_set(Evas_Object *obj, const char *theme,
                            const char *theme_group)
{
   Smart_Data *sd;
   sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   sd->theme_file = eina_stringshare_add(theme);
   sd->theme_group = eina_stringshare_add(theme_group);


   sd->obj = edje_mempool_object_add(sd->theme_group);
   evas_object_show(sd->obj);


   evas_object_move(sd->obj, sd->x, sd->y);
   evas_object_resize(sd->obj, sd->w, sd->h);

   evas_object_smart_member_add(sd->obj, obj);
   evas_object_show(sd->obj);
   if (!sd->preloading) evas_object_show(sd->image);

   edje_object_signal_callback_add(sd->obj, "clicked,menu", "photo",
                                   _clicked_menu, obj);

   if (sd->image) edje_object_part_swallow(sd->obj, "object.photo.swallow",
                                           sd->image);

   if (!sd->job_update) sd->job_update = ecore_job_add(_update, obj);
}

void
photo_object_fill_set(Evas_Object *obj, Eina_Bool fill)
{
   Smart_Data *sd;
   sd = evas_object_smart_data_get(obj);
   if (!sd) return;
   sd->fill = EINA_TRUE;
}

void
photo_object_gpx_set(Evas_Object *obj)
{
   Smart_Data *sd;
   sd = evas_object_smart_data_get(obj);
   if (!sd) return;
   sd->gpx = EINA_TRUE;
}

void
photo_object_file_set(Evas_Object *obj, const char *image,
                      const char *photo_group)
{
   Smart_Data *sd;
   sd = evas_object_smart_data_get(obj);
   Evas_Coord iw, ih;
   if (!sd) return;

   eina_stringshare_del(sd->photo_file);
   eina_stringshare_del(sd->photo_group);

   sd->photo_file = eina_stringshare_add(image);
   if (photo_group)
      sd->photo_group = eina_stringshare_add(photo_group);
   else
      sd->photo_group = NULL;

   if (sd->image) evas_object_del(sd->image);

   sd->image = evas_object_image_add(evas_object_evas_get(obj));
   evas_object_event_callback_add(sd->image, EVAS_CALLBACK_IMAGE_PRELOADED,
                                  _preloaded, sd);
   evas_object_image_filled_set(sd->image, 1);
   evas_object_smart_member_add(obj, sd->image);
   evas_object_image_file_set(sd->image, NULL, NULL);
   evas_object_image_load_scale_down_set(sd->image, 0);
   evas_object_image_file_set(sd->image, image, photo_group);
   evas_object_image_size_get(sd->image, &iw, &ih);

   sd->iw = iw;
   sd->ih = ih;

   evas_object_hide(sd->image);
   evas_object_image_preload(sd->image, EINA_FALSE);
   sd->preloading = EINA_TRUE;

   if (!sd->job_update) sd->job_update = ecore_job_add(_update, obj);
}

void
photo_object_progressbar_set(Evas_Object *obj, Eina_Bool b)
{
   Smart_Data *sd;
   sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   sd->progressbar = b;

   if (!b && sd->o_progressbar) evas_object_del(sd->o_progressbar);

   if (!sd->job_update) sd->job_update = ecore_job_add(_update, obj);
}

void
photo_object_done_set(Evas_Object *obj, Eina_Bool b)
{
   Smart_Data *sd;
   sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   sd->done = b;

   if (!sd->job_update) sd->job_update = ecore_job_add(_update, obj);
}

void
photo_object_radio_set(Evas_Object *obj, Eina_Bool b)
{
   Smart_Data *sd;
   sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   if (b)
      edje_object_signal_emit(sd->obj, "radio,on", "photo");
   else
      edje_object_signal_emit(sd->obj, "radio,off", "photo");
}

void
photo_object_camera_set(Evas_Object *obj, Eina_Bool b)
{
   Smart_Data *sd;
   sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   sd->camera = b;
   if (b)
      edje_object_signal_emit(sd->obj, "show,camera", "photo");
   else
      edje_object_signal_emit(sd->obj, "hide,camera", "photo");
}

void
photo_object_size_set(Evas_Object *obj, Evas_Coord w, Evas_Coord h)
{
   Smart_Data *sd;
   sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   sd->w = w;
   sd->h = h;
}

void
photo_object_text_set(Evas_Object *obj, const char *s)
{
   Smart_Data *sd;
   sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   edje_object_part_text_set(sd->obj, "object.text", s);
}

Evas_Object *
photo_object_netsync_state_set(Evas_Object *obj, const char *state)
{
   Smart_Data *sd;
   sd = evas_object_smart_data_get(obj);
   if (!sd) return NULL;

   if (!sd->netsync)
   {
      sd->netsync = edje_object_add(evas_object_evas_get(obj));
      evas_object_show(sd->netsync);
      edje_object_file_set(sd->netsync, Theme, "netsync/sync");
      evas_object_size_hint_weight_set(sd->netsync, 1.0, 1.0);
      evas_object_size_hint_align_set(sd->netsync, 1.0, 0.0);

      edje_object_part_swallow(sd->obj, "object.swallow.sync", sd->netsync);
   }

   edje_object_signal_emit(sd->netsync, state, "");
   return sd->netsync;
}

static void
_preloaded(void *data, Evas *e, Evas_Object *obj, void *event)
{
   Smart_Data *sd = data;

   sd->preloading = EINA_FALSE;

   if (sd->show) evas_object_show(sd->image);

   if (sd->obj) edje_object_part_swallow(sd->obj, "object.photo.swallow",
                                         sd->image);
}

void
photo_object_icon_menu_show(Evas_Object *obj, Eina_Bool show)
{
   Smart_Data *sd;
   sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   if (show)
      edje_object_signal_emit(sd->obj, "icon,menu,show", "");
   else
      edje_object_signal_emit(sd->obj, "icon,menu,hide", "");

}

static void
_update(void *data)
{
   Evas_Object *obj = data;
   int zoomw = 1, zoomh = 1;
   Smart_Data *sd;
   int x, y, w, h, w_img = 0, h_img = 0, w_img2, h_img2;
   int left_marge = 0, right_marge = 0, top_marge = 0, bottom_marge = 0;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   if (!sd->obj)
   {
      goto end;
   }

   if (sd->done)
      edje_object_signal_emit(sd->obj, "done", "photo");
   else
      edje_object_signal_emit(sd->obj, "undone", "photo");

   if (sd->gpx)
   {
      edje_object_signal_emit(sd->obj, "gpx", "photo");
   }

   if (sd->progressbar)
   {
      if (!sd->o_progressbar)
      {
         Evas_Object *loading = elm_progressbar_add(obj);
         sd->o_progressbar = loading;
         elm_object_style_set(loading, "wheel");
         elm_progressbar_pulse(loading, EINA_TRUE);
         evas_object_size_hint_weight_set(loading, 1.0, 0.0);
         evas_object_size_hint_align_set(loading, -1.0, 0.5);
         evas_object_show(loading);

         edje_object_part_swallow(sd->obj, "object.loading.swallow", loading);
         edje_object_signal_emit(sd->obj, "loading", "photo");
      }
      goto end;
   }

   if (sd->camera) edje_object_signal_emit(sd->obj, "show,camera", "photo");

   evas_object_geometry_get(sd->obj, &x, &y, &w, &h);

   w_img = sd->iw;
   h_img = sd->ih;

   while (w_img / 2 > w)
   {
      w_img = w_img / 2;
      zoomw++;
   }
   while (h_img / 2 > h)
   {
      h_img = h_img / 2;
      zoomh++;
   }
   sd->zoom = (zoomw < zoomh ? zoomw : zoomh);

   if (sd->w > -1) evas_object_image_load_size_set(sd->image, sd->w, sd->h);
   evas_object_image_file_set(sd->image, NULL, NULL);
   evas_object_image_load_scale_down_set(sd->image, sd->zoom);
   evas_object_image_file_set(sd->image, sd->photo_file, sd->photo_group);

   evas_object_hide(sd->image);
   evas_object_image_preload(sd->image, EINA_FALSE);
   sd->preloading = EINA_TRUE;

   w_img = sd->iw;
   h_img = sd->ih;

   setlocale(LC_NUMERIC, "C");
   const char *s_right_marge = edje_object_data_get(sd->obj, "right_marge");
   if (s_right_marge) right_marge = atoi(s_right_marge);
   const char *s_left_marge = edje_object_data_get(sd->obj, "left_marge");
   if (s_left_marge) left_marge = atoi(s_left_marge);

   const char *s_top_marge = edje_object_data_get(sd->obj, "top_marge");
   if (s_top_marge) top_marge = atoi(s_top_marge);
   const char *s_bottom_marge = edje_object_data_get(sd->obj, "bottom_marge");
   if (s_bottom_marge) bottom_marge = atoi(s_bottom_marge);

   w = w - right_marge - left_marge;
   h = h - top_marge - bottom_marge;

   w_img2 = w - w_img;
   h_img2 = h - h_img;

   if (!sd->fill)
   {
      if (w_img2 >= 0 && w_img2 < h_img2)
      {
         h_img2 = h_img * (w / (double) w_img);
         w_img2 = w_img * (w / (double) w_img);
      }
      else if (h_img2 >= 0 && h_img2 < w_img2)
      {
         w_img2 = w_img * (h / (double) h_img);
         h_img2 = h_img * (h / (double) h_img);
      }
      else if (w_img2 < 0 && w_img2 < h_img2)
      {
         h_img2 = h_img * (w / (double) w_img);
         w_img2 = w_img * (w / (double) w_img);
      }
      else
      {
         w_img2 = w_img * (h / (double) h_img);
         h_img2 = h_img * (h / (double) h_img);
      }
   }
   else
   {
      if (w_img2 >= 0 && w_img2 > h_img2)
      {
         h_img2 = h_img * (w / (double) w_img);
         w_img2 = w_img * (w / (double) w_img);
      }
      else if (h_img2 >= 0 && h_img2 > w_img2)
      {
         w_img2 = w_img * (h / (double) h_img);
         h_img2 = h_img * (h / (double) h_img);
      }
      else if (w_img2 < 0 && w_img2 > h_img2)
      {
         h_img2 = h_img * (w / (double) w_img);
         w_img2 = w_img * (w / (double) w_img);
      }
      else
      {
         w_img2 = w_img * (h / (double) h_img);
         h_img2 = h_img * (h / (double) h_img);
      }
   }

   Edje_Message_Int_Set *msg = alloca(sizeof(Edje_Message_Int_Set) + (3
            * sizeof(int)));
   msg->count = 4;
   msg->val[0] = (int) (w - w_img2) / 2;
   msg->val[1] = (int) (h - h_img2) / 2;
   msg->val[2] = (int) -(w - msg->val[0] - w_img2);
   msg->val[3] = (int) -(h - msg->val[1] - h_img2);

   edje_object_message_send(sd->obj, EDJE_MESSAGE_INT_SET, 0, msg);

   end: sd->job_update = NULL;
}

/*******************************************/
/* Internal smart object required routines */
/*******************************************/

static void
_smart_init(void)
{
   if (smart) return;
   {
      static const Evas_Smart_Class sc =
         { E_OBJ_NAME, EVAS_SMART_CLASS_VERSION, _smart_add, _smart_del,
           _smart_move, _smart_resize, _smart_show, _smart_hide, NULL,
           _smart_clip_set, _smart_clip_unset, NULL, NULL, NULL, NULL };
      smart = evas_smart_class_new(&sc);
   }
}

static void
_smart_add(Evas_Object *obj)
{
   Smart_Data *sd;

   sd = calloc(1, sizeof(Smart_Data));
   if (!sd) return;
   evas_object_smart_data_set(obj, sd);

   sd->w = -1;
   sd->h = -1;
}

static void
_smart_del(Evas_Object *obj)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   if (sd->theme_file) eina_stringshare_del(sd->theme_file);
   if (sd->theme_group) eina_stringshare_del(sd->theme_group);

   if (sd->photo_file) eina_stringshare_del(sd->photo_file);
   if (sd->photo_group) eina_stringshare_del(sd->photo_group);

   if (sd->o_progressbar) evas_object_del(sd->o_progressbar);

   if (sd->obj) edje_mempool_object_del(sd->obj);
   if (sd->image)
   {
      evas_object_del(sd->image);
   }
   if (sd->netsync) evas_object_del(sd->netsync);

   if (sd->job_update) ecore_job_del(sd->job_update);

   free(sd);
}

static void
_smart_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   sd->x = x;
   sd->y = y;
   evas_object_move(sd->obj, x, y);
}

static void
_smart_resize(Evas_Object *obj, Evas_Coord w, Evas_Coord h)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   sd->w = w;
   sd->h = h;
   evas_object_resize(sd->obj, w, h);

   if (!sd->job_update) sd->job_update = ecore_job_add(_update, obj);
}

static void
_smart_show(Evas_Object *obj)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;
   evas_object_show(sd->obj);
   sd->show = EINA_TRUE;
   if (!sd->preloading) evas_object_show(sd->image);
}

static void
_smart_hide(Evas_Object *obj)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;
   evas_object_hide(sd->obj);
}

static void
_smart_clip_set(Evas_Object *obj, Evas_Object *clip)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;
   evas_object_clip_set(sd->obj, clip);
}

static void
_smart_clip_unset(Evas_Object *obj)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;
   evas_object_clip_unset(sd->obj);
}

