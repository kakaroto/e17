#include <stdlib.h>

#include <libdvi29.h>

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

  DVI_PROPERTY_SET(property->dvi_property, DVI_PROP_ASYNC_GS_INVOCATION);

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
edvi_property_delayed_font_open_set (Edvi_Property *property,
                                     int            delayed_font_open)
{
  if (!property)
    return;

  if (delayed_font_open)
    DVI_PROPERTY_SET(property->dvi_property,
                     DVI_PROP_DELAYED_FONT_OPEN);
  else
    DVI_PROPERTY_UNSET(property->dvi_property,
                       DVI_PROP_DELAYED_FONT_OPEN);
}

int
edvi_property_delayed_font_open_get (Edvi_Property *property)
{
  if (!property)
    return 0;

  return DVI_PROPERTY_TEST (property->dvi_property, DVI_PROP_DELAYED_FONT_OPEN);
}
