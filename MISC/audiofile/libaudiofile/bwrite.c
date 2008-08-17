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
	bwrite.c

	This file contains block write operations.
*/

#include <sys/types.h>
#include <stdio.h>
#include <assert.h>
#include "afinternal.h"
#include "byteorder.h"

/*
	_af_blockWriteFrames is used for writing uncompressed sampled sound
	data to RIFF WAVE or AIFF files.
*/

int _af_blockWriteFrames (AFfilehandle file, int track, void *samples,
	int frameCount)
{
	size_t		done = 0, sampleCount;
	u_int8_t	*buffer8 = (u_int8_t *) samples;
	u_int16_t	*buffer16 = (u_int16_t *) samples;
	u_int32_t	*buffer32 = (u_int32_t *) samples;
	int			frameSize, channelCount;

	assert(file);
	assert(track == AF_DEFAULT_TRACK);
	assert(samples);
	assert(frameCount >= 0);

	frameSize = (file->sampleWidth + 7) / 8 * file->channelCount;

	channelCount = file->channelCount;

	sampleCount = frameCount * file->channelCount;

	if (af_fseek(file->fh, file->dataStart + file->currentFrame * frameSize,
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

	file->currentFrame += frameCount;
	file->frameCount += frameCount;

	if (file->sampleWidth <= 8)
	{
		u_int8_t	datum;

		switch (file->sampleFormat)
		{
			case AF_SAMPFMT_TWOSCOMP:
				while (done < sampleCount)
				{
					datum = *buffer8++;
					if (af_fwrite(&datum, 1, 1, file->fh) < 1)
					{
						_af_error(AF_BAD_WRITE);
						break;
					}
					done++;
				}
				break;

			case AF_SAMPFMT_UNSIGNED:
				while (done < sampleCount)
				{
					datum = *buffer8++;
					/* Convert datum to an unsigned integer. */
					datum ^= 128;
					if (af_fwrite(&datum, 1, 1, file->fh) < 1)
					{
						_af_error(AF_BAD_WRITE);
						break;
					}
					done++;
				}
				break;

			default:
				_af_error(AF_BAD_SAMPFMT);
				break;
		}
	}
	else if (file->sampleWidth <= 16)
	{
		u_int16_t	datum;

		switch (file->sampleFormat)
		{
			case AF_SAMPFMT_TWOSCOMP:
				while (done < sampleCount)
				{
					datum = *buffer16++;
					if (file->byteOrder != file->virtualByteOrder)
						datum = _af_byteswapint16(datum);
					if (af_fwrite(&datum, 2, 1, file->fh) < 1)
					{
						_af_error(AF_BAD_WRITE);
						break;
					}
					done++;
				}
				break;

			case AF_SAMPFMT_UNSIGNED:
				while (done < sampleCount)
				{
					datum = *buffer16++;
					if (file->byteOrder != file->virtualByteOrder)
						datum = _af_byteswapint16(datum);
					datum ^= 0x8000;
					if (af_fwrite(&datum, 2, 1, file->fh) < 1)
					{
						_af_error(AF_BAD_WRITE);
						break;
					}
					done++;
				}
				break;

			default:
				_af_error(AF_BAD_SAMPFMT);
				break;
		}
	}
	else if (file->sampleWidth <= 24)
	{
		u_int8_t	threeBytes[3];
		u_int32_t	datum;

		switch (file->sampleFormat)
		{
			case AF_SAMPFMT_TWOSCOMP:
				while (done < sampleCount)
				{
					datum = *buffer32++;

					threeBytes[0] = datum >> 16;
					threeBytes[1] = datum >> 8;
					threeBytes[2] = datum;

					if (file->byteOrder != file->virtualByteOrder)
					{
						threeBytes[0] = datum;
						threeBytes[1] = datum >> 8;
						threeBytes[2] = datum >> 16;
					}

					if (af_fwrite(threeBytes, 3, 1, file->fh) < 1)
					{
						_af_error(AF_BAD_WRITE);
						break;
					}

					done++;
				}
				break;

			default:
				_af_error(AF_BAD_SAMPFMT);
				break;
		}
	}
	else if (file->sampleWidth <= 32)
	{
		u_int32_t	datum;

		switch (file->sampleFormat)
		{
			case AF_SAMPFMT_TWOSCOMP:
				while (done < sampleCount)
				{
					datum = *buffer32++;
					if (file->byteOrder != file->virtualByteOrder)
						datum = _af_byteswapint32(datum);
					if (af_fwrite(&datum, 4, 1, file->fh) < 1)
					{
						_af_error(AF_BAD_WRITE);
						break;
					}
					done++;
				}
				break;

			case AF_SAMPFMT_UNSIGNED:
				while (done < sampleCount)
				{
					datum = *buffer32++;
					if (file->byteOrder != file->virtualByteOrder)
						datum = _af_byteswapint32(datum);
					/* Convert datum to a signed integer. */
					datum ^= 0x80000000;
					if (af_fwrite(&datum, 4, 1, file->fh) < 1)
					{
						_af_error(AF_BAD_WRITE);
						break;
					}
					done++;
				}
				break;

			default:
				_af_error(AF_BAD_SAMPFMT);
				break;
		}
	}

	done /= channelCount;
	return done;
}
