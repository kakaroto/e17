#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <Emotion.h>

#include "espionnage_private.h"

// #define DEBUG_LUMA 1

#ifndef ELM_LIB_QUICKLAUNCH

static const Ecore_Getopt options = {
  PACKAGE_NAME,
  "%prog [options]",
  PACKAGE_VERSION "Revision:" stringify(VREV),
  "(C) 2011 cedric.bail@free.fr",
  "GPL-3",
  "Webcam test",
  EINA_TRUE,
  {
    ECORE_GETOPT_VERSION('V', "version"),
    ECORE_GETOPT_COPYRIGHT('C', "copyright"),
    ECORE_GETOPT_LICENSE('L', "license"),
    ECORE_GETOPT_HELP('h', "help"),
    ECORE_GETOPT_SENTINEL
  }
};

static Evas_Object *video = NULL;
#ifdef DEBUG_LUMA
static Evas_Object *im = NULL;
#endif
int _log_domain = -1;

#ifdef HAVE_FACE
static Eina_List *faces = NULL;
static int frame_max = 1;
static int frame_count = 0;
static Eina_List *face_threads = NULL;
static int _2126[256];
static int _7152[256];
static int _0722[256];
#endif

static void
_espionnage_hoversel_selected(void *data, Evas_Object *obj, void *event_info __UNUSED__)
{
   const Emotion_Webcam *webcam;
   const Eina_List *webcams;
   const Eina_List *l;

   webcams = emotion_webcams_get();

   EINA_LIST_FOREACH(webcams, l, webcam)
     if (data == emotion_webcam_device_get(webcam))
       {
          elm_object_text_set(obj, emotion_webcam_name_get(webcam));
          break ;
       }

   elm_video_uri_set(video, data);
   elm_video_play(video);
}

static Eina_Bool
_espionnage_ecore_event_webcam_handler(void *data, int type __UNUSED__, void *event __UNUSED__)
{
   Evas_Object *hoversel = data;
   const Emotion_Webcam *webcam;
   const Eina_List *webcams;
   const Eina_List *l;

   if (elm_hoversel_expanded_get(hoversel))
     elm_hoversel_hover_end(hoversel);
   elm_hoversel_clear(hoversel);

   webcams = emotion_webcams_get();

   EINA_LIST_FOREACH(webcams, l, webcam)
     elm_hoversel_item_add(hoversel, emotion_webcam_name_get(webcam), NULL, 0, _espionnage_hoversel_selected, emotion_webcam_device_get(webcam));

   return ECORE_CALLBACK_PASS_ON;
}

static void
_espionnage_invert_changed(void *data __UNUSED__, Evas_Object *obj, void *event_info __UNUSED__)
{
   Evas_Map *m = NULL;

   if (elm_check_state_get(obj))
     {
        Evas_Coord x, y, w, h;

        evas_object_geometry_get(video, &x, &y, &w, &h);
        m = evas_map_new(4);
        evas_map_point_coord_set(m, 3, x, y + h, 0);
        evas_map_point_image_uv_set(m, 0, 0, h);
        evas_map_point_coord_set(m, 2, x + w, y + h, 0);
        evas_map_point_image_uv_set(m, 1, w, h);
        evas_map_point_coord_set(m, 1, x + w, y, 0);
        evas_map_point_image_uv_set(m, 2, w, 0);
        evas_map_point_coord_set(m, 0, x, y, 0);
        evas_map_point_image_uv_set(m, 3, 0, 0);
     }
   evas_object_map_set(video, m);
   evas_object_map_enable_set(video, m ? EINA_TRUE : EINA_FALSE);
}

#ifdef HAVE_FACE
typedef struct _Async_Face Async_Face;
struct _Async_Face
{
   char *data;

   Eina_List *rects;

   Evas_Coord w, h;
};

static void
_espionnage_heavy_face(void *data, Ecore_Thread *thread __UNUSED__)
{
   Async_Face *f = data;

   f->rects = face_search(thread, f->data, f->w, f->h, f->w);
}

static void
_espionnage_cancel_face(void *data, Ecore_Thread *thread)
{
   Async_Face *f = data;
   Eina_Rectangle *rect;

   face_threads = eina_list_remove(face_threads, thread);

   EINA_LIST_FREE(f->rects, rect)
     eina_rectangle_free(rect);
   free(f->data);
   free(f);
}

static void
_espionnage_end_face(void *data, Ecore_Thread *thread)
{
   Async_Face *f = data;
   Eina_Rectangle *rect;
   Evas_Object *face;
   Evas_Coord ox, oy, ow, oh;

#ifdef DEBUG_LUMA
   {
      char *tmp;
      char *tmp_cp;
      char *img8_cp;
      Evas_Coord x, y;
      int stride;

      evas_object_image_size_set(im, f->w, f->h);
      tmp = evas_object_image_data_get(im, 1);
      stride = evas_object_image_stride_get(im);

      img8_cp = f->data;
      tmp_cp = tmp;

      for (y = 0; y < f->h; y++)
        {
           int *line = (int*) tmp_cp;

           for (x = 0; x < f->w; x++, img8_cp++, line++)
             *line = 255 << 24 | ((*img8_cp) << 16) | ((*img8_cp) << 8) | ((*img8_cp));

           tmp_cp += stride;
        }

      evas_object_image_data_set(im, tmp);
      evas_object_image_data_update_add(im, 0, 0, f->w, f->h);
   }
#endif

   EINA_LIST_FREE(faces, face)
     evas_object_del(face);

   evas_object_geometry_get(emotion_object_image_get(elm_video_emotion_get(video)), &ox, &oy, &ow, &oh);

   EINA_LIST_FREE(f->rects, rect)
     {
        face = evas_object_rectangle_add(evas_object_evas_get(video));
        evas_object_resize(face, rect->w * ow / f->w, rect->h * oh / f->h);
        evas_object_move(face, rect->x * ow / f->w + ox, rect->y * oh / f->h + oy);
        evas_object_color_set(face, 128, 0, 0, 128);
        evas_object_show(face);

        faces = eina_list_append(faces, face);

        eina_rectangle_free(rect);
     }

   _espionnage_cancel_face(data, thread);
}

static void
_espionnage_face_new(void *data __UNUSED__, Evas_Object *obj, void *event_info __UNUSED__)
{
   Evas_Object *img;
   char *free_pixels = NULL;
   char *img_data;
   char *img8;
   char *img8_cp;
   Async_Face *f;
   Evas_Coord x, y, w, h;
   Eina_Bool swap;
   int stride;

   /* new data to process */
   if (frame_max == 0) return ;
   frame_count++;
   if (frame_count < frame_max) return ;
   if (eina_list_count(face_threads) > (unsigned int) eina_cpu_count() + 2) return ;
   frame_count = 0;

   img = emotion_object_image_get(obj);

   evas_object_image_size_get(img, &w, &h);

   if (evas_object_image_colorspace_get(img) != EVAS_COLORSPACE_ARGB8888)
     {
        img_data = evas_object_image_data_convert(img, EVAS_COLORSPACE_ARGB8888);
	if (!img_data) abort();
        free_pixels = img_data;
        stride = w * sizeof (unsigned int);
     }
   else
     {
        img_data = evas_object_image_data_get(img, 0);
        stride = evas_object_image_stride_get(img);
     }

   img8 = malloc(sizeof (char) * w * h);
   if (!img8) return ;
   img8_cp = img8;

   swap = !evas_object_map_enable_get(video);

   for (y = swap ? 0 : h - 1; (swap ? y < h : y >= 0); y = (swap ? y + 1 : y - 1))
     {
        unsigned char *line = (unsigned char*) img_data;

        img8_cp = img8 + y * w;

        for (x = 0; x < w; x++, line += 4)
          {
             int luma;

             luma = (char)((_2126[line[2]] + _7152[line[1]] + _0722[line[0]]) >> 8);
             *(img8_cp++) = luma;
          }

        img_data += stride;
     }

   f = malloc(sizeof (Async_Face));
   if (!f)
     {
        free(img8);
        return ;
     }

   f->data = img8;
   f->rects = NULL;
   f->w = w;
   f->h = h;

   face_threads = eina_list_append(face_threads, ecore_thread_run(_espionnage_heavy_face,
                                                                  _espionnage_end_face,
                                                                  _espionnage_cancel_face,
                                                                  f));

   free(free_pixels);
}

static void
_espionnage_face_changed(void *data __UNUSED__, Evas_Object *obj, void *event_info __UNUSED__)
{
   Evas_Object *emotion;

   emotion = elm_video_emotion_get(video);
   if (elm_check_state_get(obj))
     {
        frame_count = 0;
        evas_object_smart_callback_add(emotion, "frame_decode", _espionnage_face_new, NULL);
     }
   else
     {
        evas_object_smart_callback_del(emotion, "frame_decode", _espionnage_face_new);
     }
}

static void
_espionnage_spin_update(void *data __UNUSED__, Evas_Object *obj, void *event_info __UNUSED__)
{
   double value;

   value = elm_spinner_value_get(obj);

   frame_max = value;
}
#endif

EAPI int
elm_main(int argc, char **argv)
{
   Evas_Object *window;
   Evas_Object *vbox;
   Evas_Object *hoversel;
   Evas_Object *check_invert;
#ifdef HAVE_FACE
   Evas_Object *hbox;
   Evas_Object *check_face;
   Evas_Object *spin_interval;
   int i;
#endif
   Evas_Object *bg;
   Ecore_Event_Handler *watcher;
   Emotion_Webcam *webcam;
   const Eina_List *webcams;
   const Eina_List *l;
   Eina_Bool quit_option = EINA_FALSE;
   int args;

   Ecore_Getopt_Value values[] = {
     ECORE_GETOPT_VALUE_BOOL(quit_option),
     ECORE_GETOPT_VALUE_BOOL(quit_option),
     ECORE_GETOPT_VALUE_BOOL(quit_option),
     ECORE_GETOPT_VALUE_BOOL(quit_option),
     ECORE_GETOPT_VALUE_NONE
   };

   _log_domain = eina_log_domain_register("Espionnage", NULL);
   if (_log_domain < 0)
     {
        EINA_LOG_CRIT("could not create log domain 'Espionnage'.");
        return -1;
     }

   args = ecore_getopt_parse(&options, values, argc, argv);
   if (args < 0)
     {
        ERR("could not parse command line options.");
        return -1;
     }

   if (quit_option) return 0;

   emotion_init();

   elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);

   window = elm_win_add(NULL, PACKAGE_NAME, ELM_WIN_BASIC);
   if (!window)
     {
        ERR("couldn't create window.");
        return -1;
     }

   evas_object_resize(window, 800, 600);
   elm_win_alpha_set(window, 1);
   elm_win_title_set(window, PACKAGE_STRING);
   elm_win_autodel_set(window, 1);
   evas_object_show(window);

   bg = elm_bg_add(window);
   elm_win_resize_object_add(window, bg);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bg);

   vbox = elm_box_add(window);
   elm_win_resize_object_add(window, vbox);
   evas_object_size_hint_weight_set(vbox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(vbox);

   video = elm_video_add(vbox);
   emotion_object_priority_set(elm_video_emotion_get(video), EINA_TRUE);
   elm_box_pack_end(vbox, video);
   evas_object_size_hint_min_set(video, 640, 480);
   evas_object_size_hint_weight_set(video, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(video);

   hoversel = elm_hoversel_add(vbox);
   elm_box_pack_end(vbox, hoversel);
   elm_hoversel_hover_parent_set(hoversel, vbox);
   evas_object_size_hint_align_set(hoversel, 0.5, 0.5);
   evas_object_size_hint_weight_set(hoversel, EVAS_HINT_EXPAND, 0);
   evas_object_show(hoversel);

   webcams = emotion_webcams_get();

   EINA_LIST_FOREACH(webcams, l, webcam)
     elm_hoversel_item_add(hoversel, emotion_webcam_name_get(webcam), NULL, 0, _espionnage_hoversel_selected, emotion_webcam_device_get(webcam));

   watcher = ecore_event_handler_add(EMOTION_WEBCAM_UPDATE, _espionnage_ecore_event_webcam_handler, hoversel);

   webcam = eina_list_data_get(webcams);
   if (webcam)
     {
        elm_object_text_set(hoversel, emotion_webcam_name_get(webcam));
        elm_video_uri_set(video, emotion_webcam_device_get(webcam));
        elm_video_play(video);
     }
   else
     {
        elm_object_text_set(hoversel, "No webcam found !");
     }

   check_invert = elm_check_add(vbox);
   elm_box_pack_end(vbox, check_invert);
   evas_object_size_hint_align_set(check_invert, 0.5, 0.5);
   evas_object_size_hint_weight_set(check_invert, EVAS_HINT_EXPAND, 0);
   elm_object_text_set(check_invert, "Switch top/bottom");
   evas_object_show(check_invert);

   evas_object_smart_callback_add(check_invert, "changed", _espionnage_invert_changed, NULL);

#ifdef HAVE_FACE
   hbox = elm_box_add(vbox);
   elm_box_pack_end(vbox, hbox);
   evas_object_size_hint_align_set(hbox, 0.5, 0.5);
   evas_object_size_hint_weight_set(hbox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_box_horizontal_set(hbox, EINA_TRUE);
   evas_object_show(hbox);

   check_face = elm_check_add(hbox);
   elm_box_pack_end(hbox, check_face);
   evas_object_size_hint_weight_set(check_face, EVAS_HINT_EXPAND, 0);
   elm_object_text_set(check_face, "Face tracking");
   evas_object_show(check_face);

   evas_object_smart_callback_add(check_face, "changed", _espionnage_face_changed, NULL);

   spin_interval = elm_spinner_add(hbox);
   elm_box_pack_end(hbox, spin_interval);
   elm_spinner_label_format_set(spin_interval, "%.0f");
   elm_spinner_step_set(spin_interval, 1);
   elm_spinner_wrap_set(spin_interval, 1);
   elm_spinner_min_max_set(spin_interval, 1, 60);
   evas_object_size_hint_weight_set(spin_interval, EVAS_HINT_EXPAND, 0);
   evas_object_show(spin_interval);

   evas_object_smart_callback_add(spin_interval, "changed", _espionnage_spin_update, NULL);

   for (i = 0; i < 256; i++)
     {
        _2126[i] = 0.2116 * 256 * i;
        _7152[i] = 0.7152 * 256 * i;
        _0722[i] = 0.0722 * 256 * i;
     }
#endif

#ifdef DEBUG_LUMA
   im = evas_object_image_add(evas_object_evas_get(window));
   evas_object_resize(im, 200, 200);
   evas_object_image_fill_set(im, 0, 0, 200, 200);
   evas_object_show(im);
#endif

   elm_run();

   emotion_shutdown();

   return 0;
}
#endif

ELM_MAIN();
