/**
 * This is a template for writing your own assurances.
 * See README.md for more details.
 */

#include "assurance.h"

static int   object_check(struct ensure *, struct enobj *, void *data);
static void *init_test(struct ensure *);
static int   fini_test(struct ensure *en, void *data);

struct assurance assurance = {
   .summary = "Edje Error",
   .description = "Edje Loading error",
   /* There is no reason to have a load error, it's critical */
   .severity = ENSURE_CRITICAL,
   .init = init_test,
   .fini = fini_test,
   .object = object_check
};

static const char *edjetype;

/**
 * Init function: If you need some sort of DB do it here.
 * It's safe not to use it if you don't need it.
 * Other main use is to get the stringshare for various strings.
 */
static void *
init_test(struct ensure *en __UNUSED__)
{
   if (!edjetype)
     edjetype = eina_stringshare_add("edje");
   return NULL;
}

/**
 * Cleanup or finalisation test.
 * Also can be used to add bugs built from some sort of local store.
 */
static int
fini_test(struct ensure *en __UNUSED__, void *data __UNUSED__)
{
   eina_stringshare_del(edjetype);
   edjetype = NULL;
   return 0;
}

/**
 *  Main check function: Called once for each object
 **/
static int
object_check(struct ensure *en __UNUSED__, struct enobj *obj, void *data __UNUSED__)
{
   assert(obj);

   /* Good idea to not test wrong object types */
   if (obj->type != edjetype)
     return 0;

   if (obj->data.edje.err)
     {
        ensure_bug(obj, assurance.severity, assurance.summary, "Edje load error: %s", obj->data.edje.err);
        return 1;
     }
   return 0;
}

