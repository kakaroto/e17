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
	bread.c

	This file contains block read operations.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <stdio.h>
#include <assert.h>

#include "afinternal.h"
#include "byteorder.h"

/*
	_af_blockReadFrames is used for reading uncompressed sampled sound
	data from RIFF WAVE or AIFF files.
*/

int _af_blockReadFrames (AFfilehandle file, int track, void *samples,
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
	fseek(file->fp, file->dataStart + file->currentFrame * frameSize, SEEK_SET);

	file->currentFrame += frameCount;

	if (file->sampleWidth <= 8)
	{
		u_int8_t	datum;

		switch (file->sampleFormat)
		{
			case AF_SAMPFMT_TWOSCOMP:
				while (done < sampleCount)
				{
					if (fread(&datum, 1, 1, file->fp) < 1)
					{
						_af_error(AF_BAD_READ);
						break;
					}
					*buffer8++ = datum;
					done++;
				}
				break;

			case AF_SAMPFMT_UNSIGNED:
				while (done < sampleCount)
				{
					if (fread(&datum, 1, 1, file->fp) < 1)
					{
						_af_error(AF_BAD_READ);
						break;
					}
					/* Convert datum to a signed integer. */
					datum ^= 128;
					*buffer8++ = datum;
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
					if (fread(&datum, 2, 1, file->fp) < 1)
					{
						_af_error(AF_BAD_READ);
						break;
					}
					if (file->byteOrder != file->virtualByteOrder)
						datum = _af_byteswapint16(datum);
					*buffer16++ = datum;
					done++;
				}
				break;

			case AF_SAMPFMT_UNSIGNED:
				while (done < sampleCount)
				{
					if (fread(&datum, 2, 1, file->fp) < 1)
					{
						_af_error(AF_BAD_READ);
						break;
					}
					if (file->byteOrder != file->virtualByteOrder)
						datum = _af_byteswapint16(datum);
					/* Convert datum to a signed integer. */
					datum ^= 0x8000;
					*buffer16++ = datum;
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
					if (fread(threeBytes, 3, 1, file->fp) < 1)
					{
						_af_error(AF_BAD_READ);
						break;
					}

					datum = threeBytes[2] |
						(threeBytes[1] << 8) |
						(threeBytes[0] << 16);
					if (file->byteOrder != file->virtualByteOrder)
					{
						datum = (threeBytes[2] << 24) |
							(threeBytes[1] << 16) |
							(threeBytes[0] << 8);
					}

					datum &= 0xffffff;
					if (datum & 0x800000)
					{
						datum |= 0xff000000;
					}

					*buffer32++ = datum;
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
					if (fread(&datum, 4, 1, file->fp) < 1)
					{
						_af_error(AF_BAD_READ);
						break;
					}

					if (file->byteOrder != file->virtualByteOrder)
						datum = _af_byteswapint32(datum);
					*buffer32++ = datum;
					done++;
				}
				break;

			case AF_SAMPFMT_UNSIGNED:
				while (done < sampleCount)
				{
					if (fread(&datum, 4, 1, file->fp) < 1)
					{
						_af_error(AF_BAD_READ);
						break;
					}

					if (file->byteOrder != file->virtualByteOrder)
						datum = _af_byteswapint32(datum);
					/* Convert datum to a signed integer. */
					datum ^= 0x80000000;
					*buffer32++ = datum;
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
