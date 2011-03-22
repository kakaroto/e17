#include "assurance.h"

static int object_check(struct ensure*,struct enobj*,void *data);
static void *init_test(struct ensure*);

struct assurance assurance = {
	.summary = "Null/Empty text",
	.description = "Check for blank/empty strings.",
	.severity = ENSURE_PEDANTIC,
	.init = init_test,
	.object = object_check
};

static const char *texttype;

static void *
init_test(struct ensure *en ensure_unused){
	if (!texttype)
		texttype = eina_stringshare_add("text");
	return NULL;
}

static int
object_check(struct ensure *en ensure_unused, struct enobj *obj,
		void *data ensure_unused){
	assert(obj);

	if (obj->type != texttype) return 0;

	if (!obj->data.text.text || *obj->data.text.text == 0){
		ensure_bug(obj, ENSURE_PEDANTIC, assurance.summary,
				"Empty text string,");
		return 1;
	}
	return 0;
}
