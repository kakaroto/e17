#ifndef __EDVI_PROPERTY_H__
#define __EDVI_PROPERTY_H__


#include "edvi_forward.h"


Edvi_Property *edvi_property_new ();

void           edvi_property_delete (Edvi_Property *property);

void           edvi_property_property_set (Edvi_Property   *property,
                                           Edvi_Property_Id id);

void           edvi_property_property_unset (Edvi_Property   *property,
                                             Edvi_Property_Id id);

int            edvi_property_property_isset (Edvi_Property   *property,
                                             Edvi_Property_Id id);


#endif /* __EDVI_PROPERTY_H__ */
