#include "entrance.h"
#include "entrance_x_session.h"
#include <Edje.h>

extern void session_item_selected_cb(void *data, Evas_Object * o,
                                     const char *emission,
                                     const char *source);
extern void session_item_unselected_cb(void *data, Evas_Object * o,
                                       const char *emission,
                                       const char *source);
Entrance_X_Session *
entrance_x_session_new(char *name, char *icon, char *session)
{
   Entrance_X_Session *e = NULL;

   if ((e = malloc(sizeof(Entrance_X_Session))))
   {
      memset(e, 0, sizeof(Entrance_X_Session));
      e->name = name;
      if (icon)
         e->icon = icon;
      else
         e->icon = strdup("default.png");
      e->session = session;
   }
   return (e);
}

void
entrance_x_session_free(Entrance_X_Session * e)
{
   if (e)
   {
      if (e->name)
         free(e->name);
      if (e->icon)
         free(e->icon);
      if (e->session)
         free(e->session);
      free(e);
   }

}

/**
 * given the filename, create a new evas object(edje or image) with the
 * contents of file.  file can either bea valid edje eet or anything your
 * evas has images loaders for.
 * FIXME: Should this be its own smart object, user images are done similar 
 * FIXME: Should it support a "key" paramater as well
 * @param o - the entrance session you're working with
 * @param file - the file in $pkgdatadir/images/sessions/ we want to load
 */
static Evas_Object *
entrance_x_session_icon_load(Evas_Object * o, const char *file)
{
   Evas_Object *result = NULL;
   char buf[PATH_MAX];

   if (!o || !file)
      return (NULL);

   result = edje_object_add(evas_object_evas_get(o));
   snprintf(buf, PATH_MAX, "%s/images/sessions/%s", PACKAGE_DATA_DIR, file);
   if (!edje_object_file_set(result, buf, "Icon"))
   {
      evas_object_del(result);
      result = evas_object_image_add(evas_object_evas_get(o));
      evas_object_image_file_set(result, buf, NULL);
      if (evas_object_image_load_error_get(result))
      {
         snprintf(buf, PATH_MAX, "%s/images/sessions/default.png",
                  PACKAGE_DATA_DIR);
         result = evas_object_image_add(evas_object_evas_get(o));
         evas_object_image_file_set(result, buf, NULL);
      }
      evas_object_intercept_resize_callback_add(result,
                                                entrance_edje_object_resize_intercept_cb,
                                                NULL);
   }
   evas_object_pass_events_set(result, 1);
   evas_object_move(result, -999, -999);
   evas_object_resize(result, 48, 48);
   evas_object_layer_set(result, 0);
   evas_object_show(result);
   return (result);
}

Evas_Object *
entrance_x_session_edje_get(Entrance_X_Session * e, Evas_Object * o,
                            const char *themefile)
{
   Evas_Coord w, h;
   Evas_Object *oo = NULL;
   Evas_Object *edje = NULL;

   if (!o || !themefile || !e)
      return (NULL);

   edje = edje_object_add(evas_object_evas_get(o));
   if (edje_object_file_set(edje, themefile, "Session") > 0)
   {
      evas_object_layer_set(edje, 0);
      evas_object_move(edje, -9999, -9999);
      edje_object_size_min_get(edje, &w, &h);
      if ((w > 0) && (h > 0))
         evas_object_resize(edje, w, h);
      else
         evas_object_resize(edje, 150, 50);

      if (edje_object_part_exists(edje, "EntranceSessionIcon"))
      {
         if ((oo = entrance_x_session_icon_load(o, e->icon)))
         {
            edje_object_part_swallow(edje, "EntranceSessionIcon", oo);
         }
      }
      if (edje_object_part_exists(edje, "EntranceSessionTitle"))
      {
         edje_object_part_text_set(edje, "EntranceSessionTitle", e->name);
      }
      edje_object_signal_callback_add(edje, "SessionSelected", "",
                                      session_item_selected_cb,
                                      (Entrance_X_Session *) e);
      edje_object_signal_callback_add(edje, "SessionUnSelected", "",
                                      session_item_selected_cb,
                                      (Entrance_X_Session *) e);
      evas_object_show(edje);
   }
   else
   {
      fprintf(stderr, "Failed on: %s\n", themefile);
      evas_object_del(edje);
      edje = NULL;
   }
   return (edje);
}
