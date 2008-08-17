/*
	Audio File Library
	Copyright (C) 1998-1999, Michael Pruett <michael@68k.org>

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
	marker.c

	This file contains routines for dealing with loop markers.
*/

#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "audiofile.h"
#include "afinternal.h"

static struct _Marker *findMarkerByID (int id, struct _Marker *markers,
	int markerCount);

static struct _Marker *findMarkerByID (int id, struct _Marker *markers,
	int markerCount)
{
	int	i;

	assert(markers);
	assert(markerCount > 0);

	for (i=0; i<markerCount; i++)
	{
		if (markers[i].id == id)
			return &markers[i];
	}

	return NULL;
}

void afInitMarkIDs(AFfilesetup setup, int trackid, int markids[], int nmarks)
{
	int	i;

	assert(setup != NULL);
	assert(trackid == AF_DEFAULT_TRACK);

	setup->markerCount = nmarks;
	if (setup->markers != NULL)
		free(setup->markers);

	setup->markers = malloc(sizeof (struct _Marker) * nmarks);

	for (i=0; i<nmarks; i++)
	{
		setup->markers[i].id = markids[i];
	}
}


void afInitMarkName(AFfilesetup setup, int trackid, int markid,
	const char *namestr)
{
	struct _Marker	*marker;

	assert(setup != NULL);
	assert(trackid == AF_DEFAULT_TRACK);
	assert(markid > 0);

	marker = findMarkerByID(markid, setup->markers, setup->markerCount);

	if (marker == NULL)
	{
		_af_error(AF_BAD_MARKID);
		return;
	}

	assert(marker->name);

	if (marker->name)
		free(marker->name);
	marker->name = strdup(namestr);
}

#if 0
void afInitMarkComment(AFfilesetup setup, int trackid, int markid,
	const char *commstr)
{
	assert(setup != NULL);
	assert(trackid == AF_DEFAULT_TRACK);
	assert(markid > 0);
}
#endif

char *afGetMarkName (AFfilehandle file, int trackid, int markid)
{
	struct _Marker	*marker;

	assert(file != NULL);
	assert(trackid == AF_DEFAULT_TRACK);
	assert(markid > 0);

	marker = findMarkerByID(markid, file->markers, file->markerCount);

	if (marker == NULL)
	{
		_af_error(AF_BAD_MARKID);
		return NULL;
	}

	return marker->name;
}

#if 0
char *afGetMarkComment (AFfilehandle file, int trackid, int markid)
{
	assert(file != NULL);
	assert(trackid == AF_DEFAULT_TRACK);
	assert(markid > 0);

	return NULL;
}
#endif

void afSetMarkPosition (AFfilehandle file, int trackid, int markid,
	AFframecount pos)
{
	struct _Marker	*marker;

	assert(file);
	assert(trackid == AF_DEFAULT_TRACK);

	marker = findMarkerByID(markid, file->markers, file->markerCount);
	if (marker == NULL)
	{
		_af_error(AF_BAD_MARKID);
		return;
	}

	marker->position = pos;
}

int afGetMarkIDs (AFfilehandle file, int trackid, int markids[])
{
	assert(file != NULL);
	assert(trackid == AF_DEFAULT_TRACK);

	if (markids != NULL)
	{
		int	i;

		for (i=0; i<file->markerCount; i++)
		{
			markids[i] = file->markers[i].id;
		}
	}

	return file->markerCount;
}

AFframecount afGetMarkPosition (AFfilehandle file, int trackid, int markid)
{
	struct _Marker	*marker;

	assert(file != NULL);
	assert(trackid == AF_DEFAULT_TRACK);
	assert(markid > 0);

	marker = findMarkerByID(markid, file->markers, file->markerCount);

	if (marker == NULL)
	{
		_af_error(AF_BAD_MARKID);
		return -1;
	}

	return marker->position;
}
