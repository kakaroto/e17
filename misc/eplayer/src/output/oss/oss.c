#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <assert.h>
#include <config.h>
#include "../../output_plugin.h"

#ifdef HAVE_SYS_SOUNDCARD_H
# include <sys/soundcard.h>
#elif HAVE_MACHINE_SOUNDCARD_H
# include <machine/soundcard.h>
#endif

static int fd = -1;

void oss_shutdown() {
	if (fd != -1) {
		close(fd);
		fd = -1;
	}
}

int oss_configure(int channels, int rate, int bits, int bigendian) {
	int format, tmp;

	assert(fd != -1);

	/* set format */
	if (bits == 8)
		format = AFMT_S8;
	else if (bits == 16)
		format = bigendian ? AFMT_S16_BE : AFMT_S16_LE;

	if (ioctl(fd, SNDCTL_DSP_SETFMT, &format) == -1)
		return 0;
	
	/* set mono/stereo mode */
	tmp = channels - 1;
	if (ioctl(fd, SNDCTL_DSP_STEREO, &tmp) == -1)
		return 0;

	/* set samplerate */
	tmp = rate;
	if (ioctl(fd, SNDCTL_DSP_SPEED, &tmp) == -1)
		return 0;

	if (tmp != rate)
		printf("OSS: Requested samplerate = %i, using %i\n",
		       rate, tmp);

	return 1;
}

int oss_play(unsigned char *data, int len) {
	assert(fd != -1);

	return (write(fd, data, len) != -1);
}

static int open_mixer(int rw) {
	int fd, flags = rw ? O_RDWR : O_RDONLY;
	
	if ((fd = open("/dev/mixer", flags)) != -1)
		return fd;
	else
		return open("/dev/mixer0", flags);
}

int oss_volume_get(int *left, int *right) {
	int vol = 0, fd;

	if ((fd = open_mixer(0)) == -1) {
		fprintf(stderr, "OSS: Can't open mixer device\n");
		return 0;
	}

	if (ioctl(fd, MIXER_READ(SOUND_MIXER_VOLUME), &vol) == -1) {
		fprintf(stderr, "OSS: Can't read from mixer\n");
		close(fd);
		return 0;
	}

	close(fd);
	
	*right = (vol & 0xFF00) >> 8;
	*left = (vol & 0x00FF);

	return 1;
}

int oss_volume_set(int left, int right) {
	int fd, vol, ret;

	if ((fd = open_mixer(1)) == -1) {
		fprintf(stderr, "OSS: Can't open mixer device\n");
		return 0;
	}

	vol = left | (right << 8);
	
	if ((ret = ioctl(fd, MIXER_WRITE(SOUND_MIXER_VOLUME), &vol)) == 1)
		fprintf(stderr, "MIXER: Can't set mixer\n");
	
	close(fd);

	return (ret != -1);
}

int output_plugin_init(OutputPlugin *op) {
	op->configure = oss_configure;
	op->play = oss_play;
	op->volume_get = oss_volume_get;
	op->volume_set = oss_volume_set;
	op->shutdown = oss_shutdown;

	return ((fd = open("/dev/dsp", O_WRONLY, 0)) != -1);
}
