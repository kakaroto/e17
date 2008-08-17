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
	loop.c

	This file contains routines dealing with the Audio File Library's
	internal loop data structures.
*/

#include <stdlib.h>
#include <assert.h>

#include "audiofile.h"
#include "afinternal.h"

static struct _Loop *initLoop (struct _Loop *loop);
static struct _Loop *findLoopByID (int id, struct _Loop *loops, int count);

static struct _Loop *initLoop (struct _Loop *loop)
{
	assert(loop);

	loop->beginLoop = 1;
	loop->endLoop = 2;

	loop->loopCount = 0;
	loop->playMode = AF_LOOP_MODE_NOLOOP;

	return loop;
}

static struct _Loop *findLoopByID (int id, struct _Loop *loops, int count)
{
	int	i;

	assert(loops);
	assert(count > 0);

	for (i=0; i<count; i++)
	{
		if (loops[i].id == id)
			break;
	}

	if (i == count)
		return NULL;
	else
		return &loops[i];
}

void afInitLoopIDs (AFfilesetup setup, int instid, int *ids, int nids)
{
	int	i;

	assert(setup);
	assert(instid == AF_DEFAULT_INST);
	assert(nids>=0);
	assert(ids);

	if (nids != setup->loopCount)
	{
		setup->loopCount = nids;
		setup->loops = realloc(setup->loops,
			setup->loopCount * sizeof (struct _Loop));
	}

	for (i=0; i<nids; i++)
	{
		initLoop(&setup->loops[i]);
		setup->loops[i].id = ids[i];
	}
}

int afGetLoopIDs (AFfilehandle file, int instid, int *ids)
{
	assert(file != NULL);
	assert(instid == AF_DEFAULT_INST);

	if (file->loops == NULL)
		return 0;

	if (ids != NULL)
	{
		int i;

		for (i=0; i<file->loopCount; i++)
			ids[i] = file->loops[i].id;
	}

	return file->loopCount;
}

void afSetLoopMode (AFfilehandle file, int instid, int loopid, int mode)
{
	struct _Loop	*loop;

	assert(file != NULL);
	assert(instid == AF_DEFAULT_INST);

	loop = findLoopByID(loopid, file->loops, file->loopCount);
	if (loop == NULL)
	{
		_af_error(AF_BAD_LOOPID);
		return;
	}

	loop->playMode = mode;
}

int afGetLoopMode (AFfilehandle file, int instid, int loopid)
{
	struct _Loop	*loop;

	assert(file != NULL);
	assert(instid == AF_DEFAULT_INST);

	loop = findLoopByID(loopid, file->loops, file->loopCount);
	if (loop == NULL)
	{
		_af_error(AF_BAD_LOOPID);
		return -1;
	}

	return loop->playMode;
}

int afSetLoopCount (AFfilehandle file, int instid, int loopid, int count)
{
	struct _Loop	*loop;

	assert(file != NULL);
	assert(instid == AF_DEFAULT_INST);

	loop = findLoopByID(loopid, file->loops, file->loopCount);
	if (loop == NULL)
	{
		_af_error(AF_BAD_LOOPID);
		return -1;
	}

	loop->loopCount = count;
	return 0;
}

int afGetLoopCount (AFfilehandle file, int instid, int loopid)
{
	struct _Loop	*loop;

	assert(file != NULL);
	assert(instid == AF_DEFAULT_INST);

	loop = findLoopByID(loopid, file->loops, file->loopCount);

	if (loop == NULL)
		return -1;

	return loop->loopCount;
}

void afSetLoopStart (AFfilehandle file, int instid, int loopid, int markerid)
{
	struct _Loop	*loop;

	assert(file != NULL);
	assert(instid == AF_DEFAULT_INST);

	loop = findLoopByID(loopid, file->loops, file->loopCount);

	if (loop == NULL)
		return;

	loop->beginLoop = markerid;
}

int afGetLoopStart (AFfilehandle file, int instid, int loopid)
{
	struct _Loop	*loop;

	assert(file != NULL);
	assert(instid == AF_DEFAULT_INST);

	loop = findLoopByID(loopid, file->loops, file->loopCount);
	/* a loop matching loopid was not found */
	if (loop == NULL)
		return -1;

	return loop->beginLoop;
}

void afSetLoopEnd (AFfilehandle file, int instid, int loopid, int markerid)
{
	struct _Loop	*loop;

	assert(file != NULL);
	assert(instid == AF_DEFAULT_INST);

	loop = findLoopByID(loopid, file->loops, file->loopCount);

	if (loop == NULL)
		return;

	loop->endLoop = markerid;
}

int afGetLoopEnd (AFfilehandle file, int instid, int loopid)
{
	struct _Loop	*loop;

	assert(file != NULL);
	assert(instid == AF_DEFAULT_INST);

	loop = findLoopByID(loopid, file->loops, file->loopCount);
	/* a loop matching loopid was not found */
	if (loop == NULL)
		return -1;

	return loop->endLoop;
}

int afSetLoopStartFrame (AFfilehandle file, int instid, int loopid, AFframecount startFrame)
{
	int	startMarker, trackid;

	assert(file);
	assert(instid == AF_DEFAULT_INST);

	startMarker = afGetLoopStart(file, instid, loopid);
	/* Marker ids must be positive integers. */
	if (startMarker <= 0)
	{
		return -1;
	}

	trackid = afGetLoopTrack(file, instid, loopid);

	afSetMarkPosition(file, trackid, startMarker, startFrame);

	return 0;
}

AFframecount afGetLoopStartFrame (AFfilehandle file, int instid, int loopid)
{
	int	startMarker, trackid;

	startMarker = afGetLoopStart(file, instid, loopid);
	trackid = afGetLoopTrack(file, instid, loopid);

	if (startMarker != -1)
		return afGetMarkPosition(file, trackid, startMarker);

	return -1;
}

int afSetLoopEndFrame (AFfilehandle file, int instid, int loopid, AFframecount endFrame)
{
	int	endMarker, trackid;

	assert(file);
	assert(instid == AF_DEFAULT_INST);

	endMarker = afGetLoopEnd(file, instid, loopid);
	/* Marker ids must be positive integers. */
	if (endMarker <= 0)
	{
		return -1;
	}

	trackid = afGetLoopTrack(file, instid, loopid);

	afSetMarkPosition(file, trackid, endMarker, endFrame);

	return 0;
}

AFframecount afGetLoopEndFrame (AFfilehandle file, int instid, int loopid)
{
	int	startMarker, trackid;

	startMarker = afGetLoopEnd(file, instid, loopid);
	trackid = afGetLoopTrack(file, instid, loopid);

	if (startMarker != -1)
		return afGetMarkPosition(file, trackid, startMarker);

	return -1;
}

void afSetLoopTrack (AFfilehandle file, int instid, int loopid, int trackid)
{
	struct _Loop	*loop;

	assert(file != NULL);
	assert(instid == AF_DEFAULT_INST);
	assert(trackid == AF_DEFAULT_TRACK);

	loop = findLoopByID(loopid, file->loops, file->loopCount);
	if (loop == NULL)
	{
		_af_error(AF_BAD_LOOPID);
	}
}

int afGetLoopTrack (AFfilehandle file, int instid, int loopid)
{
	struct _Loop		*loop;

	assert(file);
	assert(instid == AF_DEFAULT_INST);

	loop = findLoopByID(loopid, file->loops, file->loopCount);

	if (loop == NULL)
	{
		_af_error(AF_BAD_LOOPID);
		return -1;
	}

	return AF_DEFAULT_TRACK;
}
