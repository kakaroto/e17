#include "etk_ps.h"
#include <Evas.h>
#include <Edje.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"
#include "config.h"


#define round(a) ( ((a)<0.0) ? (int)(floor((a) - 0.5)) : (int)(floor((a) + 0.5)) )


/**
 * @addtogroup Etk_Ps
 * @{
 */

enum _Etk_Ps_Property_Id
{
   ETK_PS_FILE_PROPERTY
};

static void _etk_ps_constructor(Etk_Ps *ps);
static void _etk_ps_destructor(Etk_Ps *ps);
static void _etk_ps_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_ps_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_ps_realize_cb(Etk_Object *object, void *data);
static void _etk_ps_unrealize_cb(Etk_Object *object, void *data);
static void _etk_ps_size_request(Etk_Widget *widget, Etk_Size *size_requisition);
static void _etk_ps_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_ps_load(Etk_Ps *ps);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Ps
 * @return Returns the type on an Etk_Ps
 */
Etk_Type *etk_ps_type_get()
{
   static Etk_Type *ps_type = NULL;

   if (!ps_type)
   {
      ps_type = etk_type_new("Etk_Ps", ETK_WIDGET_TYPE, sizeof(Etk_Ps), ETK_CONSTRUCTOR(_etk_ps_constructor), ETK_DESTRUCTOR(_etk_ps_destructor), NULL);

      etk_type_property_add(ps_type, "ps_file", ETK_PS_FILE_PROPERTY, ETK_PROPERTY_STRING, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_string(NULL));

      ps_type->property_set = _etk_ps_property_set;
      ps_type->property_get = _etk_ps_property_get;
   }

   return ps_type;
}

/**
 * @brief Creates a new ps
 * @return Returns the new ps widget
 */
Etk_Widget *etk_ps_new()
{
   return etk_widget_new(ETK_PS_TYPE, NULL);
}

/**
 * @brief Loads the ps from a file
 * @param ps an ps
 * @param filename the name of the file to load
 */
int etk_ps_file_set(Etk_Ps *ps, const char *filename)
{
   Etk_Widget *widget;

   if (!(widget = ETK_WIDGET(ps)) || (ps->filename == filename))
      return 0;

   free(ps->filename);
   ps->filename = filename ? strdup(filename) : NULL;
   etk_object_notify(ETK_OBJECT(ps), "ps_file");

   if (ps->ps_document)
      eps_document_delete(ps->ps_document);

   if (ps->ps_page)
      eps_page_delete(ps->ps_page);

   ps->ps_document = eps_document_new(ps->filename);
   if (!ps->ps_document)
   {
      return 0;
   }
   ps->ps_page = eps_page_new(ps->ps_document);
   {
      eps_document_delete(ps->ps_document);
      return 0;
   }

   _etk_ps_load(ps);

   return 1;
}

/**
 * @brief Gets the name of the file used for the ps
 * @param ps an ps
 * @return Returns the name of the file use for the ps (NULL on failure)
 */
const char *etk_ps_file_get(Etk_Ps *ps)
{
   if (!ps)
      return NULL;
   return ps->filename;
}

/**
 * @brief Set a page number
 * @param ps: the ps to change page
 * @param page: the page number
 */
void etk_ps_page_set(Etk_Ps *ps, int page)
{
   if (!ps ||
       !ps->ps_document ||
       (page < 0) ||
       (page >= eps_document_page_count_get(ps->ps_document)) ||
       (page == eps_page_page_get(ps->ps_page)))
      return;

   eps_page_page_set(ps->ps_page, page);
   _etk_ps_load(ps);
}

/**
 * @brief Get the page number
 * @param ps: the ps to change page
 * @return Returns the page number
 */
int etk_ps_page_get(Etk_Ps *ps)
{
   if (!ps)
      return 0;

   return eps_page_page_get(ps->ps_page);
}

/**
 * @brief Gets the native size of the ps
 * @param ps an ps
 * @param width the location where to set the native width of the ps
 * @param height the location where to set the native height of the ps
 */
void etk_ps_ps_size_get(Etk_Ps *ps, int *width, int *height)
{
   if (!ps)
   {
      if (width) *width = 0;
      if (height) *height = 0;
      return;
   }

   eps_page_size_get(ps->ps_page, width, height);

   evas_object_image_size_get(ps->ps_object, width, height);
}

/**
 * @param ps: the ps to change the orientation
 * @param o: the orientation
 * @return Returns no value.
 * @brief Set an orientation of the document
 *
 * Sets an orientation @p o of the document
 */
void etk_ps_orientation_set(Etk_Ps *ps, Eps_Page_Orientation o)
{
   if (!ps || !ps->ps_page)
      return;

   eps_page_orientation_set(ps->ps_page, o);
   _etk_ps_load(ps);
}

/**
 * @param ps: the ps widget to get the orientation of
 * @return Returns the orientation of the document.
 * @brief get the orientation of the document @p ps. If @p ps
 * is NULL, return EPS_PAGE_ORIENTATION_PORTRAIT
 */
Eps_Page_Orientation etk_ps_orientation_get(Etk_Ps *ps)
{
   if (!ps || !ps->ps_page)
      return EPS_PAGE_ORIENTATION_PORTRAIT;

   return eps_page_orientation_get(ps->ps_page);
}

/**
 * @param ps: the ps to change the scale
 * @param hscale: the horizontal scale
 * @param vscale: the vertical scale
 * @return Returns no value.
 * @brief Set the scale of the document
 *
 * Sets the horizontal scale @p hscale ans the vertical scale @p vscale
 * of the document @p ps
 */
void etk_ps_scale_set(Etk_Ps *ps, double hscale, double vscale)
{
   if (!ps)
      return;

   eps_page_scale_set(ps->ps_page, hscale, vscale);
   _etk_ps_load(ps);
}

/**
 * @param ps: the ps widget to get the orientation of
 * @param hscale: horizontal scale of the current page
 * @param vscale: vertical scale of the current page
 * @return Returns  no value.
 * @brief get the horizontal scale @p hscale ans the vertical scale
 * @p vscale of the document @p ps. If @p ps is NULL, their values is 1.0
 */
void etk_ps_scale_get(Etk_Ps *ps, double *hscale, double *vscale)
{
  if (!ps) {
     if (hscale) *hscale = 1.0;
     if (vscale) *vscale = 1.0;
     return;
  }

  eps_page_scale_get(ps->ps_page, hscale, vscale);
}

/**
 * @param ps: the ps widget
 * @return Returns  no value.
 * @brief go to the next page and render it
 */
void
etk_ps_page_next(Etk_Ps *ps)
{
  int page;

  if (!ps)
    return;

  page = eps_page_page_get(ps->ps_page);
  if (page < (eps_document_page_count_get(ps->ps_document) - 1))
    page++;
  etk_ps_page_set(ps, page);
}

/**
 * @param ps: the ps widget
 * @return Returns  no value.
 * @brief go to the previous page and render it
 */
void
etk_ps_page_previous(Etk_Ps *ps)
{
  int page;

  if (!ps)
    return;

  page = eps_page_page_get(ps->ps_page);
  if (page > 0)
    page--;
  etk_ps_page_set(ps, page);
}

/**
 * @brief Gets the poppler document of the ps
 * @param ps an ps
 * @return Returns the poppler document of the ps (NULL on failure)
 */
const Eps_Document *etk_ps_ps_document_get(Etk_Ps *ps)
{
   if (!ps)
      return NULL;

   return ps->ps_document;
}

/**
 * @brief Gets the current poppler page of the ps
 * @param ps an ps
 * @return Returns the current poppler page of the ps (NULL on failure)
 */
const Eps_Page *etk_ps_ps_page_get(Etk_Ps *ps)
{
   if (!ps)
      return NULL;

   return ps->ps_page;
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the members */
static void _etk_ps_constructor(Etk_Ps *ps)
{
   Etk_Widget *widget;

   if (!(widget = ETK_WIDGET(ps)))
      return;

   ps->ps_object = NULL;
   ps->filename = NULL;

   ps->ps_document = NULL;
   ps->ps_page = NULL;

   widget->size_request = _etk_ps_size_request;
   widget->size_allocate = _etk_ps_size_allocate;

   etk_signal_connect("realized", ETK_OBJECT(ps), ETK_CALLBACK(_etk_ps_realize_cb), NULL);
   etk_signal_connect("unrealized", ETK_OBJECT(ps), ETK_CALLBACK(_etk_ps_unrealize_cb), NULL);
}

/* Destroys the ps */
static void _etk_ps_destructor(Etk_Ps *ps)
{
   if (!ps)
      return;

   if (ps->filename) free(ps->filename);
   eps_document_delete (ps->ps_document);
   eps_page_delete (ps->ps_page);
}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_ps_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Ps *ps;

   if (!(ps = ETK_PS(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_PS_FILE_PROPERTY:
	 etk_ps_file_set(ps, etk_property_value_string_get(value));
	 break;
      default:
	 break;
   }
}

/* Gets the value of the property whose id is "property_id" */
static void _etk_ps_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Ps *ps;

   if (!(ps = ETK_PS(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_PS_FILE_PROPERTY:
	 etk_property_value_string_set(value, ps->filename);
	 break;
      default:
	 break;
   }
}

/* Calculates the ideal size of the ps */
static void _etk_ps_size_request(Etk_Widget *widget, Etk_Size *size_requisition)
{
   Etk_Ps *ps;

   if (!(ps = ETK_PS(widget)) || !size_requisition)
      return;

   if (ps->ps_object)
      evas_object_image_size_get(ps->ps_object, &size_requisition->w, &size_requisition->h);
   else
   {
      size_requisition->w = 0;
      size_requisition->h = 0;
   }
}

/* Resizes the ps to the allocated size */
static void _etk_ps_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Ps *ps;
   double aspect_ratio;
   int ps_w, ps_h;
   int new_size;

   if (!(ps = ETK_PS(widget)))
      return;

   if (!ps->ps_object)
      return;

      evas_object_image_size_get(ps->ps_object, &ps_w, &ps_h);
      aspect_ratio = (double)ps_w / ps_h;

      if (geometry.h * aspect_ratio > geometry.w)
      {
	 new_size = geometry.w / aspect_ratio;
	 geometry.y += (geometry.h - new_size) / 2;
	 geometry.h = new_size;
      }
      else
      {
	new_size = geometry.h * aspect_ratio;
	geometry.x += (geometry.w - new_size) / 2;
	geometry.w = new_size;
      }

   evas_object_image_fill_set(ps->ps_object, 0, 0, geometry.w, geometry.h);

   evas_object_move(ps->ps_object, geometry.x, geometry.y);
   evas_object_resize(ps->ps_object, geometry.w, geometry.h);
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when the ps is realized */
static void _etk_ps_realize_cb(Etk_Object *object, void *data)
{
   Etk_Ps *ps;
   Evas *evas;

   if (!(ps = ETK_PS(object)) || !(evas = etk_widget_toplevel_evas_get(ETK_WIDGET(ps))))
      return;
   _etk_ps_load(ps);
}

/* Called when the ps is unrealized */
static void _etk_ps_unrealize_cb(Etk_Object *object, void *data)
{
   Etk_Ps *ps;

   if (!(ps = ETK_PS(object)))
      return;
   ps->ps_object = NULL;
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Load the ps from the ps or the edje file */
static void _etk_ps_load(Etk_Ps *ps)
{
   Etk_Widget *widget;

   if (!(widget = ETK_WIDGET(ps)))
      return;

   if (ps->ps_object)
   {
      etk_widget_member_object_del(widget, ps->ps_object);
      evas_object_del(ps->ps_object);
      ps->ps_object = NULL;
   }
   if (ps->ps_document)
   {
      Evas *evas;

      if (!ps->ps_object && (evas = etk_widget_toplevel_evas_get(widget)))
      {
	 ps->ps_object = evas_object_image_add(evas);
	 etk_widget_member_object_add(widget, ps->ps_object);
      }
      if (ps->ps_page)
	 eps_page_delete (ps->ps_page);
      if (ps->ps_object)
      {
	 ps->ps_page = eps_page_new (ps->ps_document);
	 eps_page_render (ps->ps_page, ps->ps_object);
      }
      evas_object_show(ps->ps_object);
   }

   etk_widget_size_recalc_queue(widget);
}

/** @} */
