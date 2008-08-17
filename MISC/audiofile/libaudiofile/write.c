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
	write.c

	This file contains routines dealing with writing to audio files.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "audiofile.h"
#include "afinternal.h"

int aiffWriteFrames (const AFfilehandle file, int track, void *samples,
	const int count);
int auWriteFrames (const AFfilehandle file, int track, void *samples,
	const int count);
int waveWriteFrames (const AFfilehandle file, int track, void *samples,
	const int count);

AFfilesetup afNewFileSetup (void)
{
	AFfilesetup	setup;
	int			i;

	setup = malloc(sizeof (struct _AFfilesetup));

	setup->fileFormat = AF_FILE_AIFF;

#if WORDS_BIGENDIAN
	setup->byteOrder = AF_BYTEORDER_BIGENDIAN;
#else
	setup->byteOrder = AF_BYTEORDER_LITTLEENDIAN;
#endif

	setup->channelCount = 2;
	setup->sampleWidth = 16;
	setup->sampleFormat = AF_SAMPFMT_TWOSCOMP;
	setup->sampleRate = 44100.0;
	setup->compression = NULL;

/*
	Create two loops with id's 1 and 2.
*/

	setup->loopCount = 2;
	setup->loops = (struct _Loop *) malloc(2 * sizeof (struct _Loop));;
	for (i=0; i<2; i++)
	{
		setup->loops[i].id = i + 1;
		setup->loops[i].playMode = AF_LOOP_MODE_NOLOOP;
		setup->loops[i].beginLoop = 2*i + 1;
		setup->loops[i].endLoop = 2*i + 2;
		setup->loops[i].loopCount = 0;
	}

/*
	Create four markers with id's 1, 2, 3, and 4.
*/

	setup->markerCount = 4;
	setup->markers = (struct _Marker *) malloc(4 * sizeof (struct _Marker));
	for (i=0; i<4; i++)
	{
		setup->markers[i].id = i+1;
		setup->markers[i].position = 0;
		setup->markers[i].name = strdup("");
	}

/*
	Create one instrument.
*/

	setup->instrumentCount = 1;
	setup->instruments = (struct _Instrument *)
		malloc(1 * sizeof (struct _Instrument));

	setup->instruments[0].id = AF_DEFAULT_INST;
	setup->instruments[0].midiBaseNote = 60;
	setup->instruments[0].detune = 0;
	setup->instruments[0].midiLowNote = 0;
	setup->instruments[0].midiHighNote = 127;
	setup->instruments[0].midiLowVelocity = 1;
	setup->instruments[0].midiHighVelocity = 127;
	setup->instruments[0].gain = 0;
	setup->instruments[0].sustainLoopID = 1;
	setup->instruments[0].releaseLoopID = 2;

	setup->miscellaneousCount = 0;
	setup->miscellaneous = NULL;

	/* AES data is not present by default. */
	setup->aesDataPresent = 0;
	memset(setup->aesData, 0, 24);

	return setup;
}

void afFreeFileSetup (AFfilesetup setup)
{
	int	i;

	assert(setup);
	assert(setup->loops);
	assert(setup->markers);
	assert(setup->instruments);

	free(setup->loops);

	for (i=0; i<setup->markerCount; i++)
	{
		if (setup->markers[i].name != NULL)
			free(setup->markers[i].name);
	}

	free(setup->markers);
	free(setup->instruments);

	if (setup->miscellaneous != NULL)
		free(setup->miscellaneous);

	free(setup);
}

void afInitFileFormat (AFfilesetup setup, int filefmt)
{
	assert(setup);

	setup->fileFormat = filefmt;
}

void afInitChannels (AFfilesetup setup, int track, int channels)
{
	assert(setup);
	assert(track == AF_DEFAULT_TRACK);
	assert(channels > 0);

	setup->channelCount = channels;
}

void afInitSampleFormat (AFfilesetup setup, int track, int sampfmt, int sampwidth)
{
	assert(setup);
	setup->sampleFormat = sampfmt;
	setup->sampleWidth = sampwidth;
}

void afInitByteOrder (AFfilesetup setup, int track, int byteorder)
{
	assert(setup);
	assert(byteorder == AF_BYTEORDER_BIGENDIAN ||
		byteorder == AF_BYTEORDER_LITTLEENDIAN);

	setup->byteOrder = byteorder;
}

void afInitRate (AFfilesetup setup, int track, double rate)
{
	assert(setup);
	setup->sampleRate = rate;
}

int afWriteFrames (AFfilehandle file, int track, void *samples, const int count)
{
	assert(file);

	switch (file->fileFormat)
	{
		case AF_FILE_AIFFC:
			return aiffWriteFrames(file, track, samples, count);
			break;
		case AF_FILE_AIFF:
			return aiffWriteFrames(file, track, samples, count);
			break;
		case AF_FILE_NEXTSND:
			return auWriteFrames(file, track, samples, count);
			break;
		case AF_FILE_WAVE:
			return waveWriteFrames(file, track, samples, count);
			break;
		default:
			return -1;
			break;
	}
}

/* Enabled for raw reading only. */

#if 0

/*
	track data: data offset within the file (initialized for raw reading only)
*/
void afInitDataOffset (AFfilesetup file, int track, AFfileoffset offset)
{
	assert(file);
	assert(track == AF_DEFAULT_TRACK);

	file->dataStart = offset;
}

/*
	track data: data offset within the file (initialized for raw reading only)
*/
void afInitFrameCount (AFfilesetup file, int track, AFfileoffset offset)
{
	assert(file);
	assert(track == AF_DEFAULT_TRACK);

	file->frameCount = offset;
}

#endif
