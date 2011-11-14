#include "assurance.h"

static int object_check(struct ensure*,struct enobj*,void *data);

struct assurance assurance = {
	.summary = "Check non-zero alpha",
	.description = "Check each object's alpha is greater then 0",
	.severity = ENSURE_PEDANTIC,
	.object = object_check
};


static int
object_check(struct ensure *en __UNUSED__, struct enobj *obj,
		void *data __UNUSED__){
	assert(obj);

	if (!obj->a){
		ensure_bug(obj, ENSURE_PEDANTIC, assurance.summary,
				"Object's alpha is 0");
		return 1;
	}
	return 0;
}
