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
	query.c

	This file contains the implementation of the Audio File Library's
	query mechanism.  Querying through the afQuery calls can allow the
	programmer to determine the capabilities and characteristics of
	the Audio File Library implementation and its supported formats.
*/

#include <assert.h>
#include <stdlib.h>

#include "audiofile.h"
#include "aupvlist.h"
#include "error.h"

struct _instrumentparameter
{
	int	id, type;
	const char	*name;
	long	defaultvalue;
};

struct _instrument
{
	int	supported;
	int	idcount;
	int	parametersupported;
	int	parameteridcount;

	const struct _instrumentparameter	parameters[64];
	/* some other things for the instrument parameters */

	int	loopsupported;
	int	loopidcount;
};

struct _miscellaneous
{
	int	typecount;
	const int	*types;
	int	maxnumber;
};

struct _fileformat
{
	int		id;
	const char	*label, *name, *description;
	int		implemented;
	int		defaultsampleformat;
	int		defaultsamplewidth;

	const struct _instrument	*dick;
};

const int _FILEFORMATCOUNT = 7;
static const struct _instrument _aiffinstrument =
{
	1, 1, 1, 9,
	{
		{AF_INST_MIDI_BASENOTE, AU_PVTYPE_LONG, "MIDI base note", 60},
		{AF_INST_NUMCENTS_DETUNE, AU_PVTYPE_LONG, "Detune in cents", 0},
		{AF_INST_MIDI_LONOTE, AU_PVTYPE_LONG, "Low note", 0},
		{AF_INST_MIDI_HINOTE, AU_PVTYPE_LONG, "High note", 127},
		{AF_INST_MIDI_LOVELOCITY, AU_PVTYPE_LONG, "Low velocity", 1},
		{AF_INST_MIDI_HIVELOCITY, AU_PVTYPE_LONG, "High velocity", 127},
		{AF_INST_NUMDBS_GAIN, AU_PVTYPE_LONG, "Gain in dB", 0},
		{AF_INST_SUSLOOPID, AU_PVTYPE_LONG, "Sustain loop id", 1},
		{AF_INST_RELLOOPID, AU_PVTYPE_LONG, "Release loop id", 2}
	},
	1, 1
};

static const struct _instrument _otherinstrument =
{
	0, 0, 0, 0,
	{ 0 },
	0, 0
};

static struct _fileformat _FILEFORMATS[] =
{
	{
		AF_FILE_AIFF,
		"aiff",
		"AIFF",
		"Audio Interchange File Format AIFF",
		1,
		AF_SAMPFMT_TWOSCOMP,
		16,
		&_aiffinstrument
	},
	{
		AF_FILE_AIFFC,
		"aifc",
		"AIFF-C",
		"Audio Interchange File Format AIFF-C",
		1,
		AF_SAMPFMT_TWOSCOMP,
		16,
		&_aiffinstrument
	},
	{
		AF_FILE_WAVE,
		"wave",
		"MS RIFF WAVE",
		"MS RIFF WAVE Format",
		1,
		AF_SAMPFMT_TWOSCOMP,
		16,
		&_otherinstrument
	},
	{
		AF_FILE_NEXTSND,
		"next",
		"NeXT .snd/Sun .au",
		"NeXT .snd/Sun .au Format",
		1,
		AF_SAMPFMT_TWOSCOMP,
		16,
		&_otherinstrument
	},
	{
		AF_FILE_BICSF,
		"bicsf",
		"BICSF",
		"Berkeley/IRCAM/CARL Sound File Format",
		0,
		AF_SAMPFMT_TWOSCOMP,
		16,
		&_otherinstrument
	},
	{
		AF_FILE_VOC,
		"voc",
		"VOC",
		"Creative Voice File Format",
		0,
		AF_SAMPFMT_TWOSCOMP,
		16,
		&_otherinstrument
	},
	{
		AF_FILE_RAWDATA,
		"raw",
		"Raw Data",
		"Raw Sound Data",
		0,
		AF_SAMPFMT_TWOSCOMP,
		16,
		&_otherinstrument
	}
};

AUpvlist _afQueryFileFormat (int arg1, int arg2, int arg3, int arg4);
AUpvlist _afQueryInstrument (int arg1, int arg2, int arg3, int arg4);
AUpvlist _afQueryInstrumentParameter (int arg1, int arg2, int arg3, int arg4);
AUpvlist _afQueryCompression (int arg1, int arg2, int arg3, int arg4);
AUpvlist _afQueryCompressionParameter (int arg1, int arg2, int arg3, int arg4);
AUpvlist _afQueryMiscellaneous (int arg1, int arg2, int arg3, int arg4);

struct _fileformat *findfileformatbyid (int id)
{
	int	i;
	for (i=0; i<_FILEFORMATCOUNT; i++)
	{
		if (_FILEFORMATS[i].id == id)
			return &_FILEFORMATS[i];
	}

	return NULL;
}

AUpvlist afQuery (int querytype, int arg1, int arg2, int arg3, int arg4)
{
	switch (querytype)
	{
		case AF_QUERYTYPE_INST:
			return _afQueryInstrument(arg1, arg2, arg3, arg4);
			break;
		case AF_QUERYTYPE_INSTPARAM:
			return _afQueryInstrumentParameter(arg1, arg2, arg3, arg4);
			break;
		case AF_QUERYTYPE_LOOP:
			break;
		case AF_QUERYTYPE_FILEFMT:
			return _afQueryFileFormat(arg1, arg2, arg3, arg4);
			break;
		case AF_QUERYTYPE_COMPRESSION:
			/* FIXME: This selector is not implemented. */
			return AU_NULL_PVLIST;
			break;
		case AF_QUERYTYPE_COMPRESSIONPARAM:
			/* FIXME: This selector is not implemented. */
			return AU_NULL_PVLIST;
			break;
		case AF_QUERYTYPE_MISC:
			/* FIXME: This selector is not implemented. */
			return AU_NULL_PVLIST;
			break;
		default:
			_af_error(AF_BAD_QUERYTYPE);
			return AU_NULL_PVLIST;
	}

	return AU_NULL_PVLIST;
}


AUpvlist _afQueryFileFormat (int arg1, int arg2, int arg3, int arg4)
{
	AUpvlist	list;

	if (arg1 == AF_QUERY_LABEL)
	{
		struct _fileformat	*format;
		list = AUpvnew(1);
		assert(list);
		AUpvsetvaltype(list, 0, AU_PVTYPE_PTR);

		if ((format = findfileformatbyid(arg2)) != NULL)
		{
			assert(format);
			AUpvsetval(list, 0, &format->label);
		}

		return list;
	}
	else if (arg1 == AF_QUERY_NAME)
	{
		struct _fileformat	*format;
		list = AUpvnew(1);
		assert(list);
		AUpvsetvaltype(list, 0, AU_PVTYPE_PTR);

		if ((format = findfileformatbyid(arg2)) != NULL)
		{
			assert(format);
			AUpvsetval(list, 0, &format->name);
		}

		return list;
	}
	else if (arg1 == AF_QUERY_DESC)
	{
		struct _fileformat	*format;
		list = AUpvnew(1);
		assert(list);
		AUpvsetvaltype(list, 0, AU_PVTYPE_PTR);

		if ((format = findfileformatbyid(arg2)) != NULL)
		{
			assert(format);
			AUpvsetval(list, 0, &format->description);
		}

		return list;
	}
	else if (arg1 == AF_QUERY_IMPLEMENTED)
	{
		struct _fileformat	*format;
		list = AUpvnew(1);
		assert(list);
		AUpvsetvaltype(list, 0, AU_PVTYPE_LONG);

		if ((format = findfileformatbyid(arg2)) != NULL)
		{
			assert(format);
			AUpvsetval(list, 0, &format->implemented);
		}

		return list;
	}
	else if (arg1 == AF_QUERY_ID_COUNT)
	{
		long	count = 0;
		int		i;

		list = AUpvnew(1);
		assert(list);
		AUpvsetvaltype(list, 0, AU_PVTYPE_LONG);
		for (i=0; i<_FILEFORMATCOUNT; i++)
		{
			if (_FILEFORMATS[i].implemented)
				count++;
		}
		AUpvsetval(list, 0, &count);
		return list;
	}
	else if (arg1 == AF_QUERY_IDS)
	{
		long	count = 0;
		int		i;
		long	*idarray;

		for (i=0; i<_FILEFORMATCOUNT; i++)
		{
			if (_FILEFORMATS[i].implemented)
				count++;
		}

		list = AUpvnew(1);
		AUpvsetvaltype(list, 0, AU_PVTYPE_PTR);
		idarray = malloc(count * sizeof (long));

		for (i=0, count=0; i<_FILEFORMATCOUNT; i++)
		{
			if (_FILEFORMATS[i].implemented)
			{
				idarray[count] = _FILEFORMATS[i].id;
				count++;
			}
		}
		AUpvsetval(list, 0, &idarray);
		return list;
	}
	/* no compression formats are supported at the present */
	else if (arg1 == AF_QUERY_COMPRESSION_TYPES)
	{
		long	count = 0;
		long	*nullpointer = NULL;

		list = AUpvnew(1);

		switch (arg2)
		{
			case AF_QUERY_VALUE_COUNT:
				AUpvsetvaltype(list, 0, AU_PVTYPE_LONG);
				AUpvsetval(list, 0, &count);
				break;
			case AF_QUERY_VALUES:
				AUpvsetvaltype(list, 0, AU_PVTYPE_PTR);
				AUpvsetval(list, 0, &nullpointer);
				break;
		}

		return list;
	}
	else if (arg1 == AF_QUERY_SAMPLE_FORMATS)
	{
		struct _fileformat	*format;

		if (arg2 != AF_QUERY_DEFAULT)
		{
			_af_error(AF_BAD_QUERY);
			return AU_NULL_PVLIST;
		}

		list = AUpvnew(1);
		if ((format = findfileformatbyid(arg3)) != NULL)
		{
			assert(format);
			AUpvsetval(list, 0, &format->defaultsampleformat);
		}
		return list;
	}
	else if (arg1 == AF_QUERY_SAMPLE_SIZES)
	{
		struct _fileformat	*format;

		if (arg2 != AF_QUERY_DEFAULT)
		{
			_af_error(AF_BAD_QUERY);
			return AU_NULL_PVLIST;
		}

		list = AUpvnew(1);
		if ((format = findfileformatbyid(arg3)) != NULL)
		{
			assert(format);
			AUpvsetval(list, 0, &format->defaultsamplewidth);
		}
		return list;
	}
	else
		_af_error(AF_BAD_QUERY);

	return AU_NULL_PVLIST;
}

long afQueryLong (int querytype, int arg1, int arg2, int arg3, int arg4)
{
	AUpvlist	list;
	long		value;

	list = afQuery(querytype, arg1, arg2, arg3, arg4);
	assert(list);
	AUpvgetval(list, 0, &value);
	AUpvfree(list);
	return value;
}

double afQueryDouble (int querytype, int arg1, int arg2, int arg3, int arg4)
{
	AUpvlist	list;
	double		value;

	list = afQuery(querytype, arg1, arg2, arg3, arg4);
	assert(list);
	AUpvgetval(list, 0, &value);
	AUpvfree(list);
	return value;
}

void *afQueryPointer (int querytype, int arg1, int arg2, int arg3, int arg4)
{
	AUpvlist	list;
	void		*value;

	list = afQuery(querytype, arg1, arg2, arg3, arg4);
	assert(list);
	AUpvgetval(list, 0, &value);
	AUpvfree(list);
	return value;
}

AUpvlist _afQueryInstrument (int arg1, int arg2, int arg3, int arg4)
{
	_af_error(AF_BAD_NOT_IMPLEMENTED);
	return AU_NULL_PVLIST;

#if 0
	if (arg1 == AF_QUERY_SUPPORTED)
	{
		struct _format	*format;

		if (format = findformatbyid(arg2))
		{
			list = AUpvnew(1);
			AUpvsetvalue(list, 0, format->instrument.supported);
			return list;
		}
		else
		{
			/* The format specified was not a recognized format. */
			_af_error(AF_BAD_QUERY);
		}
	}
	else if (arg1 == AF_QUERY_ID_COUNT)
	{
		if (format = findformatbyid(arg2))
		{
			list = AUpvnew(1);
			AUpvsetvalue(list, 0, format->instrument.idcount);
			return list;
		}
		else
		{
			/* The format specified was not a recognized format. */
			_af_error(AF_BAD_QUERY);
		}
	}

	_af_error(AF_BAD_QUERY);
	return AU_NULL_PVLIST;
#endif
}

AUpvlist _afQueryInstrumentParameter (int arg1, int arg2, int arg3, int arg4)
{
	_af_error(AF_BAD_NOT_IMPLEMENTED);
	return AU_NULL_PVLIST;

	if (arg1 == AF_QUERY_SUPPORTED)
	{
	}
	else if (arg1 == AF_QUERY_ID_COUNT)
	{
	}
	else if (arg1 == AF_QUERY_IDS)
	{
	}
	else if (arg1 == AF_QUERY_TYPE)
	{
	}
	else if (arg1 == AF_QUERY_NAME)
	{
	}
	else if (arg1 == AF_QUERY_DEFAULT)
	{
	}

	return AU_NULL_PVLIST;
}

AUpvlist _afQueryLoop (int arg1, int arg2, int arg3, int arg4)
{
	_af_error(AF_BAD_NOT_IMPLEMENTED);
	return AU_NULL_PVLIST;

	if (arg1 == AF_QUERY_SUPPORTED)
	{
	}
	else if (arg1 == AF_QUERY_ID_COUNT)
	{
	}

	return AU_NULL_PVLIST;
}
