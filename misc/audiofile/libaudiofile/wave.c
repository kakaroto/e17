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
	wave.c

	This file contains code for parsing RIFF WAVE format sound files.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <assert.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "audiofile.h"
#include "util.h"
#include "afinternal.h"
#include "byteorder.h"
#include "wave.h"

static void ParseFormat (AFfilehandle filehandle, AF_VirtualFile *fp, u_int32_t id,
	size_t size);
static void ParseData (AFfilehandle filehandle, AF_VirtualFile *fp, u_int32_t id,
	size_t size);
int _af_parsewave (AFfilehandle file);

static void ParseFormat (AFfilehandle filehandle, AF_VirtualFile *fp, u_int32_t id,
	size_t size)
{
	u_int16_t	formatTag, channelCount;
	u_int32_t	sampleRate, averageBytesPerSecond;
	u_int16_t	blockAlign;

	assert(filehandle != NULL);
	assert(fp != NULL);
	assert(!memcmp(&id, "fmt ", 4));

	af_fread(&formatTag, 1, 2, fp);
	formatTag = LENDIAN_TO_HOST_INT16(formatTag);

	/* only PCM format data is supported at the present */
	assert(formatTag == WAVE_FORMAT_PCM);

	af_fread(&channelCount, 1, 2, fp);
	channelCount = LENDIAN_TO_HOST_INT16(channelCount);
	filehandle->channelCount = channelCount;

	af_fread(&sampleRate, 1, 4, fp);
	sampleRate = LENDIAN_TO_HOST_INT32(sampleRate);
	filehandle->sampleRate = sampleRate;

	af_fread(&averageBytesPerSecond, 1, 4, fp);
	averageBytesPerSecond = LENDIAN_TO_HOST_INT32(averageBytesPerSecond);

	af_fread(&blockAlign, 1, 2, fp);
	blockAlign = LENDIAN_TO_HOST_INT16(blockAlign);

	if (formatTag == WAVE_FORMAT_PCM)
	{
		u_int16_t	bitsPerSample;

		af_fread(&bitsPerSample, 1, 2, fp);
		bitsPerSample = LENDIAN_TO_HOST_INT16(bitsPerSample);

		filehandle->sampleWidth = bitsPerSample;

		assert(bitsPerSample > 0 && bitsPerSample <= 32);

#if 0
		if (bitsPerSample <= 8)
			filehandle->sampleFormat = AF_SAMPFMT_UNSIGNED;
		else
#endif
			filehandle->sampleFormat = AF_SAMPFMT_TWOSCOMP;
	}
}

static void ParseData (AFfilehandle filehandle, AF_VirtualFile *fp, u_int32_t id,
	size_t size)
{
	u_int32_t	frameSize;

	assert(filehandle != NULL);
	assert(fp != NULL);
	assert(!memcmp(&id, "data", 4));

	/* We always round up to the nearest byte for the frame size. */
	frameSize = filehandle->channelCount * ((filehandle->sampleWidth + 7) / 8);

	filehandle->dataStart = af_ftell(fp);
	filehandle->trackBytes = size;
	filehandle->frameCount = size / frameSize;
}

int _af_parsewave (AFfilehandle file)
{
	u_int32_t	type, size, formtype;
	u_int32_t	index = 0;
	int			hasFormat = 0, hasData = 0;

	assert(file != NULL);
	assert(file->fh != NULL);

	af_fread(&type, 4, 1, file->fh);
	af_fread(&size, 4, 1, file->fh);
	size = LENDIAN_TO_HOST_INT32(size);
	af_fread(&formtype, 4, 1, file->fh);

	assert(!memcmp(&type, "RIFF", 4));
	assert(!memcmp(&formtype, "WAVE", 4));
	
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
		af_fread(&chunkid, 4, 1, file->fh);

		af_fread(&chunksize, 4, 1, file->fh);
		chunksize = LENDIAN_TO_HOST_INT32(chunksize);

#ifdef DEBUG
		_af_printid(BENDIAN_TO_HOST_INT32(chunkid));
		printf(" size: %d\n", chunksize);
#endif

		if (memcmp(&chunkid, "data", 4) == 0)
		{
			ParseData(file, file->fh, chunkid, chunksize);
			hasData = 1;
		}
		else if (memcmp(&chunkid, "fmt ", 4) == 0)
		{
			ParseFormat(file, file->fh, chunkid, chunksize);
			hasFormat = 1;
		}

		index += chunksize + 8;

		/* all chunks must be aligned on an even number of bytes */
		if ((index % 2) != 0)
			index++;

		af_fseek(file->fh, index + 8, SEEK_SET);
	}

	/* The data chunk and the format chunk are required. */
	assert(hasFormat && hasData);

	/* A return value of zero indicates successful parsing. */
	return 0;
}
