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
	irixtest.c

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
	fprintf(stderr, "usage: irixtest filename\n");
	exit(-1);
}

main (int argc, char **argv)
{
	AFfilehandle	file;
	AFframecount	frameCount;
	int				sampleFormat, sampleWidth, channelCount, frameSize;
	char			*buffer;
	int				*loopids, *markids;
	int				i, loopCount, markCount;
	int				startmarkid, endmarkid;
	AFframecount	startloop, endloop;

	ALport			outport;
	ALconfig		outportconfig;

	if (argc < 2)
		usage();

	file = afOpenFile(argv[1], "r", NULL);
	frameCount = afGetFrameCount(file, AF_DEFAULT_TRACK);
	printf("frame count: %d\n", frameCount);

	frameSize = afGetFrameSize(file, AF_DEFAULT_TRACK, 1);
	printf("frame size: %d\n", frameSize);

	channelCount = afGetChannels(file, AF_DEFAULT_TRACK);

	afGetSampleFormat(file, AF_DEFAULT_TRACK, &sampleFormat, &sampleWidth);

	afSetVirtualByteOrder(file, AF_DEFAULT_TRACK, AF_BYTEORDER_BIGENDIAN);
	buffer = (char *) malloc(frameCount * frameSize * 2);
	afReadFrames(file, AF_DEFAULT_TRACK, buffer, frameCount);

	afCloseFile(file);

	outportconfig = alNewConfig();

	if (sampleWidth <= 8)
	{
		printf("width 8\n");
		alSetWidth(outportconfig, AL_SAMPLE_8);
	}
	else if (sampleWidth <= 16)
	{
		printf("width 16\n");
		alSetWidth(outportconfig, AL_SAMPLE_16);
	}
	else if (sampleWidth <= 24)
	{
		printf("width 24\n");
		alSetWidth(outportconfig, AL_SAMPLE_24);
	}

	alSetChannels(outportconfig, channelCount);

	outport = alOpenPort("dick", "w", outportconfig);
	alWriteFrames(outport, buffer, frameCount * frameSize);

	alClosePort(outport);
	alFreeConfig(outportconfig);
}
