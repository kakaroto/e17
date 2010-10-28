#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "Eyesight.h"
#include "eyesight_private.h"

Eyesight_Index_Item *
eyesight_index_item_new()
{
  Eyesight_Index_Item *item;

  item = (Eyesight_Index_Item *)malloc(sizeof (Eyesight_Index_Item));
  if (!item)
    return NULL;

  item->title = NULL;
  item->action = 0;
  item->children = NULL;

  return item;
}

void
eyesight_index_item_free(Eyesight_Index_Item *item)
{
  if (!item)
    return;

  if (item->title)
    free(item->title);
  if (item->children)
    {
      Eyesight_Index_Item *i;

      while (item->children)
        {
          Eyesight_Index_Item *item;

          item = (Eyesight_Index_Item *)eina_list_data_get(item->children);
          eyesight_index_item_free(item);
          item->children = eina_list_remove_list(item->children, item->children);
        }
    }

  free(item);
}

const char *
eyesight_index_item_title_get(const Eyesight_Index_Item *item)
{
  if (!item)
    return NULL;

  return item->title;
}

Eina_List *
eyesight_index_item_children_get(const Eyesight_Index_Item *item)
{
  if (!item)
    return NULL;

  return item->children;
}

Eyesight_Link_Action_Kind
eyesight_index_item_action_kind_get (const Eyesight_Index_Item *item)
{
  if (!item)
    return EYESIGHT_LINK_ACTION_UNKNOWN;

  return item->action;
}

int
eyesight_index_item_page_get (const Eyesight_Index_Item *item)
{
  if (!item)
    return 0;

  return item->page;
}
