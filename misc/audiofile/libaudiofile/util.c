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
	util.c

	This file contains general utility routines for the Audio File
	Library.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include "audiofile.h"
#include "afinternal.h"

void _af_printid (u_int32_t id)
{
	printf("%c%c%c%c",
		(id >> 24) & 0xff,
		(id >> 16) & 0xff,
		(id >> 8) & 0xff,
		id & 0xff);
}

void _af_printfilehandle (AFfilehandle filehandle)
{
	printf("channel count: %d\n", filehandle->channelCount);
	printf("sample width: %d\n", filehandle->sampleWidth);
	printf("sample rate: %f\n", filehandle->sampleRate);
	printf("frame count: %ld\n", filehandle->frameCount);
	printf("data offset: %ld\n", filehandle->dataStart);
	printf("byteOrder: %d\n", filehandle->byteOrder);
	printf("virtualByteOrder: %d\n", filehandle->virtualByteOrder);
}
