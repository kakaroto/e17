#include "assurance.h"

static int object_check(struct ensure*,struct enobj*,void *data);
static void *init_test(struct ensure*);

struct assurance assurance = {
	.summary = "Large text",
	.description = "Check for text more than 100 points in height.",
	.severity = ENSURE_POLICY,
	.init = init_test,
	.object = object_check
};

static const char *texttype;

static void *
init_test(struct ensure *en __UNUSED__){
	if (!texttype)
		texttype = eina_stringshare_add("text");
	return NULL;
}

static int
object_check(struct ensure *en __UNUSED__, struct enobj *obj,
		void *data __UNUSED__){
	assert(obj);

	if (obj->type != texttype) return 0;

	if (obj->data.text.size > 100){
		ensure_bug(obj, assurance.severity, assurance.summary,
				"Text size is too big: %d points,",
				obj->data.text.size);
		return 1;
	}
	return 0;
}
