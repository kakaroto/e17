#include "etk_dvi.h"
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
 * @addtogroup Etk_Dvi
 * @{
 */

enum _Etk_Dvi_Property_Id
{
   ETK_DVI_FILE_PROPERTY
};

static void _etk_dvi_constructor(Etk_Dvi *dvi);
static void _etk_dvi_destructor(Etk_Dvi *dvi);
static void _etk_dvi_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_dvi_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_dvi_realize_cb(Etk_Object *object, void *data);
static void _etk_dvi_unrealize_cb(Etk_Object *object, void *data);
static void _etk_dvi_size_request(Etk_Widget *widget, Etk_Size *size_requisition);
static void _etk_dvi_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_dvi_load(Etk_Dvi *dvi);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Dvi
 * @return Returns the type on an Etk_Dvi
 */
Etk_Type *etk_dvi_type_get()
{
   static Etk_Type *dvi_type = NULL;

   if (!dvi_type)
   {
      dvi_type = etk_type_new("Etk_Dvi", ETK_WIDGET_TYPE, sizeof(Etk_Dvi), ETK_CONSTRUCTOR(_etk_dvi_constructor), ETK_DESTRUCTOR(_etk_dvi_destructor));

      etk_type_property_add(dvi_type, "dvi_file", ETK_DVI_FILE_PROPERTY, ETK_PROPERTY_STRING, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_string(NULL));

      dvi_type->property_set = _etk_dvi_property_set;
      dvi_type->property_get = _etk_dvi_property_get;
   }

   return dvi_type;
}

/**
 * @brief Creates a new dvi
 * @return Returns the new dvi widget
 */
Etk_Widget *etk_dvi_new()
{
   return etk_widget_new(ETK_DVI_TYPE, NULL);
}

/**
 * @brief Loads the dvi from a file
 * @param dvi an dvi
 * @param filename the name of the file to load
 */
void etk_dvi_file_set(Etk_Dvi *dvi, const char *filename)
{
   Etk_Widget *widget;

   if (!(widget = ETK_WIDGET(dvi)) || (dvi->filename == filename))
      return;

   free(dvi->filename);
   dvi->filename = filename ? strdup(filename) : NULL;
   etk_object_notify(ETK_OBJECT(dvi), "dvi_file");

   if (dvi->dvi_document)
      edvi_document_delete (dvi->dvi_document);

   dvi->dvi_document = edvi_document_new (dvi->filename, dvi->dvi_device, dvi->dvi_property);
   dvi->page = 0;

   _etk_dvi_load(dvi);
}

/**
 * @brief Gets the name of the file used for the dvi
 * @param dvi an dvi
 * @return Returns the name of the file use for the dvi (NULL on failure)
 */
const char *etk_dvi_file_get(Etk_Dvi *dvi)
{
   if (!dvi)
      return NULL;
   return dvi->filename;
}

/**
 * @brief Set a page number
 * @param dvi: the dvi to change page
 * @param page: the page number
 */
void etk_dvi_page_set(Etk_Dvi *dvi, int page)
{
   if (!dvi ||
       !dvi->dvi_document ||
       (page >= edvi_document_page_count_get (dvi->dvi_document)) ||
       (page == dvi->page))
      return;

   dvi->page = page;
   _etk_dvi_load (dvi);
}

/**
 * @brief Get the page number
 * @param dvi: the dvi to change page
 * @param page: the page number
 * @return Returns the page number
 */
int etk_dvi_page_get(Etk_Dvi *dvi)
{
   if (!dvi)
      return 0;

   return dvi->page;
}

/**
 * @brief Gets the document of the dvi
 * @param dvi an dvi
 * @return Returns the document of the dvi (NULL on failure)
 */
Edvi_Document *etk_dvi_dvi_document_get (Etk_Dvi *dvi)
{
   if (!dvi)
      return NULL;

   return dvi->dvi_document;
}

/**
 * @brief Gets the current page of the dvi document
 * @param dvi an dvi
 * @return Returns the current page of the dvi document (NULL on failure)
 */
Edvi_Page *etk_dvi_dvi_page_get (Etk_Dvi *dvi)
{
   if (!dvi)
      return NULL;

   return dvi->dvi_page;
}

/**
 * @brief Gets the native size of the dvi
 * @param dvi an dvi
 * @param width the location where to set the native width of the dvi
 * @param height the location where to set the native height of the dvi
 */
void etk_dvi_size_get(Etk_Dvi *dvi, int *width, int *height)
{
   if (!dvi)
   {
      if (width)
	 *width = 0;
      if (height)
	 *height = 0;
   }
   else {
      if (width)
	 *width = edvi_page_width_get (dvi->dvi_page);
      if (height)
	 *height = edvi_page_height_get (dvi->dvi_page);
   }
      evas_object_image_size_get(dvi->dvi_object, width, height);
}

void etk_dvi_orientation_set (Etk_Dvi *dvi, Edvi_Page_Orientation o)
{
   if (!dvi || !dvi->dvi_page || (dvi->orientation == o))
      return;

   dvi->orientation = o;
   _etk_dvi_load (dvi);
}

Edvi_Page_Orientation etk_dvi_orientation_get (Etk_Dvi *dvi)
{
   if (!dvi || !dvi->dvi_page)
      return EDVI_PAGE_ORIENTATION_PORTRAIT;

   return edvi_page_orientation_get (dvi->dvi_page);
}

void etk_dvi_scale_set (Etk_Dvi *dvi, double hscale, double vscale)
{
   if (!dvi)
      return;

   if (hscale != dvi->hscale)
     dvi->hscale = hscale;

   if (vscale != dvi->vscale)
     dvi->vscale = vscale;
   _etk_dvi_load (dvi);
}

void etk_dvi_scale_get (Etk_Dvi *dvi, double *hscale, double *vscale)
{
  if (!dvi) {
     if (hscale)
        *hscale = 1.0;

     if (vscale)
        *vscale = 1.0;
  }
  else {
     if (hscale)
        *hscale = dvi->hscale;

      if (vscale)
         *vscale = dvi->vscale;
  }
}

void
etk_dvi_page_next (Etk_Dvi *dvi)
{
  int page;

  if (!dvi)
    return;

  page = dvi->page + 1;
  if (page >= edvi_document_page_count_get(dvi->dvi_document))
    page = edvi_document_page_count_get(dvi->dvi_document) - 1;
  etk_dvi_page_set (dvi, page);
}

void
etk_dvi_page_previous (Etk_Dvi *dvi)
{
  int page;

  if (!dvi)
    return;

  page = dvi->page - 1;
  if (page < 0)
    page = 0;
  etk_dvi_page_set (dvi, page);
}

void
etk_dvi_page_page_length_set (Etk_Dvi *dvi, int page_length)
{
  if (!dvi || (page_length <= 0) || (dvi->page_length == page_length))
    return;

  dvi->page_length = page_length;
}

int
etk_dvi_page_page_length_get (Etk_Dvi *dvi)
{
  if (!dvi)
    return 0;

  return dvi->page_length;
}

void
etk_dvi_page_page_next (Etk_Dvi *dvi)
{
  int page;

  if (!dvi)
    return;

  page = dvi->page + dvi->page_length;
  if (page >= edvi_document_page_count_get(dvi->dvi_document))
    page = edvi_document_page_count_get(dvi->dvi_document) - 1;
  etk_dvi_page_set (dvi, page);
}

void
etk_dvi_page_page_previous (Etk_Dvi *dvi)
{
  int page;

  if (!dvi)
    return;

  page = dvi->page - dvi->page_length;
  if (page < 0)
    page = 0;
  etk_dvi_page_set (dvi, page);
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the members */
static void _etk_dvi_constructor(Etk_Dvi *dvi)
{
   Etk_Widget *widget;

   if (!(widget = ETK_WIDGET(dvi)))
      return;

   dvi->dvi_object = NULL;
   dvi->filename = NULL;
   dvi->page = 0;
   dvi->page_length = 10;

   dvi->dvi_device = edvi_device_new (edvi_dpi_get(), edvi_dpi_get());
   dvi->dvi_property = edvi_property_new();
   edvi_property_delayed_font_open_set(dvi->dvi_property, 1);
   dvi->dvi_document = NULL;
   dvi->dvi_page = NULL;

   dvi->orientation = EDVI_PAGE_ORIENTATION_PORTRAIT;
   dvi->hscale = 1.0;
   dvi->vscale = 1.0;

   widget->size_request = _etk_dvi_size_request;
   widget->size_allocate = _etk_dvi_size_allocate;

   etk_signal_connect("realize", ETK_OBJECT(dvi), ETK_CALLBACK(_etk_dvi_realize_cb), NULL);
   etk_signal_connect("unrealize", ETK_OBJECT(dvi), ETK_CALLBACK(_etk_dvi_unrealize_cb), NULL);
}

/* Destroys the dvi */
static void _etk_dvi_destructor(Etk_Dvi *dvi)
{
   if (!dvi)
      return;

   free(dvi->filename);
   edvi_page_delete (dvi->dvi_page);
   edvi_document_delete (dvi->dvi_document);
  edvi_property_delete (dvi->dvi_property);
  edvi_device_delete (dvi->dvi_device);
}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_dvi_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Dvi *dvi;

   if (!(dvi = ETK_DVI(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_DVI_FILE_PROPERTY:
	 etk_dvi_file_set(dvi, etk_property_value_string_get(value));
	 break;
      default:
	 break;
   }
}

/* Gets the value of the property whose id is "property_id" */
static void _etk_dvi_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Dvi *dvi;

   if (!(dvi = ETK_DVI(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_DVI_FILE_PROPERTY:
	 etk_property_value_string_set(value, dvi->filename);
	 break;
      default:
	 break;
   }
}

/* Calculates the ideal size of the dvi */
static void _etk_dvi_size_request(Etk_Widget *widget, Etk_Size *size_requisition)
{
   Etk_Dvi *dvi;

   if (!(dvi = ETK_DVI(widget)) || !size_requisition)
      return;

   if (dvi->dvi_object)
      evas_object_image_size_get(dvi->dvi_object, &size_requisition->w, &size_requisition->h);
   else
   {
      size_requisition->w = 0;
      size_requisition->h = 0;
   }
}

/* Resizes the dvi to the allocated size */
static void _etk_dvi_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Dvi *dvi;
   double aspect_ratio;
   int dvi_w, dvi_h;
   int new_size;

   if (!(dvi = ETK_DVI(widget)))
      return;

   if (!dvi->dvi_object)
      return;

      evas_object_image_size_get(dvi->dvi_object, &dvi_w, &dvi_h);
      aspect_ratio = (double)dvi_w / dvi_h;

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

   evas_object_image_fill_set(dvi->dvi_object, 0, 0, geometry.w, geometry.h);

   evas_object_move(dvi->dvi_object, geometry.x, geometry.y);
   evas_object_resize(dvi->dvi_object, geometry.w, geometry.h);
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when the dvi is realized */
static void _etk_dvi_realize_cb(Etk_Object *object, void *data)
{
   Etk_Dvi *dvi;
   Evas *evas;

   if (!(dvi = ETK_DVI(object)) || !(evas = etk_widget_toplevel_evas_get(ETK_WIDGET(dvi))))
      return;
   _etk_dvi_load(dvi);
}

/* Called when the dvi is unrealized */
static void _etk_dvi_unrealize_cb(Etk_Object *object, void *data)
{
   Etk_Dvi *dvi;

   if (!(dvi = ETK_DVI(object)))
      return;
   dvi->dvi_object = NULL;
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Load the dvi from the dvi or the edje file */
static void _etk_dvi_load(Etk_Dvi *dvi)
{
   Etk_Widget *widget;

   if (!(widget = ETK_WIDGET(dvi)))
      return;

   if (dvi->dvi_object)
   {
      etk_widget_member_object_del(widget, dvi->dvi_object);
      evas_object_del(dvi->dvi_object);
      dvi->dvi_object = NULL;
   }
   if (dvi->dvi_document)
   {
      Evas *evas;

      if (!dvi->dvi_object && (evas = etk_widget_toplevel_evas_get(widget)))
      {
	 dvi->dvi_object = evas_object_image_add(evas);
	 etk_widget_member_object_add(widget, dvi->dvi_object);
      }
      if (dvi->dvi_page)
	 edvi_page_delete (dvi->dvi_page);
      if (dvi->dvi_object)
      {
	 dvi->dvi_page = edvi_page_new (dvi->dvi_document, dvi->page);

	 edvi_page_render (dvi->dvi_page, dvi->dvi_device, dvi->dvi_object);
      }
      evas_object_show(dvi->dvi_object);
   }

   etk_widget_size_recalc_queue(widget);
}

/** @} */
