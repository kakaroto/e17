/*
	Audio File Library
	Copyright (C) 1999, Michael Pruett <michael@68k.org>

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
	ulaw-codec.c

	This file contains the ulaw codec for handling CCITT G.711 ulaw
	compression.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <stdio.h>
#include <assert.h>
#include "afinternal.h"
#include "g711.h"
#include "byteorder.h"
#include "compression.h"

static AFframecount _af_g711_ulawReadFrames (AFfilehandle file, int track,
					     void *samples, int frameCount);
static AFframecount _af_g711_ulawWriteFrames (AFfilehandle file, int track,
					      void *samples, const int frameCount);

struct _codec g711_ulaw_codec =
{
	_af_g711_ulawReadFrames, _af_g711_ulawWriteFrames
};

struct _Compression g711_ulaw_compression =
{
	AF_COMPRESSION_G711_ULAW,
	"G.711 u-law",
	&g711_ulaw_codec
};

/*
	_af_blockReadFrames is used for reading uncompressed sampled sound
	data from RIFF WAVE or AIFF files.
*/
static AFframecount _af_g711_ulawReadFrames (AFfilehandle file, int track,
	void *samples, int frameCount)
{
	size_t		done = 0, sampleCount;
	u_int16_t	*buffer16 = (u_int16_t *) samples;
	int			frameSize, channelCount;

	assert(file);
	assert(track == AF_DEFAULT_TRACK);
	assert(samples);
	assert(frameCount >= 0);

	/* ulaw codec assertions. */
	assert(file->sampleWidth == 16);

	/*
		For ulaw compression, each sample is one byte, so each frame is
		channelCount bytes.
	*/
	channelCount = file->channelCount;
	frameSize = channelCount;

	assert(file->currentFrame <= file->frameCount);

	if (file->currentFrame + frameCount > file->frameCount)
		frameCount = file->frameCount - file->currentFrame;

	assert(file->currentFrame + frameCount <= file->frameCount);

	sampleCount = frameCount * channelCount;

	if (af_fseek(file->fh, file->dataStart + file->currentFrame * frameSize,
		SEEK_SET) != 0)
	{
		_af_error(AF_BAD_LSEEK);
		return 0;
	}

	if (file->sampleWidth <= 16)
	{
		u_int8_t	datum;

		while (done < sampleCount)
		{
			if (af_fread(&datum, 1, 1, file->fh) < 1)
			{
				_af_error(AF_BAD_READ);
				break;
			}
			*buffer16 = _af_ulaw2linear(datum);
			if (file->virtualByteOrder != _AF_BYTEORDER_NATIVE)
				*buffer16 = _af_byteswapint16(*buffer16);
			buffer16++;
			done++;
		}
	}

	done /= channelCount;
	file->currentFrame += done;

#ifdef DEBUG
	printf("ulaw passed: %d\n", frameCount);
	printf("ulaw done: %d\n", done);
#endif

	return done;
}

static AFframecount _af_g711_ulawWriteFrames (AFfilehandle file, int track,
					      void *samples, int frameCount)
{
	size_t		done = 0, sampleCount;
	u_int16_t	*buffer16 = (u_int16_t *) samples;
	int			frameSize, channelCount;

	assert(file);
	assert(track == AF_DEFAULT_TRACK);
	assert(samples);
	assert(frameCount >= 0);

	/* ulaw codec assertions. */
	assert(file->sampleWidth == 16);

	frameSize = file->channelCount;
	channelCount = file->channelCount;
	sampleCount = frameCount * channelCount;

	if (af_fseek(file->fh, file->dataStart + file->currentFrame * frameSize/2,
		SEEK_SET) != 0)
	{
		_af_error(AF_BAD_LSEEK);
		return 0;
	}

#ifdef DEBUG
	printf("****seek to: %d\n",
		file->dataStart + file->currentFrame * frameSize);
	_af_printfilehandle(file);
#endif

	if (file->sampleWidth <= 16)
	{
		u_int16_t	datum;

		while (done < sampleCount)
		{
			u_int8_t	ulawdatum;

			datum = *buffer16++;
			if (file->virtualByteOrder != _AF_BYTEORDER_NATIVE)
				datum = _af_byteswapint16(datum);

			ulawdatum = _af_linear2ulaw(datum);

			if (af_fwrite(&ulawdatum, 1, 1, file->fh) < 1)
			{
				_af_error(AF_BAD_WRITE);
				break;
			}
			done++;
		}
	}

	done /= channelCount;
	file->frameCount += done;
	file->currentFrame += done;
	return done;
}
