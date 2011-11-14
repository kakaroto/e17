/**
 * Manages loading and saving reports, as well as dealing with reports
 * generated from applications.
 */
#include <stdbool.h>

#include "config.h"
#include "ensure.h"
#include "results.h"

#define TIMEFMT "%a, %d %b %Y %T %z"

static void select_results(void *resv, Evas_Object *obj, void *event);

int
results_add(struct ensure *ensure, struct result *res)
{
   char buf[100];
   struct tm tm;

   if (!res->title)
     {
        localtime_r(&res->tm, &tm);
        /* So does 'max' include the \0 ? */
        strftime(buf, sizeof(buf) - 1, TIMEFMT, &tm);
     }

   if (!res->ensure)
     res->ensure = ensure;

   if (!ensure->results)
     elm_object_disabled_set(savebutton, false);

   ensure->results = eina_list_prepend(ensure->results, res);
   ensure->cur = res;
   elm_hoversel_item_add(ensure->reportselect, res->title ? : buf, NULL, 0, select_results, res);
   elm_object_text_set(ensure->reportselect, res->title ? : buf);

   return 0;
}

/**
 * Callback when a report is selected
 */
static void
select_results(void *resv, Evas_Object *obj __UNUSED__, void *event __UNUSED__)
{
   struct result *result = resv;
   struct ensure *ensure;

   if (!result || !result->ensure)
     return;
   ensure = result->ensure;

   /* Same: Do nothing */
   if (ensure->cur == result)
     return;

   ensure->cur = result;
   /* Now need to refill tree */
   view_dirty_set(ensure);
}

