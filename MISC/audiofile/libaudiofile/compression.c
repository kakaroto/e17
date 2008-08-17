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
	compression.c

	This file contains routines for configuring compressed audio.
*/

#include <assert.h>

#include "audiofile.h"
#include "afinternal.h"
#include "aupvlist.h"

extern struct _Compression g711_ulaw_compression;

static struct _Compression *findCompression (int compression)
{
	switch (compression)
	{
		case AF_COMPRESSION_G711_ULAW:
			return &g711_ulaw_compression;
		default:
			return NULL;
	}
}

int afGetCompression (AFfilehandle file, int trackid)
{
	assert(file);
	assert(trackid == AF_DEFAULT_TRACK);

	if (file->compression == NULL)
		return AF_COMPRESSION_NONE;

	return file->compression->type;
}

void afInitCompression (AFfilesetup setup, int trackid, int compression)
{
	assert(setup);
	assert(trackid == AF_DEFAULT_TRACK);

	setup->compression = findCompression(compression);
	if (setup->compression == NULL)
		_af_error(AF_BAD_CODEC_TYPE);
}

#if 0
int afGetCompressionParams (AFfilehandle file, int trackid,
	int *compression, AUpvlist pvlist, int numitems)
{
	assert(file);
	assert(trackid == AF_DEFAULT_TRACK);
}

void afInitCompressionParams (AFfilesetup setup, int trackid,
	int compression, AUpvlist pvlist, int numitems)
{
	assert(setup);
	assert(trackid == AF_DEFAULT_TRACK);
}
#endif
