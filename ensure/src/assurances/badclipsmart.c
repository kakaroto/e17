#include "assurance.h"

static int object_check(struct ensure*,struct enobj*,void *data);

struct assurance assurance = {
	.summary = "Clip in Smart",
	.description = "Object's clip is part of the same smart object",
	.severity = ENSURE_BADFORM,
	.object = object_check
};


static int
object_check(struct ensure *en, struct enobj *obj, void *data __UNUSED__){
	struct enobj *parent;
	struct enobj *clip, *clipp;
	assert(obj);

	if (!obj->parent) return 0;
	if (!obj->clip) return 0;

	parent = enobj_parent_get(en, obj);
	clip = enobj_clip_get(en, obj);
	if (!parent || !clip) return 0;

	clipp = enobj_parent_get(en, clip);

	if (!clipp){
		ensure_bug(obj, assurance.severity, "Not smart member",
				"Object's clip not smart member");
		return 1;
	} else if (clipp != parent){
		ensure_bug(obj, assurance.severity, "Clip and member differ",
				"Object's clip member of difference smart "
				"Obj: %"PRIuPTR"  Clip: %"PRIuPTR"",parent->id,clip->id);
		return 1;
	}

	return 0;
}
