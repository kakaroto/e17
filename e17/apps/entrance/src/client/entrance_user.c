#include "entrance_user.h"
#include <stdlib.h>
#include <stdio.h>
#include <Evas.h>
#include <Edje.h>
#include <string.h>
#include <limits.h>
#include "../config.h"

/** 
 * protos 
 */
static Evas_Object *_entrance_user_icon_load(Evas_Object * o, char *file);
extern void user_selected_cb(void *data, Evas_Object * o,
                             const char *emission, const char *source);
extern void user_unselected_cb(void *data, Evas_Object * o,
                               const char *emission, const char *source);

Entrance_User *
entrance_user_new(char *user, char *icon, char *session)
{
   Entrance_User *e = NULL;

   if ((e = malloc(sizeof(Entrance_User))))
   {
      memset(e, 0, sizeof(Entrance_User));
      e->name = user;
      e->icon = icon;
      e->session = session;
   }
   return (e);
}

void
entrance_user_auto_login_set(Entrance_User * e, int allow)
{
   if (e)
      e->autologin = allow;
}

void
entrance_user_free(Entrance_User * e)
{
   if (e)
   {
      if (e->name)
         free(e->name);
      if (e->icon)
         free(e->icon);
      free(e);
   }
}

Evas_Object *
entrance_user_edje_get(Entrance_User * e, Evas_Object * edje)
{
   Evas_Object *o = NULL;
   Evas_Object *oo = NULL;
   const char *file = NULL;
   Evas_Coord w, h;

   if (e && edje)
   {
      o = edje_object_add(evas_object_evas_get(edje));
      edje_object_file_get(edje, &file, NULL);
      if (edje_object_file_set(o, file, "User"))
      {
         evas_object_layer_set(o, 0);
         evas_object_move(o, -9999, -9999);

         if (edje_object_part_exists(o, "EntranceUserIcon"))
         {
            oo = _entrance_user_icon_load(edje, e->icon);
            if (!strcmp(evas_object_type_get(oo), "image"))
            {

               edje_object_part_geometry_get(oo, "EntranceUserIcon", NULL,
                                             NULL, &w, &h);
               evas_object_image_fill_set(oo, 0.0, 0.0, w, h);
            }
            edje_object_part_swallow(o, "EntranceUserIcon", oo);
         }
         if (edje_object_part_exists(o, "EntranceUserName"))
         {
            edje_object_part_text_set(o, "EntranceUserName", e->name);
         }
	    /**
	     * FIXME: perhaps go so far as to do Gecos type shit
	     */
         edje_object_signal_callback_add(o, "UserSelected", "",
                                         user_selected_cb, e);
         edje_object_signal_callback_add(o, "UserUnSelected", "",
                                         user_unselected_cb, e);
         evas_object_show(o);
      }
      else
      {
         fprintf(stderr, "Failed on: %s(%s)\n", e->name, e->icon);
         evas_object_del(o);
         evas_object_del(oo);
      }
   }
   return (o);
}

static Evas_Object *
_entrance_user_icon_load(Evas_Object * o, char *file)
{
   Evas_Object *result = NULL;
   char buf[PATH_MAX];

   if (!o)
      return (NULL);

   result = edje_object_add(evas_object_evas_get(o));
   snprintf(buf, PATH_MAX, "%s/users/%s", PACKAGE_DATA_DIR, file);
   if (!edje_object_file_set(result, buf, "Icon"))
   {
      evas_object_del(result);
      result = evas_object_image_add(evas_object_evas_get(o));
      evas_object_image_file_set(result, buf, NULL);
      if (evas_object_image_load_error_get(result))
      {
         snprintf(buf, PATH_MAX, "%s/users/default.png", PACKAGE_DATA_DIR);
         result = evas_object_image_add(evas_object_evas_get(o));
         evas_object_image_file_set(result, buf, NULL);
      }
   }
   evas_object_pass_events_set(result, 1);
   evas_object_move(result, -999, -999);
   evas_object_resize(result, 48, 48);
   evas_object_layer_set(result, 0);
   evas_object_show(result);
   return (result);
}
