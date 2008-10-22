#ifndef ESMART_CONTAINER_H
#define ESMART_CONTAINER_H

#include <Evas.h>

#ifdef EAPI
# undef EAPI
#endif

#ifdef _WIN32
# ifdef EFL_ESMART_CONTAINER_BUILD
#  ifdef DLL_EXPORT
#   define EAPI __declspec(dllexport)
#  else
#   define EAPI
#  endif /* ! DLL_EXPORT */
# else
#  define EAPI __declspec(dllimport)
# endif /* ! EFL_ESMART_CONTAINER_BUILD */
#else
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define EAPI __attribute__ ((visibility("default")))
#  else
#   define EAPI
#  endif
# else
#  define EAPI
# endif
#endif

/*****
 Todo:

   o add a "mover" object that tracks where moving element will go

****/
#ifdef __cplusplus
extern "C" {
#endif

enum _Container_Direction
{
  CONTAINER_DIRECTION_HORIZONTAL,  
  CONTAINER_DIRECTION_VERTICAL
};
typedef enum _Container_Direction Container_Direction;

enum _Container_Alignment
{
  CONTAINER_ALIGN_CENTER = 0,
  CONTAINER_ALIGN_LEFT = 0x01,
  CONTAINER_ALIGN_RIGHT = 0x02,
  CONTAINER_ALIGN_BOTTOM = 0x04,
  CONTAINER_ALIGN_TOP = 0x08
};
typedef enum _Container_Alignment Container_Alignment;

enum _Container_Fill_Policy
{
  CONTAINER_FILL_POLICY_NONE = 0,
  CONTAINER_FILL_POLICY_KEEP_ASPECT = 0x01,
  CONTAINER_FILL_POLICY_FILL_X = 0x02,
  CONTAINER_FILL_POLICY_FILL_Y = 0x04,
  CONTAINER_FILL_POLICY_FILL = 0x08, 
  CONTAINER_FILL_POLICY_HOMOGENOUS = 0x10
};
typedef enum _Container_Fill_Policy Container_Fill_Policy;

EAPI Evas_Object *esmart_container_new(Evas *evas);

EAPI void esmart_container_direction_set(Evas_Object *container, Container_Direction direction);
EAPI Container_Direction esmart_container_direction_get(Evas_Object *container);


EAPI void esmart_container_padding_set(Evas_Object *container, double l, double r,
                                       double t, double b);
EAPI void esmart_container_padding_get(Evas_Object *container, double *l, double *r,
                                       double *t, double *b);


EAPI void esmart_container_fill_policy_set(Evas_Object *container,
                                           Container_Fill_Policy fill);
EAPI Container_Fill_Policy  esmart_container_fill_policy_get(Evas_Object *container);


EAPI void esmart_container_alignment_set(Evas_Object *container,
                                         Container_Alignment align);
EAPI Container_Alignment esmart_container_alignment_get(Evas_Object *container);


EAPI void esmart_container_spacing_set(Evas_Object *container,
                                       int spacing);
EAPI int  esmart_container_spacing_get(Evas_Object *container);


EAPI void esmart_container_move_button_set(Evas_Object *container, int move_button);
EAPI int  esmart_container_move_button_get(Evas_Object *container);


/* element adding/removing */

/*! \brief Append a new object to the container.
 *  @param container The container object.
 *  @param element The element to add into the container.
 */
EAPI void esmart_container_element_append(Evas_Object *container, Evas_Object *element);

EAPI void esmart_container_element_prepend(Evas_Object *container, Evas_Object *element);
EAPI void esmart_container_element_append_relative(Evas_Object *container,
                                                   Evas_Object *element,
                                                   Evas_Object *relative);
EAPI void esmart_container_element_prepend_relative(Evas_Object *container,
                                                    Evas_Object *element,
                                                    Evas_Object *relative);
EAPI void esmart_container_element_remove(Evas_Object *container, Evas_Object *element);
EAPI void esmart_container_element_destroy(Evas_Object *container, Evas_Object *element);
EAPI void esmart_container_empty (Evas_Object *container);
EAPI void esmart_container_sort(Evas_Object *container, int (*func)(Evas_Object *, Evas_Object *));

EAPI Eina_List *esmart_container_elements_get(Evas_Object *container);

/* scrolling */
EAPI void esmart_container_scroll_start(Evas_Object *container, double velocity);
EAPI void esmart_container_scroll_stop(Evas_Object *container);
EAPI void esmart_container_scroll(Evas_Object *container, int val);

EAPI void   esmart_container_scroll_offset_set(Evas_Object *container, int val);
EAPI int    esmart_container_scroll_offset_get(Evas_Object *container);

/*! \brief Scroll container elements to a percent position.
 *  @param container Object
 *  @param percent The percent value 0.0 to 1.0
 */
EAPI void   esmart_container_scroll_percent_set(Evas_Object *container, double percent);

EAPI double esmart_container_scroll_percent_get(Evas_Object *container);
EAPI void   esmart_container_scroll_to(Evas_Object *container, Evas_Object *element);

/* callbacks */
EAPI void esmart_container_callback_order_change_set(Evas_Object *obj, 
                                                     void (*func)(void *data),
                                                     void *data);

EAPI double esmart_container_elements_length_get(Evas_Object *container);
EAPI double esmart_container_elements_orig_length_get(Evas_Object *container);

EAPI int esmart_container_layout_plugin_set(Evas_Object *container, const char *name);

/*! \brief Set that container should clip elements
 * @param container Object
 * @param val Boolean value: 1 to clip elements (default), 0 to not
 */
EAPI void esmart_container_clip_elements_set(Evas_Object *container, 
                                             unsigned char val);

/*! \brief Checks that container clips elements or not
 * @param container Object
 * @return Boolean value: 1 if clip elements, 0 if not
 */
EAPI unsigned char esmart_container_clip_elements_get(Evas_Object *container);

#ifdef __cplusplus
}
#endif

#endif
