/*
	Audio File Library

	Copyright 1998, Michael Pruett <michael@68k.org>

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
	read.c

	This program reads and plays a given audio file using Irix's
	default audio output device.  This file will not work on any
	operating system other than Irix.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dmedia/audio.h>
#include <dmedia/audiofile.h>

void usage (void)
{
	fprintf(stderr, "usage: irixread filename\n");
	exit(-1);
}

main (int ac, char **av)
{
	AFfilehandle	file;
	AFframecount	frameCount;
	int				frameSize, channelCount;
	char			*buffer;
	int				*loopids, *markids;
	int				i, loopCount, markCount;
	int				startmarkid, endmarkid;
	AFframecount	startloop, endloop, count;

	ALport			outport;
	ALconfig		outportconfig;

	if (ac < 2)
		usage();

	file = afOpenFile(av[1], "r", NULL);
	frameCount = afGetFrameCount(file, AF_DEFAULT_TRACK);
	frameSize = afGetFrameSize(file, AF_DEFAULT_TRACK, 1);
	channelCount = afGetChannels(file, AF_DEFAULT_TRACK);
	printf("frame count: %d\n", frameCount);
	afSetVirtualByteOrder(file, AF_DEFAULT_TRACK, AF_BYTEORDER_BIGENDIAN);
	buffer = (char *) malloc(65536 * frameSize);

	outportconfig = alNewConfig();
	alSetWidth(outportconfig, AL_SAMPLE_16);
	alSetChannels(outportconfig, channelCount);

	count = afReadFrames(file, AF_DEFAULT_TRACK, buffer, 65536);

	outport = alOpenPort("dick", "w", outportconfig);

	do
	{
		printf("count = %d\n", count);
		alWriteFrames(outport, buffer, count);
	}
	while (count = afReadFrames(file, AF_DEFAULT_TRACK, buffer, 65536));

	alClosePort(outport);
	alFreeConfig(outportconfig);

	afCloseFile(file);
}
