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

	fwrite("FORM", 4, 1, file->fp);
	fwrite(&fileSize, 4, 1, file->fp);
	if (file->fileFormat == AF_FILE_AIFF)
		fwrite("AIFF", 4, 1, file->fp);
	else if (file->fileFormat == AF_FILE_AIFFC)
		fwrite("AIFC", 4, 1, file->fp);
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
	ssize_t		result;

	assert(file);

#ifdef DEBUG
	printf("aiffSyncFile called.\n");
#endif

	if (file->dataStart != 0)
	{
		/* get the length of the file */
		fseek(file->fp, 0, SEEK_END);
		length = ftell(file->fp);
		length -= 8;
		length = HOST_TO_BENDIAN_INT32(length);

		/* set the length of the FORM chunk */
		fseek(file->fp, 4, SEEK_SET);
		fwrite(&length, 4, 1, file->fp);

		/* Update the SSND chunk's chunk size. */
		fseek(file->fp, file->dataStart - 12, SEEK_SET);
		length =
			file->frameCount * ((file->sampleWidth + 7) / 8) * file->channelCount + 8;
		length = HOST_TO_BENDIAN_INT32(length);
		fwrite(&length, 4, 1, file->fp);

		/* Update the COMM chunk's count of sample frames. */
		fseek(file->fp, 12, SEEK_SET);
		if (file->fileFormat == AF_FILE_AIFFC)
			WriteFVER(file);

		WriteCOMM(file);
		WriteAESD(file);
	}

	fflush(file->fp);

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

	fwrite("COMM", 4, 1, file->fp);
	size = HOST_TO_BENDIAN_INT32(size);
	fwrite(&size, 4, 1, file->fp);

	/* number of channels, 2 bytes */
	sb = HOST_TO_BENDIAN_INT16(file->channelCount);
	fwrite(&sb, 2, 1, file->fp);

	/* number of sample frames, 4 bytes */
	lb = HOST_TO_BENDIAN_INT32(file->frameCount);
	fwrite(&lb, 4, 1, file->fp);

	/* sample size, 2 bytes */
	sb = HOST_TO_BENDIAN_INT16(file->sampleWidth);
	fwrite(&sb, 2, 1, file->fp);

	/* sample rate, 10 bytes */
	ConvertToIeeeExtended(file->sampleRate, eb);
	fwrite(eb, 10, 1, file->fp);

	if (file->fileFormat == AF_FILE_AIFFC)
	{
		char	sizeByte, zero = 0;
		char	compressionName[] = "not compressed";

		fwrite("NONE", 4, 1, file->fp);

		sizeByte = strlen(compressionName);

		fwrite(&sizeByte, 1, 1, file->fp);
		fwrite(compressionName, sizeByte, 1, file->fp);
		if ((sizeByte % 2) == 0)
			fwrite(&zero, 1, 1, file->fp);
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

	fwrite("AESD", 4, 1, file->fp);

	size = HOST_TO_BENDIAN_INT32(size);
	fwrite(&size, 4, 1, file->fp);

	fwrite(file->aesData, 24, 1, file->fp);
}

/* WriteSSND is believed to be endian-clean. */
static AFframecount WriteSSND (const AFfilehandle file, void *samples, size_t count)
{
	u_int32_t	length, chunkSize, frameSize, zero = 0;
	u_int32_t	lb;
	AFframecount	finalCount = 0;

	assert(file);
	assert(file->fp);
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
		fwrite("SSND", 4, 1, file->fp);
		/* initial size set to zero */
		fwrite(&zero, 4, 1, file->fp);

		/* offset */
		fwrite(&zero, 4, 1, file->fp);
		/* block size */
		fwrite(&zero, 4, 1, file->fp);

		file->dataStart = ftell(file->fp);
	}

	finalCount = _af_blockWriteFrames(file, AF_DEFAULT_TRACK, samples, count);

#if 0
	/* seek to the four-byte length indicator for the SSND chunk */
	fseek(file->fp, file->dataStart - 12, SEEK_SET);
	lb = HOST_TO_BENDIAN_INT32(chunkSize);
	fwrite(&lb, 4, 1, file->fp);
	fseek(file->fp, 0, SEEK_END);
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

	fwrite("INST", 4, 1, file->fp);
	fwrite(&length, 4, 1, file->fp);

	instrumentdata.baseNote =
		afGetInstParamLong(file, AF_DEFAULT_INST, AF_INST_MIDI_BASENOTE);
	fwrite(&instrumentdata.baseNote, 1, 1, file->fp);
	instrumentdata.detune =
		afGetInstParamLong(file, AF_DEFAULT_INST, AF_INST_NUMCENTS_DETUNE);
	fwrite(&instrumentdata.detune, 1, 1, file->fp);
	instrumentdata.lowNote =
		afGetInstParamLong(file, AF_DEFAULT_INST, AF_INST_MIDI_LONOTE);
	fwrite(&instrumentdata.lowNote, 1, 1, file->fp);
	instrumentdata.highNote =
		afGetInstParamLong(file, AF_DEFAULT_INST, AF_INST_MIDI_HINOTE);
	fwrite(&instrumentdata.highNote, 1, 1, file->fp);
	instrumentdata.lowVelocity =
		afGetInstParamLong(file, AF_DEFAULT_INST, AF_INST_MIDI_LOVELOCITY);
	fwrite(&instrumentdata.lowVelocity, 1, 1, file->fp);
	instrumentdata.highVelocity =
		afGetInstParamLong(file, AF_DEFAULT_INST, AF_INST_MIDI_HIVELOCITY);
	fwrite(&instrumentdata.highVelocity, 1, 1, file->fp);

	instrumentdata.gain =
		afGetInstParamLong(file, AF_DEFAULT_INST, AF_INST_NUMDBS_GAIN);
	instrumentdata.gain = HOST_TO_BENDIAN_INT16(instrumentdata.gain);
	fwrite(&instrumentdata.gain, 2, 1, file->fp);

	fwrite(&instrumentdata.sustainLoopPlayMode, 2, 1, file->fp);
	fwrite(&instrumentdata.sustainLoopBegin, 2, 1, file->fp);
	fwrite(&instrumentdata.sustainLoopEnd, 2, 1, file->fp);

	fwrite(&instrumentdata.releaseLoopPlayMode, 2, 1, file->fp);
	fwrite(&instrumentdata.releaseLoopBegin, 2, 1, file->fp);
	fwrite(&instrumentdata.releaseLoopEnd, 2, 1, file->fp);
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

	fwrite("MARK", 4, 1, file->fp);
	fwrite(&length, 4, 1, file->fp);

	start = ftell(file->fp);

	numMarkers = file->markerCount;
	markids = (int *) malloc(numMarkers * sizeof (int));
	afGetMarkIDs(file, AF_DEFAULT_TRACK, markids);

	sb = HOST_TO_BENDIAN_INT16(numMarkers);
	fwrite(&sb, 2, 1, file->fp);

	for (i=0; i<numMarkers; i++)
	{
		u_int8_t	namelength, zero = 0;
		u_int16_t	id;
		u_int32_t	position;
		ssize_t		result;

		id = markids[i];
		position = afGetMarkPosition(file, AF_DEFAULT_TRACK, id);

		id = HOST_TO_BENDIAN_INT16(id);
		position = HOST_TO_BENDIAN_INT32(position);

		fwrite(&id, 2, 1, file->fp);
		fwrite(&position, 4, 1, file->fp);

		/* Write the strings in Pascal style. */
		assert(file->markers[i].name);
		namelength = strlen(file->markers[i].name);
		fwrite(&namelength, 1, 1, file->fp);
		fwrite(file->markers[i].name, 1, namelength, file->fp);

		if ((namelength % 2) == 0)
			fwrite(&zero, 1, 1, file->fp);
	}

	end = ftell(file->fp);
	length = end - start;

#ifdef DEBUG
	printf(" end: %d\n", end);
	printf(" length: %d\n", end - start);
#endif

	fseek(file->fp, start - 4, SEEK_SET);

	length = HOST_TO_BENDIAN_INT32(length);
	fwrite(&length, 4, 1, file->fp);
	fseek(file->fp, end, SEEK_SET);
}

/* This function is endian-clean. */
static void WriteFVER (AFfilehandle file)
{
	u_int32_t	chunkSize, timeStamp;

	fwrite("FVER", 4, 1, file->fp);

	chunkSize = 4;
	chunkSize = HOST_TO_BENDIAN_INT32(chunkSize);
	fwrite(&chunkSize, 4, 1, file->fp);

	timeStamp = AIFCVersion1;
	timeStamp = HOST_TO_BENDIAN_INT32(timeStamp);
	fwrite(&timeStamp, 4, 1, file->fp);
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

		fwrite(&chunkType, 4, 1, file->fp);
		fwrite(&chunkSize, 4, 1, file->fp);
		misc->offset = ftell(file->fp);
		/*
			Skip the appropriate number of bytes, leaving room for a
			pad byte.
		*/
		fseek(file->fp, misc->size + (misc->size % 2), SEEK_CUR);
	}
}
