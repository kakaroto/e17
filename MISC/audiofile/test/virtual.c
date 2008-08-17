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
	virtual.c
*/

#ifdef __USE_SGI_HEADERS__
#include <dmedia/audiofile.h>
#else
#include <audiofile.h>
#endif

#include <stdio.h>

void usage (void)
{
	fprintf(stderr, "usage: virtual filename\n");
	exit(-1);
}

int main (int argc, char **argv)
{
	int				dick;
	AFfilehandle	file;

	if (argc != 2)
		usage();

	file = afOpenFile(argv[1], "r", NULL);

	dick = afSetVirtualByteOrder(file, AF_DEFAULT_TRACK,
		AF_BYTEORDER_BIGENDIAN);
	printf("afSetVirtualByteOrder = %d\n");

	dick = afSetVirtualByteOrder(file, AF_DEFAULT_TRACK,
		AF_BYTEORDER_LITTLEENDIAN);
	printf("afSetVirtualByteOrder = %d\n");

	dick = afSetVirtualSampleFormat(file, AF_DEFAULT_TRACK,
		AF_SAMPFMT_UNSIGNED, 16);
	printf("afSetVirtualSampleFormat = %d\n");

	dick = afSetVirtualSampleFormat(file, AF_DEFAULT_TRACK,
		AF_SAMPFMT_TWOSCOMP, 16);
	printf("afSetVirtualSampleFormat = %d\n");

	dick = afSetVirtualSampleFormat(file, AF_DEFAULT_TRACK,
		AF_SAMPFMT_TWOSCOMP, 32);
	printf("afSetVirtualSampleFormat = %d\n");

	dick = afSetVirtualSampleFormat(file, AF_DEFAULT_TRACK,
		AF_SAMPFMT_TWOSCOMP, 24);
	printf("afSetVirtualSampleFormat = %d\n");

	dick = afSetVirtualSampleFormat(file, AF_DEFAULT_TRACK,
		AF_SAMPFMT_DOUBLE, 64);
	printf("afSetVirtualSampleFormat = %d\n");

	dick = afSetVirtualChannels(file, AF_DEFAULT_TRACK, 1);
	printf("afSetVirtualChannels = %d\n");

	dick = afSetVirtualChannels(file, AF_DEFAULT_TRACK, 4);
	printf("afSetVirtualChannels = %d\n");

	afCloseFile(file);
}
