
#include <Ewl.h>

void
ewl_object_set_minimum_size(Ewl_Object * object, int w, int h)
{
	CHECK_PARAM_POINTER("object", object);

	object->minimum.w = w;
	object->minimum.h = h;
}

void
ewl_object_set_maximum_size(Ewl_Object * object, int w, int h)
{
	CHECK_PARAM_POINTER("object", object);

	object->maximum.w = w;
	object->maximum.h = h;
}
