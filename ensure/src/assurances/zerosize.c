#include "assurance.h"

static int object_check(struct ensure *, struct enobj *, void *data);

struct assurance assurance = {
   .summary = "Check non-zero size",
   .description = "Check each object's dimensions are greater then 0",
   .severity = ENSURE_BADFORM,
   .object = object_check
};

static int
object_check(struct ensure *en __UNUSED__, struct enobj *obj, void *data __UNUSED__)
{
   assert(obj);

   if (obj->w && obj->h)
     return 0;

   ensure_bug(obj, assurance.severity, assurance.summary, "Object has zero size");
   return 1;
}

