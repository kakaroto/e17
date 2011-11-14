#include "assurance.h"

static int object_check(struct ensure*,struct enobj*,void *data);
static void *init_test(struct ensure*);

struct assurance assurance = {
	.summary = "Trailing spaces",
	.description = "Check for trailing whitespace on text string.",
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
	const char *p;
	assert(obj);

	if (obj->type != texttype) return 0;

	p = obj->data.text.text + strlen(obj->data.text.text);
	p --;
	if (isspace(*p)){
		ensure_bug(obj, ENSURE_PEDANTIC, assurance.summary,
				"Trailing whitespace (%02x) on text",
				(unsigned int)*p);
		return 1;
	}
	return 0;
}
