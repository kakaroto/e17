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
	aiff.c

	This file contains routines for parsing AIFF and AIFF-C sound
	files.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <assert.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "extended.h"
#include "audiofile.h"
#include "util.h"
#include "afinternal.h"
#include "byteorder.h"

int _af_parseaiff (AFfilehandle file);
static void ParseFVER (AFfilehandle file, FILE *fp, u_int32_t type, size_t size);
static void ParseAESD (AFfilehandle file, FILE *fp, u_int32_t type, size_t size);
static void ParseMiscellaneous (AFfilehandle file, FILE *fp, u_int32_t type,
	size_t size);
static void ParseINST (AFfilehandle file, FILE *fp, u_int32_t type, size_t size);
static void ParseMARK (AFfilehandle file, FILE *fp, u_int32_t type, size_t size);
static void ParseCOMM (AFfilehandle file, FILE *fp, u_int32_t type, size_t size);
static void ParseSSND (AFfilehandle file, FILE *fp, u_int32_t type, size_t size);

/*
	FVER chunks are only present in AIFF-C files.
*/
static void ParseFVER (AFfilehandle file, FILE *fp, u_int32_t type, size_t size)
{
	u_int32_t	timestamp;

	assert(!memcmp(&type, "FVER", 4));

	fread(&timestamp, sizeof (u_int32_t), 1, fp);
	timestamp = BENDIAN_TO_HOST_INT32(timestamp);
	/* timestamp holds the number of seconds since January 1, 1904. */
}

/*
	Parse AES recording data.
*/
static void ParseAESD (AFfilehandle file, FILE *fp, u_int32_t type, size_t size)
{
	unsigned char	aesChannelStatusData[24];

	assert(!memcmp(&type, "AESD", 4));
	assert(size == 24);

	file->aesDataPresent = 1;
	fread(aesChannelStatusData, 1, 24, fp);
	memcpy(file->aesData, aesChannelStatusData, 24);
}

/*
	Parse miscellaneous data chunks such as name, author, copyright,
	and annotation chunks.
*/
static void ParseMiscellaneous (AFfilehandle file, FILE *fp, u_int32_t type, size_t size)
{
	int	misctype;

	assert(!memcmp(&type, "NAME", 4) || !memcmp(&type, "AUTH", 4) ||
		!memcmp(&type, "(c) ", 4) || !memcmp(&type, "ANNO", 4) ||
		!memcmp(&type, "APPL", 4) || !memcmp(&type, "MIDI", 4));
	assert(size >= 0);

	if (file->miscellaneousCount == 0)
	{
		assert(file->miscellaneous == NULL);
		file->miscellaneousCount++;
		file->miscellaneous = malloc(sizeof (struct _Miscellaneous));
	}
	else
	{
		file->miscellaneousCount++;
		file->miscellaneous = realloc(file->miscellaneous,
			file->miscellaneousCount * sizeof (struct _Miscellaneous));
	}

	if (!memcmp(&type, "NAME", 4))
		misctype = AF_MISC_NAME;
	else if (!memcmp(&type, "AUTH", 4))
		misctype = AF_MISC_AUTH;
	else if (!memcmp(&type, "(c) ", 4))
		misctype = AF_MISC_COPY;
	else if (!memcmp(&type, "ANNO", 4))
		misctype = AF_MISC_ANNO;
	else if (!memcmp(&type, "APPL", 4))
		misctype = AF_MISC_APPL;
	else if (!memcmp(&type, "MIDI", 4))
		misctype = AF_MISC_MIDI;

	file->miscellaneous[file->miscellaneousCount - 1].id = file->miscellaneousCount;
	file->miscellaneous[file->miscellaneousCount - 1].type = misctype;
	file->miscellaneous[file->miscellaneousCount - 1].size = size;
	/*
		ftell should probably be replaced by ftell64 on systems such
		as Irix, but until a 64-bit audio file format comes out, I'm not
		going to worry about it.
	*/
	file->miscellaneous[file->miscellaneousCount - 1].offset = ftell(fp);
	file->miscellaneous[file->miscellaneousCount - 1].position = 0;
}

/*
	Parse instrument chunks, which contain information about using
	sound data as a sampled instrument.
*/
static void ParseINST (AFfilehandle file, FILE *fp, u_int32_t type, size_t size)
{
	u_int8_t	baseNote, detune, lowNote, highNote, lowVelocity, highVelocity;
	u_int16_t	gain;

	u_int16_t	sustainLoopPlayMode, sustainLoopBegin, sustainLoopEnd;
	u_int16_t	releaseLoopPlayMode, releaseLoopBegin, releaseLoopEnd;

	assert(!memcmp(&type, "INST", 4));

	file->instrumentCount = 1;
	file->instruments =
		(struct _Instrument *) malloc(1 * sizeof (struct _Instrument));

	fread(&baseNote, sizeof (u_int8_t), 1, fp);
	fread(&detune, sizeof (u_int8_t), 1, fp);
	fread(&lowNote, sizeof (u_int8_t), 1, fp);
	fread(&highNote, sizeof (u_int8_t), 1, fp);
	fread(&lowVelocity, sizeof (u_int8_t), 1, fp);
	fread(&highVelocity, sizeof (u_int8_t), 1, fp);
	fread(&gain, sizeof (u_int16_t), 1, fp);
	gain = BENDIAN_TO_HOST_INT16(gain);

	file->instruments[0].id = AF_DEFAULT_INST;
	file->instruments[0].midiBaseNote = baseNote;
	file->instruments[0].detune = detune;
	file->instruments[0].midiLowNote = lowNote;
	file->instruments[0].midiHighNote = highNote;
	file->instruments[0].midiLowVelocity = lowVelocity;
	file->instruments[0].midiHighVelocity = highVelocity;
	file->instruments[0].gain = gain;
	file->instruments[0].sustainLoopID = 1;
	file->instruments[0].releaseLoopID = 2;

#ifdef DEBUG
	printf(" baseNote/detune/lowNote/highNote/lowVelocity/highVelocity/gain:\n"
		" %d %d %d %d %d %d %d\n",
		baseNote, detune, lowNote, highNote, lowVelocity, highVelocity, gain);
#endif

	fread(&sustainLoopPlayMode, sizeof (u_int16_t), 1, fp);
	sustainLoopPlayMode = BENDIAN_TO_HOST_INT16(sustainLoopPlayMode);
	fread(&sustainLoopBegin, sizeof (u_int16_t), 1, fp);
	sustainLoopBegin = BENDIAN_TO_HOST_INT16(sustainLoopBegin);
	fread(&sustainLoopEnd, sizeof (u_int16_t), 1, fp);
	sustainLoopEnd = BENDIAN_TO_HOST_INT16(sustainLoopEnd);

	fread(&releaseLoopPlayMode, sizeof (u_int16_t), 1, fp);
	releaseLoopPlayMode = BENDIAN_TO_HOST_INT16(releaseLoopPlayMode);
	fread(&releaseLoopBegin, sizeof (u_int16_t), 1, fp);
	releaseLoopBegin = BENDIAN_TO_HOST_INT16(releaseLoopBegin);
	fread(&releaseLoopEnd, sizeof (u_int16_t), 1, fp);
	releaseLoopEnd = BENDIAN_TO_HOST_INT16(releaseLoopEnd);

#ifdef DEBUG
	printf("sustain loop: %d %d %d\n", sustainLoopPlayMode,
		sustainLoopBegin, sustainLoopEnd);

	printf("release loop: %d %d %d\n", releaseLoopPlayMode,
		releaseLoopBegin, releaseLoopEnd);
#endif

	file->loops = malloc(2 * sizeof (struct _Loop));
	file->loopCount = 2;

	file->loops[0].id = 1;
	file->loops[0].playMode = sustainLoopPlayMode;
	file->loops[0].beginLoop = sustainLoopBegin;
	file->loops[0].endLoop = sustainLoopEnd;

	file->loops[1].id = 2;
	file->loops[1].playMode = releaseLoopPlayMode;
	file->loops[1].beginLoop = releaseLoopBegin;
	file->loops[1].endLoop = releaseLoopEnd;
}

/*
	Parse marker chunks, which contain the positions and names of loop markers.
*/
static void ParseMARK (AFfilehandle file, FILE *fp, u_int32_t type, size_t size)
{
	int			i;
	u_int16_t	numMarkers;

	assert(!memcmp(&type, "MARK", 4));

	fread(&numMarkers, sizeof (u_int16_t), 1, fp);
	numMarkers = BENDIAN_TO_HOST_INT16(numMarkers);

	file->markerCount = numMarkers;
	file->markers = malloc(numMarkers * sizeof (struct _Marker));

	for (i=0; i<numMarkers; i++)
	{
		u_int16_t		markerID;
		u_int32_t		markerPosition;
		unsigned char	sizeByte, *markerName;

		fread(&markerID, sizeof (u_int16_t), 1, fp);
		markerID = BENDIAN_TO_HOST_INT16(markerID);
		fread(&markerPosition, sizeof (u_int32_t), 1, fp);
		markerPosition = BENDIAN_TO_HOST_INT32(markerPosition);
		fread(&sizeByte, sizeof (unsigned char), 1, fp);
		markerName = malloc(sizeByte + 1);
		fread(markerName, sizeof (unsigned char), sizeByte, fp);

		markerName[sizeByte] = '\0';

#ifdef DEBUG
		printf("marker id: %d, position: %d, name: %s\n", markerID,
			markerPosition, markerName);

		printf("size byte: %d\n", sizeByte);
#endif

		if ((sizeByte % 2) == 0)
			fseek(fp, 1, SEEK_CUR);

		file->markers[i].id = markerID;
		file->markers[i].position = markerPosition;
		file->markers[i].name = markerName;
	}
}

/*
	Parse common data chunks, which contain information regarding the
	sampling rate, the number of sample frames, and the number of
	sound channels.
*/
static void ParseCOMM (AFfilehandle file, FILE *fp, u_int32_t type, size_t size)
{
	u_int16_t		numChannels;
	u_int32_t		numSampleFrames;
	u_int16_t		sampleSize;
	unsigned char	sampleRate[10];

	assert(!memcmp(&type, "COMM", 4));

	fread(&numChannels, sizeof (u_int16_t), 1, fp);
	file->channelCount = BENDIAN_TO_HOST_INT16(numChannels);

	fread(&numSampleFrames, sizeof (u_int32_t), 1, fp);
	file->frameCount = BENDIAN_TO_HOST_INT32(numSampleFrames);

	fread(&sampleSize, sizeof (u_int16_t), 1, fp);
	file->sampleWidth = BENDIAN_TO_HOST_INT16(sampleSize);

	fread(sampleRate, 10, 1, fp);
	file->sampleRate = ConvertFromIeeeExtended(sampleRate);
}

/*
	Parse the stored sound chunk, which usually contains little more
	than the sound data.
*/
static void ParseSSND (AFfilehandle file, FILE *fp, u_int32_t type, size_t size)
{
	u_int32_t	offset, blockSize;

	assert(!memcmp(&type, "SSND", 4));

	fread(&offset, sizeof (u_int32_t), 1, fp);
	offset = BENDIAN_TO_HOST_INT32(offset);
	fread(&blockSize, sizeof (u_int32_t), 1, fp);
	blockSize = BENDIAN_TO_HOST_INT32(blockSize);

	/*
		This seems like a reasonable way to calculate the number of
		bytes in an SSND chunk.
	*/
	file->trackBytes = size - 8 - offset;

#ifdef DEBUG
	printf("offset: %d\n", offset);
	printf("block size: %d\n", blockSize);
#endif

	file->dataStart = ftell(fp) + offset;

#ifdef DEBUG
	printf("data start: %d\n", file->dataStart);
#endif

	/* sound data follows */
}

int _af_parseaiff (AFfilehandle file)
{
	u_int32_t	type, size, formtype;
	char		*buffer;
	size_t		index = 0;
	int			hasCOMM = 0;

	assert(file != NULL);
	assert(file->fp != NULL);

	fread(&type, 4, 1, file->fp);
	fread(&size, 4, 1, file->fp);
	size = BENDIAN_TO_HOST_INT32(size);
	fread(&formtype, 4, 1, file->fp);

	assert(!memcmp(&type, "FORM", 4));
	assert(!memcmp(&formtype, "AIFF", 4) || !memcmp(&formtype, "AIFC", 4));

	/*
		I'm not sure if this is the proper error to throw here. It
		shouldn't ever be true because of the way file types are
		distinguished in audiofile.c, but it might be good to have this
		code anyway.
	*/

	if (memcmp(&type, "FORM", 4) ||
		(memcmp(&formtype, "AIFF", 4) && memcmp(&formtype, "AIFC", 4)))
		_af_error(AF_BAD_AIFF_HEADER);
	
#ifdef DEBUG
	printf("size: %d\n", size);
#endif

	index += 4;

	/* include the offset of the form type */
	while (index < size)
	{
		u_int32_t	chunkid, chunksize;

#ifdef DEBUG
		printf("index: %d\n", index);
#endif
		fread(&chunkid, 4, 1, file->fp);
/*		chunkid = BENDIAN_TO_HOST_INT32(chunkid); */
		fread(&chunksize, 4, 1, file->fp);
		chunksize = BENDIAN_TO_HOST_INT32(chunksize);

#ifdef DEBUG
		_af_printid(chunkid);
		printf(" size: %d\n", chunksize);
#endif

		if (!memcmp("COMM", &chunkid, 4))
		{
				hasCOMM = 1;
				ParseCOMM(file, file->fp, chunkid, chunksize);
		}
		else if (!memcmp("FVER", &chunkid, 4))
				ParseFVER(file, file->fp, chunkid, chunksize);
		else if (!memcmp("INST", &chunkid, 4))
				ParseINST(file, file->fp, chunkid, chunksize);
		else if (!memcmp("MARK", &chunkid, 4))
				ParseMARK(file, file->fp, chunkid, chunksize);
		else if (!memcmp("AESD", &chunkid, 4))
				ParseAESD(file, file->fp, chunkid, chunksize);
		else if (!memcmp("NAME", &chunkid, 4) ||
			!memcmp("AUTH", &chunkid, 4) ||
			!memcmp("(c) ", &chunkid, 4) ||
			!memcmp("ANNO", &chunkid, 4) ||
			!memcmp("APPL", &chunkid, 4) ||
			!memcmp("MIDI", &chunkid, 4))
				ParseMiscellaneous(file, file->fp, chunkid, chunksize);
		/*
			The sound data chunk is required if there are more than
			zero sample frames.
		*/
		else if (!memcmp("SSND", &chunkid, 4))
				ParseSSND(file, file->fp, chunkid, chunksize);

		index += chunksize + 8;

		/* all chunks must be aligned on an even number of bytes */
		if ((index % 2) != 0)
			index++;

		fseek(file->fp, index + 8, SEEK_SET);
	}

	if (!hasCOMM)
	{
		_af_error(AF_BAD_AIFF_COMM);
	}

	/* A zero return value indicates successful parsing. */
	return 0;
}
