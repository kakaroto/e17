#include "assurance.h"

static int object_check(struct ensure *, struct enobj *, void *data);

struct assurance assurance = {
   .summary = "Object within Smart",
   .description = "Object is fully within smart parent",
   .severity = ENSURE_BADFORM,
   .object = object_check
};

static int
object_check(struct ensure *en, struct enobj *obj, void *data __UNUSED__)
{
   struct enobj *parent;
   assert(obj);

   if (!obj->parent)
     return 0;

   parent = enobj_parent_get(en, obj);
   assert(parent);
   if (!parent)
     return 0;

   if (obj->x > parent->x + parent->w)
     {
        ensure_bug(obj, assurance.severity, assurance.summary, "Object out of parent (obj.x (%d) > " "parent.x + parent.w (%d + %d)", obj->x, parent->x, parent->w);
     }
   else if (obj->x < parent->x)
     {
        ensure_bug(obj, ENSURE_BADFORM, assurance.summary, "Object out of parent " "(obj.x (%d) < parent.x (%d))", obj->x, parent->x);
     }
   else if (obj->y < parent->y)
     {
        ensure_bug(obj, ENSURE_BADFORM, assurance.summary, "Object out of parent " "(obj.y (%d) < parent.y (%d))", obj->y, parent->y);
     }
   else if (obj->y > parent->y + parent->h)
     {
        ensure_bug(obj, assurance.severity, assurance.summary, "Object out of parent (obj.y (%d) > " "parent.y + parent.h (%d + %d)", obj->y, parent->y, parent->h);
     }
   else
     {
        /* All good */
        return 0;
     }
   return 1;
}

