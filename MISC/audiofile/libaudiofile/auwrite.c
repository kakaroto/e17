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
	auwrite.c

	This file contains routines for writing NeXT/Sun format sound files.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "audiofile.h"
#include "afinternal.h"
#include "au.h"
#include "byteorder.h"
#include "block.h"
#include "util.h"

int auWriteFrames (const AFfilehandle file, int track, void *samples,
	const int count);
int auSyncFile (AFfilehandle file);

static u_int32_t auencodingtype (AFfilehandle file);
static void writeheader (AFfilehandle file);

/* A return value of zero indicates successful synchronisation. */
int auSyncFile (AFfilehandle file)
{
	writeheader(file);
	return 0;
}

int auWriteFrames (const AFfilehandle file, int track, void *samples,
	const int count)
{
	int	frameCount = 0;

	if (file->dataStart == 0)
	{
		file->dataStart = 28;
		writeheader(file);
	}

	if (file->compression != NULL)
	{
		AFframecount	(*writeFrames) (AFfilehandle, int, void *, const int);

		assert(file->compression->codec);

		writeFrames = file->compression->codec->writeFrames;
		assert(writeFrames);

		frameCount = writeFrames(file, track, samples, count);
	}
	else
		frameCount = _af_blockWriteFrames(file, track, samples, count);

	writeheader(file);

#ifdef DEBUG
	_af_printfilehandle(file);
#endif

	return frameCount;
}

#if 0
static int writeframes (AFfilehandle file, int track, void *samples, const int count)
{
	int	frameSize;

	frameSize = (file->sampleWidth / 8) * file->channelCount;

	af_fseek(file->fh, file->dataStart, SEEK_SET);
	af_fseek(file->fh, file->currentFrame * frameSize, SEEK_CUR);

	if (file->virtualByteOrder == AF_BYTEORDER_BIGENDIAN)
	{
		af_fwrite(samples, 1, frameSize * count, file->fh);
	}
	else
	{
		if (file->sampleWidth == 8)
			af_fwrite(samples, 1, frameSize * count, file->fh);
		else if (file->sampleWidth == 16)
			writeswapblock16(file->fh, samples, file->channelCount * count);
		else if (file->sampleWidth == 32)
			writeswapblock32(file->fh, samples, file->channelCount * count);
	}

	file->frameCount += count;
	file->currentFrame = file->frameCount;
}
#endif

static void writeheader (AFfilehandle file)
{
	struct _AUHeader	auheader;
	int					frameSize;

	frameSize = (file->sampleWidth + 7) / 8 * file->channelCount;

	if (file->compression != NULL &&
		file->compression->type == AF_COMPRESSION_G711_ULAW)
		frameSize = file->channelCount;

	memcpy(auheader.id, ".snd", 4);
	auheader.offset = HOST_TO_BENDIAN_INT32(file->dataStart);
	auheader.length = HOST_TO_BENDIAN_INT32(file->frameCount * frameSize);
	auheader.encoding = HOST_TO_BENDIAN_INT32(auencodingtype(file));
	auheader.sampleRate = HOST_TO_BENDIAN_INT32(file->sampleRate);
	auheader.channelCount = HOST_TO_BENDIAN_INT32(file->channelCount);

	if (af_fseek(file->fh, 0, SEEK_SET) != 0)
		_af_error(AF_BAD_LSEEK);

	af_fwrite(&auheader.id, sizeof (u_int32_t), 1, file->fh);
	af_fwrite(&auheader.offset, sizeof (u_int32_t), 1, file->fh);
	af_fwrite(&auheader.length, sizeof (u_int32_t), 1, file->fh);
	af_fwrite(&auheader.encoding, sizeof (u_int32_t), 1, file->fh);
	af_fwrite(&auheader.sampleRate, sizeof (u_int32_t), 1, file->fh);
	af_fwrite(&auheader.channelCount, sizeof (u_int32_t), 1, file->fh);
}

static u_int32_t auencodingtype (AFfilehandle file)
{
	u_int32_t	encoding = 0;

	if (file->compression != NULL &&
		file->compression->type == AF_COMPRESSION_G711_ULAW)
	{
		encoding = _AU_8BIT_G711_ULAW;
	}
	else if (file->sampleFormat == AF_SAMPFMT_DOUBLE)
		encoding = _AU_64BIT_FLOAT;
	else if (file->sampleFormat == AF_SAMPFMT_FLOAT)
		encoding = _AU_32BIT_FLOAT;
	else if (file->sampleFormat == AF_SAMPFMT_TWOSCOMP)
	{
		if (file->sampleWidth == 32)
			encoding = _AU_32BIT_PCM;
		else if (file->sampleWidth == 24)
			encoding = _AU_24BIT_PCM;
		else if (file->sampleWidth == 16)
			encoding = _AU_16BIT_PCM;
		else if (file->sampleWidth == 8)
			encoding = _AU_8BIT_PCM;
	}

	return encoding;
}
