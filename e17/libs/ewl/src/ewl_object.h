
#ifndef __EWL_OBJECT_H__
#define __EWL_OBJECT_H__

typedef struct _ewl_object Ewl_Object;

struct _ewl_object {
	struct {
		int x, y, w, h;
	} current, request;
	struct {
		int w, h;
	} maximum, minimum;
	int realized;
	int visible;
	int layer;
};

#define EWL_OBJECT(object) ((Ewl_Object *) object)

void ewl_object_set_minimum_size(Ewl_Object * object, int w, int h);
void ewl_object_set_maximum_size(Ewl_Object * object, int w, int h);

#endif
