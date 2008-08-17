/*
	Audio File Library
	Copyright (C) 1998, Michael Pruett <michael@68k.org>

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Library General Public
	License as published by the Free Software Foundation; either
	version 2 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Library General Public License for more details.

	You should have received a copy of the GNU Library General Public
	License along with this library; if not, write to the 
	Free Software Foundation, Inc., 59 Temple Place - Suite 330, 
	Boston, MA  02111-1307  USA.
*/

/*
	aupv.c

	This file contains an implementation of SGI's Audio Library parameter
	value list functions.
*/

#include <sys/types.h>
#include <stdlib.h>
#include <assert.h>

#include "aupvinternal.h"
#include "aupvlist.h"

AUpvlist AUpvnew (int maxitems)
{
	AUpvlist	aupvlist;
	int			i;

	if (maxitems <= 0)
		return AU_NULL_PVLIST;

	aupvlist = (AUpvlist) malloc(sizeof (struct _AUpvlist));
	assert(aupvlist);
	if (aupvlist == NULL)
		return AU_NULL_PVLIST;

	aupvlist->count = maxitems;
	aupvlist->items = malloc(sizeof (struct _AUpvitem) * maxitems);

	assert(aupvlist->items);
	if (aupvlist->items == NULL)
	{
		free(aupvlist);
		return AU_NULL_PVLIST;
	}

	for (i=0; i<maxitems; i++)
	{
		aupvlist->items[i].type = AU_PVTYPE_LONG;
		aupvlist->items[i].parameter = 0;
		memset(&aupvlist->items[i].value, 0, sizeof (aupvlist->items[i].value));
	}

	return aupvlist;
}

int AUpvgetmaxitems (AUpvlist list)
{
	assert(list);

	return list->count;
}

int AUpvfree (AUpvlist list)
{
	size_t	size;

	assert(list);
	assert(list->items);

	size = list->count;

	free(list->items);
	free(list);

	return 0;
}

int AUpvsetparam (AUpvlist list, int item, int param)
{
	assert(list);
	assert(list->items);
	assert(item >= 0);
	assert(item < list->count);

	list->items[item].parameter = param;
	return 0;
}

int AUpvsetvaltype (AUpvlist list, int item, int type)
{
	assert(list);
	assert(list->items);
	assert(item >= 0);
	assert(item < list->count);

	list->items[item].type = type;
	return 0;
}

int AUpvsetval (AUpvlist list, int item, void *val)
{
	assert(list);
	assert(list->items);
	assert(item >= 0);
	assert(item < list->count);

	switch (list->items[item].type)
	{
		case AU_PVTYPE_LONG:
			list->items[item].value.l = *((long *) val);
			break;
		case AU_PVTYPE_DOUBLE:
			list->items[item].value.d = *((double *) val);
			break;
		case AU_PVTYPE_PTR:
			list->items[item].value.v = *((void **) val);
			break;
	}

	return 0;
}

int AUpvgetparam (AUpvlist list, int item, int *param)
{
	assert(list);
	assert(list->items);
	assert(item >= 0);
	assert(item < list->count);

	*param = list->items[item].parameter;
	return 0;
}

int AUpvgetvaltype (AUpvlist list, int item, int *type)
{
	assert(list);
	assert(list->items);
	assert(item >= 0);
	assert(item < list->count);

	*type = list->items[item].type;
	return 0;
}

int AUpvgetval (AUpvlist list, int item, void *val)
{
	assert(list);
	assert(list->items);
	assert(item >= 0);
	assert(item < list->count);

	switch (list->items[item].type)
	{
		case AU_PVTYPE_LONG:
			*((long *) val) = list->items[item].value.l;
			break;
		case AU_PVTYPE_DOUBLE:
			*((double *) val) = list->items[item].value.d;
			break;
		case AU_PVTYPE_PTR:
			*((void **) val) = list->items[item].value.v;
			break;
	}

	return 0;
}
