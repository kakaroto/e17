#include "entrance_user.h"
#include "entrance_smart.h"
#include <stdlib.h>
#include <stdio.h>
#include <Evas.h>
#include <Edje.h>
#include <string.h>
#include <limits.h>
#include "../config.h"
#include "util.h"

/**
@file entrance_user.c
@brief Variables and Data relating to an instance of a user in entrance

*/

static Evas_Object *_entrance_user_icon_load(Evas_Object * o, char *file);
extern void user_selected_cb(void *data, Evas_Object * o,
                             const char *emission, const char *source);
extern void user_unselected_cb(void *data, Evas_Object * o,
                               const char *emission, const char *source);

/**
 * entrance_user_new - given a user, icon, and session, create a new
 * Entrance_User instance.  We don't copy the pointers here, simply use
 * them, it allows for us to pass in NULL, and easily use ecore_config
 * supplied data w/o having to free it.
 * @param user - the user's user id on the system
 * @param icon - the user's "face" icon, an edje or eet
 * @param session - the key into the session hash that this user considers their
 * "default"
 */
Entrance_User *
entrance_user_new(char *user, char *icon, char *session)
{
   Entrance_User *e = NULL;

   if ((e = malloc(sizeof(Entrance_User))))
   {
      memset(e, 0, sizeof(Entrance_User));
      e->name = user;
      if (icon)
         e->icon = icon;
      else
         e->icon = strdup("default.eet");
      e->session = session;
   }
   return (e);
}

/**
 * entrance_user_auto_login_set : allow this user to auto login, currently
 * not used
 * @param e - the Entrance_User the request is for
 * @param allow - true(1) to allow, false(0) to disallow
 */
void
entrance_user_auto_login_set(Entrance_User * e, int allow)
{
   if (e)
      e->autologin = allow;
}

/**
 * entrance_user_free: free this Entrance_User
 * @param e - the Entrance_User to free
 */
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

/**
 * entrance_user_edje_get : get the Evas_Object(edje||image) that is
 * specified in config db.  Their "entrance.user.avatar"
 * @param e - the Entrance_User to get the edje for
 * @param edje - a pointer to the main edje in entrance
 * @return NULL on failure, a valid Evas_Object on success
 */
Evas_Object *
entrance_user_edje_get(Entrance_User * e, Evas_Object * edje,
                       const char *file)
{
   Evas_Coord w, h;
   Evas_Object *o = NULL;
   Evas_Object *result = NULL;
   Evas_Object *avatar = NULL;

   if (e && edje)
   {
      result = entrance_smart_add(evas_object_evas_get(edje));
      o = edje_object_add(evas_object_evas_get(edje));
      entrance_smart_edje_set(result, o);
      if (edje_object_file_set(o, file, "User"))
      {
         evas_object_move(o, -9999, -9999);
         edje_object_size_min_get(o, &w, &h);
         if ((w > 0) && (h > 0))
            evas_object_resize(result, w, h);

         if (edje_object_part_exists(o, "entrance.user.icon"))
         {
            if ((avatar = _entrance_user_icon_load(edje, e->icon)))
            {
               entrance_smart_avatar_set(result, avatar);
               edje_object_part_swallow(o, "entrance.user.icon", avatar);
            }
         }
         if (edje_object_part_exists(o, "entrance.user.name"))
         {
            edje_object_part_text_set(o, "entrance.user.name", e->name);
         }
	    /**
	     * FIXME: perhaps go so far as to do Gecos type stuff
	     */
         edje_object_signal_callback_add(o, "entrance,user,selected", "",
                                         user_selected_cb, e);
         edje_object_signal_callback_add(o, "entrance,user,unselected", "",
                                         user_unselected_cb, e);
         evas_object_show(result);
      }
      else
      {
         fprintf(stderr, "Failed on: %s(%s)\n", e->name, file);
         evas_object_del(result);
         result = NULL;
      }
   }
   return (result);
}

/**
 * _entrance_user_icon_load : It says this is the filename, gimme an evas
 * object for it, I don't care what type. :)
 * @param o - another object in the Canvas
 * @param file - the filename on disk, might be an edje, might be an image
 * @return NULL on failure, a valid Evas_Object on success
 */
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
   evas_object_move(result, -999, -999);
   evas_object_resize(result, 48, 48);
   evas_object_show(result);
   return (result);
}
