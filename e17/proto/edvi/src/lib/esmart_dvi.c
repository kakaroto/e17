#include <stdlib.h>
#include <string.h>

#include <Evas.h>

#include "esmart_dvi.h"


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

#define E_OBJ_NAME "dvilib_object"


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
 * @addtogroup Esmart_Dvi
 *
 * @{
 */

/**
 * @brief Add a smart dvi object to an evas
 *
 * @param evas The Evas canvas
 * @return The file name
 *
 * Add a smart dvi object to the evas @p evas, or NULL on failure
 */
Evas_Object *
esmart_dvi_add (Evas *evas)
{
  _smart_init ();

  return evas_object_smart_add (evas, smart);
}

/**
 * @brief Initialize a smart dvi object
 *
 * @param obj The Evas object
 * @return 1 on success, 0 otherwise
 *
 * Initialize the smart dvi object @p obj
 */
Evas_Bool
esmart_dvi_init (Evas_Object *obj)
{
  Smart_Dvi *sp;

  E_SMART_OBJ_GET_RETURN (sp, obj, E_OBJ_NAME, 0);

  if (sp->filename) free (sp->filename);
  sp->filename = NULL;

  sp->dvi_device = edvi_device_new (edvi_dpi_get(), edvi_dpi_get());
  sp->dvi_property = edvi_property_new();
  edvi_property_property_set (sp->dvi_property, EDVI_PROPERTY_DELAYED_FONT_OPEN);
  sp->dvi_document = NULL;
  sp->dvi_page = NULL;

  return 1;
}

/**
 * @brief Set the file name of a smart dvi object
 *
 * @param obj The Evas object
 * @param filename: The file name
 *
 * Set the file name of the smart dvi object @p obj
 */
void
esmart_dvi_file_set (Evas_Object *obj, const char *filename)
{
  Smart_Dvi *sp;

  E_SMART_OBJ_GET (sp, obj, E_OBJ_NAME);

  if ((filename) &&
      (sp->filename) &&
      (!strcmp (filename, sp->filename))) return;

  if ((filename) && (filename[0] != 0))
    {
      sp->filename = strdup (filename);

      if (sp->dvi_document)
        edvi_document_delete (sp->dvi_document);

      sp->dvi_document = edvi_document_new (sp->filename, sp->dvi_device, sp->dvi_property);
      sp->dvi_page = edvi_page_new (sp->dvi_document);
    }
}

/**
 * @brief Return the name of the file used for a smart dvi object
 *
 * @param obj The Evas object
 * @return The name of the file, or @c NULL on failure
 *
 * Return the name of the file used for the smart dvi object @p obj
 */
const char *
esmart_dvi_file_get (Evas_Object *obj)
{
  Smart_Dvi *sp;

  E_SMART_OBJ_GET_RETURN (sp, obj, E_OBJ_NAME, NULL);

  return sp->filename;
}

/**
 * @brief Set the page number of a smart dvi object
 *
 * @param obj The Evas object
 * @param page: The page number
 *
 * Set the page number of the smart dvi object @p obj
 */
void
esmart_dvi_page_set (Evas_Object *obj, int page)
{
  Smart_Dvi *sp;

  E_SMART_OBJ_GET(sp, obj, E_OBJ_NAME);

  if (!sp->dvi_document ||
      (page < 0) ||
      (page >= edvi_document_page_count_get (sp->dvi_document)) ||
      (page == edvi_page_page_get (sp->dvi_page)))
    return;

  edvi_page_page_set (sp->dvi_page, page);
}

/**
 * @brief Return the page number of a smart dvi object
 *
 * @param obj The Evas object
 * @return The page number
 *
 * Return the page number of the smart dvi object @p obj
 */
int
esmart_dvi_page_get(Evas_Object *obj)
{
  Smart_Dvi *sp;

  E_SMART_OBJ_GET_RETURN(sp, obj, E_OBJ_NAME, 0);

  return edvi_page_page_get (sp->dvi_page);
}

/**
 * @brief Gets the native size of a smart dvi object
 *
 * @param obj The Evas object
 * @param width the location where to set the native width of the dvi
 * @param height the location where to set the native height of the dvi
 */
void esmart_dvi_size_get(Evas_Object *obj, int *width, int *height)
{
  Smart_Dvi *sp;

  E_SMART_OBJ_GET(sp, obj, E_OBJ_NAME);

   if (!sp)
   {
      if (width) *width = 0;
      if (height) *height = 0;
      return;
   }

   edvi_page_size_get (sp->dvi_page, width, height);
}

/**
 * @brief Sets the orientation of a smart dvi object
 *
 * @param obj The Evas object
 * @param o The orientation
 */
void
esmart_dvi_orientation_set (Evas_Object          *obj,
                            Edvi_Page_Orientation o)
{
  Smart_Dvi *sp;

  E_SMART_OBJ_GET(sp, obj, E_OBJ_NAME);

  if (!sp->dvi_page)
    return;

  edvi_page_orientation_set(sp->dvi_page, o);
}

/**
 * @brief Gets the orientation of a smart dvi object
 *
 * @param obj The Evas object
 * @return The orientation
 */
Edvi_Page_Orientation
esmart_dvi_orientation_get (Evas_Object *obj)
{
  Smart_Dvi *sp;

  E_SMART_OBJ_GET_RETURN(sp, obj, E_OBJ_NAME, EDVI_PAGE_ORIENTATION_PORTRAIT);

  if (!sp->dvi_page)
    return EDVI_PAGE_ORIENTATION_PORTRAIT;

  return edvi_page_orientation_get(sp->dvi_page);
}

/**
 * @brief Sets the scale of the DVI document
 *
 * @param obj The Evas object
 * @param hscale The horizontal scale
 * @param vscale The vertical scale
 */
void
esmart_dvi_mag_set (Evas_Object *obj,
                    double       mag)
{
  Smart_Dvi *sp;

  E_SMART_OBJ_GET(sp, obj, E_OBJ_NAME);

  edvi_page_mag_set(sp->dvi_page, mag);
}

/**
 * @brief Gets the scale of the DVI document
 *
 * @param obj The Evas object
 * @param hscale The horizontal scale
 * @param vscale The vertical scale
 */
double
esmart_dvi_mag_get (Evas_Object *obj)
{
  Smart_Dvi *sp;

  E_SMART_OBJ_GET_RETURN (sp, obj, E_OBJ_NAME, 1.0);

  return edvi_page_mag_get(sp->dvi_page);
}

/**
 * @brief Go to the next page and render it
 *
 * @param obj: the smart object
 */
void
esmart_dvi_page_next (Evas_Object *obj)
{
  Smart_Dvi *sp;
  int        page;

  E_SMART_OBJ_GET(sp, obj, E_OBJ_NAME);

  page = edvi_page_page_get (sp->dvi_page);
  if (page < (edvi_document_page_count_get(sp->dvi_document) - 1))
    page++;
  esmart_dvi_page_set (obj, page);
}

/**
 * @brief Go to the previous page and render it
 *
 * @param obj: the smart object
 */
void
esmart_dvi_page_previous (Evas_Object *obj)
{
  Smart_Dvi *sp;
  int        page;

  E_SMART_OBJ_GET(sp, obj, E_OBJ_NAME);

  page = edvi_page_page_get (sp->dvi_page);
  if (page > 0)
    page--;
  esmart_dvi_page_set (obj, page);
}

/**
 * @brief Render the current page
 *
 * @param obj: the smart object
 */
/* void */
/* esmart_dvi_render (Evas_Object *obj) */
/* { */
/*   Smart_Dvi *sp; */

/*   E_SMART_OBJ_GET (sp, obj, E_OBJ_NAME); */

/*   if (!sp->filename) return; */

/*   if (sp->dvi_document) */
/*     { */
/*       if (sp->obj) */
/*         { */
/*           edvi_page_render (sp->dvi_page, sp->obj); */
/*         } */
/*       evas_object_show (sp->obj); */
/*     } */
/* } */

void
esmart_dvi_render (Evas_Object *obj)
{
  Smart_Dvi *sp;

  E_SMART_OBJ_GET (sp, obj, E_OBJ_NAME);

  if (!sp->filename) return;

  if (sp->dvi_document)
    {
      if (sp->dvi_page)
        edvi_page_delete (sp->dvi_page);

      if (sp->obj)
        {
          unsigned int *m;
          int           w;
          int           h;

          sp->dvi_page = edvi_page_new (sp->dvi_document);
          edvi_page_size_get (sp->dvi_page, &w, &h);
          evas_object_image_size_set (sp->obj, w, h);
          evas_object_image_fill_set (sp->obj, 0, 0, w, h);
          m = (unsigned int *)evas_object_image_data_get (sp->obj, 1);
          if (!m)
            return;

          memset(m, (255 << 24) | (255 << 16) | (255 << 8) | 255, w * h * 4);
          evas_object_image_data_update_add (sp->obj, 0, 0, w, h);
          evas_object_resize (sp->obj, w, h);
          edvi_page_render (sp->dvi_page, sp->dvi_device, sp->obj);
        }
      evas_object_show (sp->obj);
    }
}

/**
 * @brief Return the document of a smart dvi object
 *
 * @param obj The Evas object
 * @return The document of the dvi (NULL on failure)
 *
 * Return the document of the smart dvi object @p obj
 */
const Edvi_Document *
esmart_dvi_dvi_document_get (Evas_Object *obj)
{
  Smart_Dvi *sp;

  E_SMART_OBJ_GET_RETURN(sp, obj, E_OBJ_NAME, NULL);

  return sp->dvi_document;
}

/**
 * Return the current page of a smart dvi object
 *
 * @param obj The Evas object
 * @return The current page of the dvi (NULL on failure)
 *
 * Return the current page of the smart dvi object @p obj
 */
const Edvi_Page *
esmart_dvi_dvi_page_get (Evas_Object *obj)
{
  Smart_Dvi *sp;

  E_SMART_OBJ_GET_RETURN(sp, obj, E_OBJ_NAME, NULL);

  return sp->dvi_page;
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
  Smart_Dvi *sp;

  sp = calloc (1, sizeof (Smart_Dvi));
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
  Smart_Dvi *sp;

  sp = evas_object_smart_data_get (obj);
  if (!sp) return;

  if (sp->filename)
    free (sp->filename);

  if (sp->dvi_document)
    edvi_document_delete (sp->dvi_document);
  if (sp->dvi_page)
    edvi_page_delete (sp->dvi_page);

  free (sp);
}

static void
_smart_move (Evas_Object *obj, Evas_Coord x, Evas_Coord y)
{
  Smart_Dvi *sp;

  sp = evas_object_smart_data_get (obj);
  if (!sp) return;

  evas_object_move (sp->obj, x, y);
}

static void
_smart_resize (Evas_Object *obj, Evas_Coord w, Evas_Coord h)
{
  Smart_Dvi *sp;

  sp = evas_object_smart_data_get (obj);
  if (!sp) return;

  evas_object_image_fill_set (sp->obj, 0, 0, w, h);
  evas_object_resize (sp->obj, w, h);
}

static void
_smart_show (Evas_Object *obj)
{
  Smart_Dvi *sp;

  sp = evas_object_smart_data_get (obj);
  if (!sp) return;

  evas_object_show (sp->obj);

}

static void
_smart_hide (Evas_Object *obj)
{
  Smart_Dvi *sp;

  sp = evas_object_smart_data_get (obj);
  if (!sp) return;

  evas_object_hide (sp->obj);
}

static void
_smart_color_set (Evas_Object *obj, int r, int g, int b, int a)
{
  Smart_Dvi *sp;

  sp = evas_object_smart_data_get (obj);
  if (!sp) return;

  evas_object_color_set (sp->obj, r, g, b, a);
}

static void
_smart_clip_set (Evas_Object *obj, Evas_Object *clip)
{
  Smart_Dvi *sp;

  sp = evas_object_smart_data_get (obj);
  if (!sp) return;

  evas_object_clip_set (sp->obj, clip);
}

static void
_smart_clip_unset (Evas_Object *obj)
{
  Smart_Dvi *sp;

  sp = evas_object_smart_data_get (obj);
  if (!sp) return;

  evas_object_clip_unset (sp->obj);
}
