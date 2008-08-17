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
	au.c

	This file contains routines for parsing NeXT/Sun .snd format sound
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

#include "au.h"
#include "audiofile.h"
#include "afinternal.h"
#include "util.h"
#include "byteorder.h"

extern struct _Compression g711_ulaw_compression;

int _af_parseau (AFfilehandle file)
{
	u_int32_t	id, offset, length, encoding, sampleRate, channelCount;

	assert(file != NULL);
	assert(file->fh != NULL);

	af_fread(&id, 4, 1, file->fh);
	assert(!memcmp(&id, ".snd", 4));

	af_fread(&offset, 4, 1, file->fh);
	af_fread(&length, 4, 1, file->fh);
	af_fread(&encoding, 4, 1, file->fh);
	af_fread(&sampleRate, 4, 1, file->fh);
	af_fread(&channelCount, 4, 1, file->fh);

	offset = BENDIAN_TO_HOST_INT32(offset);
	length = BENDIAN_TO_HOST_INT32(length);
	encoding = BENDIAN_TO_HOST_INT32(encoding);
	sampleRate = BENDIAN_TO_HOST_INT32(sampleRate);
	channelCount = BENDIAN_TO_HOST_INT32(channelCount);

#ifdef DEBUG
	printf("id, offset, length, encoding, sampleRate, channelCount:\n"
		" %d %d %d %d %d %d\n",
		id, offset, length, encoding, sampleRate, channelCount);
#endif

	file->dataStart = offset;
	file->trackBytes = length;

	assert(file->compression == NULL);

	switch (encoding)
	{
		case _AU_8BIT_G711_ULAW:
			file->sampleWidth = 16;
			file->sampleFormat = AF_SAMPFMT_TWOSCOMP;
			file->compression = &g711_ulaw_compression;
			break;
		case _AU_8BIT_PCM:
			file->sampleWidth = 8;
			file->sampleWidth = AF_SAMPFMT_TWOSCOMP;	/* maybe unsigned? */
			break;
		case _AU_16BIT_PCM:
			file->sampleWidth = 16;
			file->sampleFormat = AF_SAMPFMT_TWOSCOMP;
			break;
		case _AU_24BIT_PCM:
			file->sampleWidth = 24;
			file->sampleFormat = AF_SAMPFMT_TWOSCOMP;
			break;
		case _AU_32BIT_PCM:
			file->sampleWidth = 32;
			file->sampleFormat = AF_SAMPFMT_TWOSCOMP;
			break;
		case _AU_32BIT_FLOAT:
			file->sampleWidth = 32;
			file->sampleFormat = AF_SAMPFMT_FLOAT;
			break;
		case _AU_64BIT_FLOAT:
			file->sampleWidth = 64;
			file->sampleFormat = AF_SAMPFMT_DOUBLE;
			break;

/*
		case _AU_8BIT_G721_ULAW_ADPCM:
			file->sampleWidth = 16;
			file->sampleFormat = AF_SAMPFMT_TWOSCOMP;
			file->compression = &g721_ulaw_compression;
			break;
*/

		default:
			assert(0);
			/*
				This encoding method is not recognized or supported.
				An appropriate error code should be returned.
			*/
			break;
	}

	file->sampleRate = sampleRate;
	file->channelCount = channelCount;
	file->frameCount = length /
		((file->sampleWidth + 7) / 8 * file->channelCount);

	if (file->compression != NULL &&
		file->compression->type == AF_COMPRESSION_G711_ULAW)
	{
		assert(file->channelCount > 0);
		file->frameCount = length / file->channelCount;
	}

#ifdef DEBUG
	_af_printfilehandle(file);
#endif

	/* A return value of zero indicates successful parsing. */
	return 0;
}
