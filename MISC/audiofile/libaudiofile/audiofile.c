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
	audiofile.c

	This file implements many of the main interface routines of the
	Audio File Library.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "audiofile.h"
#include "util.h"
#include "afinternal.h"
#include "afinternal.h"
#include "block.h"

int _af_parseaiff (AFfilehandle filehandle);
int _af_parsewave (AFfilehandle filehandle);
int _af_parseau (AFfilehandle filehandle);

int aiffSyncFile (AFfilehandle);
int waveSyncFile (AFfilehandle);
int auSyncFile (AFfilehandle);

void aiffOpenFileWrite (AFfilehandle);

int afSetVirtualByteOrder (AFfilehandle file, int track, int byteorder)
{
	assert(file);
	assert(track == AF_DEFAULT_TRACK);
	assert(byteorder == AF_BYTEORDER_BIGENDIAN ||
		byteorder == AF_BYTEORDER_LITTLEENDIAN);

	if (file == NULL)
	{
		_af_error(AF_BAD_FILEHANDLE);
		return -1;
	}

	if (track != AF_DEFAULT_TRACK)
	{
		_af_error(AF_BAD_TRACKID);
		return -1;
	}

	if (byteorder != AF_BYTEORDER_BIGENDIAN &&
		byteorder != AF_BYTEORDER_LITTLEENDIAN)
	{
		_af_error(AF_BAD_BYTEORDER);
		return -1;
	}

	/* Assert that the current virtual byte order is valid. */
	assert(file->virtualByteOrder == AF_BYTEORDER_BIGENDIAN ||
		file->virtualByteOrder == AF_BYTEORDER_LITTLEENDIAN);

	file->virtualByteOrder = byteorder;
	return 0;
}

int afSetVirtualSampleFormat (AFfilehandle file, int track,
	int sampleFormat, int sampleWidth)
{
	assert(file);
	assert(track == AF_DEFAULT_TRACK);
	assert(sampleWidth > 0);
	assert(sampleWidth <= 32 || sampleWidth == 64);

	if (sampleFormat == AF_SAMPFMT_UNSIGNED ||
		sampleFormat == AF_SAMPFMT_TWOSCOMP ||
		sampleFormat == AF_SAMPFMT_FLOAT ||
		sampleFormat == AF_SAMPFMT_DOUBLE)
	{
		file->virtualSampleFormat = sampleFormat;
		file->virtualSampleWidth = sampleWidth;
		return 0;
	}

	return -1;
}

void afGetVirtualSampleFormat (AFfilehandle file, int track,
	int *sampleFormat, int *sampleWidth)
{
	assert(file);
	assert(track == AF_DEFAULT_TRACK);

	assert(file->virtualSampleWidth > 0);
	assert(file->virtualSampleWidth <= 32 || file->virtualSampleWidth == 64);

	if (sampleFormat != NULL)
		*sampleFormat = file->sampleFormat;
	if (sampleWidth != NULL)
		*sampleWidth = file->sampleWidth;
}

int afGetByteOrder (AFfilehandle file, int track)
{
	assert(file);
	assert(track == AF_DEFAULT_TRACK);
	assert(file->byteOrder == AF_BYTEORDER_BIGENDIAN ||
		file->byteOrder == AF_BYTEORDER_LITTLEENDIAN);

	return file->byteOrder;
}

int afGetVirtualByteOrder(AFfilehandle file, int track)
{
	assert(file);
	assert(track == AF_DEFAULT_TRACK);
	assert(file->virtualByteOrder == AF_BYTEORDER_BIGENDIAN ||
		file->virtualByteOrder == AF_BYTEORDER_LITTLEENDIAN);

	return file->virtualByteOrder;
}

int afGetChannels(AFfilehandle file, int track)
{
	assert(file);
	assert(track == AF_DEFAULT_TRACK);

	if (file == NULL)
	{
		_af_error(AF_BAD_FILEHANDLE);
		return -1;
	}

	assert(file->channelCount >= 1);

	return file->channelCount;
}

void afGetSampleFormat(AFfilehandle file, int track, int *sampfmt, int *sampwidth)
{
	assert(file);
	assert(track == AF_DEFAULT_TRACK);

	if (sampfmt != NULL)
		*sampfmt = file->sampleFormat;
	if (sampwidth != NULL)
		*sampwidth = file->sampleWidth;
}

int afReadFrames (const AFfilehandle file, int track, void *samples, const int count)
{
	assert(file);
	assert(track == AF_DEFAULT_TRACK);
	assert(samples);
	assert(count >= 0);

	if (file->compression != NULL)
	{
		AFframecount (*readFrames) (const AFfilehandle, int, void *, const int);

		assert(file->compression);
		assert(file->compression->codec);

		readFrames = file->compression->codec->readFrames;

		assert(readFrames);

		return (readFrames(file, track, samples, count));
	}

	return _af_blockReadFrames(file, track, samples, count);
}

/*
	afGetFrameCount returns the total number of sample frames for a given
	track in an audio file.
*/
AFframecount afGetFrameCount (AFfilehandle file, int track)
{
	if (file != NULL)
	{
		return file->frameCount;
	}
	else
	{
		_af_error(AF_BAD_FILEHANDLE);
		return -1;
	}
}

/* afSyncFile returns 0 on success and -1 on failure */
int afSyncFile (AFfilehandle file)
{
	assert(file != NULL);

	/* Assert that the file type is one which is handled. */
	assert(file->fileFormat == AF_FILE_AIFFC ||
		file->fileFormat == AF_FILE_AIFF ||
		file->fileFormat == AF_FILE_WAVE ||
		file->fileFormat == AF_FILE_NEXTSND);

	/*
		It is an error to sync a null file.
	*/
	if (file == NULL)
	{
		_af_error(AF_BAD_FILEHANDLE);
		return -1;
	}

	if (file->fileFormat == AF_FILE_AIFFC)
		return aiffSyncFile(file);
	else if (file->fileFormat == AF_FILE_AIFF)
		return aiffSyncFile(file);
	else if (file->fileFormat == AF_FILE_WAVE)
		return waveSyncFile(file);
	else if (file->fileFormat == AF_FILE_NEXTSND)
		return auSyncFile(file);
	/*
		If none of the above branches are taken, the filehandle must be
		invalid.
	*/
	else
	{
		_af_error(AF_BAD_FILEHANDLE);
		return -1;
	}
}

/* afCloseFile returns 0 on success and -1 on failure. */
int afCloseFile (AFfilehandle file)
{
	int	result;

	assert(file != NULL);

	/* It is an error to close a null file. */
	if (file == NULL)
	{
		_af_error(AF_BAD_FILEHANDLE);
		return -1;
	}

	result = afSyncFile(file);

	if (result != 0)
		return result;

	result = af_fclose(file->fh);

	/* fclose returns -1 on error. */
	if (result != 0)
		return result;

	/* Free memory associated with loops. */
	if (file->loops != NULL)
		free(file->loops);

	/* Free memory associated with markers. */
	if (file->markers != NULL)
	{
		int	i;

		for (i=0; i<file->markerCount; i++)
			free(file->markers[i].name);

		free(file->markers);
	}

	/* Free memory associated with instruments. */
	if (file->instruments != NULL)
		free(file->instruments);

	/* Free memory associated with the miscellaneous chunk. */
	if (file->miscellaneous != NULL)
	{
		free(file->miscellaneous);
	}

	free(file);

	return 0;
}

static AFfilehandle _afOpenFileWrite2 (AFvirtualfile *vf, AFfilesetup setup)
{
	AFfilehandle	filehandle;

	filehandle = malloc(sizeof (struct _AFfilehandle));
	if (filehandle == NULL)
		return NULL;

	filehandle->fileFormat = setup->fileFormat;
	filehandle->channelCount = setup->channelCount;
	filehandle->sampleWidth = setup->sampleWidth;
	filehandle->sampleFormat = setup->sampleFormat;
	filehandle->byteOrder = setup->byteOrder;
	filehandle->sampleRate = setup->sampleRate;
	filehandle->dataStart = 0;
	filehandle->frameCount = 0;
	filehandle->currentFrame = 0;

	filehandle->aesDataPresent = setup->aesDataPresent;

	filehandle->fh = vf;
	filehandle->markerCount = 0;
	filehandle->markers = NULL;
	filehandle->loopCount = 0;
	filehandle->loops = NULL;
	filehandle->instrumentCount = 0;
	filehandle->instruments = NULL;

#if WORDS_BIGENDIAN
	filehandle->virtualByteOrder = AF_BYTEORDER_BIGENDIAN;
#else
	filehandle->virtualByteOrder = AF_BYTEORDER_LITTLEENDIAN;
#endif
	filehandle->virtualSampleFormat = filehandle->sampleFormat;
	filehandle->virtualSampleWidth = filehandle->sampleWidth;

	filehandle->compression = setup->compression;

	if (setup->miscellaneousCount == 0)
	{
		filehandle->miscellaneousCount = 0;
		filehandle->miscellaneous = NULL;
	}
	else
	{
		filehandle->miscellaneousCount = setup->miscellaneousCount;
		filehandle->miscellaneous = malloc(filehandle->miscellaneousCount *
			sizeof (struct _Miscellaneous));
		memcpy(filehandle->miscellaneous, setup->miscellaneous,
			filehandle->miscellaneousCount * sizeof (struct _Miscellaneous));
	}

	if (filehandle->fileFormat == AF_FILE_AIFF ||
		filehandle->fileFormat == AF_FILE_AIFFC)
	{
		int	i;

		filehandle->byteOrder = AF_BYTEORDER_BIGENDIAN;
		filehandle->sampleFormat = AF_SAMPFMT_TWOSCOMP;

		filehandle->markerCount = 4;
		filehandle->markers = malloc(4 * sizeof (struct _Marker));
		memcpy(filehandle->markers, setup->markers,
			4 * sizeof (struct _Marker));

		for (i=0; i<4; i++)
		{
			filehandle->markers[i].name = strdup(setup->markers[i].name);
		}

		filehandle->loopCount = 2;
		filehandle->loops = malloc(2 * sizeof (struct _Loop));
		assert(filehandle->loops);
		assert(setup->loops);
		memcpy(filehandle->loops, setup->loops, 2 * sizeof (struct _Loop));

		filehandle->instrumentCount = 1;
		filehandle->instruments = malloc(2 * sizeof (struct _Instrument));
		assert(filehandle->instruments);
		assert(setup->instruments);
		memcpy(filehandle->instruments, setup->instruments,
			1 * sizeof (struct _Instrument));

		aiffOpenFileWrite(filehandle);
	}
	else if (filehandle->fileFormat == AF_FILE_WAVE)
	{
		filehandle->byteOrder = AF_BYTEORDER_LITTLEENDIAN;
	}
	else if (filehandle->fileFormat == AF_FILE_NEXTSND)
	{
		filehandle->byteOrder = AF_BYTEORDER_BIGENDIAN;
	}

#ifdef DEBUG
	_af_printfilehandle(filehandle);
#endif

	return filehandle;
}

AFfilehandle afOpenVirtualFile (AFvirtualfile *vfile, const char *mode, AFfilesetup setup)
{
	AFfilehandle	filehandle;
	char			data[12];

	if (!strcmp(mode, "w"))
		return _afOpenFileWrite2 (vfile, setup);

	filehandle = malloc(sizeof (struct _AFfilehandle));
	if (filehandle == NULL)
	{
		_af_error(AF_BAD_NO_FILEHANDLE);
		return NULL;
	}

	af_fread(data, 1, 12, vfile);
	af_fseek(vfile, 0, SEEK_SET);

	filehandle->fh = vfile;
	filehandle->currentFrame = 0;
	filehandle->markerCount = 0;
	filehandle->markers = NULL;
	filehandle->loopCount = 0;
	filehandle->loops = NULL;
	filehandle->instrumentCount = 0;
	filehandle->instruments = NULL;
	filehandle->miscellaneousCount = 0;
	filehandle->miscellaneous = NULL;
	filehandle->aesDataPresent = 0;
	filehandle->compression = NULL;

#if WORDS_BIGENDIAN
	filehandle->virtualByteOrder = AF_BYTEORDER_BIGENDIAN;
#else
	filehandle->virtualByteOrder = AF_BYTEORDER_LITTLEENDIAN;
#endif

	if (!memcmp(data, "FORM", 4) && !memcmp(data + 8, "AIFC", 4))
	{
#ifdef DEBUG
		printf("detected aifc file\n");
#endif

		filehandle->fileFormat = AF_FILE_AIFFC;
		filehandle->sampleFormat = AF_SAMPFMT_TWOSCOMP;
		filehandle->byteOrder = AF_BYTEORDER_BIGENDIAN;

		_af_parseaiff(filehandle);
	}
	else if (!memcmp(data, "FORM", 4) && !memcmp(data + 8, "AIFF", 4))
	{
#ifdef DEBUG
		printf("detected aiff file\n");
#endif

		filehandle->fileFormat = AF_FILE_AIFF;
		filehandle->sampleFormat = AF_SAMPFMT_TWOSCOMP;
		filehandle->byteOrder = AF_BYTEORDER_BIGENDIAN;

		_af_parseaiff(filehandle);
	}
	else if (!memcmp(data, "RIFF", 4) && !memcmp(data + 8, "WAVE", 4))
	{
#ifdef DEBUG
		printf("detected wave file\n");
#endif

		filehandle->fileFormat = AF_FILE_WAVE;
		filehandle->sampleFormat = AF_SAMPFMT_TWOSCOMP;
		filehandle->byteOrder = AF_BYTEORDER_LITTLEENDIAN;

		_af_parsewave(filehandle);
	}
	else if (!memcmp(data, ".snd", 4))
	{
#ifdef DEBUG
		printf("detected next/sun sound file\n");
#endif

		filehandle->fileFormat = AF_FILE_NEXTSND;
		filehandle->byteOrder = AF_BYTEORDER_BIGENDIAN;

		_af_parseau(filehandle);
	}
	else
	{
		/*
			The only memory allocated is that from the initial malloc.
			None of the structures referenced within the file handle
			structure have had memory allocated if none of the parsers
			recognized the file.
		*/

		free(filehandle);
		return AF_NULL_FILEHANDLE;
	}

	filehandle->virtualSampleFormat = filehandle->sampleFormat;
	filehandle->virtualSampleWidth = filehandle->sampleWidth;

#ifdef DEBUG
	_af_printfilehandle(filehandle);
#endif

	return filehandle;
}

AFfilehandle afOpenFile (const char *filename, const char *mode, AFfilesetup setup)
{
	FILE			*fp;

	fp = fopen(filename, mode);
	if (fp == NULL)
	{
		_af_error(AF_BAD_OPEN);
		return AF_NULL_FILEHANDLE;
	}

	return afOpenVirtualFile(af_virtual_file_new_for_file(fp), mode, setup);
}

AFfilehandle afOpenFD (int fd, const char *mode, AFfilesetup setup)
{
	FILE	*fp;

	fp = fdopen(fd, mode);
	if (fp == NULL)
	{
		_af_error(AF_BAD_OPEN);
		return AF_NULL_FILEHANDLE;
	}

	return afOpenVirtualFile(af_virtual_file_new_for_file(fp), mode, setup);
}

int afGetFileFormat (AFfilehandle file, int *version)
{
	assert(file);

	if (file->fileFormat == AF_FILE_AIFFC && version != NULL)
		*version = AIFC_VERSION_1;

	return file->fileFormat;
}

double afGetRate (AFfilehandle file, int track)
{
	assert(file);
	assert(track == AF_DEFAULT_TRACK);

	return file->sampleRate;
}

AFfileoffset afGetDataOffset (AFfilehandle file, int track)
{
	assert(file);
	assert(track == AF_DEFAULT_TRACK);

	return file->dataStart;
}

AFfileoffset afGetTrackBytes (AFfilehandle file, int track)
{
	assert(file);
	assert(track == AF_DEFAULT_TRACK);

	return file->trackBytes;
}

/*
	afGetFrameSize returns the size (in bytes) of a sample frame from
	the specified track of an audio file.

	This routine might not be implemented quite properly.

	I'm not really sure under what circumstances afGetFrameSize can
	return a non-integral value.
*/
float afGetFrameSize (AFfilehandle file, int track, int stretch3to4)
{
	assert(file);
	assert(track == AF_DEFAULT_TRACK);

	if (file->sampleWidth <= 8)
		return 1 * file->channelCount;
	else if (file->sampleWidth <= 16)
		return 2 * file->channelCount;
	else if (file->sampleWidth <= 24)
	{
		if (stretch3to4 == 0)
			return 3 * file->channelCount;
		else
			return 4 * file->channelCount;
	}
	else if (file->sampleWidth <= 32)
		return 4 * file->channelCount;

	return (file->channelCount * ((file->sampleWidth + 7) / 8));
}

AFframecount afSeekFrame (AFfilehandle file, int track, AFframecount frame)
{
	int	frameSize;

	assert(file);
	assert(track == AF_DEFAULT_TRACK);

	if (frame == -1)
	{
		return afTellFrame(file, track);
	}

	/* frameSize represents the size of a frame in bytes. */
	frameSize = file->channelCount * ((file->sampleWidth + 7) / 8);

	file->currentFrame = frame;

	if (af_fseek(file->fh, file->dataStart + frameSize * frame, SEEK_SET) != 0)
	{
		_af_error(AF_BAD_LSEEK);
		return -1;
	}

	return frame;
}

AFfileoffset afTellFrame (AFfilehandle file, int track)
{
	assert(file);
	assert(track == AF_DEFAULT_TRACK);

	return file->currentFrame;
}
