#include "etk_pdf.h"
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
   pdf->page = 0;

   pdf->search.o = NULL;
   pdf->search.text = NULL;
   pdf->search.list = NULL;
   pdf->search.page = -1;
   pdf->search.is_case_sensitive = ETK_FALSE;
   pdf->search.is_circular = ETK_FALSE;

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
   {
      if (width)
	 *width = 0;
      if (height)
	 *height = 0;
   }
   else {
      if (width)
	 *width = evas_poppler_page_width_get (pdf->pdf_page);
      if (height)
	 *height = evas_poppler_page_height_get (pdf->pdf_page);
   }
      evas_object_image_size_get(pdf->pdf_object, width, height);
}

void
etk_pdf_search_text_set (Etk_Pdf *pdf, const char *text)
{
   if (!pdf)
      return;

   if ((!text) ||
       (pdf->search.text &&
        strcmp (text, pdf->search.text) == 0))
     return;

   if (pdf->search.text) free (pdf->search.text);
   pdf->search.text = strdup (text);
   pdf->search.page = -1;
}

void
etk_pdf_search_first_page_set (Etk_Pdf *pdf, int page)
{
   if (!pdf)
      return;

   if (page != pdf->search.page)
     pdf->search.page = page;
}

void
etk_pdf_search_is_case_sensitive (Etk_Pdf *pdf, int is_case_sensitive)
{
   if (!pdf)
      return;

   if (is_case_sensitive != pdf->search.is_case_sensitive)
     pdf->search.is_case_sensitive = is_case_sensitive;
}

int
etk_pdf_search_next (Etk_Pdf *pdf)
{
   if (!pdf)
      return ETK_FALSE;

   if (!pdf->search.text)
      return ETK_FALSE;

   if (!pdf->search.o) {
      Evas *evas;

      evas = etk_widget_toplevel_evas_get(ETK_WIDGET(pdf));
      if (!evas)
         return ETK_FALSE;
      pdf->search.o = evas_object_rectangle_add (evas);
      if (!pdf->search.o)
         return ETK_FALSE;
      evas_object_color_set(pdf->search.o, 0, 128, 0, 128);
      evas_object_hide (pdf->search.o);
   }

 next_page:
   /* no list, we search one */
   while (!pdf->search.list &&
          pdf->search.page < evas_poppler_document_page_count_get (pdf->pdf_document)) {
     Evas_Poppler_Page *page;

     pdf->search.page++;
     printf ("page : %d\n", pdf->search.page);
     page = evas_poppler_document_page_get (pdf->pdf_document, pdf->search.page);
     pdf->search.list = evas_poppler_page_text_find (page,
                                                     pdf->search.text,
                                                     pdf->search.is_case_sensitive);
     if (pdf->search.list)
        ecore_list_goto_first (pdf->search.list);
     evas_poppler_page_delete (page);
   }

   /* an already existing list or a netky one */
   if (pdf->search.list) {
     Rectangle *rect;
     int x, y, w, h;

     if ((rect = (Rectangle *)ecore_list_next (pdf->search.list))) {
       if (pdf->search.page != pdf->page) {
         etk_pdf_page_set (pdf, pdf->search.page);
         _etk_pdf_load (pdf);
       }
       etk_widget_geometry_get (ETK_WIDGET (pdf), &x, &y, &w, &h);
       evas_object_move (pdf->search.o,
                         x + round (rect->x1 - 1),
                         y + round (rect->y1 - 1));
       evas_object_resize (pdf->search.o,
                           round (rect->x2 - rect->x1 + 1),
                           round (rect->y2 - rect->y1));
       if (!evas_object_visible_get (pdf->search.o))
         evas_object_show (pdf->search.o);
       /* we leave... */
       return ETK_TRUE;
     }
     else { /* no more word to find. We destroy the list */
       ecore_list_destroy (pdf->search.list);
       pdf->search.list = NULL;
       /* we search a new one */
       printf ("page0 : %d\n", pdf->search.page);
       goto next_page;
     }
   }
   evas_object_hide (pdf->search.o);

   if (pdf->search.is_circular) {
      pdf->search.page = -1;
      return ETK_TRUE;
   }
   else
      return ETK_TRUE;
}

void etk_pdf_orientation_set (Etk_Pdf *pdf, Evas_Poppler_Page_Orientation o)
{
   if (!pdf || !pdf->pdf_page || (pdf->orientation == o))
      return;

   printf ("orientation %d\n", o);
   pdf->orientation = o;
   _etk_pdf_load (pdf);
}

Evas_Poppler_Page_Orientation etk_pdf_orientation_get (Etk_Pdf *pdf)
{
   if (!pdf || !pdf->pdf_page)
      return EVAS_POPPLER_PAGE_ORIENTATION_PORTRAIT;

   return evas_poppler_page_orientation_get (pdf->pdf_page);
}

void etk_pdf_scale_set (Etk_Pdf *pdf, double hscale, double vscale)
{
   if (!pdf)
      return;

   if (hscale != pdf->hscale)
     pdf->hscale = hscale;

   if (vscale != pdf->vscale)
     pdf->vscale = vscale;
   _etk_pdf_load (pdf);
}

void etk_pdf_scale_get (Etk_Pdf *pdf, double *hscale, double *vscale)
{
  if (!pdf) {
     if (hscale)
        *hscale = 1.0;

     if (vscale)
        *vscale = 1.0;
  }
  else {
     if (hscale)
        *hscale = pdf->hscale;

      if (vscale)
         *vscale = pdf->vscale;
  }
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

   pdf->orientation = EVAS_POPPLER_PAGE_ORIENTATION_PORTRAIT;
   pdf->hscale = 1.0;
   pdf->vscale = 1.0;

   pdf->search.o = NULL;
   pdf->search.text = NULL;
   pdf->search.list = NULL;
   pdf->search.page = -1;
   pdf->search.is_case_sensitive = ETK_FALSE;
   pdf->search.is_circular = ETK_FALSE;

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
	 evas_poppler_page_render (pdf->pdf_page, pdf->pdf_object,
                                   pdf->orientation,
                                   0, 0, -1, -1,
                                   pdf->hscale, pdf->vscale);
      }
      evas_object_show(pdf->pdf_object);
   }

   etk_widget_size_recalc_queue(widget);
}

/** @} */
