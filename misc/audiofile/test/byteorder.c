/*
	Audio File Library

	Copyright 1998-1999, Michael Pruett <michael@68k.org>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License as
	published by the Free Software Foundation; either version 2 of
	the License, or (at your option) any later version.

	This program is distributed in the hope that it will be
	useful, but WITHOUT ANY WARRANTY; without even the implied
	warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
	PURPOSE.  See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public
	License along with this program; if not, write to the Free
	Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
	MA 02111-1307, USA.
*/

/*
	byteorder.c
*/

#ifdef __USE_SGI_HEADERS__
#include <dmedia/audiofile.h>
#else
#include <audiofile.h>
#endif

#include <stdio.h>

main (int argc, char **argv)
{
	AFfilehandle	file;
	int				sampleFormat, sampleWidth, byteOrder;

	file = afOpenFile(argv[1], "r", NULL);

	byteOrder = afGetByteOrder(file, AF_DEFAULT_TRACK);
	printf("byte order: %d\n", byteOrder);
	afGetSampleFormat(file, AF_DEFAULT_TRACK, &sampleFormat, &sampleWidth);
	printf("sample format: %d, sample width: %d\n", sampleFormat, sampleWidth);

	byteOrder = afGetByteOrder(file, AF_DEFAULT_TRACK);
	printf("virtual byte order: %d\n", byteOrder);
	afGetVirtualSampleFormat(file, AF_DEFAULT_TRACK, &sampleFormat, &sampleWidth);
	printf("virtual sample format: %d, virtual sample width: %d\n", sampleFormat, sampleWidth);

	afCloseFile(file);
}
