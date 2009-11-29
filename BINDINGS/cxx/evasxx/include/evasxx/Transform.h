#ifndef EVAS_TRANSFORM_H
#define EVAS_TRANSFORM_H

/* EFL */
#include <Evas.h>

namespace Evasxx {

class Transform
{
  friend class Image;
  
public:
  /* Set t to the identity */
  void setIdentity();
  
  /* Left-multiply t by an xy rotation defined by the given angle (in degrees) */
  void rotate (double angle);

  /* Left-multiply t by an xy translation defined by the given dx, dy values */
  void translate (float dx, float dy);

  /* Left-multiply t by an xy scaling defined by the given sx, sy factors */
  void scale (float sx, float sy);

  /* Left-multiply t by an xy shearing defined by the given sh, sv values */
  void shear (float sh, float sv);

  /* Left-multiply t by the given transform l */
  // TODO: const? why not?
  void compose (Transform &l, Transform &t);
  
private:
  Evas_Transform t;
};

} // end namespace Evasxx

#endif // EVAS_TRANSFORM_H
