#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "../include/evasxx/Transform.h"

namespace Evasxx {

void Transform::setIdentity()
{
  evas_transform_identity_set (&t);
}

void Transform::rotate (double angle)
{
  evas_transform_rotate (angle, &t);
}

void Transform::translate (float dx, float dy)
{
  evas_transform_translate (dx, dy, &t);
}

void Transform::scale (float sx, float sy)
{
  evas_transform_scale (sx, sy, &t);
}

void Transform::shear (float sh, float sv)
{
  evas_transform_shear (sh, sv, &t);
}

void Transform::compose (Transform &l, Transform &t)
{
  evas_transform_compose (&l.t, &t.t);
}

} // end namespoace efl
