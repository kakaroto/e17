#include "assurance.h"

static int object_check(struct ensure *, struct enobj *, void *data);

struct assurance assurance = {
   .summary = "Object offscreen",
   .description = "Object is at least partially on screen",
   .severity = ENSURE_BADFORM,
   .object = object_check
};

static int
object_check(struct ensure *en, struct enobj *obj, void *data __UNUSED__)
{
   struct enwin *enwin;
   assert(obj);

   assert(en);
   assert(obj->enwin);
   assert(obj->enwin->w && obj->enwin->h);
   enwin = obj->enwin;

   if (obj->x >= enwin->w)
     {
        ensure_bug(obj, ENSURE_BADFORM, assurance.summary, "Object offscreen (obj.x (%d) > W (%d))", obj->x, enwin->w);
     }
   else if (obj->y >= enwin->h)
     {
        ensure_bug(obj, ENSURE_BADFORM, assurance.summary, "Object offscreen (obj.y (%d) > h (%d))", obj->y, enwin->h);
     }
   else if (obj->x + obj->w < 0)
     {
        ensure_bug(obj, ENSURE_BADFORM, assurance.summary, "Object offscreen (obj.x (%d) + obj.w (%d) <0)", obj->x, obj->h);
     }
   else if (obj->y + obj->h < 0)
     {
        ensure_bug(obj, ENSURE_BADFORM, assurance.summary, "Object offscreen (obj.y (%d) + obj.h (%d) <0)", obj->y, obj->h);
     }
   else
     {
        /* All good */
        return 0;
     }
   return 1;
}

