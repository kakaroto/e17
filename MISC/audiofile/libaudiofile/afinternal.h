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
	afinternal.h

	This file defines the internal structures for the Audio File Library.
*/

#ifndef AFINTERNAL_H
#define AFINTERNAL_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <stdio.h>
#include "audiofile.h"
#include "af_vfs.h"
#include "compression.h"

struct _AFfilesetup
{
	int		fileFormat;
	int		channelCount;
	int		sampleWidth, sampleFormat;
	int		byteOrder;
	double	sampleRate;

	int					loopCount;
	struct _Loop		*loops;
	int					markerCount;
	struct _Marker		*markers;
	int					instrumentCount;
	struct _Instrument	*instruments;

	int						miscellaneousCount;
	struct _Miscellaneous	*miscellaneous;

	int		aesDataPresent;
	char	aesData[24];

	struct _Compression	*compression;
};

struct _Marker
{
	short			id;
	unsigned long	position;
	char			*name;
};

struct _Loop
{
	int		id;
	short	playMode;
	short	beginLoop;
	short	endLoop;
	int		loopCount;
};

struct _Instrument
{
	int		id;

	char		midiBaseNote;
	char		detune;			/* detune is expressed in cents. */
	char		midiLowNote;
	char		midiHighNote;
	char		midiLowVelocity;
	char		midiHighVelocity;
	u_int16_t	gain;			/* gain is expressed in decibels. */

	int		sustainLoopID;
	int		releaseLoopID;
};

struct _Miscellaneous
{
	int		id;
	int		type;
	int		size;

	/*
		offset is the offset within the file to the start of the data
		in the miscellaneous chunk.

		position is the working offset within the miscellaneous chunk.
	*/

	AFfileoffset	offset;
	AFfileoffset	position;
};

struct _AFfilehandle
{
        AF_VirtualFile *fh;

	int 	channelCount;
	int 	sampleWidth, sampleFormat;
	int		virtualSampleWidth, virtualSampleFormat;
	double  sampleRate;
	off_t   frameCount;
	off_t   dataStart;
	off_t   currentFrame;
	off_t   trackBytes;
	int		byteOrder, virtualByteOrder;
	int		fileFormat;

	int					markerCount;
	struct _Marker		*markers;

	int					loopCount;
	struct _Loop		*loops;

	int					instrumentCount;
	struct _Instrument	*instruments;

	int						miscellaneousCount;
	struct _Miscellaneous	*miscellaneous;

	int		aesDataPresent;
	char	aesData[24];

	struct _Compression	*compression;
};

enum
{
	AIFC_VERSION_1 = 0xa2805140
};

#include "error.h"

#endif
