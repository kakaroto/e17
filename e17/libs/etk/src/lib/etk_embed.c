/** @file etk_embed.c */
#include "etk_embed.h"
#include <stdlib.h>
#include "etk_main.h"
#include "etk_utils.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "config.h"

/**
 * @addtogroup Etk_Embed
 * @{
 */
 
static void _etk_embed_geometry_get(Etk_Toplevel_Widget *toplevel, int *x, int *y, int *w, int *h);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Embed
 * @return Returns the type on an Etk_Embed
 */
Etk_Type *etk_embed_type_get()
{
   static Etk_Type *embed_type = NULL;

   if (!embed_type)
   {
      embed_type = etk_type_new("Etk_Embed", ETK_TOPLEVEL_WIDGET_TYPE, sizeof(Etk_Embed), NULL, NULL);
   }

   return embed_type;
}

/**
 * @brief Creates a new embed widget
 * @param evas the evas where the embed object should belong
 * @return Returns the new embed widget, or NULL on failure (if the evas is invalid)
 */
Etk_Widget *etk_embed_new(Evas *evas)
{
   Etk_Widget *embed;
   
   if (!evas)
      return NULL;
   
   embed = etk_widget_new(ETK_EMBED_TYPE, NULL);
   ETK_TOPLEVEL_WIDGET(embed)->evas = evas;
   ETK_TOPLEVEL_WIDGET(embed)->geometry_get = _etk_embed_geometry_get;
   
   /* TODO: font path */
   evas_font_path_append(evas, PACKAGE_DATA_DIR "/fonts/");
   /* TODO: FIXME: We need that to force the widget to realize */
   etk_object_properties_set(ETK_OBJECT(embed), "theme_group", "", NULL);
   
   /* If the widget has failed to realize, we destroy it, and we return NULL */
   if (!(ETK_WIDGET(embed)->smart_object))
   {
      ETK_WARNING("The embed widget could not be created, the evas seems invalid");
      etk_object_destroy(ETK_OBJECT(embed));
      return NULL;
   }
   
   return embed;
}

/**
 * @brief Gets the evas smart object of the embed widget. This object can be manipulate like the other evas object,
 * with evas_object_move(), evas_object_resize(), evas_object_clip_set(), ...
 * @param embed an embed widget
 * @return Returns the evas smart object of the embed widget
 */
Evas_Object *etk_embed_object_get(Etk_Embed *embed)
{
   if (!embed)
      return NULL;
   return ETK_WIDGET(embed)->smart_object;
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Gets the geometry of the embed widget */
static void _etk_embed_geometry_get(Etk_Toplevel_Widget *toplevel, int *x, int *y, int *w, int *h)
{
   etk_widget_geometry_get(ETK_WIDGET(toplevel), x, y, w, h);
}

/** @} */
