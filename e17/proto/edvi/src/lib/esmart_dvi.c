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
 * Add a smart dvi object to an evas
 *
 * @param evas The Evas canvas
 * @return The file name
 *
 * Add a smart dvi object to the evas @p evas, or NULL on failure
 *
 */
Evas_Object *
esmart_dvi_add (Evas *evas)
{
  _smart_init ();

  return evas_object_smart_add (evas, smart);
}

/**
 * Initialize a smart dvi object
 *
 * @param obj The Evas object
 * @return 1 on success, 0 otherwise
 *
 * Initialize the smart dvi object @p obj
 *
 */
Evas_Bool
esmart_dvi_init (Evas_Object *obj)
{
  Smart_Dvi *sp;

  E_SMART_OBJ_GET_RETURN (sp, obj, E_OBJ_NAME, 0);

  if (sp->filename) free (sp->filename);
  sp->filename = NULL;
  sp->page = 0;
  sp->page_length = 10;

  sp->dvi_device = edvi_device_new (edvi_dpi_get(), edvi_dpi_get());
  sp->dvi_property = edvi_property_new();
  edvi_property_delayed_font_open_set(sp->dvi_property, 1);
  sp->dvi_document = NULL;
  sp->dvi_page = NULL;

  sp->orientation = EDVI_PAGE_ORIENTATION_PORTRAIT;
  sp->hscale = 1.0;
  sp->vscale = 1.0;

  return 1;
}

/**
 * Set the file name of a smart dvi object
 *
 * @param obj The Evas object
 * @param filename: The file name
 *
 * Set the file name of the smart dvi object @p obj
 *
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
    }
}

/**
 * Return the name of the file used for a smart dvi object
 *
 * @param obj The Evas object
 * @return The name of the file, or @c NULL on failure
 *
 * Return the name of the file used for the smart dvi object @p obj
 *
 */
const char *
esmart_dvi_file_get (Evas_Object *obj)
{
  Smart_Dvi *sp;

  E_SMART_OBJ_GET_RETURN (sp, obj, E_OBJ_NAME, NULL);

  return sp->filename;
}

/**
 * Set the page number of a smart dvi object
 *
 * @param obj The Evas object
 * @param page: The page number
 *
 * Set the page number of the smart dvi object @p obj
 *
 */
void
esmart_dvi_page_set (Evas_Object *obj, int page)
{
  Smart_Dvi *sp;

  E_SMART_OBJ_GET(sp, obj, E_OBJ_NAME);

  if ((page >= edvi_document_page_count_get (sp->dvi_document)) ||
      (page == sp->page))
    return;

  sp->page = page;
  _smart_page_render (obj);
}

/**
 * Return the page number of a smart dvi object
 *
 * @param obj The Evas object
 * @return The page number
 *
 * Return the page number of the smart dvi object @p obj
 *
 */
int
esmart_dvi_page_get(Evas_Object *obj)
{
  Smart_Dvi *sp;

  E_SMART_OBJ_GET_RETURN(sp, obj, E_OBJ_NAME, 0);

  return sp->page;
}

/**
 * Return the document of a smart dvi object
 *
 * @param obj The Evas object
 * @return The document of the dvi (NULL on failure)
 *
 * Return the document of the smart dvi object @p obj
 *
 */
Edvi_Document *
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
 *
 */
Edvi_Page *
esmart_dvi_dvi_page_get (Evas_Object *obj)
{
  Smart_Dvi *sp;

  E_SMART_OBJ_GET_RETURN(sp, obj, E_OBJ_NAME, NULL);

  return sp->dvi_page;
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
      if (width)
	 *width = 0;
      if (height)
	 *height = 0;
   }
   else {
      if (width)
	 *width = edvi_page_width_get (sp->dvi_page);
      if (height)
	 *height = edvi_page_height_get (sp->dvi_page);
   }
}

void
esmart_dvi_orientation_set (Evas_Object          *obj,
                            Edvi_Page_Orientation o)
{
  Smart_Dvi *sp;

  E_SMART_OBJ_GET(sp, obj, E_OBJ_NAME);

  if (o == sp->orientation)
    return;

  sp->orientation = o;
  _smart_page_render (obj);
}

Edvi_Page_Orientation
esmart_dvi_orientation_get (Evas_Object *obj)
{
  Smart_Dvi *sp;

  E_SMART_OBJ_GET_RETURN(sp, obj, E_OBJ_NAME, EDVI_PAGE_ORIENTATION_PORTRAIT);

  return sp->orientation;
}

void
esmart_dvi_scale_set (Evas_Object *obj,
                      double       hscale,
                      double       vscale)
{
  Smart_Dvi *sp;

  E_SMART_OBJ_GET(sp, obj, E_OBJ_NAME);

  if (hscale != sp->hscale)
    sp->hscale = hscale;

  if (vscale != sp->vscale)
    sp->vscale = vscale;

  _smart_page_render (obj);
}

void
esmart_dvi_scale_get (Evas_Object *obj,
                      double      *hscale,
                      double      *vscale)
{
  Smart_Dvi *sp;

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

void
esmart_dvi_page_next (Evas_Object *obj)
{
  Smart_Dvi *sp;
  int        page;

  E_SMART_OBJ_GET(sp, obj, E_OBJ_NAME);

  page = sp->page;
  if (page < (edvi_document_page_count_get(sp->dvi_document) - 1))
    page++;
  esmart_dvi_page_set (obj, page);
}

void
esmart_dvi_page_previous (Evas_Object *obj)
{
  Smart_Dvi *sp;
  int        page;

  E_SMART_OBJ_GET(sp, obj, E_OBJ_NAME);

  page = sp->page;
  if (page > 0)
    page--;
  esmart_dvi_page_set (obj, page);
}

void
esmart_dvi_page_page_length_set (Evas_Object *obj, int page_length)
{
  Smart_Dvi *sp;

  E_SMART_OBJ_GET(sp, obj, E_OBJ_NAME);

  if ((page_length <= 0) || (sp->page_length == page_length))
    return;
  sp->page_length = page_length;
}

int
esmart_dvi_page_page_length_get (Evas_Object *obj)
{
  Smart_Dvi *sp;

  E_SMART_OBJ_GET_RETURN(sp, obj, E_OBJ_NAME, 0);

  return sp->page_length;
}

void
esmart_dvi_page_page_next (Evas_Object *obj)
{
  Smart_Dvi *sp;
  int        page;

  E_SMART_OBJ_GET(sp, obj, E_OBJ_NAME);

  page = sp->page + sp->page_length;
  if (page > (edvi_document_page_count_get(sp->dvi_document) - 1))
    page = edvi_document_page_count_get(sp->dvi_document) - 1;
  esmart_dvi_page_set (obj, sp->page);
}

void
esmart_dvi_page_page_previous (Evas_Object *obj)
{
  Smart_Dvi *sp;
  int        page;

  E_SMART_OBJ_GET(sp, obj, E_OBJ_NAME);

  page = sp->page - sp->page_length;
  if (page < 0)
    page = 0;
  esmart_dvi_page_set (obj, page);
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
_smart_layer_set (Evas_Object *obj, int layer)
{
  Smart_Dvi *sp;

  sp = evas_object_smart_data_get (obj);
  if (!sp) return;

  evas_object_layer_set (sp->obj, layer);
}

static void
_smart_raise (Evas_Object *obj)
{
  Smart_Dvi *sp;

  sp = evas_object_smart_data_get (obj);
  if (!sp) return;

  evas_object_raise (sp->obj);
}

static void
_smart_lower (Evas_Object *obj)
{
  Smart_Dvi *sp;

  sp = evas_object_smart_data_get (obj);
  if (!sp) return;

  evas_object_lower (sp->obj);
}

static void
_smart_stack_above (Evas_Object *obj, Evas_Object *above)
{
  Smart_Dvi *sp;

  sp = evas_object_smart_data_get (obj);
  if (!sp) return;

  evas_object_stack_above (sp->obj, above);
}

static void
_smart_stack_below (Evas_Object *obj, Evas_Object *below)
{
  Smart_Dvi *sp;

  sp = evas_object_smart_data_get (obj);
  if (!sp) return;

  evas_object_stack_below (sp->obj, below);
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

static void
_smart_page_render (Evas_Object *obj)
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
          sp->dvi_page = edvi_page_new (sp->dvi_document, sp->page);
          edvi_page_render (sp->dvi_page, sp->dvi_device, sp->obj);
        }
      evas_object_show (sp->obj);
    }
}
