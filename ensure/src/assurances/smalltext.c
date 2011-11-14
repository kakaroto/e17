#include "assurance.h"

static int object_check(struct ensure*,struct enobj*,void *data);
static void *init_test(struct ensure*);

struct assurance assurance = {
	.summary = "Small text",
	.description = "Check for text less then 5 points in height.",
	.severity = ENSURE_PEDANTIC,
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

	if (obj->data.text.size < 6){
		ensure_bug(obj, ENSURE_PEDANTIC, assurance.summary,
				"Text size is too small: %d points,",
				obj->data.text.size);
		return 1;
	}
	return 0;
}
