/*
 * $Id$
 */

#include <config.h>
#include <string.h>
#include <vorbis/vorbisfile.h>
#include <vorbis/codec.h>
#include "../../plugin.h"

static OggVorbis_File track = {0};
static char comment[COMMENT_ID_NUM][MAX_COMMENT_LEN] = {{0}};
static int channels = 0, sample_rate = 0, duration = 0, bitrate = 0;

static void parse_comments(vorbis_comment *vc) {
	char *cmt, *key[COMMENT_ID_NUM] = {NULL};
	int i, j, len[COMMENT_ID_NUM] = {0};
	
	if (!vc)
		return;

	key[COMMENT_ID_ARTIST] = "artist=";
	key[COMMENT_ID_TITLE] = "title=";
	key[COMMENT_ID_ALBUM] = "album=";
	
	for (i = 0; i < COMMENT_ID_NUM; i++) {
		comment[i][0] = 0;
		len[i] = strlen(key[i]);
	}

	for (i = 0; i < vc->comments; i++) {
		cmt = vc->user_comments[i];

		for (j = 0; j < COMMENT_ID_NUM; j++)
			if (!strncasecmp(cmt, key[j], len[j]))
				snprintf(comment[j], MAX_COMMENT_LEN, "%s",
				         &cmt[len[j]]);
	}
}

int vorbis_open(const char *file) {
	FILE *fp;
	vorbis_info *info;
	
	if (!file || !*file)
		return 0;

	if (!(fp = fopen(file, "rb")))
		return 0;

	if (ov_open(fp, &track, NULL, 0)) {
		fclose(fp);
		return 0;
	}

	parse_comments(ov_comment(&track, -1));
	
	info = ov_info(&track, -1);
	
	sample_rate = info->rate;
	channels = info->channels;
	duration = ov_time_total(&track, -1);
	bitrate = ov_bitrate(&track, -1);

	return 1;
}

void vorbis_close() {
	/* ov_clear() closes the file, too */
	ov_clear(&track);
}

char *vorbis_get_comment(CommentID id) {
	return comment[id];
}

int vorbis_get_duration() {
	return duration;
}

int vorbis_get_channels() {
	return channels;
}

int vorbis_get_sample_rate() {
	return sample_rate;
}

int vorbis_get_bitrate() {
	return bitrate;
}

int vorbis_read(unsigned char **buf) {
#define BUF_SIZE 4096
	static unsigned char outbuf[BUF_SIZE];
	int ret;
	
#ifdef WORDS_BIGENDIAN
	static int bigendian = 1;
#else
	static int bigendian = 0;
#endif

	*buf = outbuf;
	ret = ov_read(&track, *buf, BUF_SIZE, bigendian, 2, 1, NULL);
	
	return ret ? ret : EOF;
}

int vorbis_get_current_pos() {
	return ov_time_tell(&track);
}

void vorbis_set_current_pos(int pos) {
	ov_time_seek(&track, pos);
}

int plugin_init(InputPlugin *ip) {
	ip->name = strdup("Vorbis");

	ip->open = vorbis_open;
	ip->close = ip->shutdown = vorbis_close;

	ip->get_comment = vorbis_get_comment;
	ip->get_duration = vorbis_get_duration;
	ip->get_channels = vorbis_get_channels;
	ip->get_sample_rate = vorbis_get_sample_rate;
	ip->get_bitrate = vorbis_get_bitrate;
	
	ip->read = vorbis_read;
	
	ip->get_current_pos = vorbis_get_current_pos;
	ip->set_current_pos = vorbis_set_current_pos;

	return 1;
}

