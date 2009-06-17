/*
 * $Id$
 */

#include <config.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <avcodec.h>
#include <avformat.h>
#include <assert.h>
#include "../../plugin.h"

#define INBUF_SIZE 2048

static FILE *fp = NULL;
static AVCodecContext *ctx = NULL;
static char comment[COMMENT_ID_NUM][MAX_COMMENT_LEN] = {{0}};
static int duration = 0, sample_rate = 0, channels = 0;

static void strchomp(char *str) {
	char *ptr;

	/* remove trailing whitespace */
	ptr = &str[strlen(str) - 1];

	if (!isspace(*ptr))
		return;
	
	while (isspace(*ptr) && ptr >= str)
		ptr--;

	ptr[1] = 0;
}

static void avdec_init() {
	static unsigned char inbuf[INBUF_SIZE + FF_INPUT_BUFFER_PADDING_SIZE];
	static short outbuf[AVCODEC_MAX_AUDIO_FRAME_SIZE];
	int read, written;

	memset(&inbuf[INBUF_SIZE], 0, FF_INPUT_BUFFER_PADDING_SIZE);
	
	if (!(read = fread(inbuf, 1, INBUF_SIZE, fp)))
		return;

	avcodec_decode_audio(ctx, outbuf, &written, inbuf, read);
	
	sample_rate = ctx->sample_rate;
	channels = ctx->channels;
}

static void parse_mp3_comments(const char *file) {
	AVFormatContext *fmt = NULL;
	int i;

	for (i = 0; i < COMMENT_ID_NUM; i++)
		comment[i][0] = 0;
	
	if (av_open_input_file(&fmt, file, NULL, 0, NULL))
		return;

	snprintf(comment[COMMENT_ID_ARTIST], MAX_COMMENT_LEN, "%s",
	         fmt->author);
	
	snprintf(comment[COMMENT_ID_TITLE], MAX_COMMENT_LEN, "%s",
	         fmt->title);
	
	snprintf(comment[COMMENT_ID_ALBUM], MAX_COMMENT_LEN, "%s",
	         fmt->album);

	for (i = 0; i < COMMENT_ID_NUM; i++)
		if (comment[i][0])
			strchomp(comment[i]);

	return;
}

void avdec_close() {
	if (fp) {
		fclose(fp);
		fp = NULL;
	}

	if (ctx) {
		avcodec_close(ctx);
		free(ctx);
		ctx = NULL;
	}
}

int avdec_open(const char *file) {
	AVCodec *codec;
	
	if (!file || !*file)
		return 0;

	if (!(fp = fopen(file, "rb")))
		return 0;

	if (!(codec = avcodec_find_decoder(CODEC_ID_MP3))) {
		fprintf(stderr, "AVCODEC: Cannot find MP3 codec!\n");
		fclose(fp);
		return 0;
	}

	if (!(ctx = avcodec_alloc_context())) {
		fprintf(stderr, "AVCODEC: Cannot allocate context!\n");
		avdec_close();
		return 0;
	}

	if (avcodec_open(ctx, codec) < 0) {
		fprintf(stderr, "AVCODEC: Cannot open codec!\n");
		avdec_close();
		return 0;
	}
	
	/* read a chunk of the file so that we can get the
	 * sample rate and number of channels
	 */
	avdec_init();
	
	parse_mp3_comments(file);
	
	return 1;
}

char *avdec_get_comment(CommentID id) {
	return comment[id];
}

int avdec_get_duration() {
	return duration;
}

int avdec_get_channels() {
	return channels;
}

int avdec_get_sample_rate() {
	return sample_rate;
}

int avdec_read(unsigned char **buf) {
#define OUTBUF_SIZE 65536
	static unsigned char inbuf[INBUF_SIZE + FF_INPUT_BUFFER_PADDING_SIZE];
	static unsigned char outbuf[OUTBUF_SIZE];
	static short tmp[AVCODEC_MAX_AUDIO_FRAME_SIZE];
	unsigned char *ptr = inbuf;
	int read = 0, decoded, written, total = 0;

	*buf = outbuf;

	memset(&inbuf[INBUF_SIZE], 0, FF_INPUT_BUFFER_PADDING_SIZE);
	
	if (!(read = fread(inbuf, 1, INBUF_SIZE, fp)))
		return EOF;
	
	while (read > 0) {
		written = 0;

		decoded = avcodec_decode_audio(ctx, tmp, &written, ptr, read);

		if (decoded < 0) {
			fprintf(stderr, "AVCODEC: Error while decoding\n");
			return EOF;
		}
		
		if (written) {
			/* make sure we don't overflow the output buffer */
			assert(total + written <= OUTBUF_SIZE);
			memcpy(&outbuf[total], tmp, written);
			total += written;
		}
			
		read -= decoded;
		ptr += decoded;
	}
	
	return total;
}

int avdec_get_current_pos() {
	return 0;
}

void avdec_set_current_pos(int pos) {
}

int plugin_init(InputPlugin *ip) {
	ip->name = strdup("AVcodec");

	avcodec_init();
	register_avcodec(&mp3_decoder);
	av_register_all();

	ip->open = avdec_open;
	ip->close = ip->shutdown = avdec_close;

	ip->get_comment = avdec_get_comment;
	ip->get_duration = avdec_get_duration;
	ip->get_channels = avdec_get_channels;
	ip->get_sample_rate = avdec_get_sample_rate;
	
	ip->read = avdec_read;
	
	ip->get_current_pos = avdec_get_current_pos;
	ip->set_current_pos = avdec_set_current_pos;

	return 1;
}

