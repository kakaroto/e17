#include "assurance.h"

static int premul_object_check(struct ensure*,struct enobj*,void *data);

struct assurance assurance = {
	.summary = "Check Premultiplied colour",
	.description = "Check each object's color is such that "
			"the alpha value is greater or equal to each "
			"other component.",
	.severity = ENSURE_BUG,
	.object = premul_object_check
};


static int
premul_object_check(struct ensure *en ensure_unused, struct enobj *obj,
		void *data ensure_unused){
	assert(obj);

	if (obj->a < obj->r || obj->a < obj->g || obj->a < obj->b){
		ensure_bug(obj,ENSURE_BUG, assurance.summary,
				"Alpha (%d) must be <= rgb values (%d,%d,%d)",
				obj->a, obj->r, obj->g, obj->b);
		return 1;
	}
	return 0;
}
