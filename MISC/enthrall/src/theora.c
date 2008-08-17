/*
 * Copyright (C) 2002-2003 Xiph.org Foundation http://xiph.org/
 * Copyright (C) 2006 Tilman Sauerbeck (tilman at code-monkey de)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * - Neither the name of the Xiph.org Foundation nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#define _GNU_SOURCE
#define _LARGEFILE_SOURCE
#define _LARGEFILE64_SOURCE
#define _FILE_OFFSET_BITS 64

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <assert.h>

#include "theora.h"
#include "rgb2yuv420.h"

void
enthrall_theora_encode_frame (EnthrallTheora *et, uint32_t *data)
{
	ogg_page page;
	ogg_packet op;

	/* If data is NULL, we assume that it's the last frame.
	 *
	 * FIXME:
	 * According to this ticket
	 * https://trac.xiph.org/changeset/11119
	 * it seems we can just put in an empty packet to repeat
	 * the last frame.
	 */
	if (data)
		rgb2yuv420 (data, et->yuv.y_width, et->yuv.y_height, et->yuv.y, et->yuv.u, et->yuv.v);

	/* Theora is a one-frame-in, one-frame-out system;
	 * submit a frame for compression and pull out the packet.
	 */
	theora_encode_YUVin (&et->td, &et->yuv);
	theora_encode_packetout (&et->td, data == NULL, &op);

	ogg_stream_packetin (&et->to, &op);

	/* page completed? */
	if (ogg_stream_pageout (&et->to, &page) > 0) {
		/* FIXME: handle short writes */
		fwrite (page.header, 1, page.header_len, et->file);
		fwrite (page.body, 1, page.body_len, et->file);
	}
}

bool
enthrall_theora_init (EnthrallTheora *et, const char *filename,
                      int quality, int *width, int *height,
                      int *offset_x, int *offset_y)
{
	ogg_stream_state vo;
	ogg_page og; /* one Ogg bitstream page. Vorbis packets are inside */
	ogg_packet op; /* one raw packet of data for decode */
	theora_info ti;
	theora_comment tc;
	int serial1, serial2;

	et->file = fopen (filename, "wb");
	if (!et->file)
		return false;

	/* yayness.  Set up Ogg output stream */
	srand (time (NULL));

	/* need two inequal serial numbers */
	serial1 = rand ();
	serial2 = rand ();

	if (serial1 == serial2)
		serial2++;

	ogg_stream_init (&et->to, serial1);
	ogg_stream_init (&vo, serial2);

	/* Set up Theora encoder */
	theora_info_init (&ti);

	ti.frame_width = *width;
	ti.frame_height = *height;

	/* Theora has a divisible-by-sixteen restriction for the encoded
	 * video size.
	 * scale the frame size up to the nearest /16 and calculate offsets.
	 */
	ti.width = *width = (ti.frame_width + 15) & ~15;
	ti.height = *height = (ti.frame_height + 15) & ~15;

	/* We force the offset to be even.
	 * This ensures that the chroma samples align properly with
	 * the luma samples.
	 */
	ti.offset_x = *offset_x = ((ti.width - ti.frame_width) / 2) & ~1;
	ti.offset_y = *offset_y = ((ti.height - ti.frame_height) / 2) & ~1;

	/* 25 fps */
	ti.fps_numerator = 25;
	ti.fps_denominator = 1;

	/* FIXME */
	ti.aspect_numerator = 0;
	ti.aspect_denominator = 0;

	ti.colorspace = OC_CS_UNSPECIFIED;
	ti.pixelformat = OC_PF_420;
	ti.target_bitrate = 100000; /* bit/s */

	ti.quality = 63 * (float) (quality / 100.0);
	ti.dropframes_p = 0;
	ti.quick_p = 1;
	ti.keyframe_auto_p = 1;
	ti.keyframe_frequency = 64;
	ti.keyframe_frequency_force = 64;
	ti.keyframe_data_target_bitrate = ti.target_bitrate * 1.5;
	ti.keyframe_auto_threshold = 80;
	ti.keyframe_mindistance = 8;
	ti.noise_sensitivity = 1;

	if (theora_encode_init (&et->td, &ti))
		return false;

	et->yuv.y = malloc (ti.width * ti.height);
	et->yuv.u = malloc (ti.width * ti.height / 4);
	et->yuv.v = malloc (ti.width * ti.height / 4);

	et->yuv.y_width = ti.width;
	et->yuv.y_height = ti.height;
	et->yuv.y_stride = et->yuv.y_width;

	et->yuv.uv_width = ti.width / 2;
	et->yuv.uv_height = ti.height / 2;
	et->yuv.uv_stride = et->yuv.uv_width;

	theora_info_clear (&ti);

	/* write the bitstream header packets with proper page interleave */

	/* first packet will get its own page automatically */
	theora_encode_header (&et->td, &op);
	ogg_stream_packetin (&et->to, &op);

	if (ogg_stream_pageout (&et->to, &og) != 1) {
		fprintf (stderr, "Internal Ogg library error.\n");
		return false;
	}

	/* FIXME: handle short writes */
	fwrite (og.header, 1, og.header_len, et->file);
	fwrite (og.body, 1, og.body_len, et->file);

	/* create the remaining theora headers */
	theora_comment_init (&tc);
	theora_encode_comment (&tc, &op);
	ogg_stream_packetin (&et->to, &op);

	theora_encode_tables (&et->td, &op);
	ogg_stream_packetin (&et->to, &op);

	/* Flush the rest of our headers.
	 * This ensures the actual data in each stream will start
	 * on a new page, as per spec.
	 */
	while (1) {
		int result = ogg_stream_flush (&et->to, &og);

		if (result < 0) {
			/* can't get here */
			fprintf (stderr, "Internal Ogg library error.\n");
			return false;
		}

		if (!result)
			break;

		/* FIXME: handle short writes */
		fwrite (og.header, 1, og.header_len, et->file);
		fwrite (og.body, 1, og.body_len, et->file);
	}

	return true;
}

void
enthrall_theora_finish (EnthrallTheora *et)
{
	ogg_stream_clear (&et->to);
	theora_clear (&et->td);

	fclose (et->file);

	free (et->yuv.y);
	free (et->yuv.u);
	free (et->yuv.v);
}
