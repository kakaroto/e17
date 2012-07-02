#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "ephysics_test.h"

typedef struct _Collision_Data Collision_Data;

struct _Collision_Data {
     Test_Data base;
     Evas_Object *impact;
     EPhysics_Body *sphere;
     EPhysics_Body *sphere2;
};

static void
_collision_cb(void *data, EPhysics_Body *body __UNUSED__, void *event_info __UNUSED__)
{
   Collision_Data *collision_data = data;
   Evas_Object *obj;
   int x;

   if (event_info != collision_data->sphere2) return;

   INF("Collision Detected");

   obj = ephysics_body_evas_object_get(collision_data->sphere);
   evas_object_geometry_get(obj, &x, NULL, NULL, NULL);
   evas_object_move(collision_data->impact, x - 4, FLOOR_Y - 80);
   elm_object_signal_emit(collision_data->impact, "impact,show",
                          "ephysics_test");
}

static void
_world_populate(Collision_Data *collision_data)
{
   Evas_Object *sphere1, *sphere2, *sh1, *sh2, *impact;
   EPhysics_Body *sphere_body1, *sphere_body2;

   sh1 = elm_layout_add(collision_data->base.win);
   elm_layout_file_set(
      sh1, PACKAGE_DATA_DIR "/" EPHYSICS_TEST_THEME ".edj", "shadow-ball");
   evas_object_move(sh1, WIDTH / 2, FLOOR_Y);
   evas_object_resize(sh1, 70, 3);
   evas_object_show(sh1);
   collision_data->base.evas_objs = eina_list_append(
      collision_data->base.evas_objs, sh1);

   sphere1 = elm_image_add(collision_data->base.win);
   elm_image_file_set(
      sphere1, PACKAGE_DATA_DIR "/" EPHYSICS_TEST_THEME ".edj",
      "big-blue-ball");
   evas_object_move(sphere1, WIDTH / 2, FLOOR_Y - 70 + 1);
   evas_object_resize(sphere1, 70, 70);
   evas_object_show(sphere1);
   collision_data->base.evas_objs = eina_list_append(
      collision_data->base.evas_objs, sphere1);

   sphere_body1 = ephysics_body_circle_add(collision_data->base.world);
   ephysics_body_evas_object_set(sphere_body1, sphere1, EINA_TRUE);
   ephysics_body_event_callback_add(sphere_body1,
                                    EPHYSICS_CALLBACK_BODY_UPDATE,
                                    update_object_cb, sh1);
   ephysics_body_restitution_set(sphere_body1, 0.8);
   ephysics_body_friction_set(sphere_body1, 0.4);
   collision_data->base.bodies = eina_list_append(
      collision_data->base.bodies, sphere_body1);
   collision_data->sphere = sphere_body1;

   sh2 = elm_layout_add(collision_data->base.win);
   elm_layout_file_set(
      sh2, PACKAGE_DATA_DIR "/" EPHYSICS_TEST_THEME ".edj", "shadow-ball");
   evas_object_move(sh2, WIDTH / 8, FLOOR_Y);
   evas_object_resize(sh2, 70, 3);
   evas_object_show(sh2);
   collision_data->base.evas_objs = eina_list_append(
      collision_data->base.evas_objs, sh2);

   sphere2 = elm_image_add(collision_data->base.win);
   elm_image_file_set(
      sphere2, PACKAGE_DATA_DIR "/" EPHYSICS_TEST_THEME ".edj",
      "big-red-ball");
   evas_object_move(sphere2, WIDTH / 8, FLOOR_Y - 70 + 1);
   evas_object_resize(sphere2, 70, 70);
   evas_object_show(sphere2);
   collision_data->base.evas_objs = eina_list_append(
      collision_data->base.evas_objs, sphere2);

   sphere_body2 = ephysics_body_circle_add(collision_data->base.world);
   ephysics_body_evas_object_set(sphere_body2, sphere2, EINA_TRUE);
   ephysics_body_event_callback_add(sphere_body2,
                                    EPHYSICS_CALLBACK_BODY_UPDATE,
                                    update_object_cb, sh2);
   ephysics_body_restitution_set(sphere_body2, 1);
   ephysics_body_friction_set(sphere_body2, 0.4);
   ephysics_body_central_impulse_apply(sphere_body2, 12, 0);
   collision_data->sphere2 = sphere_body2;
   collision_data->base.bodies = eina_list_append(
      collision_data->base.bodies, sphere_body2);

   impact = elm_layout_add(collision_data->base.win);
   elm_layout_file_set(
      impact, PACKAGE_DATA_DIR "/" EPHYSICS_TEST_THEME ".edj", "impact");
   evas_object_resize(impact, 20, 20);
   evas_object_show(impact);
   collision_data->base.evas_objs = eina_list_append(
      collision_data->base.evas_objs, impact);
   collision_data->impact = impact;
}

static void
_restart(void *data, Evas_Object *obj __UNUSED__, const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Collision_Data *collision_data = data;

   DBG("Restart pressed");
   ephysics_body_event_callback_del(collision_data->sphere,
                                    EPHYSICS_CALLBACK_BODY_COLLISION,
                                    _collision_cb);
   test_clean((Test_Data *)collision_data);
   _world_populate(collision_data);
   ephysics_body_event_callback_add(collision_data->sphere,
                                    EPHYSICS_CALLBACK_BODY_COLLISION,
                                    _collision_cb, collision_data);
}

static void
_win_del(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Collision_Data *collision_data = data;

   test_clean((Test_Data *)collision_data);
   evas_object_del(collision_data->base.layout);
   ephysics_world_del(collision_data->base.world);
   free(collision_data);
   ephysics_shutdown();
}

void
test_collision(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Collision_Data *collision_data;
   EPhysics_Body *boundary;
   EPhysics_World *world;
   Evas_Object *edje;

   if (!ephysics_init())
     return;

   collision_data = calloc(1, sizeof(Collision_Data));
   if (!collision_data)
     {
        ERR("Failed to create test data");
        ephysics_shutdown();
        return;
     }

   test_win_add((Test_Data *)collision_data, "Collision Detection", EINA_FALSE);
   evas_object_smart_callback_add(collision_data->base.win,
                                  "delete,request", _win_del, collision_data);

   edje = elm_layout_edje_get(collision_data->base.layout);
   edje_object_signal_callback_add(edje, "restart", "test-theme", _restart,
                                   collision_data);

   world = ephysics_world_new();
   ephysics_world_render_geometry_set(world, 50, 40, WIDTH - 100, FLOOR_Y - 40);
   collision_data->base.world = world;

   boundary = ephysics_body_bottom_boundary_add(collision_data->base.world);
   ephysics_body_restitution_set(boundary, 0);
   ephysics_body_friction_set(boundary, 20);

   boundary = ephysics_body_right_boundary_add(collision_data->base.world);
   ephysics_body_restitution_set(boundary, 0.8);

   ephysics_body_left_boundary_add(collision_data->base.world);
   ephysics_body_top_boundary_add(collision_data->base.world);

   _world_populate(collision_data);

   ephysics_body_event_callback_add(collision_data->sphere,
                                    EPHYSICS_CALLBACK_BODY_COLLISION,
                                    _collision_cb, collision_data);
}