#include "assurance.h"

static int object_check(struct ensure*,struct enobj*,void *data);

struct assurance assurance = {
	.summary = "Check for negative size",
	.description = "Check each object's size is positive",
	.severity = ENSURE_BUG,
	.object = object_check
};


static int
object_check(struct ensure *en ensure_unused, struct enobj *obj,
		void *data ensure_unused){
	assert(obj);

	if (obj->w < 0 || obj->h < 0){
		ensure_bug(obj, ENSURE_BUG, assurance.summary,
				"Object has negative size (%dx%d)",
				obj->w,obj->h);
		return 1;
	}
	return 0;
}
