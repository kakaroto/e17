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
	error.c

	This file contains the routines used in the Audio File Library's
	error handling.
*/

#include <stdio.h>
#include <stdarg.h>
#include "audiofile.h"

static void defaultErrorFunction (long error, const char *str);
static const char *errordescription (int error);

AFerrfunc errorFunction = defaultErrorFunction;

AFerrfunc afSetErrorHandler (AFerrfunc efunc)
{
	AFerrfunc	old;
	
	old = errorFunction;
	errorFunction = efunc;

	return old;
}

static void defaultErrorFunction (long error, const char *str)
{
	fprintf(stderr, "Audio File Library: error %ld: ", error);
	fprintf(stderr, str);
	fprintf(stderr, "\n");
}

void _af_error (int errorCode)
{
	if (errorFunction != NULL)
		errorFunction(errorCode, errordescription(errorCode));
}

static const char *errordescription (int error)
{
	switch (error)
	{
		case AF_BAD_NOT_IMPLEMENTED:
			return "not implemented yet";
		case AF_BAD_FILEHANDLE:
			return "tried to use invalid filehandle";
		case AF_BAD_OPEN:
			return "Unix open failed";
		case AF_BAD_CLOSE:
			return "Unix close failed";
		case AF_BAD_READ:
			return "Unix read failed";
		case AF_BAD_WRITE:
			return "Unix write failed";
		case AF_BAD_LSEEK:
			return "Unix lseek failed";
		case AF_BAD_NO_FILEHANDLE:
			return "failed to allocate a filehandle structure";
		case AF_BAD_ACCMODE:
			return "unrecognized audio file access mode";
		case AF_BAD_NOWRITEACC:
			return "file not open for writing";
		case AF_BAD_NOREADACC:
			return "file not open for reading";
		case AF_BAD_FILEFMT:
			return "unrecognized audio file format";
		case AF_BAD_RATE:
			return "invalid sample rate";
		case AF_BAD_CHANNELS:
			return "invalid number of channel";
		case AF_BAD_SAMPCNT:
			return "invalid sample count";
		case AF_BAD_WIDTH:
			return "invalid sample width";
		case AF_BAD_SEEKMODE:
			return "invalid seek mode";
		case AF_BAD_NO_LOOPDATA:
			return "failed to allocate loop struct";
		case AF_BAD_MALLOC:
			return "malloc failed somewhere";
		case AF_BAD_LOOPID:
			return "bad loop id";
		case AF_BAD_SAMPFMT:
			return "bad sample format";
		case AF_BAD_FILESETUP:
			return "bad file setup structure";
		case AF_BAD_TRACKID:
			return "no track corresponding to id";
		case AF_BAD_NUMTRACKS:
			return "wrong number of tracks for file format";
		case AF_BAD_NO_FILESETUP:
			return "failed to allocate a filesetup struct";
		case AF_BAD_LOOPMODE:
			return "unrecognized loop mode value";
		case AF_BAD_INSTID:
			return "invalid instrument id";
		case AF_BAD_NUMLOOPS:
			return "bad number of loops";
		case AF_BAD_NUMMARKS:
			return "bad number of markers";
		case AF_BAD_MARKID:
			return "bad marker id";
		case AF_BAD_MARKPOS:
			return "invalid marker position value";
		case AF_BAD_NUMINSTS:
			return "invalid number of instruments";
		case AF_BAD_NOAESDATA:
			return "no AES data";
		case AF_BAD_MISCID:
			return "bad miscellaneous id";
		case AF_BAD_NUMMISC:
			return "bad miscellaneous count";
		case AF_BAD_MISCSIZE:
			return "bad miscellaneous size";
		case AF_BAD_MISCTYPE:
			return "bad miscellaneous type";
		case AF_BAD_MISCSEEK:
			return "bad miscellaneous seek";
		case AF_BAD_STRLEN:
			return "invalid string length";
		case AF_BAD_RATECONV:
			return "bad rate conversion";
		case AF_BAD_SYNCFILE:
			return "bad file synchronization";
		case AF_BAD_CODEC_CONFIG:
			return "improperly configured codec";
		case AF_BAD_CODEC_STATE:
			return "invalid codec state: can't recover";
		case AF_BAD_CODEC_LICENSE:
			return "no license available for codec";
		case AF_BAD_CODEC_TYPE:
			return "unsupported codec type";

		case AF_BAD_INSTPTYPE:
			return "invalid instrument parameter type";
		case AF_BAD_INSTPID:
			return "invalid instrument parameter id";
		case AF_BAD_BYTEORDER:
			return "bad file byte order";
		case AF_BAD_FILEFMT_PARAM:
			return "unrecognized file format parameter";
		case AF_BAD_COMP_PARAM:
			return "unrecognized compression parameter";
		case AF_BAD_DATAOFFSET:
			return "bad data offset";
		case AF_BAD_FRAMECNT:
			return "bad frame count";
		case AF_BAD_QUERYTYPE:
			return "bad query type";
		case AF_BAD_QUERY:
			return "bad argument to afQuery()";
		case AF_WARNING_CODEC_RATE:
			return "using 8k instead of codec rate 8012";
		case AF_WARNING_RATECVT:
			return "warning about rate conversion used";

		case AF_BAD_HEADER:
			return "failed to parse header";
		case AF_BAD_FRAME:
			return "bad frame number";
		case AF_BAD_LOOPCOUNT:
			return "bad loop count";
		case AF_BAD_DMEDIA_CALL:
			return "error in dmedia subsystem call";

		case AF_BAD_AIFF_HEADER:
			return "failed to parse chunk header";
		case AF_BAD_AIFF_FORM:
			return "failed to parse FORM chunk";
		case AF_BAD_AIFF_SSND:
			return "failed to parse SSND chunk";
		case AF_BAD_AIFF_CHUNKID:
			return "unrecognized AIFF/AIFF-C chunk id";
		case AF_BAD_AIFF_COMM:
			return "failed to parse COMM chunk";
		case AF_BAD_AIFF_INST:
			return "failed to parse INST chunk";
		case AF_BAD_AIFF_MARK:
			return "failed to parse MARK chunk";
		case AF_BAD_AIFF_SKIP:
			return "failed to skip unsupported chunk";
		case AF_BAD_AIFF_LOOPMODE:
			return "unrecognized loop mode (forw, etc)";
		default:
			return "unknown error";
	}

	return "You should never see this message.";
}
