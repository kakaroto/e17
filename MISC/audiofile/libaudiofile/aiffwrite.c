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
	aiffwrite.c

	This file contains routines for writing AIFF and AIFF-C format
	sound files.
*/

#include <assert.h>
#include <sys/types.h>
#include <stdlib.h>

#include "extended.h"
#include "afinternal.h"
#include "audiofile.h"
#include "aiff.h"
#include "byteorder.h"
#include "block.h"

int aiffWriteFrames (const AFfilehandle file, int track, void *samples, const int count);
int aiffSyncFile (AFfilehandle file);
void aiffOpenFileWrite (AFfilehandle file);

static void WriteCOMM (const AFfilehandle file);
static AFframecount WriteSSND (const AFfilehandle file, void *samples, size_t count);
static void WriteMARK (AFfilehandle file);
static void WriteINST (AFfilehandle file);
static void WriteFVER (AFfilehandle file);
static void WriteAESD (AFfilehandle file);
static void WriteMiscellaneous (AFfilehandle file);

void aiffOpenFileWrite (AFfilehandle file)
{
	u_int32_t	fileSize = HOST_TO_BENDIAN_INT32(0);

	af_fwrite("FORM", 4, 1, file->fh);
	af_fwrite(&fileSize, 4, 1, file->fh);
	if (file->fileFormat == AF_FILE_AIFF)
		af_fwrite("AIFF", 4, 1, file->fh);
	else if (file->fileFormat == AF_FILE_AIFFC)
		af_fwrite("AIFC", 4, 1, file->fh);
	else
		assert(0);

	if (file->fileFormat == AF_FILE_AIFFC)
		WriteFVER(file);

	WriteCOMM(file);
	WriteAESD(file);
	WriteMARK(file);
	WriteINST(file);
	WriteMiscellaneous(file);
}

/* aiffWriteFrames is endian-clean. */
int aiffWriteFrames (const AFfilehandle file, int track, void *samples, const int count)
{
	AFframecount	finalCount;

	assert(file);
	assert(samples);

	finalCount = WriteSSND(file, samples, count);

#ifdef DEBUG
	_af_printfilehandle(file);
#endif

	return finalCount;
}

int aiffSyncFile (AFfilehandle file)
{
	u_int32_t	length;

	assert(file);

#ifdef DEBUG
	printf("aiffSyncFile called.\n");
#endif

	if (file->dataStart != 0)
	{
		/* get the length of the file */
		af_fseek(file->fh, 0, SEEK_END);
		length = af_ftell(file->fh);
		length -= 8;
		length = HOST_TO_BENDIAN_INT32(length);

		/* set the length of the FORM chunk */
		af_fseek(file->fh, 4, SEEK_SET);
		af_fwrite(&length, 4, 1, file->fh);

		/* Update the SSND chunk's chunk size. */
		af_fseek(file->fh, file->dataStart - 12, SEEK_SET);
		length =
			file->frameCount * ((file->sampleWidth + 7) / 8) * file->channelCount + 8;
		length = HOST_TO_BENDIAN_INT32(length);
		af_fwrite(&length, 4, 1, file->fh);

		/* Update the COMM chunk's count of sample frames. */
		af_fseek(file->fh, 12, SEEK_SET);
		if (file->fileFormat == AF_FILE_AIFFC)
			WriteFVER(file);

		WriteCOMM(file);
		WriteAESD(file);
	}

	return 0;
}

/* WriteCOMM is believed to be endian-clean. */
static void WriteCOMM (const AFfilehandle file)
{
	u_int32_t		size;
	u_int16_t		sb;
	u_int32_t		lb;
	unsigned char	eb[10];

	size = 18;
	if (file->fileFormat == AF_FILE_AIFFC)
		size = 38;

	af_fwrite("COMM", 4, 1, file->fh);
	size = HOST_TO_BENDIAN_INT32(size);
	af_fwrite(&size, 4, 1, file->fh);

	/* number of channels, 2 bytes */
	sb = HOST_TO_BENDIAN_INT16(file->channelCount);
	af_fwrite(&sb, 2, 1, file->fh);

	/* number of sample frames, 4 bytes */
	lb = HOST_TO_BENDIAN_INT32(file->frameCount);
	af_fwrite(&lb, 4, 1, file->fh);

	/* sample size, 2 bytes */
	sb = HOST_TO_BENDIAN_INT16(file->sampleWidth);
	af_fwrite(&sb, 2, 1, file->fh);

	/* sample rate, 10 bytes */
	ConvertToIeeeExtended(file->sampleRate, eb);
	af_fwrite(eb, 10, 1, file->fh);

	if (file->fileFormat == AF_FILE_AIFFC)
	{
		char	sizeByte, zero = 0;
		char	compressionName[] = "not compressed";

		af_fwrite("NONE", 4, 1, file->fh);

		sizeByte = strlen(compressionName);

		af_fwrite(&sizeByte, 1, 1, file->fh);
		af_fwrite(compressionName, sizeByte, 1, file->fh);
		if ((sizeByte % 2) == 0)
			af_fwrite(&zero, 1, 1, file->fh);
	}
}

/* The AESD chunk contains information pertinent to audio recording devices. */
/* WriteAESD is endian-clean. */

/*
	The AESD chunk always follows the COMM chunk.  Whenever the file
	is created or the file is synchronized, these two chunks are
	always rewritten as AES data is able to be changed after
	afOpenFile.
*/
static void WriteAESD (const AFfilehandle file)
{
	u_int32_t	size = 24;

	assert(file);

	if (!file->aesDataPresent)
		return;

	af_fwrite("AESD", 4, 1, file->fh);

	size = HOST_TO_BENDIAN_INT32(size);
	af_fwrite(&size, 4, 1, file->fh);

	af_fwrite(file->aesData, 24, 1, file->fh);
}

/* WriteSSND is believed to be endian-clean. */
static AFframecount WriteSSND (const AFfilehandle file, void *samples, size_t count)
{
	u_int32_t	length, chunkSize, frameSize, zero = 0;
	AFframecount	finalCount = 0;

	assert(file);
	assert(file->fh);
	assert(samples);

#ifdef DEBUG
	printf("WriteSSND called.\n");
#endif

	frameSize = file->channelCount * ((file->sampleWidth + 7) / 8);

	length = (file->frameCount + count) * frameSize;
	chunkSize = length + 8;

#ifdef DEBUG
	printf("count: %d\n", count);
	printf("chunkSize: %d\n", chunkSize);
#endif

	if (file->dataStart == 0)
	{
		af_fwrite("SSND", 4, 1, file->fh);
		/* initial size set to zero */
		af_fwrite(&zero, 4, 1, file->fh);

		/* offset */
		af_fwrite(&zero, 4, 1, file->fh);
		/* block size */
		af_fwrite(&zero, 4, 1, file->fh);

		file->dataStart = af_ftell(file->fh);
	}

	finalCount = _af_blockWriteFrames(file, AF_DEFAULT_TRACK, samples, count);

#if 0
	/* seek to the four-byte length indicator for the SSND chunk */
	af_fseek(file->fh, file->dataStart - 12, SEEK_SET);
	lb = HOST_TO_BENDIAN_INT32(chunkSize);
	af_fwrite(&lb, 4, 1, file->fh);
	af_fseek(file->fh, 0, SEEK_END);
#endif

	return finalCount;
}

static void WriteINST (AFfilehandle file)
{
	u_int32_t		length;
	struct _INST	instrumentdata;

	length = 20;
	length = HOST_TO_BENDIAN_INT32(length);

	instrumentdata.sustainLoopPlayMode =
		HOST_TO_BENDIAN_INT16(afGetLoopMode(file, AF_DEFAULT_INST, 1));
	instrumentdata.sustainLoopBegin =
		HOST_TO_BENDIAN_INT16(afGetLoopStart(file, AF_DEFAULT_INST, 1));
	instrumentdata.sustainLoopEnd =
		HOST_TO_BENDIAN_INT16(afGetLoopEnd(file, AF_DEFAULT_INST, 1));

	instrumentdata.releaseLoopPlayMode =
		HOST_TO_BENDIAN_INT16(afGetLoopMode(file, AF_DEFAULT_INST, 2));
	instrumentdata.releaseLoopBegin =
		HOST_TO_BENDIAN_INT16(afGetLoopStart(file, AF_DEFAULT_INST, 2));
	instrumentdata.releaseLoopEnd =
		HOST_TO_BENDIAN_INT16(afGetLoopEnd(file, AF_DEFAULT_INST, 2));

	af_fwrite("INST", 4, 1, file->fh);
	af_fwrite(&length, 4, 1, file->fh);

	instrumentdata.baseNote =
		afGetInstParamLong(file, AF_DEFAULT_INST, AF_INST_MIDI_BASENOTE);
	af_fwrite(&instrumentdata.baseNote, 1, 1, file->fh);
	instrumentdata.detune =
		afGetInstParamLong(file, AF_DEFAULT_INST, AF_INST_NUMCENTS_DETUNE);
	af_fwrite(&instrumentdata.detune, 1, 1, file->fh);
	instrumentdata.lowNote =
		afGetInstParamLong(file, AF_DEFAULT_INST, AF_INST_MIDI_LONOTE);
	af_fwrite(&instrumentdata.lowNote, 1, 1, file->fh);
	instrumentdata.highNote =
		afGetInstParamLong(file, AF_DEFAULT_INST, AF_INST_MIDI_HINOTE);
	af_fwrite(&instrumentdata.highNote, 1, 1, file->fh);
	instrumentdata.lowVelocity =
		afGetInstParamLong(file, AF_DEFAULT_INST, AF_INST_MIDI_LOVELOCITY);
	af_fwrite(&instrumentdata.lowVelocity, 1, 1, file->fh);
	instrumentdata.highVelocity =
		afGetInstParamLong(file, AF_DEFAULT_INST, AF_INST_MIDI_HIVELOCITY);
	af_fwrite(&instrumentdata.highVelocity, 1, 1, file->fh);

	instrumentdata.gain =
		afGetInstParamLong(file, AF_DEFAULT_INST, AF_INST_NUMDBS_GAIN);
	instrumentdata.gain = HOST_TO_BENDIAN_INT16(instrumentdata.gain);
	af_fwrite(&instrumentdata.gain, 2, 1, file->fh);

	af_fwrite(&instrumentdata.sustainLoopPlayMode, 2, 1, file->fh);
	af_fwrite(&instrumentdata.sustainLoopBegin, 2, 1, file->fh);
	af_fwrite(&instrumentdata.sustainLoopEnd, 2, 1, file->fh);

	af_fwrite(&instrumentdata.releaseLoopPlayMode, 2, 1, file->fh);
	af_fwrite(&instrumentdata.releaseLoopBegin, 2, 1, file->fh);
	af_fwrite(&instrumentdata.releaseLoopEnd, 2, 1, file->fh);
}

/* This function is probably fairly endian-clean. */
static void WriteMARK (AFfilehandle file)
{
	off_t		start, end;
	u_int32_t	length;
	u_int16_t	numMarkers, sb;
	int			i, *markids;

	assert(file);

	length = 0;

	af_fwrite("MARK", 4, 1, file->fh);
	af_fwrite(&length, 4, 1, file->fh);

	start = af_ftell(file->fh);

	numMarkers = file->markerCount;
	markids = (int *) malloc(numMarkers * sizeof (int));
	afGetMarkIDs(file, AF_DEFAULT_TRACK, markids);

	sb = HOST_TO_BENDIAN_INT16(numMarkers);
	af_fwrite(&sb, 2, 1, file->fh);

	for (i=0; i<numMarkers; i++)
	{
		u_int8_t	namelength, zero = 0;
		u_int16_t	id;
		u_int32_t	position;

		id = markids[i];
		position = afGetMarkPosition(file, AF_DEFAULT_TRACK, id);

		id = HOST_TO_BENDIAN_INT16(id);
		position = HOST_TO_BENDIAN_INT32(position);

		af_fwrite(&id, 2, 1, file->fh);
		af_fwrite(&position, 4, 1, file->fh);

		/* Write the strings in Pascal style. */
		assert(file->markers[i].name);
		namelength = strlen(file->markers[i].name);
		af_fwrite(&namelength, 1, 1, file->fh);
		af_fwrite(file->markers[i].name, 1, namelength, file->fh);

		if ((namelength % 2) == 0)
			af_fwrite(&zero, 1, 1, file->fh);
	}

	end = af_ftell(file->fh);
	length = end - start;

#ifdef DEBUG
	printf(" end: %d\n", end);
	printf(" length: %d\n", end - start);
#endif

	af_fseek(file->fh, start - 4, SEEK_SET);

	length = HOST_TO_BENDIAN_INT32(length);
	af_fwrite(&length, 4, 1, file->fh);
	af_fseek(file->fh, end, SEEK_SET);
}

/* This function is endian-clean. */
static void WriteFVER (AFfilehandle file)
{
	u_int32_t	chunkSize, timeStamp;

	af_fwrite("FVER", 4, 1, file->fh);

	chunkSize = 4;
	chunkSize = HOST_TO_BENDIAN_INT32(chunkSize);
	af_fwrite(&chunkSize, 4, 1, file->fh);

	timeStamp = AIFCVersion1;
	timeStamp = HOST_TO_BENDIAN_INT32(timeStamp);
	af_fwrite(&timeStamp, 4, 1, file->fh);
}

/*
	WriteMiscellaneous writes all the miscellaneous data chunks in a
	file handle structure to an AIFF or AIFF-C file.
*/
static void WriteMiscellaneous (AFfilehandle file)
{
	int	i;

	for (i=0; i<file->miscellaneousCount; i++)
	{
		struct _Miscellaneous	*misc = &file->miscellaneous[i];
		u_int32_t				chunkType, chunkSize;

#ifdef DEBUG
		printf("WriteMiscellaneous: %d, type %d\n", i, misc->type);
#endif

		switch (misc->type)
		{
			case AF_MISC_NAME:
				memcpy(&chunkType, "NAME", 4); break;
			case AF_MISC_AUTH:
				memcpy(&chunkType, "AUTH", 4); break;
			case AF_MISC_COPY:
				memcpy(&chunkType, "(c) ", 4); break;
			case AF_MISC_ANNO:
				memcpy(&chunkType, "ANNO", 4); break;
			case AF_MISC_MIDI:
				memcpy(&chunkType, "MIDI", 4); break;
			case AF_MISC_APPL:
				memcpy(&chunkType, "APPL", 4); break;
		}

		chunkSize = HOST_TO_BENDIAN_INT32(misc->size);

		af_fwrite(&chunkType, 4, 1, file->fh);
		af_fwrite(&chunkSize, 4, 1, file->fh);
		misc->offset = af_ftell(file->fh);
		/*
			Skip the appropriate number of bytes, leaving room for a
			pad byte.
		*/
		af_fseek(file->fh, misc->size + (misc->size % 2), SEEK_CUR);
	}
}
