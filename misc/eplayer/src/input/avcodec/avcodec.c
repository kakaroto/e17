#include <config.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <avcodec.h>
#include <assert.h>
#include "../../plugin.h"

static FILE *fp = NULL;
static AVCodecContext *ctx = NULL;
static int duration = 0;
static int sample_rate = 44100;
static int channels = 2;

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
	
	return 1;
}

char *avdec_get_comment(CommentID id) {
	return "Foo Bar";
}

int avdec_get_duration() {
	return 190;
}

int avdec_get_channels() {
	return channels;
}

int avdec_get_sample_rate() {
	return sample_rate;
}

int avdec_read(unsigned char *outbuf, int outbuf_len) {
#define BUF_SIZE 2048
	static unsigned char inbuf[BUF_SIZE + FF_INPUT_BUFFER_PADDING_SIZE];
	static short tmp[AVCODEC_MAX_AUDIO_FRAME_SIZE];
	char *ptr = inbuf;
	int read = 0, decoded, written, total = 0;

	memset(&inbuf[BUF_SIZE], 0, FF_INPUT_BUFFER_PADDING_SIZE);
	
	if (!(read = fread(inbuf, 1, BUF_SIZE, fp)))
		return 0;
	
	while (read > 0) {
		written = 0;

		decoded = avcodec_decode_audio(ctx, tmp, &written, ptr, read);

		if (decoded < 0) {
			fprintf(stderr, "AVCODEC: Error while decoding\n");
			return 0;
		}
		
		if (written) {
			/* make sure we don't overflow the output buffer */
			assert(total + written <= outbuf_len);
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
	avcodec_init();
	avcodec_register_all();

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

