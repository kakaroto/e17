/*
 * $Id$
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <assert.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <config.h>
#include "../../plugin.h"

#ifdef HAVE_SYS_SOUNDCARD_H
# include <sys/soundcard.h>
#elif HAVE_MACHINE_SOUNDCARD_H
# include <machine/soundcard.h>
#endif

static OutputPlugin *op = NULL;
static int fd = -1;
static pthread_cond_t cond;

void oss_shutdown() {
	if (fd != -1) {
		ioctl(fd, SNDCTL_DSP_RESET, 0);
		close(fd);
		fd = -1;
	}

	op = NULL;
}

int oss_configure(int channels, int rate, int bits) {
#ifdef WORDS_BIGENDIAN
	static int bigendian = 1;
#else
	static int bigendian = 0;
#endif

	int format, tmp;

	assert(fd != -1);

	ioctl(fd, SNDCTL_DSP_RESET, 0);

	/* set format */
	if (bits == 8)
		format = AFMT_S8;
	else if (bits == 16)
		format = bigendian ? AFMT_S16_BE : AFMT_S16_LE;

	if (ioctl(fd, SNDCTL_DSP_SETFMT, &format) == -1) {
		op->debug(DEBUG_LEVEL_CRITICAL, "OSS: Cannot set format!\n");
		return 0;
	}
	
	/* set mono/stereo mode */
	tmp = channels - 1;
	if (ioctl(fd, SNDCTL_DSP_STEREO, &tmp) == -1) {
		op->debug(DEBUG_LEVEL_CRITICAL, "OSS: Cannot set channels!\n");
		return 0;
	}

	/* set samplerate */
	tmp = rate;
	if (ioctl(fd, SNDCTL_DSP_SPEED, &tmp) == -1) {
		op->debug(DEBUG_LEVEL_CRITICAL,
		          "OSS: Cannot set samplerate!\n");
		return 0;
	}

	if (tmp != rate)
		op->debug(DEBUG_LEVEL_WARNING,
		      "OSS: Requested samplerate = %i, using %i\n",
		      rate, tmp);

	return 1;
}

int oss_play(unsigned char *data, int len) {
	int written;
	
	assert(fd != -1);

	while (len > 0) {
		if ((written = write(fd, data, len)) > 0) {
			/* success: play next sample */
			len -= written;
			data += written;
		} else
			return 0;
	}
	
	return 1;
}

static int open_mixer(int rw) {
	int mixer, flags = rw ? O_RDWR : O_RDONLY;
	
	if ((mixer = open("/dev/mixer", flags)) != -1)
		return mixer;
	else
		return open("/dev/mixer0", flags);
}

int oss_volume_get(int *left, int *right) {
	int vol = 0, mixer;

	if ((mixer = open_mixer(0)) == -1) {
		fprintf(stderr, "OSS: Can't open mixer device\n");
		return 0;
	}

	if (ioctl(mixer, MIXER_READ(SOUND_MIXER_VOLUME), &vol) == -1) {
		fprintf(stderr, "OSS: Can't read from mixer\n");
		close(mixer);
		return 0;
	}

	close(mixer);
	
	*right = (vol & 0xFF00) >> 8;
	*left = (vol & 0x00FF);

	return 1;
}

int oss_volume_set(int left, int right) {
	int mixer, vol, ret;

	if ((mixer = open_mixer(1)) == -1) {
		fprintf(stderr, "OSS: Can't open mixer device\n");
		return 0;
	}

	vol = left | (right << 8);
	
	if ((ret = ioctl(mixer, MIXER_WRITE(SOUND_MIXER_VOLUME),
	                 &vol)) == -1)
		fprintf(stderr, "MIXER: Can't set mixer\n");
	
	close(mixer);

	return (ret != -1);
}

static void *thread_open(char *file) {
	fd = open(file, O_WRONLY);
	pthread_cond_signal(&cond);
	pthread_exit(NULL);
}

static void open_device () {
	struct timespec ts;
	pthread_mutex_t mutex;
	pthread_t thread;

	/* Set up our time structure */
	ts.tv_sec = time(NULL) + 5; /* Wait 5 seconds */
	ts.tv_nsec = 0;

	/* Spawn and try to open */
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);
	pthread_create(&thread, NULL, (void *) thread_open, "/dev/dsp");

	/* wait until either thread_open() returns or the timeout occurs */
	if (pthread_cond_timedwait(&cond, &mutex, &ts))
		pthread_cancel(thread); /* timeout -> kill the thread */

	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);
}

int plugin_init(OutputPlugin *p) {
	op = p;
	op->name = strdup("OSS");

	op->configure = oss_configure;
	op->play = oss_play;
	op->volume_get = oss_volume_get;
	op->volume_set = oss_volume_set;
	op->shutdown = oss_shutdown;
	
	open_device();

	return (fd != -1);
}
