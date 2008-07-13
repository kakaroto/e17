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

#if HAVE___ATTRIBUTE__
#define __UNUSED__ __attribute__((unused))
#else
#define __UNUSED__
#endif


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
static Etk_Bool _etk_dvi_realize_cb(Etk_Object *object, void *data);
static Etk_Bool _etk_dvi_unrealize_cb(Etk_Object *object, void *data);
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
      dvi_type = etk_type_new("Etk_Dvi", ETK_WIDGET_TYPE, sizeof(Etk_Dvi), ETK_CONSTRUCTOR(_etk_dvi_constructor), ETK_DESTRUCTOR(_etk_dvi_destructor), NULL);

      etk_type_property_add(dvi_type, "dvi_file", ETK_DVI_FILE_PROPERTY, ETK_PROPERTY_STRING, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_string(NULL));

      dvi_type->property_set = _etk_dvi_property_set;
      dvi_type->property_get = _etk_dvi_property_get;
   }

   return dvi_type;
}

/**
 * @brief Creates a new dvi with the property EDVI_PROPERTY_DELAYED_FONT_OPEN set by default.
 * @return Returns the new dvi widget
 */
Etk_Widget *etk_dvi_new()
{
   return etk_widget_new(ETK_DVI_TYPE, NULL);
}

/**
 * @brief Loads the dvi from a file
 * @param dvi a dvi
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

   if (dvi->dvi_page)
      edvi_page_delete(dvi->dvi_page);

   dvi->dvi_document = edvi_document_new (dvi->filename, dvi->dvi_device, dvi->dvi_property);
   dvi->dvi_page = edvi_page_new(dvi->dvi_document);

   _etk_dvi_load(dvi);
}

/**
 * @brief Gets the name of the file used for the dvi
 * @param dvi a dvi
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
 * @param dvi: the dvi to change the page
 * @param page: the page number
 */
void etk_dvi_page_set(Etk_Dvi *dvi, int page)
{
   if (!dvi ||
       !dvi->dvi_document ||
       (page >= edvi_document_page_count_get (dvi->dvi_document)) ||
       (page == edvi_page_page_get(dvi->dvi_page)))
      return;

   edvi_page_page_set(dvi->dvi_page, page);
   _etk_dvi_load (dvi);
}

/**
 * @brief Get the page number
 * @param dvi: the dvi to get the page
 * @return Returns the page number
 */
int etk_dvi_page_get(Etk_Dvi *dvi)
{
   if (!dvi)
      return 0;

   return edvi_page_page_get(dvi->dvi_page);
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
      if (width) *width = 0;
      if (height) *height = 0;
   }

   edvi_page_size_get (dvi->dvi_page, width, height);

   evas_object_image_size_get(dvi->dvi_object, width, height);
}

/**
 * @param dvi: the dvi to change the orientation
 * @param o: the orientation
 * @return Returns no value.
 * @brief Set an orientation of the document
 *
 * Sets an orientation @p o of the document
 */
void etk_dvi_orientation_set (Etk_Dvi *dvi, Edvi_Page_Orientation o)
{
   if (!dvi || !dvi->dvi_page)
      return;

   edvi_page_orientation_set(dvi->dvi_page, o);
   _etk_dvi_load (dvi);
}

/**
 * @param dvi: the dvi widget to get the orientation of
 * @return Returns the orientation of the document.
 * @brief get the orientation of the document @p dvi. If @p dvi
 * is NULL, return EDVI_PAGE_ORIENTATION_PORTRAIT
 */
Edvi_Page_Orientation etk_dvi_orientation_get (Etk_Dvi *dvi)
{
   if (!dvi || !dvi->dvi_page)
      return EDVI_PAGE_ORIENTATION_PORTRAIT;

   return edvi_page_orientation_get (dvi->dvi_page);
}

/**
 * @param dvi: the dvi to change the magnification
 * @param mag: the magnification
 * @return Returns no value.
 * @brief Set the magnification of the document
 *
 * Sets the magnification @p mag of the document @p dvi
 */
void etk_dvi_mag_set (Etk_Dvi *dvi, double mag)
{
   if (!dvi)
      return;

   edvi_page_mag_set(dvi->dvi_page, mag);
   _etk_dvi_load (dvi);
}

/**
 * @param dvi: the dvi widget to get the orientation of
 * @param hscale: horizontal scale of the current page
 * @param vscale: vertical scale of the current page
 * @return Returns  no value.
 * @brief get the horizontal scale @p hscale and the vertical scale
 * @p vscale of the document @p dvi. If @p dvi is NULL, their values are 1.0
 */
double etk_dvi_mag_get (Etk_Dvi *dvi)
{
   if (!dvi)
      return 1.0;

   return edvi_page_mag_get(dvi->dvi_page);
}

/**
 * @param dvi: the dvi widget
 * @return Returns  no value.
 * @brief go to the next page and render it
 */
void
etk_dvi_page_next (Etk_Dvi *dvi)
{
   int page;

   if (!dvi)
      return;

   page = edvi_page_page_get(dvi->dvi_page);
   if (page < (edvi_document_page_count_get(dvi->dvi_document) - 1))
      page++;
   etk_dvi_page_set (dvi, page);
}

/**
 * @param dvi: the dvi widget
 * @return Returns  no value.
 * @brief go to the previous page and render it
 */
void
etk_dvi_page_previous (Etk_Dvi *dvi)
{
   int page;

   if (!dvi)
      return;

   page = edvi_page_page_get(dvi->dvi_page);
   if (page > 0)
      page--;
   etk_dvi_page_set(dvi, page);
}

/**
 * @brief Gets the document of the dvi
 * @param dvi an dvi
 * @return Returns the document of the dvi (NULL on failure)
 */
const Edvi_Document *etk_dvi_dvi_document_get (Etk_Dvi *dvi)
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
const Edvi_Page *etk_dvi_dvi_page_get (Etk_Dvi *dvi)
{
   if (!dvi)
      return NULL;

   return dvi->dvi_page;
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

   dvi->dvi_device = edvi_device_new (edvi_dpi_get(), edvi_dpi_get());
   dvi->dvi_property = edvi_property_new();
   edvi_property_property_set (dvi->dvi_property, EDVI_PROPERTY_DELAYED_FONT_OPEN);
   dvi->dvi_document = NULL;
   dvi->dvi_page = NULL;

   widget->size_request = _etk_dvi_size_request;
   widget->size_allocate = _etk_dvi_size_allocate;

   Etk_Signal_Connect_Desc desc[] = {
      ETK_SC_DESC(ETK_WIDGET_REALIZED_SIGNAL, _etk_dvi_realize_cb),
      ETK_SC_DESC(ETK_WIDGET_UNREALIZED_SIGNAL, _etk_dvi_unrealize_cb),
      ETK_SC_DESC_SENTINEL
   };
   etk_signal_connect_multiple(desc, ETK_OBJECT(dvi), NULL);
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
static Etk_Bool _etk_dvi_realize_cb(Etk_Object *object, void *data __UNUSED__)
{
   Etk_Dvi *dvi;
   Evas *evas;
/*    Evas_Object *o; */

   if (!(dvi = ETK_DVI(object)) || !(evas = etk_widget_toplevel_evas_get(ETK_WIDGET(dvi))))
      return ETK_FALSE;

/*    o = evas_object_rectangle_add(evas); */
/*    evas_object_color_set(o, 255, 255, 0, 128); */
/*    etk_widget_member_object_add(ETK_WIDGET(object), o); */
/*    evas_object_show(o); */
   _etk_dvi_load(dvi);
   return ETK_TRUE;
}

/* Called when the dvi is unrealized */
static Etk_Bool _etk_dvi_unrealize_cb(Etk_Object *object, void *data __UNUSED__)
{
   Etk_Dvi *dvi;

   if (!(dvi = ETK_DVI(object)))
      return ETK_FALSE;

   dvi->dvi_object = NULL;
   return ETK_TRUE;
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
      if (dvi->dvi_object)
      {
         unsigned int *m;
         int           w;
         int           h;

	 dvi->dvi_page = edvi_page_new (dvi->dvi_document);

         edvi_page_size_get (dvi->dvi_page, &w, &h);
         evas_object_image_size_set (dvi->dvi_object, w, h);
         evas_object_image_fill_set (dvi->dvi_object, 0, 0, w, h);
         m = (unsigned int *)evas_object_image_data_get (dvi->dvi_object, 1);
         if (!m)
            return;

         memset(m, (255 << 24) | (255 << 16) | (255 << 8) | 255, w * h * 4);
         evas_object_image_data_update_add (dvi->dvi_object, 0, 0, w, h);
         evas_object_resize (dvi->dvi_object, w, h);

	 edvi_page_render (dvi->dvi_page, dvi->dvi_device, dvi->dvi_object);
      }
      evas_object_show(dvi->dvi_object);
   }

   etk_widget_size_recalc_queue(widget);
}

/**
 * @}
 */
