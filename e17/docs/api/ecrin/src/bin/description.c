#include <stdlib.h>
#include <string.h>

#include "description.h"



/* 
 * Description of an element
 */

Ecrin_Description *
ecrin_description_new (char *brief,
		       char *detailed)
{
  Ecrin_Description *d;

  d = (Ecrin_Description *)malloc (sizeof (Ecrin_Description));
  if (!d)
    return NULL;

  if (brief)
    d->brief = strdup (brief);
  else
    d->brief = NULL;

  if (detailed)
    d->detailed = strdup (detailed);
  else
    d->detailed = NULL;

  return d;
}

void
ecrin_description_free (Ecrin_Description *d)
{
  if (!d)
    return;

  if (d->brief)
    free (d->brief);

  if (d->detailed)
    free (d->detailed);

  free (d);
}
