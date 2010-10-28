#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "Eyesight.h"
#include "eyesight_private.h"


EAPI Eyesight_Link *
_eyesight_link_new(Eyesight_Link_Action_Kind action)
{
  Eyesight_Link *link;
  link = (Eyesight_Link *)calloc(1, sizeof(Eyesight_Link));
  if (!link)
    return NULL;

  link->action = action;

  return link;
}

EAPI void
_eyesight_link_free(Eyesight_Link *link)
{
  if (!link)
    return;

  free(link);
}

EAPI Eina_Rectangle
eyesight_link_rect_get(Eyesight_Link *link)
{
  if (!link)
    {
      Eina_Rectangle rect = { 0, 0, 0, 0 };
      return rect;
    }

  return link->rect;
}

EAPI int
eyesight_link_page_get(Eyesight_Link *link)
{
  if (!link)
    return -1;

  return link->page;
}
