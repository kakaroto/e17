#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Edje.h>

#include "efbb.h"

#define CANNON_DATA_GET_RETURN(_obj, _data, _ret) \
   if (!_obj) \
     { \
        ERR("Cannon is null."); \
        return _ret; \
     } \
 \
   _data = evas_object_data_get(_obj, "cannon_data"); \
   if (!_data) \
     { \
        ERR("No cannon data."); \
        return _ret; \
     }

#define CANNON_DATA_GET(_obj, _data) \
   if (!_obj) \
     { \
        ERR("Cannon is null."); \
        return; \
     } \
 \
   _data = evas_object_data_get(_obj, "cannon_data"); \
   if (!_data) \
     { \
        ERR("No cannon data."); \
        return; \
     }

typedef enum
{
   MSG_ID_AMMO,
   MSG_ID_ANGLE,
   MSG_ID_STRENGTH
} Msg_Id;

typedef struct _Cannon_Data
{
   int strength, strength_max, strength_min;
   int angle, angle_max, angle_min;
   int ammo;
   Eina_Bool loaded:1;
} Cannon_Data;

static void
_cannon_del_cb(void *data __UNUSED__, Evas *e __UNUSED__, Evas_Object *obj,
               void *event_info __UNUSED__)
{
   Cannon_Data *cannon_data = evas_object_data_del(obj, "cannon_data");
   if (!cannon_data)
     return;
   free(cannon_data);
}

static void
_cannon_empty_cb(void *data, Evas_Object *obj __UNUSED__,
                 const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Cannon_Data *cannon_data = data;
   cannon_data->loaded = EINA_FALSE;
   INF("Cannon empty. Ammo: %i.", cannon_data->ammo);
}

static void
_cannon_message_send(Evas_Object *cannon, Msg_Id msg_id, int value)
{
   Edje_Message_Int msg;
   msg.val = value;
   edje_object_message_send(cannon, EDJE_MESSAGE_INT, msg_id, &msg);
}

Evas_Object *
cannon_add(Evas_Object *win)
{
   Cannon_Data *cannon_data;
   Evas_Object *cannon;

   cannon_data = calloc(1, sizeof(Cannon_Data));
   if (!cannon_data)
     {
        ERR("Failed to create cannon!");
        return NULL;
     }

   cannon_data->angle_max = 60;
   cannon_data->strength_max = 100;
   cannon_data->angle = cannon_data->angle_min;
   cannon_data->strength = (cannon_data->strength_max -
                            cannon_data->strength_min) / 2;

   cannon = edje_object_add(evas_object_evas_get(win));
   edje_object_file_set(cannon, PACKAGE_DATA_DIR "/" GAME_THEME ".edj",
                        "cannon");
   edje_object_signal_callback_add(cannon, "empty", "cannon_theme",
                                   _cannon_empty_cb, cannon_data);
   evas_object_data_set(cannon, "cannon_data", cannon_data);
   evas_object_event_callback_add(cannon, EVAS_CALLBACK_DEL, _cannon_del_cb,
                                  NULL);

   _cannon_message_send(cannon, MSG_ID_AMMO, cannon_data->ammo);
   _cannon_message_send(cannon, MSG_ID_ANGLE, cannon_data->angle);
   _cannon_message_send(cannon, MSG_ID_STRENGTH, cannon_data->strength);

   return cannon;
}

void
cannon_shoot(Evas_Object *cannon)
{
   Cannon_Data *cannon_data;
   CANNON_DATA_GET(cannon, cannon_data);

   if (!cannon_data->loaded)
     {
        INF("Trying to shoot, but not loaded.");
        return;
     }

   edje_object_signal_emit(cannon, "shoot", "cannon");
}

void
cannon_load(Evas_Object *cannon)
{
   Cannon_Data *cannon_data;
   CANNON_DATA_GET(cannon, cannon_data);

   if (!cannon_data->ammo)
     {
        INF("Trying to load, but no remaining ammo.");
        return;
     }

   if (cannon_data->loaded)
     {
        INF("Trying to load, but already loaded.");
        return;
     }

   edje_object_signal_emit(cannon, "load", "cannon");

   cannon_data->loaded = EINA_TRUE;
   cannon_data->ammo--;
   _cannon_message_send(cannon, MSG_ID_AMMO, cannon_data->ammo);
}

Eina_Bool
cannon_loaded_get(Evas_Object *cannon)
{
   Cannon_Data *cannon_data;
   CANNON_DATA_GET_RETURN(cannon, cannon_data, EINA_FALSE);
   return cannon_data->loaded;
}

void
cannon_angle_set(Evas_Object *cannon, int angle)
{
   Cannon_Data *cannon_data;
   CANNON_DATA_GET(cannon, cannon_data);

   if ((angle < cannon_data->angle_min) || (angle > cannon_data->angle_max))
     {
        ERR("Invalid angle. Must to be between %i and %i.",
            cannon_data->angle_min, cannon_data->angle_max);
        return;
     }

   cannon_data->angle = angle;
   _cannon_message_send(cannon, MSG_ID_ANGLE, cannon_data->angle);
}

int
cannon_angle_get(Evas_Object *cannon)
{
   Cannon_Data *cannon_data;
   CANNON_DATA_GET_RETURN(cannon, cannon_data, -1);
   return cannon_data->angle;
}

int
cannon_angle_min_get(Evas_Object *cannon)
{
   Cannon_Data *cannon_data;
   CANNON_DATA_GET_RETURN(cannon, cannon_data, 0);
   return cannon_data->angle_min;
}

int
cannon_angle_max_get(Evas_Object *cannon)
{
   Cannon_Data *cannon_data;
   CANNON_DATA_GET_RETURN(cannon, cannon_data, 0);
   return cannon_data->angle_max;
}

void
cannon_strength_set(Evas_Object *cannon, int strength)
{
   Cannon_Data *cannon_data;
   CANNON_DATA_GET(cannon, cannon_data);

   if ((strength < cannon_data->strength_min) ||
       (strength > cannon_data->strength_max))
     {
        ERR("Invalid strength. Must to be between %i and %i.",
            cannon_data->strength_min, cannon_data->strength_max);
        return;
     }

   cannon_data->strength = strength;
   _cannon_message_send(cannon, MSG_ID_STRENGTH, cannon_data->strength);
}

int
cannon_strength_get(Evas_Object *cannon)
{
   Cannon_Data *cannon_data;
   CANNON_DATA_GET_RETURN(cannon, cannon_data, EINA_FALSE);
   return cannon_data->strength;
}

int
cannon_strength_min_get(Evas_Object *cannon)
{
   Cannon_Data *cannon_data;
   CANNON_DATA_GET_RETURN(cannon, cannon_data, 0);
   return cannon_data->strength_min;
}

int
cannon_strength_max_get(Evas_Object *cannon)
{
   Cannon_Data *cannon_data;
   CANNON_DATA_GET_RETURN(cannon, cannon_data, 0);
   return cannon_data->strength_max;
}

void
cannon_ammo_set(Evas_Object *cannon, int ammo)
{
   Cannon_Data *cannon_data;
   CANNON_DATA_GET(cannon, cannon_data);

   if (ammo < 0)
     {
        ERR("Trying to set negative ammo. Not allowed");
        return;
     }

   cannon_data->ammo = ammo;
   _cannon_message_send(cannon, MSG_ID_AMMO, cannon_data->ammo);
}

int
cannon_ammo_get(Evas_Object *cannon)
{
   Cannon_Data *cannon_data;
   CANNON_DATA_GET_RETURN(cannon, cannon_data, -1);
   return cannon_data->ammo;
}
