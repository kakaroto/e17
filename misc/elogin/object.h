#ifndef __ELOGIN_OBJECT_H__
#define __ELOGIN_OBJECT_H__

#include "ui.h"

#ifdef __cplusplus
extern "C"
{
#endif				/* __cplusplus */

#define ELOGIN_OBJECT(obj) obj->o

  typedef struct _elogin_object Elogin_Object;

  struct _elogin_object
  {
    int x, y;
    int dx, dy;

    int h, w;

    Evas_Object clip;
  };


/* funcs */

  Elogin_Object *elogin_object_new (void);
  void elogin_object_set_size (Elogin_Object * object, int h, int w);
  void elogin_object_get_size (Elogin_Object * object, int *h, int *w);
  void elogin_object_set_pos (Elogin_Object * object, int x, int y);
  void elogin_object_get_pos (Elogin_Object * object, int *x, int *y);
  void elogin_object_show (Elogin_Object * object);
  void elogin_object_hide (Elogin_Object * object);
  void elogin_object_free (Elogin_Object * object);

#ifdef __cplusplus
}
#endif				/* __cplusplus */


#endif				/* __ELOGIN_OBJECT_H__ */
