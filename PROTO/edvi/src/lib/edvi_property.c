#include <stdlib.h>

#include "config.h"

#ifdef HAVE_DVILIB_2_9
#  include <libdvi29.h>
#else
#  include <dvi-2_6.h>
#endif /* HAVE_DVILIB_2_9 */

#include "edvi_enum.h"
#include "edvi_forward.h"
#include "edvi_private.h"


Edvi_Property *
edvi_property_new ()
{
  Edvi_Property *property;

  property = (Edvi_Property *)malloc (sizeof (Edvi_Property));
  if (!property)
    return NULL;

  property->dvi_property = DVI_PROPERTY_ALLOC_DEFAULT();
  if (!property->dvi_property) {
    free (property);
    return 0;
  }

  return property;
}

void
edvi_property_delete (Edvi_Property *property)
{
  if (!property)
    return;

  DVI_PROPERTY_RELEASE (property->dvi_property);
  free (property);
  property = NULL;
}

void
edvi_property_property_set (Edvi_Property   *property,
                            Edvi_Property_Id id)
{
  if (!property)
    return;

  DVI_PROPERTY_SET(property->dvi_property, id);
}

void
edvi_property_property_unset (Edvi_Property   *property,
                              Edvi_Property_Id id)
{
  if (!property)
    return;

  DVI_PROPERTY_UNSET(property->dvi_property, id);
}

int
edvi_property_property_isset (Edvi_Property   *property,
                              Edvi_Property_Id id)
{
  if (!property)
    return 0;

  return DVI_PROPERTY_TEST (property->dvi_property, id);
}
