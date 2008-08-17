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
	aiff.h

	This file contains structures and constants related to the AIFF
	and AIFF-C formats.
*/

#ifndef AIFF_H
#define AIFF_H

enum
{
	AIFCVersion1 = 0xa2805140
};

struct _COMM
{
	short			numChannels;
	long			numSampleFrames;
	short			sampleSize;
	unsigned char	sampleRate[10];
};

struct _MARK
{
	short			numMarkers;
	struct _Marker	*markers;
};

struct _INST
{
	char	baseNote;
	char	detune;
	char	lowNote;
	char	highNote;
	char	lowVelocity;
	char	highVelocity;
	short	gain;

	short	sustainLoopPlayMode;
	short	sustainLoopBegin;
	short	sustainLoopEnd;

	short	releaseLoopPlayMode;
	short	releaseLoopBegin;
	short	releaseLoopEnd;
};

#endif
