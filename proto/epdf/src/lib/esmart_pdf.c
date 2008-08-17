#include <stdlib.h>
#include <string.h>

#include <Evas.h>

#include "esmart_pdf.h"


#define E_SMART_OBJ_GET(smart, o, type) \
  { \
    char *_e_smart_str; \
    \
    if (!o) return; \
    \
    smart = evas_object_smart_data_get(o); \
    if (!smart) return; \
    \
    _e_smart_str = (char *)evas_object_type_get(o); \
    if (!_e_smart_str) return; \
    \
    if (strcmp(_e_smart_str, type)) return; \
  }

#define E_SMART_OBJ_GET_RETURN(smart, o, type, ret) \
  { \
    char *_e_smart_str; \
    \
    if (!o) return ret; \
    \
    smart = evas_object_smart_data_get(o); \
    if (!smart) return ret; \
    \
    _e_smart_str = (char *)evas_object_type_get(o); \
    if (!_e_smart_str) return ret; \
    \
    if (strcmp(_e_smart_str, type)) return ret; \
  }

#define E_OBJ_NAME "poppler_object"


static void _smart_init (void);
static void _smart_add (Evas_Object *obj);
static void _smart_del (Evas_Object *obj);
static void _smart_move (Evas_Object *obj, Evas_Coord x, Evas_Coord y);
static void _smart_resize (Evas_Object *obj, Evas_Coord w, Evas_Coord h);
static void _smart_show (Evas_Object *obj);
static void _smart_hide (Evas_Object *obj);
static void _smart_color_set (Evas_Object *obj, int r, int g, int b, int a);
static void _smart_clip_set (Evas_Object *obj, Evas_Object *clip);
static void _smart_clip_unset (Evas_Object *obj);


/**********************************/
/* Globals for the E Video Object */
/**********************************/
static Evas_Smart  *smart = NULL;


/*******************************/
/* Externally accessible calls */
/*******************************/


/**
 * @addtogroup Esmart_Pdf
 *
 * @{
 */

/**
 * @brief Add a smart pdf object to an evas
 *
 * @param evas The Evas canvas
 * @return The file name
 *
 * Add a smart pdf object to the evas @p evas, or NULL on failure
 */
Evas_Object *
esmart_pdf_add (Evas *evas)
{
  _smart_init ();

  return evas_object_smart_add (evas, smart);
}

/**
 * @brief Initialize a smart pdf object
 *
 * @param obj The Evas object
 * @return 1 on success, 0 otherwise
 *
 * Initialize the smart pdf object @p obj
 */
Evas_Bool
esmart_pdf_init (Evas_Object *obj)
{
  Smart_Pdf *sp;

  E_SMART_OBJ_GET_RETURN (sp, obj, E_OBJ_NAME, 0);

  if (sp->filename) free (sp->filename);
  sp->filename = NULL;

  sp->pdf_document = NULL;
  sp->pdf_page = NULL;
  sp->pdf_index = NULL;

  return 1;
}

/**
 * @brief Set the file name of a smart pdf object
 *
 * @param obj The Evas object
 * @param filename: The file name
 *
 * Set the file name of the smart pdf object @p obj. If the current
 * filename and @p filename are the same, nothing is done and 1 is
 * returned. If @p filename is @c NULL or empty, 0 is returned. If
 * @p filename is a valid PDF document, 1 is returned, otherwise 0
 * is returned.
 */
int
esmart_pdf_file_set (Evas_Object *obj, const char *filename)
{
  Smart_Pdf *sp;

  E_SMART_OBJ_GET_RETURN (sp, obj, E_OBJ_NAME, 0);

  if ((filename) &&
      (sp->filename) &&
      (!strcmp (filename, sp->filename))) return 1;

  if ((filename) && (filename[0] != 0))
    {
      if (sp->filename) free (sp->filename);
      sp->filename = strdup (filename);

      if (sp->pdf_index)
        epdf_index_delete (sp->pdf_index);
      sp->pdf_index = NULL;

      if (sp->pdf_page)
        epdf_page_delete (sp->pdf_page);
      sp->pdf_page = NULL;

      if (sp->pdf_document)
        epdf_document_delete (sp->pdf_document);

      sp->pdf_document = epdf_document_new (sp->filename);
      if (!sp->pdf_document)
        return 0;

      sp->pdf_page = epdf_page_new (sp->pdf_document);
      sp->pdf_index = epdf_index_new (sp->pdf_document);

      return 1;
    }

  return 0;
}

/**
 * @brief Return the name of the file used for a smart pdf object
 *
 * @param obj The Evas object
 * @return The name of the file, or @c NULL on failure
 *
 * Return the name of the file used for the smart pdf object @p obj
 */
const char *
esmart_pdf_file_get (Evas_Object *obj)
{
  Smart_Pdf *sp;

  E_SMART_OBJ_GET_RETURN (sp, obj, E_OBJ_NAME, NULL);

  return sp->filename;
}

/**
 * @brief Set the page number of a smart pdf object
 *
 * @param obj The Evas object
 * @param page: The page number
 *
 * Set the page number of the smart pdf object @p obj
 */
void
esmart_pdf_page_set (Evas_Object *obj, int page)
{
  Smart_Pdf *sp;

  E_SMART_OBJ_GET(sp, obj, E_OBJ_NAME);

  if (!sp->pdf_document ||
      (page < 0) ||
      (page >= epdf_document_page_count_get (sp->pdf_document)) ||
      (page == epdf_page_page_get (sp->pdf_page)))
    return;

  epdf_page_page_set (sp->pdf_page, page);
}

/**
 * @brief Return the page number of a smart pdf object
 *
 * @param obj The Evas object
 * @return The page number
 *
 * Return the page number of the smart pdf object @p obj
 */
int
esmart_pdf_page_get(Evas_Object *obj)
{
  Smart_Pdf *sp;

  E_SMART_OBJ_GET_RETURN(sp, obj, E_OBJ_NAME, 0);

  return epdf_page_page_get (sp->pdf_page);
}

/**
 * @brief Gets the native size of a smart pdf object
 *
 * @param obj The Evas object
 * @param width the location where to set the native width of the pdf
 * @param height the location where to set the native height of the pdf
 */
void esmart_pdf_size_get(Evas_Object *obj, int *width, int *height)
{
  Smart_Pdf *sp;

  E_SMART_OBJ_GET(sp, obj, E_OBJ_NAME);

   if (!sp)
   {
      if (width) *width = 0;
      if (height) *height = 0;
      return;
   }

   epdf_page_size_get (sp->pdf_page, width, height);
}

/**
 * @brief Sets the orientation of a smart pdf object
 *
 * @param obj The Evas object
 * @param o The orientation
 */
void
esmart_pdf_orientation_set (Evas_Object          *obj,
                            Epdf_Page_Orientation o)
{
  Smart_Pdf *sp;

  E_SMART_OBJ_GET(sp, obj, E_OBJ_NAME);

  if (!sp->pdf_page)
    return;

  epdf_page_orientation_set(sp->pdf_page, o);
}

/**
 * @brief Gets the orientation of a smart pdf object
 *
 * @param obj The Evas object
 * @return The orientation
 */
Epdf_Page_Orientation
esmart_pdf_orientation_get (Evas_Object *obj)
{
  Smart_Pdf *sp;

  E_SMART_OBJ_GET_RETURN(sp, obj, E_OBJ_NAME, EPDF_PAGE_ORIENTATION_PORTRAIT);

  if (!sp->pdf_page)
    return EPDF_PAGE_ORIENTATION_PORTRAIT;

  return epdf_page_orientation_get(sp->pdf_page);
}

/**
 * @brief Sets the scale of the PDF document
 *
 * @param obj The Evas object
 * @param hscale The horizontal scale
 * @param vscale The vertical scale
 */
void
esmart_pdf_scale_set (Evas_Object *obj,
                      double       hscale,
                      double       vscale)
{
  Smart_Pdf *sp;

  E_SMART_OBJ_GET(sp, obj, E_OBJ_NAME);

  epdf_page_scale_set(sp->pdf_page, hscale, vscale);
}

/**
 * @brief Gets the scale of the PDF document
 *
 * @param obj The Evas object
 * @param hscale The horizontal scale
 * @param vscale The vertical scale
 */
void
esmart_pdf_scale_get (Evas_Object *obj,
                      double      *hscale,
                      double      *vscale)
{
  Smart_Pdf *sp;

  E_SMART_OBJ_GET(sp, obj, E_OBJ_NAME);

  if (!sp) {
     if (hscale) *hscale = 1.0;
     if (vscale) *vscale = 1.0;
     return;
  }

  epdf_page_scale_get(sp->pdf_page, hscale, vscale);
}

/**
 * @brief Go to the next page
 *
 * @param obj: the smart object
 */
void
esmart_pdf_page_next (Evas_Object *obj)
{
  Smart_Pdf *sp;
  int        page;

  E_SMART_OBJ_GET(sp, obj, E_OBJ_NAME);

  page = epdf_page_page_get (sp->pdf_page);
  if (page < (epdf_document_page_count_get(sp->pdf_document) - 1))
    page++;
  esmart_pdf_page_set (obj, page);
}

/**
 * @brief Go to the previous page
 *
 * @param obj: the smart object
 */
void
esmart_pdf_page_previous (Evas_Object *obj)
{
  Smart_Pdf *sp;
  int        page;

  E_SMART_OBJ_GET(sp, obj, E_OBJ_NAME);

  page = epdf_page_page_get (sp->pdf_page);
  if (page > 0)
    page--;
  esmart_pdf_page_set (obj, page);
}

/**
 * @brief Render the current page
 *
 * @param obj: the smart object
 */
void
esmart_pdf_render (Evas_Object *obj)
{
  Smart_Pdf *sp;

  E_SMART_OBJ_GET (sp, obj, E_OBJ_NAME);

  if (!sp->filename) return;

  if (sp->pdf_document)
    {
      if (sp->obj)
        {
          epdf_page_render (sp->pdf_page, sp->obj);
        }
      evas_object_show (sp->obj);
    }
}

/**
 * @brief Return the poppler document of a smart pdf object
 *
 * @param obj The Evas object
 * @return The poppler document of the pdf (NULL on failure)
 *
 * Return the poppler document of the smart pdf object @p obj
 *
 */
Epdf_Document *
esmart_pdf_pdf_document_get (Evas_Object *obj)
{
  Smart_Pdf *sp;

  E_SMART_OBJ_GET_RETURN(sp, obj, E_OBJ_NAME, NULL);

  return sp->pdf_document;
}

/**
 * @brief Return the current poppler page of a smart pdf object
 *
 * @param obj The Evas object
 * @return The current poppler page of the pdf (NULL on failure)
 *
 * Return the current poppler page of the smart pdf object @p obj
 *
 */
Epdf_Page *
esmart_pdf_pdf_page_get (Evas_Object *obj)
{
  Smart_Pdf *sp;

  E_SMART_OBJ_GET_RETURN(sp, obj, E_OBJ_NAME, NULL);

  return sp->pdf_page;
}

/**
 * Return the index of a smart pdf object
 *
 * @param obj The Evas object
 * @return The index of the pdf (NULL on failure)
 *
 * Return the index of the smart pdf object @p obj
 *
 */
Ecore_List *
esmart_pdf_pdf_index_get (Evas_Object *obj)
{
  Smart_Pdf *sp;

  E_SMART_OBJ_GET_RETURN(sp, obj, E_OBJ_NAME, NULL);

  return sp->pdf_index;
}

/**
 * @}
 */


/*******************************************/
/* Internal smart object required routines */
/*******************************************/
static void
_smart_init (void)
{
  if (smart) return;
  static const Evas_Smart_Class sc =
    {
      E_OBJ_NAME,
      EVAS_SMART_CLASS_VERSION,
      _smart_add,
      _smart_del,
      _smart_move,
      _smart_resize,
      _smart_show,
      _smart_hide,
      _smart_color_set,
      _smart_clip_set,
      _smart_clip_unset,
      NULL
    };
  smart = evas_smart_class_new(&sc);
}

static void
_smart_add (Evas_Object *obj)
{
  Smart_Pdf *sp;

  sp = calloc (1, sizeof (Smart_Pdf));
  if (!sp) return;

  sp->obj = evas_object_image_add (evas_object_evas_get (obj));
/*   evas_object_event_callback_add (sp->obj, EVAS_CALLBACK_MOUSE_MOVE, _mouse_move, sp); */
/*   evas_object_event_callback_add (sp->obj, EVAS_CALLBACK_MOUSE_DOWN, _mouse_down, sp); */
/*   evas_object_image_pixels_get_callback_set (sp->obj, _pixels_get, sp); */
  evas_object_smart_member_add (sp->obj, obj);

  evas_object_image_alpha_set (sp->obj, 0);
  evas_object_smart_data_set (obj, sp);
}

static void
_smart_del (Evas_Object *obj)
{
  Smart_Pdf *sp;

  sp = evas_object_smart_data_get (obj);
  if (!sp) return;

  if (sp->filename)
    free (sp->filename);

  if (sp->pdf_document)
    epdf_document_delete (sp->pdf_document);
  if (sp->pdf_page)
    epdf_page_delete (sp->pdf_page);
  if (sp->pdf_index)
    epdf_index_delete (sp->pdf_index);

  free (sp);
}

static void
_smart_move (Evas_Object *obj, Evas_Coord x, Evas_Coord y)
{
  Smart_Pdf *sp;

  sp = evas_object_smart_data_get (obj);
  if (!sp) return;

  evas_object_move (sp->obj, x, y);
}

static void
_smart_resize (Evas_Object *obj, Evas_Coord w, Evas_Coord h)
{
  Smart_Pdf *sp;

  sp = evas_object_smart_data_get (obj);
  if (!sp) return;

  evas_object_image_fill_set (sp->obj, 0, 0, w, h);
  evas_object_resize (sp->obj, w, h);
}

static void
_smart_show (Evas_Object *obj)
{
  Smart_Pdf *sp;

  sp = evas_object_smart_data_get (obj);
  if (!sp) return;

  evas_object_show (sp->obj);

}

static void
_smart_hide (Evas_Object *obj)
{
  Smart_Pdf *sp;

  sp = evas_object_smart_data_get (obj);
  if (!sp) return;

  evas_object_hide (sp->obj);
}

static void
_smart_color_set (Evas_Object *obj, int r, int g, int b, int a)
{
  Smart_Pdf *sp;

  sp = evas_object_smart_data_get (obj);
  if (!sp) return;

  evas_object_color_set (sp->obj, r, g, b, a);
}

static void
_smart_clip_set (Evas_Object *obj, Evas_Object *clip)
{
  Smart_Pdf *sp;

  sp = evas_object_smart_data_get (obj);
  if (!sp) return;

  evas_object_clip_set (sp->obj, clip);
}

static void
_smart_clip_unset (Evas_Object *obj)
{
  Smart_Pdf *sp;

  sp = evas_object_smart_data_get (obj);
  if (!sp) return;

  evas_object_clip_unset (sp->obj);
}
