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
	aes.c

	This file contains routines for dealing with AES recording data.
*/

#include <string.h>
#include <assert.h>

#include "audiofile.h"
#include "afinternal.h"

void afInitAESChannelData (AFfilesetup setup, int track)
{
	assert(track == AF_DEFAULT_TRACK);
	assert(setup);

	setup->aesDataPresent = 1;
}

void afInitAESChannelDataTo (AFfilesetup setup, int track, int willBeData)
{
	assert(track == AF_DEFAULT_TRACK);
	assert(setup);

	setup->aesDataPresent = willBeData;
}

int afGetAESChannelData (AFfilehandle file, int track, unsigned char buf[24])
{
	assert(file);
	assert(track == AF_DEFAULT_TRACK);
	assert(buf);

	if (file->aesDataPresent)
	{
		memcpy(buf, file->aesData, 24);
		return 1;
	}
	else
	{
		return 0;
	}
}

void afSetAESChannelData (AFfilehandle file, int track, unsigned char buf[24])
{
	assert(file);
	assert(track == AF_DEFAULT_TRACK);
	assert(buf);

	if (file->aesDataPresent)
	{
		memcpy(file->aesData, buf, 24);
	}
}
