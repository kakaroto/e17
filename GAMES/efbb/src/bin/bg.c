#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "efbb.h"
#include "bg.h"

#define WIDTH_BG (2560)
#define FLOOR_Y (610)

typedef struct _Game_Bg Game_Bg;

struct _Game_Bg {
     Evas_Object *event;
     Eina_List *objs;
     Ecore_Animator *animator;
};

typedef struct _Game_Bg_Obj Game_Bg_Obj;

struct _Game_Bg_Obj {
     Evas_Object *obj;
     int x, delta, move, pos_x;
};

static Game_Bg_Obj *
_bg_obj_add(Game_Bg *bg, const char *name, int x, int y,
            int w, int h, int delta)
{
   Game_Bg_Obj *bg_obj;
   char buf[1024];
   Evas *evas;

   bg_obj = calloc(1, sizeof(Game_Bg_Obj));
   if (!bg_obj)
     {
        ERR("Failed to create bg obj");
        return NULL;
     }

   bg_obj->delta = delta;
   bg_obj->x = x;

   evas = evas_object_evas_get(bg->event);
   bg_obj->obj = evas_object_image_filled_add(evas);
   snprintf(buf, sizeof(buf), PACKAGE_DATA_DIR "/%s.png", name);
   evas_object_image_file_set(bg_obj->obj, buf, NULL);
   evas_object_move(bg_obj->obj, x, FLOOR_Y + y);
   evas_object_resize(bg_obj->obj, w, h);
   evas_object_layer_set(bg_obj->obj, LAYER_BG);
   evas_object_show(bg_obj->obj);

   bg->objs = eina_list_append(bg->objs, bg_obj);
   return bg_obj;
}

static void
_bg_del(void *data, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__,
        void *event_info __UNUSED__)
{
   Game_Bg_Obj *bg_obj;
   Game_Bg *bg = data;

   EINA_LIST_FREE(bg->objs, bg_obj)
     {
        evas_object_del(bg_obj->obj);
        free(bg_obj);
     }

   if (bg->animator)
     ecore_animator_del(bg->animator);

   free(bg);
}

static Eina_Bool
_bg_move(void *data)
{
   Game_Bg_Obj *bg_obj;
   Game_Bg *bg = data;
   Eina_List *l;

   EINA_LIST_FOREACH(bg->objs, l, bg_obj)
     {
        int y;

        if (!bg_obj->move) continue;

        bg_obj->x += bg_obj->move;
        if ((bg_obj->move < 0) && (bg_obj->x <= -WIDTH_BG))
          bg_obj->x = WIDTH_BG + bg_obj->x % WIDTH;
        else if ((bg_obj->move > 0) && (bg_obj->x >= WIDTH_BG))
          bg_obj->x = -WIDTH_BG + bg_obj->x % WIDTH;

        evas_object_geometry_get(bg_obj->obj, NULL, &y, NULL, NULL);
        evas_object_move(bg_obj->obj, bg_obj->x - bg_obj->pos_x / bg_obj->delta,
                         y);
     }

   return EINA_TRUE;
}

Evas_Object *
bg_add(Evas_Object *win, Game_Bg_Type type)
{
   Game_Bg_Obj *obj;
   Game_Bg *bg;
   Evas *evas;

   bg = calloc(1, sizeof(Game_Bg));
   if (!bg)
     {
        ERR("Failed to create bg");
        return NULL;
     }

   evas = evas_object_evas_get(win);
   bg->event = evas_object_rectangle_add(evas);
   evas_object_resize(bg->event, WIDTH, HEIGHT);
   evas_object_color_set(bg->event, 0, 0, 0, 0);
   evas_object_layer_set(bg->event, LAYER_EVENT);
   evas_object_data_set(bg->event, "bg", bg);
   evas_object_event_callback_add(bg->event, EVAS_CALLBACK_DEL, _bg_del, bg);
   evas_object_show(bg->event);

   switch(type)
     {
      case BG_GROUND:
         _bg_obj_add(bg, "bg1_sky", 0, - 610, WIDTH_BG, 720, 5);
         _bg_obj_add(bg, "bg1_sun", 550, - 270, 182, 182, 7);
         _bg_obj_add(bg, "bg1_mountains", -50, - 140, WIDTH_BG, 200, 3);
         _bg_obj_add(bg, "bg1_trees", 0, - 220, WIDTH_BG, 256, 2);
         _bg_obj_add(bg, "bg1_ground", 0, 0, WIDTH_BG, 110, 1);
         break;

      case BG_ISLAND:
         _bg_obj_add(bg, "bg2_sky", 0, - 610, WIDTH_BG, 720, 5);
         _bg_obj_add(bg, "bg2_beach", 0, - 140, WIDTH_BG, 250, 3);
         _bg_obj_add(bg, "bg2_sand", 0, 0, WIDTH_BG, 110, 1);
         break;

      case BG_SEA:
         bg->animator = ecore_animator_add(_bg_move, bg);

         _bg_obj_add(bg, "bg3_sky", 0, - 610, WIDTH_BG, 720, 6);
         _bg_obj_add(bg, "bg3_fog", 0, - 610, WIDTH_BG, 270, 5);
         _bg_obj_add(bg, "bg3_mountain", 0, - 190, 1782, 220, 4);

         obj = _bg_obj_add(bg, "bg3_sea4", 0, 0, WIDTH_BG, 110, 3);
         obj->move = 1;
         obj = _bg_obj_add(bg, "bg3_sea4", -WIDTH_BG, 0, WIDTH_BG, 110, 3);
         obj->move = 1;

         obj = _bg_obj_add(bg, "bg3_sea3", 0, 0, WIDTH_BG, 110, 2);
         evas_object_layer_set(obj->obj, LAYER_BG_ABOVE);
         obj->move = -2;
         obj = _bg_obj_add(bg, "bg3_sea3", WIDTH_BG, 0, WIDTH_BG, 110, 2);
         evas_object_layer_set(obj->obj, LAYER_BG_ABOVE);
         obj->move = -2;

         obj = _bg_obj_add(bg, "bg3_sea2", 0, 0, WIDTH_BG, 110, 1);
         evas_object_layer_set(obj->obj, LAYER_BG_ABOVE);
         obj->move = 3;
         obj = _bg_obj_add(bg, "bg3_sea2", -WIDTH_BG, 0, WIDTH_BG, 110, 1);
         evas_object_layer_set(obj->obj, LAYER_BG_ABOVE);
         obj->move = 3;

         obj = _bg_obj_add(bg, "bg3_sea1", 0, 0, WIDTH_BG, 110, 1);
         evas_object_layer_set(obj->obj, LAYER_BG_ABOVE);
         obj->move = -3;
         obj = _bg_obj_add(bg, "bg3_sea1", WIDTH_BG, 0, WIDTH_BG, 110, 1);
         evas_object_layer_set(obj->obj, LAYER_BG_ABOVE);
         obj->move = -3;
         break;

      default:
         ERR("Background type not supported: %i", type);
     }

   return bg->event;
}

void
bg_update(Evas_Object *obj, int pos_x)
{
   Game_Bg_Obj *bg_obj;
   Eina_List *l;
   Game_Bg *bg;
   int x, y;

   if (!obj) return;
   bg = evas_object_data_get(obj, "bg");
   if (!bg) return;

   EINA_LIST_FOREACH(bg->objs, l, bg_obj)
     {
        evas_object_geometry_get(bg_obj->obj, NULL, &y, NULL, NULL);
        x = bg_obj->x - pos_x / bg_obj->delta;
        evas_object_move(bg_obj->obj, x, y);
        bg_obj->pos_x = pos_x;
     }
}

void
bg_pause(Evas_Object *obj)
{
   Game_Bg *bg = evas_object_data_get(obj, "bg");
   if (bg->animator)
     ecore_animator_freeze(bg->animator);
}

void
bg_resume(Evas_Object *obj)
{
   Game_Bg *bg = evas_object_data_get(obj, "bg");
   if (bg->animator)
     ecore_animator_thaw(bg->animator);
}
