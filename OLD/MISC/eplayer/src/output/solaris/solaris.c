/*
 * $Id$
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <assert.h>
#include <config.h>
#include "../../plugin.h"
#include <sys/audioio.h>

static int fd = -1;

void solaris_shutdown() {
	if (fd != -1) {
		close(fd);
		fd = -1;
	}
}

int solaris_configure(int channels, int rate, int bits) {
	audio_info_t info;

	assert(fd != -1);

	AUDIO_INITINFO(&info);

	info.play.encoding = AUDIO_ENCODING_LINEAR;
	info.play.precision = bits;
	info.play.sample_rate = rate;
	info.play.channels = channels;

	return (ioctl(fd, AUDIO_SETINFO, &info) != -1);
}

int solaris_play(unsigned char *data, int len) {
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

int solaris_volume_get(int *left, int *right) {
	int ctl;
	int vol, bal;
	audio_info_t info;

	if ((ctl = (open("/dev/audioctl", O_WRONLY))) == -1)
		return 0;

	if (ioctl(ctl, AUDIO_GETINFO, &info) == -1)
		return 0;

	vol = info.play.gain * 100 / (AUDIO_MAX_GAIN - AUDIO_MIN_GAIN);
	bal = info.play.balance;

	if (bal == AUDIO_MID_BALANCE)
		*left = *right = vol;
	else if (bal > AUDIO_MID_BALANCE) {
		*left = vol * (AUDIO_RIGHT_BALANCE - bal) / AUDIO_MID_BALANCE;
		*right = vol;
	} else {
		*left = vol;
		*right = vol * bal / AUDIO_MID_BALANCE;
	}

	close(ctl);

	return 1;
}

int solaris_volume_set(int left, int right) {
	int ctl, vol, ret;
	audio_info_t info;

	if ((ctl = (open("/dev/audioctl", O_WRONLY))) == -1)
		return 0;

	AUDIO_INITINFO(&info);

	vol = left > right ? left : right;
	info.play.gain = vol * (AUDIO_MAX_GAIN - AUDIO_MIN_GAIN) / 100;

	if (left == right)
		info.play.balance = AUDIO_MID_BALANCE;
	else
		info.play.balance = (vol - left + right)
			* AUDIO_RIGHT_BALANCE / vol * 2;
	
	ret = ioctl(ctl, AUDIO_SETINFO, &info);
	close(ctl);

	return (ret != -1);
}

int plugin_init(OutputPlugin *op) {
	op->name = strdup("Solaris");

	op->configure = solaris_configure;
	op->play = solaris_play;
	op->shutdown = solaris_shutdown;

	return ((fd = open("/dev/audio", O_WRONLY, 0)) != -1);
}
