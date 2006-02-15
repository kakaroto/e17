#include "etk_pdf.h"
#include <Evas.h>
#include <Edje.h>
#include <stdlib.h>
#include <string.h>
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"
#include "config.h"

/**
 * @addtogroup Etk_Pdf
 * @{
 */

enum _Etk_Pdf_Property_Id
{
   ETK_PDF_FILE_PROPERTY
};

static void _etk_pdf_constructor(Etk_Pdf *pdf);
static void _etk_pdf_destructor(Etk_Pdf *pdf);
static void _etk_pdf_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_pdf_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_pdf_realize_cb(Etk_Object *object, void *data);
static void _etk_pdf_unrealize_cb(Etk_Object *object, void *data);
static void _etk_pdf_size_request(Etk_Widget *widget, Etk_Size *size_requisition);
static void _etk_pdf_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_pdf_load(Etk_Pdf *pdf);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Pdf
 * @return Returns the type on an Etk_Pdf
 */
Etk_Type *etk_pdf_type_get()
{
   static Etk_Type *pdf_type = NULL;

   if (!pdf_type)
   {
      pdf_type = etk_type_new("Etk_Pdf", ETK_WIDGET_TYPE, sizeof(Etk_Pdf), ETK_CONSTRUCTOR(_etk_pdf_constructor), ETK_DESTRUCTOR(_etk_pdf_destructor));

      etk_type_property_add(pdf_type, "pdf_file", ETK_PDF_FILE_PROPERTY, ETK_PROPERTY_STRING, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_string(NULL));

      pdf_type->property_set = _etk_pdf_property_set;
      pdf_type->property_get = _etk_pdf_property_get;
   }

   return pdf_type;
}

/**
 * @brief Creates a new pdf
 * @return Returns the new pdf widget
 */
Etk_Widget *etk_pdf_new()
{
   return etk_widget_new(ETK_PDF_TYPE, NULL);
}

/**
 * @brief Loads the pdf from a file
 * @param pdf an pdf
 * @param filename the name of the file to load
 */
void etk_pdf_file_set(Etk_Pdf *pdf, const char *filename)
{
   Etk_Widget *widget;

   if (!(widget = ETK_WIDGET(pdf)) || (pdf->filename == filename))
      return;

   free(pdf->filename);
   pdf->filename = filename ? strdup(filename) : NULL;
   etk_object_notify(ETK_OBJECT(pdf), "pdf_file");

   if (pdf->pdf_document)
      evas_poppler_document_delete (pdf->pdf_document);

   if (pdf->pdf_index)
      evas_poppler_index_delete (pdf->pdf_index);

   pdf->pdf_document = evas_poppler_document_new (pdf->filename);
   pdf->pdf_index = evas_poppler_index_new (pdf->pdf_document);

   _etk_pdf_load(pdf);
}

/**
 * @brief Gets the name of the file used for the pdf
 * @param pdf an pdf
 * @return Returns the name of the file use for the pdf (NULL on failure)
 */
const char *etk_pdf_file_get(Etk_Pdf *pdf)
{
   if (!pdf)
      return NULL;
   return pdf->filename;
}

/**
 * @brief Set a page number
 * @param pdf: the pdf to change page
 * @param page: the page number
 */
void etk_pdf_page_set(Etk_Pdf *pdf, int page)
{
   if (!pdf ||
       !pdf->pdf_document ||
       (page >= evas_poppler_document_page_count_get (pdf->pdf_document)) ||
       (page == pdf->page))
      return;

   pdf->page = page;
   _etk_pdf_load (pdf);
}

/**
 * @brief Get the page number
 * @param pdf: the pdf to change page
 * @param page: the page number
 * @return Returns the page number
 */
int etk_pdf_page_get(Etk_Pdf *pdf)
{
   if (!pdf)
      return 0;

   return pdf->page;
}

/**
 * @brief Gets the poppler document of the pdf
 * @param pdf an pdf
 * @return Returns the poppler document of the pdf (NULL on failure)
 */
Evas_Poppler_Document *etk_pdf_pdf_document_get (Etk_Pdf *pdf)
{
   if (!pdf)
      return NULL;

   return pdf->pdf_document;
}

/**
 * @brief Gets the current poppler page of the pdf
 * @param pdf an pdf
 * @return Returns the current poppler page of the pdf (NULL on failure)
 */
Evas_Poppler_Page *etk_pdf_pdf_page_get (Etk_Pdf *pdf)
{
   if (!pdf)
      return NULL;

   return pdf->pdf_page;
}

/**
 * @brief Gets the index of the pdf document
 * @param pdf an pdf
 * @return Returns the index of the pdf document (NULL on failure)
 */
Ecore_List *etk_pdf_pdf_index_get (Etk_Pdf *pdf)
{
   if (!pdf)
      return NULL;

   return pdf->pdf_index;
}

/**
 * @brief Gets the native size of the pdf
 * @param pdf an pdf
 * @param width the location where to set the native width of the pdf
 * @param height the location where to set the native height of the pdf
 */
void etk_pdf_size_get(Etk_Pdf *pdf, int *width, int *height)
{
   if (!pdf)
      return;

   if (!pdf)
   {
      if (width)
	 *width = 0;
      if (height)
	 *height = 0;
   }
   else
      evas_object_image_size_get(pdf->pdf_object, width, height);
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the members */
static void _etk_pdf_constructor(Etk_Pdf *pdf)
{
   Etk_Widget *widget;

   if (!(widget = ETK_WIDGET(pdf)))
      return;

   pdf->pdf_object = NULL;
   pdf->filename = NULL;
   pdf->page = 0;

   pdf->pdf_document = NULL;
   pdf->pdf_page = NULL;
   pdf->pdf_index = NULL;

   widget->size_request = _etk_pdf_size_request;
   widget->size_allocate = _etk_pdf_size_allocate;

   etk_signal_connect("realize", ETK_OBJECT(pdf), ETK_CALLBACK(_etk_pdf_realize_cb), NULL);
   etk_signal_connect("unrealize", ETK_OBJECT(pdf), ETK_CALLBACK(_etk_pdf_unrealize_cb), NULL);
}

/* Destroys the pdf */
static void _etk_pdf_destructor(Etk_Pdf *pdf)
{
   if (!pdf)
      return;

   free(pdf->filename);
   evas_poppler_document_delete (pdf->pdf_document);
   evas_poppler_page_delete (pdf->pdf_page);
   evas_poppler_index_delete (pdf->pdf_index);
}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_pdf_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Pdf *pdf;

   if (!(pdf = ETK_PDF(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_PDF_FILE_PROPERTY:
	 etk_pdf_file_set(pdf, etk_property_value_string_get(value));
	 break;
      default:
	 break;
   }
}

/* Gets the value of the property whose id is "property_id" */
static void _etk_pdf_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Pdf *pdf;

   if (!(pdf = ETK_PDF(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_PDF_FILE_PROPERTY:
	 etk_property_value_string_set(value, pdf->filename);
	 break;
      default:
	 break;
   }
}

/* Calculates the ideal size of the pdf */
static void _etk_pdf_size_request(Etk_Widget *widget, Etk_Size *size_requisition)
{
   Etk_Pdf *pdf;

   if (!(pdf = ETK_PDF(widget)) || !size_requisition)
      return;

   if (pdf->pdf_object)
      evas_object_image_size_get(pdf->pdf_object, &size_requisition->w, &size_requisition->h);
   else
   {
      size_requisition->w = 0;
      size_requisition->h = 0;
   }
}

/* Resizes the pdf to the allocated size */
static void _etk_pdf_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Pdf *pdf;
   double aspect_ratio;
   int pdf_w, pdf_h;
   int new_size;

   if (!(pdf = ETK_PDF(widget)))
      return;

   if (!pdf->pdf_object)
      return;

      evas_object_image_size_get(pdf->pdf_object, &pdf_w, &pdf_h);
      aspect_ratio = (double)pdf_w / pdf_h;

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

   evas_object_image_fill_set(pdf->pdf_object, 0, 0, geometry.w, geometry.h);

   evas_object_move(pdf->pdf_object, geometry.x, geometry.y);
   evas_object_resize(pdf->pdf_object, geometry.w, geometry.h);
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when the pdf is realized */
static void _etk_pdf_realize_cb(Etk_Object *object, void *data)
{
   Etk_Pdf *pdf;
   Evas *evas;

   if (!(pdf = ETK_PDF(object)) || !(evas = etk_widget_toplevel_evas_get(ETK_WIDGET(pdf))))
      return;
   _etk_pdf_load(pdf);
}

/* Called when the pdf is unrealized */
static void _etk_pdf_unrealize_cb(Etk_Object *object, void *data)
{
   Etk_Pdf *pdf;

   if (!(pdf = ETK_PDF(object)))
      return;
   pdf->pdf_object = NULL;
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Load the pdf from the pdf or the edje file */
static void _etk_pdf_load(Etk_Pdf *pdf)
{
   Etk_Widget *widget;

   if (!(widget = ETK_WIDGET(pdf)))
      return;

   if (pdf->pdf_object)
   {
      etk_widget_member_object_del(widget, pdf->pdf_object);
      evas_object_del(pdf->pdf_object);
      pdf->pdf_object = NULL;
   }
   if (pdf->pdf_document)
   {
      Evas *evas;

      if (!pdf->pdf_object && (evas = etk_widget_toplevel_evas_get(widget)))
      {
	 pdf->pdf_object = evas_object_image_add(evas);
	 etk_widget_member_object_add(widget, pdf->pdf_object);
      }
      if (pdf->pdf_page)
	 evas_poppler_page_delete (pdf->pdf_page);
      if (pdf->pdf_object)
      {
	 pdf->pdf_page = evas_poppler_document_page_get (pdf->pdf_document, pdf->page);
	 evas_poppler_page_render (pdf->pdf_page, pdf->pdf_object, 0, 0, 0, 0, 72.0, 72.0);
      }
      evas_object_show(pdf->pdf_object);
   }

   etk_widget_size_recalc_queue(widget);
}

/** @} */
