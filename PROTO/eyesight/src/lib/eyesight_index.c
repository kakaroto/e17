/*
 * Eyesight - EFL-based document renderer
 * Copyright (C) 2010 Vincent Torri <vtorri at univ-evry dot fr>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "Eyesight.h"
#include "eyesight_private.h"

Eyesight_Index_Item *
eyesight_index_item_new()
{
  Eyesight_Index_Item *item;

  item = (Eyesight_Index_Item *)calloc(1, sizeof (Eyesight_Index_Item));
  if (!item)
    return NULL;

  item->page = -1;

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

      EINA_LIST_FREE(item->children, i)
        eyesight_index_item_free(item);
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

const Eina_List *
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

Eina_Bool
eyesight_index_item_is_open (const Eyesight_Index_Item *item)
{
  if (!item)
    return EINA_FALSE;

  return item->is_open;
}
