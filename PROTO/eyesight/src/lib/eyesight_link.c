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
