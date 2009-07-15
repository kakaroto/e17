#include "../include/evasxx/EvasTransform.h"

namespace efl {

void EvasTransform::setIdentity()
{
  evas_transform_identity_set (&t);
}

void EvasTransform::rotate (double angle)
{
  evas_transform_rotate (angle, &t);
}

void EvasTransform::translate (float dx, float dy)
{
  evas_transform_translate (dx, dy, &t);
}

void EvasTransform::scale (float sx, float sy)
{
  evas_transform_scale (sx, sy, &t);
}

void EvasTransform::shear (float sh, float sv)
{
  evas_transform_shear (sh, sv, &t);
}

void EvasTransform::compose (EvasTransform &l, EvasTransform &t)
{
  evas_transform_compose (&l.t, &t.t);
}

} // end namespoace efl
