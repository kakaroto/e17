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
	instrument.c

	This file contains routines dealing with instruments.
*/

#include <stdlib.h>
#include <assert.h>

#include "afinternal.h"
#include "audiofile.h"
#include "aupvlist.h"

static struct _Instrument *initInstrument (struct _Instrument *instrument);
static struct _Instrument *findInstrumentByID (int id,
	struct _Instrument *instruments, int count);

static struct _Instrument *initInstrument (struct _Instrument *instrument)
{
	assert(instrument);

	instrument->id = AF_DEFAULT_INST;
	instrument->midiBaseNote = 60;
	instrument->detune = 0;
	instrument->midiLowNote = 0;
	instrument->midiHighNote = 127;
	instrument->midiLowVelocity = 1;
	instrument->midiHighVelocity = 127;
	instrument->gain = 0;
	instrument->sustainLoopID = 1;
	instrument->releaseLoopID = 2;

	return instrument;
}

static struct _Instrument *findInstrumentByID (int id,
	struct _Instrument *instruments, int count)
{
	int	i;

	assert(instruments);
	assert(count > 0);

	for (i=0; i<count; i++)
	{
		if (instruments[i].id == id)
		{
			return &instruments[i];
		}
	}

	return NULL;
}

/* afInitInstIDs */
void afInitInstIDs (AFfilesetup setup, int *ids, int nids)
{
	assert(setup);
	assert(nids >= 0);

	if (nids > 0)
	{
		assert(ids);
		if (nids != setup->instrumentCount)
		{
			int	i;

			setup->instruments =
				realloc(setup->instruments, nids * sizeof (struct _Instrument));

			for (i=0; i<nids; i++)
			{
				initInstrument(&setup->instruments[i]);
				setup->instruments[i].id = ids[i];
			}
		}
	}
}

int afGetInstIDs (AFfilehandle file, int *ids)
{
	assert(file);

	if (ids != NULL)
	{
		int	i;

		for (i=0; i<file->instrumentCount; i++)
		{
			ids[i] = file->instruments[i].id;
		}
	}

	return file->instrumentCount;
}

void afSetInstParams (AFfilehandle file, int instid, AUpvlist pvlist,
	int parameterCount)
{
	int	i;

	assert(file);

	for (i=0; i<parameterCount; i++)
	{
		int		parameter;
		int		valueType;
		long	value;

		AUpvgetparam(pvlist, i, &parameter);
		AUpvgetvaltype(pvlist, i, &valueType);

		/* At present, only long-valued parameters are recognized. */
		if (valueType != AU_PVTYPE_LONG)
			continue;

		AUpvgetval(pvlist, i, &value);

		switch (parameter)
		{
			case AF_INST_MIDI_BASENOTE:
			case AF_INST_NUMCENTS_DETUNE:
			case AF_INST_MIDI_LONOTE:
			case AF_INST_MIDI_HINOTE:
			case AF_INST_MIDI_LOVELOCITY:
			case AF_INST_MIDI_HIVELOCITY:
			case AF_INST_NUMDBS_GAIN:
			case AF_INST_SUSLOOPID:
			case AF_INST_RELLOOPID:
				afSetInstParamLong(file, instid, parameter, value);
				break;

			default:
				_af_error(AF_BAD_INSTPID);
				break;
		}
	}
}

void afGetInstParams (AFfilehandle file, int instid, AUpvlist pvlist,
	int parameterCount)
{
	int	i;

	assert(file);

	for (i=0; i<parameterCount; i++)
	{
		int		parameter;
		long	value;

		AUpvgetparam(pvlist, i, &parameter);

		switch (parameter)
		{
			case AF_INST_MIDI_BASENOTE:
			case AF_INST_NUMCENTS_DETUNE:
			case AF_INST_MIDI_LONOTE:
			case AF_INST_MIDI_HINOTE:
			case AF_INST_MIDI_LOVELOCITY:
			case AF_INST_MIDI_HIVELOCITY:
			case AF_INST_NUMDBS_GAIN:
			case AF_INST_SUSLOOPID:
			case AF_INST_RELLOOPID:
				value = afGetInstParamLong(file, instid, parameter);
				AUpvsetvaltype(pvlist, i, AU_PVTYPE_LONG);
				AUpvsetval(pvlist, i, &value);
				break;

			default:
				_af_error(AF_BAD_INSTPID);
				break;
		}
	}
}

void afSetInstParamLong (AFfilehandle file, int instid, int parameter,
	long value)
{
	struct _Instrument	*instrument;

	assert(file);
	assert(instid == AF_DEFAULT_INST);

	instrument = findInstrumentByID(instid, file->instruments,
		file->instrumentCount);

	if (instrument == NULL)
	{
		_af_error(AF_BAD_INSTID);
		return;
	}

	switch (parameter)
	{
		case AF_INST_MIDI_BASENOTE:
			if (parameter >= 0 && parameter <= 127)
				instrument->midiBaseNote = parameter;
			else
				_af_error(AF_BAD_INSTPID);

			break;

		case AF_INST_NUMCENTS_DETUNE:
			instrument->detune = parameter;
			break;

		case AF_INST_MIDI_LONOTE:
			if (parameter >= 0 && parameter <= 127)
				instrument->midiLowNote = parameter;
			else
				_af_error(AF_BAD_INSTPID);
			break;

		case AF_INST_MIDI_HINOTE:
			if (parameter >= 0 && parameter <= 127)
				instrument->midiHighNote = parameter;
			else
				_af_error(AF_BAD_INSTPID);
			break;

		case AF_INST_MIDI_LOVELOCITY:
			if (parameter >= 1 && parameter <= 127)
				instrument->midiLowVelocity = parameter;
			else
				_af_error(AF_BAD_INSTPID);
			break;

		case AF_INST_MIDI_HIVELOCITY:
			if (parameter >= 1 && parameter <= 127)
				instrument->midiHighVelocity = parameter;
			else
				_af_error(AF_BAD_INSTPID);
			break;

		case AF_INST_NUMDBS_GAIN:
			instrument->gain = parameter;
			break;

		case AF_INST_SUSLOOPID:
			if (parameter >= 1)
				instrument->sustainLoopID = parameter;
			else
				_af_error(AF_BAD_INSTPID);
			break;

		case AF_INST_RELLOOPID:
			if (parameter >= 1)
				instrument->releaseLoopID = parameter;
			else
				_af_error(AF_BAD_INSTPID);
			break;

		default:
			_af_error(AF_BAD_INSTPID);
			break;
	}
}

long afGetInstParamLong (AFfilehandle file, int instid, int parameter)
{
	struct _Instrument	*instrument;

	assert(file);
	assert(instid == AF_DEFAULT_INST);

	instrument = findInstrumentByID(instid, file->instruments,
		file->instrumentCount);

	if (instrument == NULL)
	{
		/* _af_error(AF_BAD_INSTID); */
		return 0;
	}

	switch (parameter)
	{
		case AF_INST_MIDI_BASENOTE:
			return instrument->midiBaseNote;
			break;
		case AF_INST_NUMCENTS_DETUNE:
			return instrument->detune;
			break;
		case AF_INST_MIDI_LONOTE:
			return instrument->midiLowNote;
			break;
		case AF_INST_MIDI_HINOTE:
			return instrument->midiHighNote;
			break;
		case AF_INST_MIDI_LOVELOCITY:
			return instrument->midiLowVelocity;
			break;
		case AF_INST_MIDI_HIVELOCITY:
			return instrument->midiHighVelocity;
			break;
		case AF_INST_NUMDBS_GAIN:
			return instrument->gain;
			break;
		case AF_INST_SUSLOOPID:
			return instrument->sustainLoopID;
			break;
		case AF_INST_RELLOOPID:
			return instrument->releaseLoopID;
			break;
		default:
			/* _af_error(AF_BAD_INSTPID); */
			break;
	}

	return 0;
}
