#include "assurance.h"

static int object_check(struct ensure*,struct enobj*,void *data);

struct assurance assurance = {
	.summary = "Object onscreen",
	.description = "Object is at least partially on screen",
	.severity = ENSURE_BADFORM,
	.object = object_check
};


static int
object_check(struct ensure *en, struct enobj *obj, void *data ensure_unused){
	assert(obj);

	assert(en);
	assert(en->w && en->h);

	if (obj->x >= en->w){
		ensure_bug(obj, ENSURE_BADFORM,
				"Object offscreen (obj.x (%d) > W (%d))",
				obj->x,en->w);
	} else if (obj->y >= en->h){
		ensure_bug(obj, ENSURE_BADFORM,
				"Object offscreen (obj.y (%d) > h (%d))",
				obj->y,en->h);
	} else if (obj->x + obj->w < 0){
		ensure_bug(obj, ENSURE_BADFORM,
				"Object offscreen (obj.x (%d) + obj.w (%d) <0)",
				obj->x,obj->h);
	} else if (obj->y + obj->h < 0){
		ensure_bug(obj, ENSURE_BADFORM,
				"Object offscreen (obj.y (%d) + obj.h (%d) <0)",
				obj->y,obj->h);
	} else {
		/* All good */
		return 0;
	}
	return 1;
}
