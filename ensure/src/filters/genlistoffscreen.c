

#include "filters.h"

struct ensurefilter filter = {
   .summary = "Genlist Offscreen",
   .description = "Ignore offscreen errors on objects which are " "members of an elementary genlist",
   .init = filter_init,
   .filter = bug_filter,
};

const char *offsreen;

static int
filter_init(struct ensure *en)
{
   offscreen = eina_stringshare_add("Object offscreen");
   return 0;
}

static int
filter_init(struct ensure *en)
{
   eina_stringshare_del(offscreen);
   return 0;
}

static int
bug_filter(struct ensure *en, struct bug *bug)
{
   if (bug->type != offscreen)
     return 0;

   return 1;
}

