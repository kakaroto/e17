#include "assurance.h"

static int object_check(struct ensure *, struct enobj *, void *data);

struct assurance assurance = {
   .summary = "Object within Clip",
   .description = "Object is fully within clip",
   .severity = ENSURE_PEDANTIC,
   .object = object_check
};

static int
object_check(struct ensure *en, struct enobj *obj, void *data __UNUSED__)
{
   struct enobj *clip;
   assert(obj);

   if (!obj->clip)
     return 0;

   clip = enobj_clip_get(en, obj);
   assert(clip);
   if (!clip)
     return 0;

   if (obj->x > clip->x + clip->w)
     {
        ensure_bug(obj, assurance.severity, assurance.summary, "Object out of clip (obj.x (%d) > " "clip.x + clip.w (%d + %d)", obj->x, clip->x, clip->w);
     }
   else if (obj->x < clip->x)
     {
        ensure_bug(obj, assurance.severity, assurance.summary, "Object out of clip " "(obj.x (%d) < clip.x (%d))", obj->x, clip->x);
     }
   else if (obj->y < clip->y)
     {
        ensure_bug(obj, assurance.severity, assurance.summary, "Object out of clip " "(obj.y (%d) < clip.y (%d))", obj->y, clip->y);
     }
   else if (obj->y > clip->y + clip->h)
     {
        ensure_bug(obj, assurance.severity, assurance.summary, "Object out of clip (obj.y (%d) > " "clip.y + clip.h (%d + %d)", obj->y, clip->y, clip->h);
     }
   else
     {
        /* All good */
        return 0;
     }
   return 1;
}

