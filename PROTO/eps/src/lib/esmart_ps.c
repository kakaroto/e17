#include <stdlib.h>
#include <string.h>

#include <Evas.h>

#include "esmart_ps.h"


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
 * @addtogroup Esmart_Ps
 *
 * @{
 */

/**
 * @brief Add a smart ps object to an evas
 *
 * @param evas The Evas canvas
 * @return The file name
 *
 * Add a smart ps object to the evas @p evas, or NULL on failure
 */
Evas_Object *
esmart_ps_add (Evas *evas)
{
   _smart_init ();

   return evas_object_smart_add (evas, smart);
}

/**
 * @brief Initialize a smart ps object
 *
 * @param obj The Evas object
 * @return 1 on success, 0 otherwise
 *
 * Initialize the smart ps object @p obj
 */
Evas_Bool
esmart_ps_init (Evas_Object *obj)
{
   Smart_Ps *sp;

   E_SMART_OBJ_GET_RETURN (sp, obj, E_OBJ_NAME, 0);

   if (sp->filename) free (sp->filename);
   sp->filename = NULL;

   sp->ps_document = NULL;
   sp->ps_page = NULL;

   return 1;
}

/**
 * @brief Set the file name of a smart ps object
 *
 * @param obj The Evas object
 * @param filename: The file name
 * @return 1 if the file can be loaded or is unchanged, 0 otherwise
 *
 * Set the file name of the smart ps object @p obj. If the current
 * filename and @p filename are the same, nothing is done and 1 is
 * returned. If @p filename is @c NULL or empty, 0 is returned. If
 * @p filename is a valid Postscript document, 1 is returned,
 * otherwise 0 is returned.
 */
int
esmart_ps_file_set (Evas_Object *obj, const char *filename)
{
   Smart_Ps *sp;

   E_SMART_OBJ_GET_RETURN (sp, obj, E_OBJ_NAME, 0);

   if ((filename) &&
       (sp->filename) &&
       (!strcmp (filename, sp->filename))) return 1;

   if ((filename) && (filename[0] != 0))
     {
       if (sp->filename) free (sp->filename);
        sp->filename = strdup (filename);

        if (sp->ps_page)
          eps_page_delete (sp->ps_page);
        sp->ps_page = NULL;

        if (sp->ps_document)
          eps_document_delete (sp->ps_document);

        sp->ps_document = eps_document_new (sp->filename);
      if (!sp->ps_document)
        return 0;

      sp->ps_page = eps_page_new (sp->ps_document);

      return 1;
     }

   return 0;
}

/**
 * @brief Return the name of the file used for a smart ps object
 *
 * @param obj The Evas object
 * @return The name of the file, or @c NULL on failure
 *
 * Return the name of the file used for the smart ps object @p obj
 */
const char *
esmart_ps_file_get (Evas_Object *obj)
{
   Smart_Ps *sp;

   E_SMART_OBJ_GET_RETURN (sp, obj, E_OBJ_NAME, NULL);

   return sp->filename;
}

/**
 * @brief Set the page number of a smart ps object
 *
 * @param obj The Evas object
 * @param page: The page number
 *
 * Set the page number of the smart ps object @p obj
 */
void
esmart_ps_page_set (Evas_Object *obj, int page)
{
   Smart_Ps *sp;

   E_SMART_OBJ_GET(sp, obj, E_OBJ_NAME);

   if (!sp->ps_document ||
      (page < 0) ||
      (page >= eps_document_page_count_get (sp->ps_document)) ||
       (page == eps_page_page_get (sp->ps_page)))
     return;

   eps_page_page_set (sp->ps_page, page);
}

/**
 * @brief Return the page number of a smart ps object
 *
 * @param obj The Evas object
 * @return The page number
 *
 * Return the page number of the smart ps object @p obj
 */
int
esmart_ps_page_get(Evas_Object *obj)
{
   Smart_Ps *sp;

   E_SMART_OBJ_GET_RETURN(sp, obj, E_OBJ_NAME, 0);

   return eps_page_page_get (sp->ps_page);
}

/**
 * @brief Gets the native size of a smart ps object
 *
 * @param obj The Evas object
 * @param width the location where to set the native width of the ps
 * @param height the location where to set the native height of the ps
 */
void esmart_ps_size_get(Evas_Object *obj,
                        int         *width,
                        int         *height)
{
   Smart_Ps *sp;

   E_SMART_OBJ_GET(sp, obj, E_OBJ_NAME);

   if (!sp)
   {
      if (width) *width = 0;
      if (height) *height = 0;
      return;
   }

   eps_page_size_get (sp->ps_page, width, height);
}

/**
 * @brief Sets the orientation of a smart ps object
 *
 * @param obj The Evas object
 * @param o The orientation
 */
void
esmart_ps_orientation_set (Evas_Object         *obj,
                           Eps_Page_Orientation o)
{
   Smart_Ps *sp;

   E_SMART_OBJ_GET(sp, obj, E_OBJ_NAME);

   if (!sp->ps_page)
     return;

   eps_page_orientation_set(sp->ps_page, o);
}

/**
 * @brief Gets the orientation of a smart ps object
 *
 * @param obj The Evas object
 * @return The orientation
 */
Eps_Page_Orientation
esmart_ps_orientation_get (Evas_Object *obj)
{
   Smart_Ps *sp;

   E_SMART_OBJ_GET_RETURN(sp, obj, E_OBJ_NAME, EPS_PAGE_ORIENTATION_PORTRAIT);

   if (!sp->ps_page)
     return EPS_PAGE_ORIENTATION_PORTRAIT;

   return eps_page_orientation_get(sp->ps_page);
}

/**
 * @brief Sets the scale of the Postscript document
 *
 * @param obj The Evas object
 * @param hscale The horizontal scale
 * @param vscale The vertical scale
 */
void
esmart_ps_scale_set (Evas_Object *obj,
                      double       hscale,
                      double       vscale)
{
   Smart_Ps *sp;

   E_SMART_OBJ_GET(sp, obj, E_OBJ_NAME);

   eps_page_scale_set(sp->ps_page, hscale, vscale);
}

/**
 * @brief Gets the scale of the Postscript document
 *
 * @param obj The Evas object
 * @param hscale The horizontal scale
 * @param vscale The vertical scale
 */
void
esmart_ps_scale_get (Evas_Object *obj,
                     double      *hscale,
                     double      *vscale)
{
   Smart_Ps *sp;

   E_SMART_OBJ_GET(sp, obj, E_OBJ_NAME);

   if (!sp)
     {
        if (hscale) *hscale = 1.0;
        if (vscale) *vscale = 1.0;
        return;
     }

   eps_page_scale_get(sp->ps_page, hscale, vscale);
}

/**
 * @brief Go to the next page
 *
 * @param obj: the smart object
 */
void
esmart_ps_page_next (Evas_Object *obj)
{
   Smart_Ps *sp;
   int       page;

   E_SMART_OBJ_GET(sp, obj, E_OBJ_NAME);

   page = eps_page_page_get (sp->ps_page);
   if (page < (eps_document_page_count_get(sp->ps_document) - 1))
     page++;
   esmart_ps_page_set (obj, page);
}

/**
 * @brief Go to the previous page
 *
 * @param obj: the smart object
 */
void
esmart_ps_page_previous (Evas_Object *obj)
{
   Smart_Ps *sp;
   int       page;

   E_SMART_OBJ_GET(sp, obj, E_OBJ_NAME);

   page = eps_page_page_get (sp->ps_page);
   if (page > 0)
     page--;
   esmart_ps_page_set (obj, page);
}

/**
 * @brief Render the current page
 *
 * @param obj: the smart object
 */
void
esmart_ps_render (Evas_Object *obj)
{
   Smart_Ps *sp;

   E_SMART_OBJ_GET (sp, obj, E_OBJ_NAME);

   if (!sp->filename) return;

   if (sp->ps_document)
     {
        if (sp->obj)
          {
             eps_page_render (sp->ps_page, sp->obj);
          }
        evas_object_show (sp->obj);
     }
}

/**
 * Return the poppler document of a smart ps object
 *
 * @param obj The Evas object
 * @return The poppler document of the ps (NULL on failure)
 *
 * Return the poppler document of the smart ps object @p obj
 *
 */
Eps_Document *
esmart_ps_ps_document_get (Evas_Object *obj)
{
   Smart_Ps *sp;

   E_SMART_OBJ_GET_RETURN(sp, obj, E_OBJ_NAME, NULL);

   return sp->ps_document;
}

/**
 * Return the current poppler page of a smart ps object
 *
 * @param obj The Evas object
 * @return The current poppler page of the ps (NULL on failure)
 *
 * Return the current poppler page of the smart ps object @p obj
 *
 */
Eps_Page *
esmart_ps_ps_page_get (Evas_Object *obj)
{
   Smart_Ps *sp;

   E_SMART_OBJ_GET_RETURN(sp, obj, E_OBJ_NAME, NULL);

   return sp->ps_page;
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
       NULL,
       NULL,
       NULL
     };
   smart = evas_smart_class_new(&sc);
}

static void
_smart_add (Evas_Object *obj)
{
   Smart_Ps *sp;

   sp = calloc (1, sizeof (Smart_Ps));
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
   Smart_Ps *sp;

   sp = evas_object_smart_data_get (obj);
   if (!sp) return;

   if (sp->filename)
     free (sp->filename);

   if (sp->ps_document)
     eps_document_delete (sp->ps_document);
   if (sp->ps_page)
     eps_page_delete (sp->ps_page);

   free (sp);
}

static void
_smart_move (Evas_Object *obj,
             Evas_Coord   x,
             Evas_Coord   y)
{
   Smart_Ps *sp;

   sp = evas_object_smart_data_get (obj);
   if (!sp) return;

   evas_object_move (sp->obj, x, y);
}

static void
_smart_resize (Evas_Object *obj,
               Evas_Coord   w,
               Evas_Coord   h)
{
   Smart_Ps *sp;

   sp = evas_object_smart_data_get (obj);
   if (!sp) return;

   evas_object_image_fill_set (sp->obj, 0, 0, w, h);
   evas_object_resize (sp->obj, w, h);
}

static void
_smart_show (Evas_Object *obj)
{
   Smart_Ps *sp;

   sp = evas_object_smart_data_get (obj);
   if (!sp) return;

   evas_object_show (sp->obj);

}

static void
_smart_hide (Evas_Object *obj)
{
   Smart_Ps *sp;

   sp = evas_object_smart_data_get (obj);
   if (!sp) return;

   evas_object_hide (sp->obj);
}

static void
_smart_color_set (Evas_Object *obj,
                  int          r,
                  int          g,
                  int          b,
                  int          a)
{
   Smart_Ps *sp;

   sp = evas_object_smart_data_get (obj);
   if (!sp) return;

   evas_object_color_set (sp->obj, r, g, b, a);
}

static void
_smart_clip_set (Evas_Object *obj,
                 Evas_Object *clip)
{
   Smart_Ps *sp;

   sp = evas_object_smart_data_get (obj);
   if (!sp) return;

   evas_object_clip_set (sp->obj, clip);
}

static void
_smart_clip_unset (Evas_Object *obj)
{
   Smart_Ps *sp;

   sp = evas_object_smart_data_get (obj);
   if (!sp) return;

   evas_object_clip_unset (sp->obj);
}
