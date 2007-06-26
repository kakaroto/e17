#ifndef ESMART_RESIZE_H
#define ESMART_RESIZE_H

#ifdef EAPI
#undef EAPI
#endif
#ifdef WIN32
# ifdef BUILDING_DLL
#  define EAPI __declspec(dllexport)
# else
#  define EAPI __declspec(dllimport)
# endif
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

#include<Evas.h>
#include<Ecore_Evas.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * How to use Esmart Resize.
 */
typedef struct _Esmart_Resize Esmart_Resize;

struct _Esmart_Resize
{
  Ecore_Evas *ee;		/* The ecore_evas that should resize */
  int dx, dy;			/* offset from 0,0 of the window */
  int w, h;			/* The size of the window at the begining*/
  int x, y;			/* The position of the window at the 
				   beginning */
  int resize_x, resize_y;       /* the resize direction */
  int clicked;			/* Whether the mouse is down now or not */
  int button;			/* the button that handles dragging */
  Evas_Object *obj;		/* the rectangle that gets events */
};

typedef enum {
	ESMART_RESIZE_LEFT = 0,
	ESMART_RESIZE_RIGHT,
	ESMART_RESIZE_TOP,
	ESMART_RESIZE_BOTTOM,
	ESMART_RESIZE_LEFT_TOP,
	ESMART_RESIZE_RIGHT_TOP,
	ESMART_RESIZE_LEFT_BOTTOM,
	ESMART_RESIZE_RIGHT_BOTTOM
} Esmart_Resize_Type;

EAPI Evas_Object *esmart_resize_new (Ecore_Evas * evas);
EAPI void esmart_resize_button_set (Evas_Object * o, int button);
EAPI void esmart_resize_event_callback_add (Evas_Object * o,
					 Evas_Callback_Type type,
					 void (*func) (void *data, Evas * e,
						       Evas_Object * o,
						       void *ev),
					 const void *data);
EAPI void esmart_resize_type_set(Evas_Object * o, Esmart_Resize_Type type);

#ifdef __cplusplus
}
#endif
#endif
