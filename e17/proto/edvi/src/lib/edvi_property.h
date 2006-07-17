#ifndef __EDVI_PROPERTY_H__
#define __EDVI_PROPERTY_H__


#include "edvi_forward.h"


Edvi_Property *edvi_property_new ();

void           edvi_property_delete (Edvi_Property *property);

void           edvi_property_delayed_font_open_set (Edvi_Property *property,
                                                    int            delayed_font_open);

int            edvi_property_delayed_font_open_get (Edvi_Property *property);

#endif /* __EDVI_PROPERTY_H__ */
