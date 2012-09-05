#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <math.h>

#include "efbb.h"
#include "cannon.h"
#include "sound.h"
#include "score.h"

#include "main_screen.h"
#include "level_select_screen.h"
#include "bg.h"

#ifndef ELM_LIB_QUICKLAUNCH

#define FLOOR_Y (610)
#define DEG_TO_RAD (0.01745) /* 2 * pi radians == 360 degree */
#define TARGET_MASS (1)
#define AMMO_MASS (6)
#define INFINITE_HP (9000) //ITS OVER NINE THOUSAND!!

int _efbb_log_dom = -1;

struct _Game {
   Evas_Object *main_screen;
   Evas_Object *level_select_screen;
   Evas_Object *options_layout;

   Evas_Object *win;
   Evas_Object *bg;
   Evas_Object *cannon;
   Evas_Object *cannon_area;

   EPhysics_World *cur_world;
   EPhysics_Camera *camera;
   EPhysics_Body *ship;

   Evas *evas;
   Ecore_Timer *timer;
   Ecore_Timer *end_timer;
   Eina_List *levels;
   Eina_List *extra_obj;

   Etrophy_Gamescore *gamescore;
   Score *score;
   Level *cur_level;
   World *world;

   int targets;
   int old_cx;
   int strength;
   int angle;
   Evas_Coord x;
   Evas_Coord y;

   Eina_Bool mouse_moving:1;
   Eina_Bool camera_moving:1;
};

typedef struct _Body_Data {
   int hp;
   Evas_Object *relto;
} Body_Data;

static void _level_load(Game *game);
static void _level_win(void * data);

static void
_relto_del_cb(void *data, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__,
              void *event_info __UNUSED__)
{
   Body_Data *body_data = data;
   body_data->relto = NULL;
}

static void
_body_del(void *data, EPhysics_Body *body, void *event_info)
{
   Body_Data *body_data = ephysics_body_data_get(body);
   if (body_data)
     {
        if (body_data->relto)
          evas_object_event_callback_del(body_data->relto, EVAS_CALLBACK_DEL,
                                         _relto_del_cb);
        free(body_data);
     }
   if (event_info) evas_object_del(event_info);
   if (data) evas_object_del(data);
}

static Eina_Bool
_win_cb(void *data)
{
   Game *game = data;

   if (game->timer)
     {
        ecore_timer_del(game->timer);
        game->timer = NULL;
     }
   ephysics_world_running_set(game->cur_world, EINA_FALSE);
   _level_win(game);

   game->end_timer = NULL;
   return EINA_FALSE;
}

static void
_target_dec(void *data, EPhysics_Body *body __UNUSED__,
            void *event_info __UNUSED__)
{
   Game *game = data;
   game->targets--;

   if (!game->targets)
     game->end_timer = ecore_timer_add(3, _win_cb, game);
}

static void
_sea_of_death(void *data, EPhysics_Body *body, void *event_info __UNUSED__)
{
   Game *game = data;
   double body_mass;
   int y;

   ephysics_body_geometry_get(body, NULL, &y, NULL, NULL);
   if (y <= FLOOR_Y + 100) return;

   body_mass = ephysics_body_mass_get(body);
   if (body_mass == TARGET_MASS)
       body_mass *= 200;
   else if (body_mass == AMMO_MASS || body_mass < 0)
       body_mass = 0;

   score_inc(game->score, 20 * body_mass);
}

static void
_switch_sound_cb(void *data, Evas_Object *o __UNUSED__,
                const char *sig __UNUSED__, const char *src __UNUSED__)
{
   Game *game = data;
   sound_mute_toggle(!!game->cur_world);
}

static void
_extra_obj_del_cb(void *data, Evas_Object *obj, const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Game *game = data;
   game->extra_obj = eina_list_remove(game->extra_obj, obj);
   evas_object_del(obj);
}

static void
_boom_add(Game *game, int x, int y, int pts)
{
   char points_text[16];
   Evas_Object *boom;

   boom = edje_object_add(game->evas);
   edje_object_file_set(boom, PACKAGE_DATA_DIR "/" GAME_THEME ".edj", "boom");
   evas_object_move(boom, x, y);
   edje_object_signal_callback_add(boom, "end", "boom", _extra_obj_del_cb,
                                   game);

   snprintf(points_text, sizeof(points_text), "%d", pts);
   edje_object_part_text_set(boom, "points", points_text);

   if (pts >= ((TARGET_MASS * 200) * 20))
     edje_object_signal_emit(boom, "istarget", "boom");

   evas_object_layer_set(boom, LAYER_OBJ_ABOVE);
   evas_object_show(boom);
   game->extra_obj = eina_list_append(game->extra_obj, boom);

   sound_play("pop.wav");
   INF("BOOM X: %d  Y: %d", x, y);
}

static int
_body_points_get(EPhysics_Body *body)
{
   Body_Data *body_data = ephysics_body_data_get(body);
   double mass = ephysics_body_mass_get(body);
   int pts;

   if ((!mass) || (body_data->hp > 1))
     return 0;

   if (mass == AMMO_MASS)
     return 0;

   if (mass == TARGET_MASS)
     mass = TARGET_MASS * 200;

   pts = 20 * mass;
   ephysics_body_del(body);
   return pts;
}

static void
_body_col(void *data, EPhysics_Body *body, void *event_info)
{
   EPhysics_Body_Collision *collision = event_info;
   double vel_x1, vel_y1, vel_x2, vel_y2;
   EPhysics_Body *body2;
   Body_Data *body_data;
   Evas_Coord x, y, cx;
   Game *game = data;
   int pts;

   body2 = ephysics_body_collision_contact_body_get(collision);
   ephysics_body_linear_velocity_get(body, &vel_x1, &vel_y1);
   ephysics_body_linear_velocity_get(body2, &vel_x2, &vel_y2);

   body_data = ephysics_body_data_get(body);

   if ((vel_x1 > 60) || (vel_y1 > 60) || (vel_x2 > 60) || (vel_y2 > 60))
        if (body_data->relto)
          edje_object_signal_emit(body_data->relto, "blink", "game");

   if ((vel_x1 < 135) && (vel_y1 < 135) && (vel_x2 < 135) && (vel_y2 < 135))
     return;

   if (body_data->hp <= 0) return;
   body_data->hp = body_data->hp - ((vel_x1 + vel_y1 + vel_x2 + vel_y2) / 4);
   INF("Body HP: %d", body_data->hp);
   if (body_data->hp > 0) return;

   pts = _body_points_get(body);
   score_inc(game->score, pts);

   ephysics_body_collision_position_get(collision, &x, &y);
   ephysics_camera_position_get(ephysics_world_camera_get(
         ephysics_body_world_get(body)), &cx, NULL);

   _boom_add(game, x - cx, y, pts);
}

static void
_update_obj(Evas_Object *obj, int delta_x)
{
   int x, y, fx;
   evas_object_geometry_get(obj, &x, &y, NULL, NULL);
   fx = x + delta_x;
   evas_object_move(obj, fx, y);
}

static void
_camera_update(Game *game)
{
   int x;

   if (!game->camera_moving) return;

   ephysics_camera_position_get(game->camera, &x, NULL);
   if (x <= 0)
     {
        game->camera_moving = EINA_FALSE;
        return;
     }

   x -= 5;
   if (x < 0) x = 0;
   ephysics_camera_position_set(game->camera, x, 0);
}

static void
_camera_moved_cb(void *data, EPhysics_World *world __UNUSED__,
                 void *event_info __UNUSED__)
{
   Evas_Object *object;
   Game *game = data;
   int cx, delta_x;
   Eina_List *l;

   ephysics_camera_position_get(game->camera, &cx, NULL);
   delta_x = game->old_cx - cx;
   bg_update(game->bg, cx);
   _update_obj(game->cannon_area, delta_x);

   EINA_LIST_FOREACH(game->extra_obj, l, object)
        _update_obj(object, delta_x);

   game->old_cx = cx;
   _camera_update(game);
}

static void
_body_camera_limit(void *data, EPhysics_Body *body, void *event_info __UNUSED__)
{
   EPhysics_Body *tracked_body;
   Game *game = data;
   int x, w;

   ephysics_camera_tracked_body_get(game->camera, &tracked_body,
                                    EINA_FALSE, EINA_FALSE);
   if (tracked_body && tracked_body != body)
     {
        ephysics_body_event_callback_del(body, EPHYSICS_CALLBACK_BODY_UPDATE,
                                         _body_camera_limit);
        return;
     }

   ephysics_body_geometry_get(body, &x, NULL, &w, NULL);

   if (!tracked_body)
     {
        if (x + w / 2 >= WIDTH / 2)
          ephysics_camera_body_track(game->camera, body, EINA_TRUE, EINA_FALSE);
        return;
     }

   if (x + w / 2 >= WIDTH * 3 / 2)
     {
        ephysics_camera_position_set(game->camera, WIDTH, 0);
        ephysics_body_event_callback_del(body, EPHYSICS_CALLBACK_BODY_UPDATE,
                                         _body_camera_limit);
        return;
     }

   if (x + w / 2 <= WIDTH / 2)
     {
        ephysics_camera_position_set(game->camera, 0, 0);
        ephysics_body_event_callback_del(body, EPHYSICS_CALLBACK_BODY_UPDATE,
                                         _body_camera_limit);
     }
}

static void
_body_update(void *data __UNUSED__, EPhysics_Body *body,
             void *event_info __UNUSED__)
{
   ephysics_body_evas_object_update(body);
}

static void
_mouse_move(void *data, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__,
            void *event_info)
{
   Evas_Event_Mouse_Down *mouse_info = event_info;
   int strength, strength_min, strength_max;
   int angle, angle_min, angle_max;
   Game *game = data;

   strength_min = cannon_strength_min_get(game->cannon);
   strength_max = cannon_strength_max_get(game->cannon);

   strength = game->strength +
      (mouse_info->canvas.x - game->x) * strength_max * 4 / 1280;

   if (strength > strength_max)
     strength = strength_max;
   else if (strength < strength_min)
     strength = strength_min;

   cannon_strength_set(game->cannon, strength);

   angle_min = cannon_angle_min_get(game->cannon);
   angle_max = cannon_angle_max_get(game->cannon);

   angle = game->angle - (mouse_info->canvas.y - game->y) * angle_max * 4 / 720;

   if (angle > angle_max)
     angle = angle_max;
   else if (angle < angle_min)
     angle = angle_min;

   cannon_angle_set(game->cannon, angle);
}

static void
_mouse_up(void *data, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__,
          void *event_info)
{
   Game *game = data;
   EPhysics_Body *body;
   Evas_Object *body_image;
   double impulse_x, impulse_y;
   int impulse, angle;
   int err;
   Evas_Event_Mouse_Up *mouse_info = event_info;
   Body_Data *body_data;

   if (mouse_info->button != 1) return;

   body_data = calloc(1, sizeof(Body_Data));
   if (!body_data)
     {
        ERR("Failed to alloc body data");
        return;
     }
   body_data->hp = INFINITE_HP;

   cannon_shoot(game->cannon);
   game->mouse_moving = EINA_FALSE;

   impulse = cannon_strength_get(game->cannon);
   angle = cannon_angle_get(game->cannon);
   impulse_x = (impulse * 90) * cos((double)angle * DEG_TO_RAD);
   impulse_y = (impulse * 90) * sin((double)angle * DEG_TO_RAD);

   body_image = evas_object_image_filled_add(game->evas);
   evas_object_image_file_set(body_image, PACKAGE_DATA_DIR "/bomb.png", NULL);
   err = evas_object_image_load_error_get(body_image);
   if (err != EVAS_LOAD_ERROR_NONE)
       WRN("could not load image '%s': %s", PACKAGE_DATA_DIR "/bomb.png",
               evas_load_error_str(err));
   evas_object_move(body_image, (level_cannon_pos_x_get(game->cur_level) + 310)
                    - (angle * 1.66), level_cannon_pos_y_get(game->cur_level)
                    - (angle * 3.4));
   evas_object_resize(body_image, 30, 30);
   evas_object_layer_set(body_image, LAYER_BLOCKS);
   evas_object_show(body_image);

   body = ephysics_body_circle_add(game->cur_world);
   ephysics_body_mass_set(body, AMMO_MASS);
   ephysics_body_damping_set(body, 0.3, 0.16);
   ephysics_body_sleeping_threshold_set(body, 30, 240);
   ephysics_body_restitution_set(body, 0.05);
   ephysics_body_evas_object_set(body, body_image, EINA_TRUE);
   ephysics_body_central_impulse_apply(body, impulse_x, - impulse_y);
   ephysics_body_data_set(body, body_data);

   sound_play("cannon.wav");

   ephysics_body_event_callback_add(body, EPHYSICS_CALLBACK_BODY_UPDATE,
                                    _body_camera_limit, game);
   ephysics_body_event_callback_add(body, EPHYSICS_CALLBACK_BODY_UPDATE,
                                    _body_update, NULL);
   ephysics_body_event_callback_add(body, EPHYSICS_CALLBACK_BODY_DEL,
                                    _body_del, NULL);

   evas_object_event_callback_del(game->cannon_area, EVAS_CALLBACK_MOUSE_UP,
                                  _mouse_up);
   evas_object_event_callback_del(game->cannon_area, EVAS_CALLBACK_MOUSE_MOVE,
                                  _mouse_move);

   DBG("Shoot with impulse %i and %i degrees", impulse, angle);
   DBG("Shoot with x %lf and y %lf", impulse_x, impulse_y);
}

static void
_mouse_down(void *data, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__,
            void *event_info)
{
   Evas_Event_Mouse_Down *mouse_info = event_info;
   Game *game = data;

   if ((mouse_info->button != 1) ||
       (!cannon_ammo_get(game->cannon)) ||
       (cannon_loaded_get(game->cannon)) ||
       (game->camera_moving))
     return;

   game->mouse_moving = EINA_TRUE;
   game->x = mouse_info->canvas.x;
   game->y = mouse_info->canvas.y;

   cannon_load(game->cannon);
   game->strength = cannon_strength_get(game->cannon);
   game->angle = cannon_angle_get(game->cannon);

   evas_object_event_callback_add(game->cannon_area, EVAS_CALLBACK_MOUSE_UP,
                                  _mouse_up, game);
   evas_object_event_callback_add(game->cannon_area, EVAS_CALLBACK_MOUSE_MOVE,
                                  _mouse_move, game);
}

static void
_mouse_move_bg(void *data, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__,
               void *event_info)
{
   Evas_Event_Mouse_Down *mouse_info = event_info;
   Game *game = data;
   int camera_x, pos_x;

   ephysics_camera_position_get(game->camera, &camera_x, NULL);
   pos_x = camera_x + (game->x - mouse_info->canvas.x);
   if (pos_x < 0) pos_x = 0;
   else if (pos_x > WIDTH) pos_x = WIDTH;
   ephysics_camera_position_set(game->camera, pos_x, 0);
   game->x = mouse_info->canvas.x;
}

static void
_mouse_up_bg(void *data, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__,
             void *event_info)
{
   Game *game = data;
   Evas_Event_Mouse_Up *mouse_info = event_info;

   if (mouse_info->button != 1) return;

   game->mouse_moving = EINA_FALSE;
   evas_object_event_callback_del(game->bg, EVAS_CALLBACK_MOUSE_UP,
                                  _mouse_up_bg);
   evas_object_event_callback_del(game->bg, EVAS_CALLBACK_MOUSE_MOVE,
                                  _mouse_move_bg);
}

static void
_mouse_down_bg(void *data, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__,
               void *event_info)
{
   Evas_Event_Mouse_Down *mouse_info = event_info;
   Game *game = data;
   EPhysics_Body *body;

   if ((mouse_info->button != 1) || (game->camera_moving)) return;

   game->mouse_moving = EINA_TRUE;
   ephysics_camera_tracked_body_get(game->camera, &body, NULL, NULL);
   if (body)
     {
        ephysics_camera_body_track(game->camera, NULL, EINA_FALSE, EINA_FALSE);
        ephysics_body_event_callback_del(body, EPHYSICS_CALLBACK_BODY_UPDATE,
                                         _body_camera_limit);
     }

   game->x = mouse_info->canvas.x;

   evas_object_event_callback_add(game->bg, EVAS_CALLBACK_MOUSE_UP,
                                  _mouse_up_bg, game);
   evas_object_event_callback_add(game->bg, EVAS_CALLBACK_MOUSE_MOVE,
                                  _mouse_move_bg, game);
}

static void
_create_cube(Game *game, const char *type, int w, int h, int x, int y,
             float mass, int relto)
{
   Evas_Object *body_image;
   Body_Data *body_data;
   EPhysics_Body *body;
   char image[250];
   int err;

   body_data = calloc(1, sizeof(Body_Data));
   if (!body_data)
     {
        ERR("Failed to alloc body data");
        return;
     }
   body_data->hp = mass * 30;

   snprintf(image, sizeof(image), PACKAGE_DATA_DIR "/%s_%i_%i.png",
            type, w, h);

   body_image = evas_object_image_filled_add(game->evas);
   evas_object_image_file_set(body_image, image, NULL);
   err = evas_object_image_load_error_get(body_image);
   if (err != EVAS_LOAD_ERROR_NONE)
       WRN("could not load image '%s': %s", image, evas_load_error_str(err));
   evas_object_move(body_image, x, y);
   evas_object_resize(body_image, w, h);
   evas_object_show(body_image);
   evas_object_layer_set(body_image, LAYER_BLOCKS);

   body = ephysics_body_box_add(game->cur_world);
   ephysics_body_mass_set(body, mass);
   ephysics_body_evas_object_set(body, body_image, EINA_TRUE);

   if (relto != -1)
     {
        body_data->relto = eina_list_nth(game->extra_obj, relto);
        evas_object_event_callback_add(body_data->relto, EVAS_CALLBACK_DEL,
                                       _relto_del_cb, body_data);
     }
   ephysics_body_data_set(body, body_data);

   if (level_time_attack_get(game->cur_level))
       {
         ephysics_body_linear_velocity_set(body, 30, 0);

         ephysics_body_event_callback_add(body, EPHYSICS_CALLBACK_BODY_DEL,
                                          _sea_of_death, game);

          if (!strcmp(type, "SHIP_ENGINE"))
           {
              game->ship = body;
              ephysics_body_friction_set(body, 1);
           }
       }

   ephysics_body_event_callback_add(body,
                                    EPHYSICS_CALLBACK_BODY_COLLISION, _body_col,
                                    game);
   ephysics_body_event_callback_add(body, EPHYSICS_CALLBACK_BODY_DEL,
                                    _body_del, NULL);

   DBG("Block of type %s created with geometry %i %i %i %i and mass %lf",
           type, x, y, w, h, mass);
}

static void
_create_target(Game *game, const char *type, int w, int h, int x, int y)
{
   Evas_Object *body_image;
   Body_Data *body_data;
   EPhysics_Body *body;
   char image[1024];
   int err;

   body_data = calloc(1, sizeof(Body_Data));
   if (!body_data)
     {
        ERR("Failed to alloc body data");
        return;
     }
   body_data->hp = 130;

   snprintf(image, sizeof(image), PACKAGE_DATA_DIR "/%s.png",type);

   body_image = evas_object_image_filled_add(game->evas);
   evas_object_image_file_set(body_image, image, NULL);
   err = evas_object_image_load_error_get(body_image);
   if (err != EVAS_LOAD_ERROR_NONE)
       WRN("could not load image '%s': %s", image, evas_load_error_str(err));
   evas_object_move(body_image, x, y);
   evas_object_resize(body_image, w, h);
   evas_object_layer_set(body_image, LAYER_BLOCKS);
   evas_object_show(body_image);

   if (!strcmp(type, "boss"))
     body = ephysics_body_box_add(game->cur_world);
   else
     body = ephysics_body_circle_add(game->cur_world);

   ephysics_body_mass_set(body, TARGET_MASS);
   ephysics_body_evas_object_set(body, body_image, EINA_TRUE);
   ephysics_body_damping_set(body, 0.2, 0.4);
   ephysics_body_sleeping_threshold_set(body, 30, 240);
   ephysics_body_data_set(body, body_data);

   if (level_time_attack_get(game->cur_level))
     {
        ephysics_body_linear_velocity_set(body, 30, 0);
        ephysics_body_damping_set(body, 0, 0.4);
        ephysics_body_event_callback_add(body, EPHYSICS_CALLBACK_BODY_DEL,
                                          _sea_of_death, game);
     }


   ephysics_body_event_callback_add(body,
                                    EPHYSICS_CALLBACK_BODY_COLLISION, _body_col,
                                    game);
   ephysics_body_event_callback_add(body, EPHYSICS_CALLBACK_BODY_DEL,
                                    _target_dec, game);
   ephysics_body_event_callback_add(body, EPHYSICS_CALLBACK_BODY_DEL,
                                    _body_del, NULL);
   game->targets++;

   DBG("Target created at (%i, %i)", x, y);
}

static void
_create_object(Game *game, const char *type, int w, int h, int x, int y,
               int above)
{
   Evas_Object *object_image;

   object_image = edje_object_add(game->evas);
   edje_object_file_set(object_image, PACKAGE_DATA_DIR "/"
                        GAME_THEME ".edj", type);
   evas_object_move(object_image, x, y);
   evas_object_resize(object_image, w, h);
   edje_object_signal_callback_add(object_image, "end", "object",
                                   _extra_obj_del_cb, game);

   if (above)
     evas_object_layer_set(object_image, LAYER_OBJ_ABOVE);
   else
     evas_object_layer_set(object_image, LAYER_OBJ);

   evas_object_show(object_image);
   game->extra_obj = eina_list_append(game->extra_obj, object_image);

   DBG("Object created at (%i, %i)", x, y);
}

static void
level_unload(Game *game)
{
   Evas_Object *extra_object;

   if (game->cur_world)
     {
        ephysics_world_del(game->cur_world);
        game->cur_world = NULL;
     }

   if (game->bg)
     {
        evas_object_del(game->bg);
        game->bg = NULL;
     }

   if (game->timer)
     {
        ecore_timer_del(game->timer);
        game->timer = NULL;
     }

   if (game->end_timer)
     {
        ecore_timer_del(game->end_timer);
        game->end_timer = NULL;
     }

   EINA_LIST_FREE(game->extra_obj, extra_object)
     evas_object_del(extra_object);

   score_del(game->score);
}

static void
_menu(void *data, Evas_Object *layout, const char *emission __UNUSED__,
          const char *source __UNUSED__)
{
   Game *game = data;

   level_unload(game);

   if (sound_playing_get(GAME_MUSIC_ID))
     sound_stop(GAME_MUSIC_ID);

   if (!sound_playing_get(MENU_MUSIC_ID))
     sound_play_music(MENU_MUSIC, MENU_MUSIC_ID);

   evas_object_del(layout);
   elm_layout_signal_emit(game->options_layout, "hide_pause", "game");
   game_main_to_level_select(game);
}

static void
_resume_game_cb(void *data, Evas_Object *layout,
                const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Game *game = data;
   ephysics_world_running_set(game->cur_world, EINA_TRUE);
   bg_resume(game->bg);
   if (game->timer) ecore_timer_thaw(game->timer);
   if (game->end_timer) ecore_timer_thaw(game->end_timer);
   evas_object_del(layout);
}

static void
_pause_game_cb(void *data, Evas_Object *layout __UNUSED__,
               const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Game *game = data;
   Evas_Object *pause_layout;

   ephysics_world_running_set(game->cur_world, EINA_FALSE);
   bg_pause(game->bg);
   if (game->timer) ecore_timer_freeze(game->timer);
   if (game->end_timer) ecore_timer_freeze(game->end_timer);

   pause_layout = elm_layout_add(game->win);
   elm_layout_file_set(pause_layout, PACKAGE_DATA_DIR "/"
                       GAME_THEME ".edj", "pause_win");
   evas_object_resize(pause_layout, 1280, 720);
   evas_object_size_hint_min_set(pause_layout, WIDTH, HEIGHT);
   evas_object_size_hint_max_set(pause_layout, WIDTH, HEIGHT);

   elm_layout_signal_callback_add(pause_layout, "resume", "game",
                                  _resume_game_cb, game);
   elm_layout_signal_callback_add(pause_layout, "menu", "btn_menu",
                                  _menu, game);
   elm_layout_signal_callback_add(pause_layout, "*", "sound",
                                  sound_play_cb, NULL);
   evas_object_layer_set(pause_layout, LAYER_UI);

   evas_object_show(pause_layout);
}

static Evas_Object *
_options_layout_add(Game *game)
{
   Evas_Object *options_layout;

   options_layout = elm_layout_add(game->win);
   elm_layout_file_set(options_layout, PACKAGE_DATA_DIR "/" GAME_THEME ".edj",
                       "options");
   evas_object_resize(options_layout, 140, 70);
   evas_object_move(options_layout, 12, 12);
   evas_object_size_hint_min_set(options_layout, WIDTH, HEIGHT);
   evas_object_size_hint_max_set(options_layout, WIDTH, HEIGHT);
   evas_object_layer_set(options_layout, LAYER_UI);
   evas_object_show(options_layout);

   elm_layout_signal_callback_add(options_layout, "*", "sound",
                                  sound_play_cb, NULL);
   elm_layout_signal_callback_add(options_layout, "sound", "switch",
                                  _switch_sound_cb, game);
   elm_layout_signal_callback_add(options_layout, "pause", "game",
                                  _pause_game_cb, game);

   return options_layout;
}

static void
_level_load_cb(void *data, Evas_Object *layout, const char *emission __UNUSED__,
          const char *source __UNUSED__)
{
   Game *game = data;

   level_unload(game);

   evas_object_del(layout);
   _level_load(game);
}

static Evas_Object *
_retry_layout_add(Game *game, Eina_Bool win)
{
   Evas_Object *retry_layout;
   char buffer[20];

   if (win)
     snprintf(buffer, sizeof(buffer), "next_win");
   else
     snprintf(buffer, sizeof(buffer), "retry_win");

   retry_layout = elm_layout_add(game->win);
   elm_layout_file_set(retry_layout, PACKAGE_DATA_DIR "/"
                       GAME_THEME ".edj", buffer);
   evas_object_resize(retry_layout, 1280, 720);
   evas_object_size_hint_min_set(retry_layout, WIDTH, HEIGHT);
   evas_object_size_hint_max_set(retry_layout, WIDTH, HEIGHT);

   elm_layout_signal_callback_add(retry_layout, "menu", "btn_menu",
                                  _menu, game);
   elm_layout_signal_callback_add(retry_layout, "*", "sound",
                                  sound_play_cb, NULL);
   evas_object_layer_set(retry_layout, LAYER_UI);

   if (win)
     elm_layout_signal_callback_add(retry_layout, "next", "btn_next",
                                    _level_load_cb, game);
   else
     elm_layout_signal_callback_add(retry_layout, "retry", "btn_retry",
                                    _level_load_cb, game);

   return retry_layout;
}

static void
_next_level(void *data, Evas_Object *retry_layout)
{
   Eina_List *next_levels;
   Etrophy_Lock *lock;
   Game *game = data;

   next_levels = eina_list_data_find_list(game->levels, game->cur_level);

   if (eina_list_next(next_levels) != NULL)
     {
       next_levels = eina_list_next(next_levels);
       elm_layout_signal_emit(retry_layout, "next_exist", "level_end");
     }

   game->cur_level = eina_list_data_get(next_levels);
   lock = etrophy_gamescore_lock_get(game->gamescore,
                                     level_level_id_get(game->cur_level));
   etrophy_lock_state_set(lock, ETROPHY_LOCK_STATE_UNLOCKED);

   INF("Level %s unlocked", level_level_id_get(game->cur_level));
}

static void
_level_win(void * data)
{
   Game *game = data;
   Evas_Object *retry_layout;
   char buf[32];
   int total;

   retry_layout = _retry_layout_add(game, EINA_TRUE);

   snprintf(buf, sizeof(buf), "X %d", cannon_ammo_get(game->cannon));
   elm_object_part_text_set(retry_layout, "xbonus", buf);

   snprintf(buf, sizeof(buf), "+ %d BONUS",
            cannon_ammo_get(game->cannon) * 2000);
   elm_object_part_text_set(retry_layout, "bonus", buf);

   snprintf(buf, sizeof(buf), "%d POINTS", score_get(game->score));
   elm_object_part_text_set(retry_layout, "score", buf);

   total = score_get(game->score) + (cannon_ammo_get(game->cannon) * 2000);
   snprintf(buf, sizeof(buf), "%d", total);
   elm_object_part_text_set(retry_layout, "score_total", buf);

   elm_layout_signal_emit(retry_layout, "success", "level_end");

   etrophy_gamescore_level_score_add(game->gamescore,
                                     level_level_id_get(game->cur_level),
                                     NULL, total, 0);
   _next_level(game, retry_layout);
   evas_object_show(retry_layout);
}

static void
_wstopped(void *data, EPhysics_World *world __UNUSED__,
          void *event_info __UNUSED__)
{
   Evas_Object *retry_layout;
   EPhysics_Body *body;
   Game *game = data;

   INF("World Stopped!");

   ephysics_camera_tracked_body_get(game->camera, &body, NULL, NULL);
   if (body)
     {
        ephysics_camera_body_track(game->camera, NULL, EINA_FALSE, EINA_FALSE);
        ephysics_body_event_callback_del(body, EPHYSICS_CALLBACK_BODY_UPDATE,
                                         _body_camera_limit);
     }

   if (!game->mouse_moving)
     {
        game->camera_moving = EINA_TRUE;
        _camera_update(game);
     }

   if (game->end_timer)
     {
        ecore_timer_del(game->end_timer);
        game->end_timer = NULL;
     }

   if (((cannon_ammo_get(game->cannon) > 0) ||
        (cannon_loaded_get(game->cannon))) &&
       ((game->targets > 0) && (!level_time_attack_get(game->cur_level))))
      return;

   game->camera_moving = EINA_FALSE;

   if (game->targets <= 0)
     {
        _level_win(game);
        return;
     }

   retry_layout = _retry_layout_add(game, EINA_FALSE);
   evas_object_show(retry_layout);
}

static Eina_Bool
_ship_time_attack_cb(void *data)
{
   Evas_Object *object;
   Game *game = data;
   int x, y, w, h;
   Eina_List *l;

   ephysics_body_geometry_get(game->ship, &x, &y, &w, &h);

   if (x >= 2260)
     {
        ecore_timer_del(game->timer);
        game->timer = NULL;
        ephysics_world_running_set(game->cur_world, EINA_FALSE);
        _wstopped(game, NULL, NULL);
        return EINA_FALSE;
     }

   ephysics_body_geometry_set(game->ship, ++x, y, w, h);

   EINA_LIST_FOREACH(game->extra_obj, l, object)
     {
        evas_object_geometry_get(object, &x, &y, NULL, NULL);
        evas_object_move(object, ++x, y);
     }

   return EINA_TRUE;
}

static void
_level_load(Game *game)
{
   EPhysics_Body *ground_body, *cannon_body, *cannon_wall;
   Target *target;
   Object *object;
   Block *block;
   Eina_List *l;
   int hiscore;

   game->targets = 0;
   game->old_cx = 0;

   game->bg = bg_add(game->win, level_bg_get(game->cur_level));
   evas_object_event_callback_add(game->bg, EVAS_CALLBACK_MOUSE_DOWN,
                                  _mouse_down_bg, game);

   /* World */
   game->cur_world = ephysics_world_new();
   ephysics_world_simulation_set(game->cur_world, 1/60., 4);
   ephysics_world_render_geometry_set(game->cur_world, 0, 0, WIDTH, HEIGHT);
   ephysics_world_bodies_outside_bottom_autodel_set(game->cur_world, EINA_TRUE);
   ephysics_world_event_callback_add(game->cur_world,
                                     EPHYSICS_CALLBACK_WORLD_STOPPED,
                                     _wstopped, game);
   ephysics_world_event_callback_add(game->cur_world,
                                     EPHYSICS_CALLBACK_WORLD_CAMERA_MOVED,
                                     _camera_moved_cb, game);
   game->camera = ephysics_world_camera_get(game->cur_world);

   /* Ground */
   if (!level_time_attack_get(game->cur_level))
     {
        Body_Data *body_data = calloc(1, sizeof(Body_Data));
        if (!body_data)
          {
             ERR("Failed to alloc body data");
             return;
          }
        body_data->hp = INFINITE_HP;

        ground_body = ephysics_body_box_add(game->cur_world);
        ephysics_body_mass_set(ground_body, 0);
        ephysics_body_geometry_set(ground_body, 0, FLOOR_Y, 2560, 10);
        ephysics_body_restitution_set(ground_body, 0.65);
        ephysics_body_friction_set(ground_body, 12);
        ephysics_body_data_set(ground_body, body_data);
     }

   /* Cannon */
   game->cannon = cannon_add(game->win);
   evas_object_layer_set(game->cannon, LAYER_EVENT);
   cannon_ammo_set(game->cannon, level_ammo_get(game->cur_level));
   evas_object_move(game->cannon, level_cannon_pos_x_get(game->cur_level),
                    level_cannon_pos_y_get(game->cur_level));
   evas_object_show(game->cannon);

   cannon_body = ephysics_body_box_add(game->cur_world);
   ephysics_body_mass_set(cannon_body, 0);
   ephysics_body_evas_object_set(cannon_body, game->cannon, EINA_TRUE);

   cannon_wall = ephysics_body_box_add(game->cur_world);
   ephysics_body_geometry_set(cannon_wall,
                              level_cannon_pos_x_get(game->cur_level) + 5,
                              level_cannon_pos_y_get(game->cur_level) + 18,
                              123, 50);
   ephysics_body_mass_set(cannon_wall, 0);

   game->cannon_area = evas_object_rectangle_add(game->evas);
   evas_object_move(game->cannon_area, 0,
                    level_cannon_pos_y_get(game->cur_level) - 285);
   evas_object_resize(game->cannon_area, 550, 465);
   evas_object_color_set(game->cannon_area, 0, 0, 0, 0);
   evas_object_show(game->cannon_area);
   evas_object_layer_set(game->cannon_area, LAYER_EVENT);
   evas_object_event_callback_add(game->cannon_area, EVAS_CALLBACK_MOUSE_DOWN,
                                  _mouse_down, game);
   ephysics_body_event_callback_add(cannon_body, EPHYSICS_CALLBACK_BODY_DEL,
                                    _body_del, game->cannon_area);

   /* Objects */
   EINA_LIST_FOREACH(level_objects_list_get(game->cur_level), l, object)
      _create_object(game, object_type_get(object), object_w_get(object),
                     object_h_get(object), object_x_get(object),
                     object_y_get(object), object_above_get(object));

   /* Blocks */
   EINA_LIST_FOREACH(level_blocks_list_get(game->cur_level), l, block)
      _create_cube(game, block_type_get(block), block_w_get(block),
                   block_h_get(block), block_x_get(block), block_y_get(block),
                   block_mass_get(block), block_relto_get(block));

   /* Targets */
   EINA_LIST_FOREACH(level_targets_list_get(game->cur_level), l, target)
      _create_target(game, target_type_get(target), target_w_get(target),
                     target_h_get(target), target_x_get(target),
                     target_y_get(target));

   ephysics_camera_position_set(game->camera, 500, 0);
   game->camera_moving = EINA_TRUE;

   if (level_time_attack_get(game->cur_level))
       game->timer = ecore_timer_add(0.0336, _ship_time_attack_cb, game);

   if (!sound_playing_get(GAME_MUSIC_ID))
     sound_play_music(GAME_MUSIC, GAME_MUSIC_ID);

   /* Layouts */
   elm_layout_signal_emit(game->options_layout, "show_pause", "game");
   hiscore = etrophy_gamescore_level_hi_score_get(
      game->gamescore, level_level_id_get(game->cur_level));
   game->score = score_add(game->win, hiscore);
}

static Eina_Bool
_levels_load(Game *game)
{
   game_world_init();

   game->world = world_load(PACKAGE_DATA_DIR "/levels.eet");
   if (!game->world)
     {
        ERR("Failed to load world from " PACKAGE_DATA_DIR "/levels.eet");
        return EINA_FALSE;
     }

   game->levels = world_levels_list_get(game->world);
   if (!game->levels)
     {
        ERR("Failed to load levels from " PACKAGE_DATA_DIR "/levels.eet");
        return EINA_FALSE;
     }

   INF("Levels loaded: %i", world_levels_count(game->world));
   return EINA_TRUE;
}

static void
_levels_unload(Game *game)
{
   if (game->world)
     {
        world_free(game->world);
        game->world = NULL;
        game->levels = NULL;
     }

   game_world_shutdown();
}

static void
_win_del(void *data, Evas_Object *obj, void *event_info __UNUSED__)
{
   Game *game = data;

   DBG("Request to close window");

   if (game->cur_world)
     ephysics_world_del(game->cur_world);

   evas_object_del(obj);
}

static Evas_Object *
_win_add(Game *game)
{
   Evas_Object *win;

   win = elm_win_add(NULL, PACKAGE_NAME, ELM_WIN_BASIC);
   elm_win_title_set(win, "EPhysics Game");
   evas_object_smart_callback_add(win, "delete,request", _win_del, game);
   elm_win_screen_constrain_set(win, EINA_TRUE);
   evas_object_show(win);

   return win;
}

static Etrophy_Gamescore *
_gamescore_load(Game *game)
{
   Etrophy_Gamescore *gamescore;
   Eina_List *locks, *l;
   Level *level;

   gamescore = etrophy_gamescore_load("efbb");
   if (!gamescore)
     {
        gamescore = etrophy_gamescore_new("efbb");
        EINA_LIST_FOREACH(game->levels, l, level)
          {
             Etrophy_Lock *etrophy_lock;
             etrophy_lock = etrophy_lock_new(level_level_id_get(level),
                                             ETROPHY_LOCK_STATE_LOCKED, 0);
             etrophy_gamescore_lock_add(gamescore, etrophy_lock);
          }
        /* first level should be unlocked */
        locks = etrophy_gamescore_locks_list_get(gamescore);
        etrophy_lock_state_set(eina_list_data_get(locks),
                               ETROPHY_LOCK_STATE_UNLOCKED);
     }
   return gamescore;
}

void
game_main_to_level_select(Game *game)
{
   main_screen_hide(game->main_screen);
   level_select_screen_show(game->level_select_screen);
}

void
game_level_select_to_main(Game *game)
{
   level_select_screen_hide(game->level_select_screen);
   main_screen_show(game->main_screen);
}

void
game_level_select_to_level(Game *game, Level *level)
{
   sound_stop(MENU_MUSIC_ID);
   level_select_screen_hide(game->level_select_screen);
   game->cur_level = level;
   _level_load(game);
}

EAPI_MAIN int
elm_main(int argc __UNUSED__, char **argv __UNUSED__)
{
   Game *game;
   short int r = 0;

   _efbb_log_dom = eina_log_domain_register(
      "efbb", GAME_LOG_COLOR);

   if (_efbb_log_dom < 0)
     {
        EINA_LOG_CRIT("Could not register log domain: efbb");
        return -1;
     }

   if (!sound_init())
     {
        ERR("Failed to init sound");
        r = -1;
        goto end;
     }

   if (!ephysics_init())
     {
        ERR("Failed to init ephysics");
        r = -1;
        goto sound_shutdown;
     }

   game = calloc(1, sizeof(Game));
   if (!game)
     {
        ERR("Failed to alloc game data");
        r = -1;
        goto shutdown;
     }

   if (!_levels_load(game))
     {
        r = -1;
        goto unload_levels;
     }

    etrophy_init();
    game->gamescore = _gamescore_load(game);
    if (!game->gamescore)
    {
        ERR("Failed to init Score");
        r = -1;
        goto etrophy_shutdown;
    }

   elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);
   elm_theme_extension_add(NULL, PACKAGE_DATA_DIR "/" GAME_THEME ".edj");

   game->win = _win_add(game);
   game->main_screen = main_screen_add(game->win, game);
   game->evas = evas_object_evas_get(game->win);
   game->level_select_screen = level_select_screen_add(
      game->win, game, game->gamescore, game->levels);
   game->options_layout = _options_layout_add(game);

   main_screen_show(game->main_screen);

   elm_run();

   etrophy_gamescore_save(game->gamescore, NULL);

unload_levels:
   _levels_unload(game);
   free(game);

shutdown:
   ephysics_shutdown();

sound_shutdown:
   sound_shutdown();

etrophy_shutdown:
    etrophy_shutdown();

end:
   eina_log_domain_unregister(_efbb_log_dom);
   _efbb_log_dom = -1;

   elm_shutdown();
   return r;
}

#endif
ELM_MAIN()
