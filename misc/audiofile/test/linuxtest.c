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
	linuxtest.c

	This file plays a 16-bit, 44.1 kHz stereo audio file through a PC
	sound card on a Linux system.  This file will not compile under
	Irix or probably any operating system other than Linux.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/soundcard.h>
#include <audiofile.h>

void usage (void)
{
	fprintf(stderr, "usage: linuxtest filename\n");
	fprintf(stderr,
		"where filename refers to a 16-bit stereo 44.1 kHz audio file\n");
	exit(-1);
}

main (int ac, char **av)
{
	AFfilehandle	file;
	AFframecount	frameCount;
	int				sampleFormat, sampleWidth, channelCount;
	char			*buffer;
	int				audiofd;
	int				format, frequency, channels;

	file = afOpenFile(av[1], "r", NULL);
	frameCount = afGetFrameCount(file, AF_DEFAULT_TRACK);
	printf("frame count: %d\n", frameCount);

	channelCount = afGetChannels(file, AF_DEFAULT_TRACK);
	afGetSampleFormat(file, AF_DEFAULT_TRACK, &sampleFormat, &sampleWidth);

#if defined(i386) || defined(alpha)
	afSetVirtualByteOrder(file, AF_DEFAULT_TRACK, AF_BYTEORDER_LITTLEENDIAN);
#else
	afSetVirtualByteOrder(file, AF_DEFAULT_TRACK, AF_BYTEORDER_BIGENDIAN);
#endif

	printf("sample format: %d, sample width: %d\n", sampleFormat, sampleWidth);

	if ((sampleWidth != 16) || (channelCount != 2))
	{
		printf("The audio file must be of a 16-bit stereo format.\n");
		exit(0);
	}

	buffer = (char *) malloc(frameCount * (sampleWidth/8) * channelCount);
	afReadFrames(file, AF_DEFAULT_TRACK, buffer, frameCount);

	audiofd = open("/dev/dsp", O_WRONLY);
	if (audiofd < 0)
	{
		perror("open");
		exit(-1);
	}

#if WORDS_BIGENDIAN
	format = AFMT_S16_BE;
#else
	format = AFMT_S16_LE;
#endif
	if (ioctl(audiofd, SNDCTL_DSP_SETFMT, &format) == -1)
	{
		perror("set format");
		exit(-1);
	}

#if WORDS_BIGENDIAN
	if (format != AFMT_S16_BE)
#else
	if (format != AFMT_S16_LE)
#endif
	{
		fprintf(stderr, "format not correct.\n");
		exit(-1);
	}

	channels = 2;
	if (ioctl(audiofd, SNDCTL_DSP_CHANNELS, &channels) == -1)
	{
		perror("set channels");
		exit(-1);
	}

	frequency = 44100;
	if (ioctl(audiofd, SNDCTL_DSP_SPEED, &frequency) == -1)
	{
		perror("set frequency");
		exit(-1);
	}

	write(audiofd, buffer, frameCount * (sampleWidth/8) * channelCount);
	close(audiofd);
}
