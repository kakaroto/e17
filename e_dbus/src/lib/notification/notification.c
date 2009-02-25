#include "E_Notify.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <Ecore_Data.h>
#include "e_notify_private.h"

/* private functions */
static Eina_List * e_notification_action_list_new();
static E_Notification_Action *e_notification_action_new(const char *id, const char *name);
static void e_notification_action_free(E_Notification_Action *act);

/* (con|de)structor */

EAPI E_Notification *
e_notification_full_new(const char *app_name, unsigned int replaces_id, const char *app_icon, const char *summary, const char *body, int expire_timeout)
{
  E_Notification *n;

  n = e_notification_new();
  if (!n) return NULL;

  if (app_name) n->app_name = strdup(app_name); 
  n->replaces_id = replaces_id;
  if (app_icon) n->app_icon = strdup(app_icon); 
  if (summary) n->summary = strdup(summary); 
  if (body) n->body = strdup(body);
  n->expire_timeout = expire_timeout;


  return n;
}

EAPI E_Notification *
e_notification_new()
{
  E_Notification *n;
  n = calloc(1, sizeof(E_Notification));
  if (!n) return NULL;
  n->refcount = 1;

  return n;
}

EAPI void
e_notification_ref(E_Notification *n)
{
  n->refcount++;
}

EAPI void
e_notification_unref(E_Notification *n)
{
  if (--n->refcount == 0) e_notification_free(n);
}

EAPI void
e_notification_free(E_Notification *n)
{
  if (!n) return;

  if (n->app_name) free(n->app_name);
  if (n->app_icon) free(n->app_icon);
  if (n->summary) free(n->summary);
  if (n->body) free(n->body);

  while (n->actions) 
    n->actions = eina_list_remove_list(n->actions, n->actions);

  if (n->hints.category) free(n->hints.category);
  if (n->hints.desktop) free(n->hints.desktop);
  if (n->hints.sound_file) free(n->hints.sound_file);
  if (n->hints.image_data) e_notification_image_free(n->hints.image_data);
  if (n->hints.icon_data) e_notification_image_free(n->hints.icon_data);
  free(n);
}

/* mutators */
EAPI void
e_notification_id_set(E_Notification *note, unsigned int id)
{
  note->id = id;
}

EAPI void
e_notification_app_name_set(E_Notification *note, const char *app_name)
{
  if (note->app_name) free(note->app_name);
  if (app_name) note->app_name = strdup(app_name);
}

EAPI void
e_notification_app_icon_set(E_Notification *note, const char *app_icon)
{
  if (note->app_icon) free(note->app_icon);
  if (app_icon) note->app_icon = strdup(app_icon);
}

EAPI void
e_notification_summary_set(E_Notification *note, const char *summary)
{
  if (note->summary) free(note->summary);
  if (summary) note->summary = strdup(summary);
}

EAPI void
e_notification_body_set(E_Notification *note, const char *body)
{
  if (note->body) free(note->body);
  if (body) note->body = strdup(body);
}

EAPI void
e_notification_action_add(E_Notification *n, const char *action_id, const char *action_name)
{
  E_Notification_Action *a;

  if (!n->actions) 
    n->actions = e_notification_action_list_new();

  a = e_notification_action_new(action_id, action_name);
  n->actions = eina_list_append(n->actions, a);
}


EAPI void
e_notification_replaces_id_set(E_Notification *note, int replaces_id)
{
  note->replaces_id = replaces_id;
}

EAPI void
e_notification_timeout_set(E_Notification *note, int timeout)
{
  note->expire_timeout = timeout;
}

EAPI void
e_notification_closed_set(E_Notification *note, unsigned char closed)
{
  note->closed = closed;
}


/* accessors */
EAPI unsigned int
e_notification_id_get(E_Notification *note)
{
  return note->id;
}

EAPI const char *
e_notification_app_name_get(E_Notification *note)
{
  return note->app_name;
}

EAPI const char *
e_notification_app_icon_get(E_Notification *note)
{
  return note->app_icon;
}

EAPI const char *
e_notification_summary_get(E_Notification *note)
{
  return note->summary;
}

EAPI const char *
e_notification_body_get(E_Notification *note)
{
  return note->body;
}

EAPI Eina_List *
e_notification_actions_get(E_Notification *note)
{
  return note->actions;
}

EAPI int
e_notification_replaces_id_get(E_Notification *note)
{
  return note->replaces_id;
}

EAPI int
e_notification_timeout_get(E_Notification *note)
{
  return note->expire_timeout;
}

EAPI unsigned char
e_notification_closed_get(E_Notification *note)
{
  return note->closed;
}

/***** actions *****/

static Eina_List *
e_notification_action_list_new()
{
  Eina_List *alist;

  alist = NULL;
  return alist;
}

static E_Notification_Action *
e_notification_action_new(const char *id, const char *name)
{
  E_Notification_Action *act;
  act = malloc(sizeof(E_Notification_Action));
  act->id = strdup(id);
  act->name = strdup(name);
  return act;
}


static void
e_notification_action_free(E_Notification_Action *act)
{
  if (!act) return;
  if (act->id) free(act->id);
  if (act->name) free(act->name);
  free(act);
}


/********* hints *******/


EAPI void 
e_notification_hint_urgency_set(E_Notification *n, char urgency)
{
  n->hints.urgency = urgency;
  n->hint_flags |= E_NOTIFICATION_HINT_URGENCY;
}

EAPI void 
e_notification_hint_category_set(E_Notification *n, const char *category)
{
  if (n->hints.category) free(n->hints.category);
  n->hints.category = strdup(category);
  n->hint_flags |= E_NOTIFICATION_HINT_CATEGORY;
}

EAPI void 
e_notification_hint_desktop_set(E_Notification *n, const char *desktop)
{
  if (n->hints.desktop) free(n->hints.desktop);
  n->hints.desktop = strdup(desktop);
  n->hint_flags |= E_NOTIFICATION_HINT_DESKTOP;
}

EAPI void 
e_notification_hint_sound_file_set(E_Notification *n, const char *sound_file)
{
  if (n->hints.sound_file) free(n->hints.sound_file);
  n->hints.sound_file = strdup(sound_file);
  n->hint_flags |= E_NOTIFICATION_HINT_SOUND_FILE;
}

EAPI void 
e_notification_hint_suppress_sound_set(E_Notification *n, char suppress_sound)
{
  n->hints.suppress_sound = suppress_sound;
  n->hint_flags |= E_NOTIFICATION_HINT_SUPPRESS_SOUND;
}

EAPI void 
e_notification_hint_xy_set(E_Notification *n, int x, int y)
{
  n->hints.x = x;
  n->hints.y = y;
  n->hint_flags |= E_NOTIFICATION_HINT_XY;
}

EAPI void 
e_notification_hint_image_data_set(E_Notification *n, E_Notification_Image *image)
{
  n->hints.image_data = image;
}


EAPI char  
e_notification_hint_urgency_get(E_Notification *n)
{
  return (n->hint_flags & E_NOTIFICATION_HINT_URGENCY ? n->hints.urgency : 1);
}

EAPI const char *
e_notification_hint_category_get(E_Notification *n)
{
  return n->hints.category;
}

EAPI const char *
e_notification_hint_desktop_get(E_Notification *n)
{
  return n->hints.desktop;
}

EAPI const char *
e_notification_hint_sound_file_get(E_Notification *n)
{
  return n->hints.sound_file;
}

EAPI char  
e_notification_hint_suppress_sound_get(E_Notification *n)
{
  return n->hints.suppress_sound;
}

EAPI int  
e_notification_hint_xy_get(E_Notification *n, int *x, int *y)
{
  if (x) *x = n->hints.x;
  if (y) *y = n->hints.y;

  return (n->hint_flags & E_NOTIFICATION_HINT_XY ? 1 : 0);
}

EAPI E_Notification_Image *
e_notification_hint_image_data_get(E_Notification *n)
{
  return n->hints.image_data;
}


EAPI E_Notification_Image *
e_notification_hint_icon_data_get(E_Notification *n)
{
  return n->hints.icon_data;
}

EAPI E_Notification_Image *
e_notification_image_new()
{
  E_Notification_Image *img;

  img = calloc(1, sizeof(E_Notification_Image));
  return img;
}

EAPI void
e_notification_image_free(E_Notification_Image *img)
{
  if (img->data) free(img->data);
  if (img) free(img);
}


EAPI Evas_Object *
e_notification_image_evas_object_add(Evas *evas, E_Notification_Image *img)
{
  int *imgdata;
  Evas_Object *o = NULL;

  if (!evas || !img) return NULL;

  o = evas_object_image_add(evas);
  evas_object_resize(o, img->width, img->height);
  evas_object_image_alpha_set(o, img->has_alpha);
  evas_object_image_size_set(o, img->width, img->height);
  evas_object_image_fill_set(o, 0, 0, img->width, img->height);
  imgdata = evas_object_image_data_get(o, 1);

  if (img->bits_per_sample == 8)
    {
      /* Although not specified.
       * The data are very likely to come from a GdkPixbuf
       * which align each row on a 4-bytes boundary when using RGB.
       * And is RGBA otherwise.
       */
      int x, y;
      int32_t *dest;
      unsigned char *src;
      for (y = 0; y < img->height; y++)
        {
          src  = img->data + y * img->rowstride;
          dest = imgdata + y * img->width;

          for (x = 0; x < img->width; x++, src += img->channels, dest++)
            {
              *dest  = *(src + 2);
              *dest += *(src + 1) << 8;
              *dest += *(src + 0) << 16;
              if (img->has_alpha)
                *dest += *(src + 3) << 24;
              else
                *dest += 255 << 24;
            }
        }
    }
  evas_object_image_data_update_add(o, 0, 0, img->width, img->height);
  evas_object_image_data_set(o, imgdata);
  return o;
}
