#include <config.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "mixer.h"

Mixer *mixer_new(MixerControl ctrl) {
	Mixer *mixer;

	if (!(mixer = malloc(sizeof(Mixer))))
		return NULL;

	mixer->control = ctrl;
	mixer->volume = 0;

	return mixer;
}

void mixer_free(Mixer *mixer) {
	if (!mixer)
		return;

	free(mixer);
}

static int open_mixer(Mixer *mixer, int rw) {
	int fd, flags = rw ? O_RDWR : O_RDONLY;
	
	if (!mixer)
		return -1;
	
	if ((fd = open("/dev/mixer", flags)) != -1)
		return fd;
	else
		return open("/dev/mixer0", flags);
}

/**
 * Reads a Mixer.
 *
 * @param mixer The Mixer to read
 * @return The volume the mixer is set to.
 */
int mixer_read(Mixer *mixer) {
	int vol = 0, fd;

	if ((fd = open_mixer(mixer, 0)) == -1) {
		fprintf(stderr, "MIXER: Can't open mixer device\n");
		return -1;
	}

	if (ioctl(fd, MIXER_READ(mixer->control), &vol) == -1) {
		fprintf(stderr, "MIXER: Can't read from mixer\n");
		close(fd);
		return -1;
	}

	close(fd);

	/* Average between left and right */
	return (mixer->volume = ((vol & 0xFF) + ((vol >> 8) & 0xFF)) / 2);
}

/**
 * Changes the volume of a Mixer.
 *
 * @param mixer The Mixer to set
 * @param vol The value to add to the current volume.
 * @return Boolean success or failure.
 */
int mixer_change(Mixer *mixer, int diff) {
	int fd, ret;

	if ((fd = open_mixer(mixer, 1)) == -1) {
		fprintf(stderr, "MIXER: Can't open mixer device\n");
		return 0;
	}

	diff += mixer->volume;

	/* vol is > 0 and < 100 */
	if (diff > 100)
		diff = 100;
	else if (diff < 0)
		diff = 0;

	/* save the new value */
	mixer->volume = diff;

	diff = diff | (diff << 8);
	
	if ((ret = ioctl(fd, MIXER_WRITE(mixer->control), &diff)) == 1)
		fprintf(stderr, "MIXER: Can't set mixer\n");
	
	close(fd);

	return (ret != -1);
}

