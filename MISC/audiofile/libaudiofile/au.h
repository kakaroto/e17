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
	au.h

	This file contains headers and constants related to the NeXT/Sun
	.snd audio file format.
*/

#ifndef NEXTSND_H
#define NEXTSND_H

struct _AUHeader
{
	char		id[4];	/* ".snd" */
	u_int32_t	offset;
	u_int32_t	length;
	u_int32_t	encoding;
	u_int32_t	sampleRate;
	u_int32_t	channelCount;
};

/*
	encoding methods:

	1 - 8-bit ISDN u-law (G.711?)
	2 - 8-bit linear PCM (REF-PCM)
	3 - 16-bit linear PCM
	4 - 24-bit linear PCM
	5 - 32-bit linear PCM
	6 - 32-bit IEEE floating-point
	7 - 64-bit IEEE floating-point
	23 - 8-bit ISDN u-law compressed (G.721 ADPCM)
*/

enum
{
	_AU_8BIT_G711_ULAW = 1,
	_AU_8BIT_PCM = 2,
	_AU_16BIT_PCM = 3,
	_AU_24BIT_PCM = 4,
	_AU_32BIT_PCM = 5,
	_AU_32BIT_FLOAT = 6,
	_AU_64BIT_FLOAT = 7,
	_AU_8BIT_G721_ULAW_ADPCM = 23
};

#endif
