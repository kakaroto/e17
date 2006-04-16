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
static void _smart_layer_set (Evas_Object *obj, int layer);
static void _smart_raise (Evas_Object *obj);
static void _smart_lower (Evas_Object *obj);
static void _smart_stack_above (Evas_Object *obj, Evas_Object *above);
static void _smart_stack_below (Evas_Object *obj, Evas_Object *below);
static void _smart_move (Evas_Object *obj, Evas_Coord x, Evas_Coord y);
static void _smart_resize (Evas_Object *obj, Evas_Coord w, Evas_Coord h);
static void _smart_show (Evas_Object *obj);
static void _smart_hide (Evas_Object *obj);
static void _smart_color_set (Evas_Object *obj, int r, int g, int b, int a);
static void _smart_clip_set (Evas_Object *obj, Evas_Object *clip);
static void _smart_clip_unset (Evas_Object *obj);

static void _smart_page_render (Evas_Object *obj);


/**********************************/
/* Globals for the E Video Object */
/**********************************/
static Evas_Smart  *smart = NULL;


/*******************************/
/* Externally accessible calls */
/*******************************/

/**
 * Add a smart pdf object to an evas
 *
 * @param evas The Evas canvas
 * @return The file name
 *
 * Add a smart pdf object to the evas @p evas, or NULL on failure
 * 
 */
Evas_Object *
esmart_pdf_add (Evas *evas)
{
  _smart_init ();

  return evas_object_smart_add (evas, smart);
}

/**
 * Initialize a smart pdf object
 *
 * @param obj The Evas object
 * @return 1 on success, 0 otherwise
 *
 * Initialize the smart pdf object @p obj
 * 
 */
Evas_Bool
esmart_pdf_init (Evas_Object *obj)
{
  Smart_Pdf *sp;

  E_SMART_OBJ_GET_RETURN (sp, obj, E_OBJ_NAME, 0);

  if (sp->filename) free (sp->filename);
  sp->filename = NULL;
  sp->page = 0;

  sp->pdf_document = NULL;
  sp->pdf_page = NULL;
  sp->pdf_index = NULL;

  sp->orientation = EVAS_POPPLER_PAGE_ORIENTATION_PORTRAIT;
  sp->hscale = 1.0;
  sp->vscale = 1.0;

  return 1;
}

/**
 * Set the file name of a smart pdf object
 *
 * @param obj The Evas object
 * @param filename: The file name
 *
 * Set the file name of the smart pdf object @p obj
 * 
 */
void
esmart_pdf_file_set (Evas_Object *obj, const char *filename)
{
  Smart_Pdf *sp;

  E_SMART_OBJ_GET (sp, obj, E_OBJ_NAME);

  if ((filename) &&
      (sp->filename) &&
      (!strcmp (filename, sp->filename))) return;

  if ((filename) && (filename[0] != 0))
    {
      sp->filename = strdup (filename);

      if (sp->pdf_document)
        evas_poppler_document_delete (sp->pdf_document);

      if (sp->pdf_index)
        evas_poppler_index_delete (sp->pdf_index);

      sp->pdf_document = evas_poppler_document_new (sp->filename);
      sp->pdf_index = evas_poppler_index_new (sp->pdf_document);
    }
}

/**
 * Return the name of the file used for a smart pdf object
 *
 * @param obj The Evas object
 * @return The name of the file, or @c NULL on failure
 *
 * Return the name of the file used for the smart pdf object @p obj
 * 
 */
const char *
esmart_pdf_file_get (Evas_Object *obj)
{
  Smart_Pdf *sp;

  E_SMART_OBJ_GET_RETURN (sp, obj, E_OBJ_NAME, NULL);

  return sp->filename;
}

/**
 * Set the page number of a smart pdf object
 *
 * @param obj The Evas object
 * @param page: The page number
 *
 * Set the page number of the smart pdf object @p obj
 * 
 */
void
esmart_pdf_page_set (Evas_Object *obj, int page)
{
  Smart_Pdf *sp;
   
  E_SMART_OBJ_GET(sp, obj, E_OBJ_NAME);

  if ((page >= evas_poppler_document_page_count_get (sp->pdf_document)) ||
      (page == sp->page))
    return;

    sp->page = page;
    _smart_page_render (obj);
}

/**
 * Return the page number of a smart pdf object
 *
 * @param obj The Evas object
 * @return The page number
 *
 * Return the page number of the smart pdf object @p obj
 * 
 */
int
esmart_pdf_page_get(Evas_Object *obj)
{
  Smart_Pdf *sp;
   
  E_SMART_OBJ_GET_RETURN(sp, obj, E_OBJ_NAME, 0);

  return sp->page;
}

/**
 * Return the poppler document of a smart pdf object
 *
 * @param obj The Evas object
 * @return The poppler document of the pdf (NULL on failure)
 *
 * Return the poppler document of the smart pdf object @p obj
 * 
 */
Evas_Poppler_Document *
esmart_pdf_pdf_document_get (Evas_Object *obj)
{
  Smart_Pdf *sp;
   
  E_SMART_OBJ_GET_RETURN(sp, obj, E_OBJ_NAME, NULL);

  return sp->pdf_document;
}

/**
 * Return the current poppler page of a smart pdf object
 *
 * @param obj The Evas object
 * @return The current poppler page of the pdf (NULL on failure)
 *
 * Return the current poppler page of the smart pdf object @p obj
 * 
 */
Evas_Poppler_Page *
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
      if (width)
	 *width = 0;
      if (height)
	 *height = 0;
   }
   else {
      if (width)
	 *width = evas_poppler_page_width_get (sp->pdf_page);
      if (height)
	 *height = evas_poppler_page_height_get (sp->pdf_page);
   }
}

void
esmart_pdf_orientation_set (Evas_Object *obj,
                            Evas_Poppler_Page_Orientation o)
{
  Smart_Pdf *sp;
   
  E_SMART_OBJ_GET(sp, obj, E_OBJ_NAME);

  if (o == sp->orientation)
    return;

  sp->orientation = o;
  _smart_page_render (obj);
}

Evas_Poppler_Page_Orientation
esmart_pdf_orientation_get (Evas_Object *obj)
{
  Smart_Pdf *sp;
   
  E_SMART_OBJ_GET_RETURN(sp, obj, E_OBJ_NAME, EVAS_POPPLER_PAGE_ORIENTATION_PORTRAIT);

  return sp->orientation;
}

void
esmart_pdf_scale_set (Evas_Object *obj,
                      double       hscale,
                      double       vscale)
{
  Smart_Pdf *sp;
   
  E_SMART_OBJ_GET(sp, obj, E_OBJ_NAME);

  if (hscale != sp->hscale)
    sp->hscale = hscale;

  if (vscale != sp->vscale)
    sp->vscale = vscale;

  _smart_page_render (obj);
}

void
esmart_pdf_scale_get (Evas_Object *obj,
                      double      *hscale,
                      double      *vscale)
{
  Smart_Pdf *sp;
   
  E_SMART_OBJ_GET(sp, obj, E_OBJ_NAME);

  if (!sp) {
     if (hscale)
        *hscale = 1.0;

     if (vscale)
        *vscale = 1.0;
  }
  else {
     if (hscale)
        *hscale = sp->hscale;

      if (vscale)
         *vscale = sp->vscale;
  }
}


/*******************************************/
/* Internal smart object required routines */
/*******************************************/
static void
_smart_init (void)
{
  if (smart) return;
  smart = evas_smart_new (E_OBJ_NAME,
                          _smart_add,
                          _smart_del,
                          _smart_layer_set,
                          _smart_raise,
                          _smart_lower,
                          _smart_stack_above,
                          _smart_stack_below,
                          _smart_move,
                          _smart_resize,
                          _smart_show,
                          _smart_hide,
                          _smart_color_set,
                          _smart_clip_set,
                          _smart_clip_unset,
                          NULL);
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
    evas_poppler_document_delete (sp->pdf_document);
  if (sp->pdf_page)
    evas_poppler_page_delete (sp->pdf_page);
  if (sp->pdf_index)
    evas_poppler_index_delete (sp->pdf_index);

  free (sp);
}

static void
_smart_layer_set (Evas_Object *obj, int layer)
{
  Smart_Pdf *sp;

  sp = evas_object_smart_data_get (obj);
  if (!sp) return;

  evas_object_layer_set (sp->obj, layer);
}

static void
_smart_raise (Evas_Object *obj)
{
  Smart_Pdf *sp;

  sp = evas_object_smart_data_get (obj);
  if (!sp) return;

  evas_object_raise (sp->obj);
}

static void
_smart_lower (Evas_Object *obj)
{
  Smart_Pdf *sp;

  sp = evas_object_smart_data_get (obj);
  if (!sp) return;

  evas_object_lower (sp->obj);
}

static void
_smart_stack_above (Evas_Object *obj, Evas_Object *above)
{
  Smart_Pdf *sp;

  sp = evas_object_smart_data_get (obj);
  if (!sp) return;

  evas_object_stack_above (sp->obj, above);
}

static void
_smart_stack_below (Evas_Object *obj, Evas_Object *below)
{
  Smart_Pdf *sp;

  sp = evas_object_smart_data_get (obj);
  if (!sp) return;

  evas_object_stack_below (sp->obj, below);
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

static void
_smart_page_render (Evas_Object *obj)
{
  Smart_Pdf *sp;

  E_SMART_OBJ_GET (sp, obj, E_OBJ_NAME);

  if (!sp->filename) return;
      
  if (sp->pdf_document)
    {
      if (sp->pdf_page)
        evas_poppler_page_delete (sp->pdf_page);
      if (sp->obj)
        {
          sp->pdf_page = evas_poppler_document_page_get (sp->pdf_document, sp->page);
          evas_poppler_page_render (sp->pdf_page, sp->obj, sp->orientation,
                                    0, 0, -1, -1,
                                    sp->hscale, sp->vscale);
        }
      evas_object_show (sp->obj);
    }
}
