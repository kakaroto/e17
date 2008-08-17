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
	misc.c

	This file contains routines for dealing with the Audio File
	Library's internal miscellaneous data types.
*/

#include <sys/types.h>
#include <stdlib.h>
#include <assert.h>

#include "audiofile.h"
#include "afinternal.h"

static struct _Miscellaneous *initMiscellaneous
	(struct _Miscellaneous *miscellaneous)
{
	miscellaneous->id = 0;
	miscellaneous->type = 0;
	miscellaneous->size = 0;
	miscellaneous->offset = 0;
	miscellaneous->position = 0;

	return miscellaneous;
}

struct _Miscellaneous *findMiscellaneousByID (int id,
	struct _Miscellaneous *miscellaneous, int count)
{
	int	i;

	for (i=0; i<count; i++)
	{
		if (miscellaneous[i].id == id)
			return &miscellaneous[i];
	}

	return NULL;
}

void afInitMiscIDs (AFfilesetup setup, int *ids, int nids)
{
	int	i;

	assert(setup);
	assert(ids);
	assert(nids >= 0);

	if (setup->miscellaneous != NULL)
	{
		free(setup->miscellaneous);
	}

	setup->miscellaneousCount = nids;
	setup->miscellaneous = malloc(nids * sizeof (struct _Miscellaneous));

	for (i=0; i<nids; i++)
	{
		initMiscellaneous(&setup->miscellaneous[i]);
		setup->miscellaneous[i].id = ids[i];
	}
}

int afGetMiscIDs (AFfilehandle file, int *ids)
{
	int	i;

	assert(file);

	if (ids != NULL)
	{
		for (i=0; i<file->miscellaneousCount; i++)
		{
			ids[i] = file->miscellaneous[i].id;
		}
	}

	return file->miscellaneousCount;
}

void afInitMiscType (AFfilesetup setup, int miscellaneousid, int type)
{
	struct _Miscellaneous	*miscellaneous;

	assert(setup);

	miscellaneous = findMiscellaneousByID(miscellaneousid,
		setup->miscellaneous, setup->miscellaneousCount);

	if (miscellaneous)
		miscellaneous->type = type;
	else
		_af_error(AF_BAD_MISCID);
}

int afGetMiscType (AFfilehandle file, int miscellaneousid)
{
	struct _Miscellaneous	*miscellaneous;

	assert(file);

	miscellaneous = findMiscellaneousByID(miscellaneousid,
		file->miscellaneous, file->miscellaneousCount);

	if (miscellaneous)
	{
		return miscellaneous->type;
	}
	else
	{
		_af_error(AF_BAD_MISCID);
		return -1;
	}
}

void afInitMiscSize (AFfilesetup setup, int miscellaneousid, int size)
{
	struct _Miscellaneous	*miscellaneous;

	assert(setup);

	miscellaneous = findMiscellaneousByID(miscellaneousid,
		setup->miscellaneous, setup->miscellaneousCount);

	if (miscellaneous)
	{
		miscellaneous->size = size;
	}
	else
		_af_error(AF_BAD_MISCID);
}

int afGetMiscSize (AFfilehandle file, int miscellaneousid)
{
	struct _Miscellaneous	*miscellaneous;

	assert(file);

	miscellaneous = findMiscellaneousByID(miscellaneousid,
		file->miscellaneous, file->miscellaneousCount);

	if (miscellaneous)
	{
		return miscellaneous->size;
	}
	else
	{
		_af_error(AF_BAD_MISCID);
		return -1;
	}
}

int afWriteMisc (AFfilehandle file, int miscellaneousid, void *buf, int bytes)
{
	struct _Miscellaneous	*miscellaneous;
	ssize_t					result;

	assert(file);

	miscellaneous = findMiscellaneousByID(miscellaneousid,
		file->miscellaneous, file->miscellaneousCount);

	if (miscellaneous == NULL)
	{
		_af_error(AF_BAD_MISCID);
		return -1;
	}
	else
	{
		off_t	currentPosition;
		currentPosition = af_ftell(file->fh);

/*
		if (miscellaneous->offset == 0)
		{
			if (file->fileFormat == AF_FILE_AIFF ||
				file->fileFormat == AF_FILE_AIFFC)
				aiffWriteHeader(file);
		}
*/

		af_fseek(file->fh, miscellaneous->offset + miscellaneous->position,
			SEEK_SET);

		assert(bytes + miscellaneous->position <= miscellaneous->size);
		if (bytes + miscellaneous->position > miscellaneous->size)
		{
			_af_error(AF_BAD_MISCSEEK);
			return -1;
		}

		result = af_fwrite(buf, bytes, 1, file->fh);

		af_fseek(file->fh, currentPosition, SEEK_SET);
	}

	return result;
}

int afReadMisc (AFfilehandle file, int miscellaneousid, void *buf, int bytes)
{
	struct _Miscellaneous	*miscellaneous;

	assert(file);

	miscellaneous = findMiscellaneousByID(miscellaneousid,
		file->miscellaneous, file->miscellaneousCount);

	if (miscellaneous == NULL)
	{
		_af_error(AF_BAD_MISCID);
		return -1;
	}
	else
	{
		off_t	position;
		ssize_t	result;

		position = af_ftell(file->fh);

		assert(miscellaneous->size);
		assert(miscellaneous->position + bytes <= miscellaneous->size);

		/*
			Is this the correct error to return in this case?
			I'd be surprised if anyone ever used this function.
		*/
		if (miscellaneous->size == 0)
			_af_error(AF_BAD_MISCSIZE);
		if (miscellaneous->position + bytes > miscellaneous->size)
			_af_error(AF_BAD_MISCSEEK);

		af_fseek(file->fh, miscellaneous->offset + miscellaneous->position, SEEK_SET);
		result = af_fread(buf, bytes, 1, file->fh);
		af_fseek(file->fh, position, SEEK_SET);
		return result;
	}
}

int afSeekMisc (AFfilehandle file, int miscellaneousid, int offset)
{
	struct _Miscellaneous	*miscellaneous;

	assert(file);
	assert(offset >= 0);

	miscellaneous = findMiscellaneousByID(miscellaneousid,
		file->miscellaneous, file->miscellaneousCount);

	if (miscellaneous == NULL)
	{
		_af_error(AF_BAD_MISCID);
		return 0;
	}

	assert(offset <= miscellaneous->size);

	if (offset < 0 || offset > miscellaneous->size)
		_af_error(AF_BAD_MISCSEEK);

	miscellaneous->position = offset;

	return offset;
}
