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

#ifdef __USE_SGI_HEADERS__
#include <dmedia/audiofile.h>
#else
#include <audiofile.h>
#endif

#include <stdio.h>
#include <stdlib.h>

main (int argc, char **argv)
{
	AFfilehandle	file;
	AFframecount	frameCount;
	int				channelCount;
	int				frameSize;
	char			*buffer;
	int				i;

	if (argc != 2)
	{
		fprintf(stderr, "usage: seek filename\n");
		exit(-1);
	}

	file = afOpenFile(argv[1], "r", NULL);
	afSeekFrame(file, AF_DEFAULT_TRACK, 0);
	printf("%d\n", afTellFrame(file, AF_DEFAULT_TRACK));

	afSeekFrame(file, AF_DEFAULT_TRACK,
		afGetFrameCount(file, AF_DEFAULT_TRACK));
	printf("%d\n", afTellFrame(file, AF_DEFAULT_TRACK));

	afSeekFrame(file, AF_DEFAULT_TRACK, -1);
	printf("%d\n", afTellFrame(file, AF_DEFAULT_TRACK));
	afCloseFile(file);
}
