#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>


typedef struct {
	AVOutputFormat  *fmt;
	AVFormatContext *oc;
	AVStream        *video_st;
	uint8_t         *video_outbuf;
	AVFrame         *picture_rgb;
	AVFrame         *picture_yuv;
	int              video_outbuf_size;
	int              frame_count;
} EnthrallFFmpeg;


void *
enthrall_ffmpeg_new ()
{
	EnthrallFFmpeg *ef;

	ef = (EnthrallFFmpeg *)malloc (sizeof (EnthrallFFmpeg));
	if (!ef)
		return NULL;
	return ef;
}


bool
enthrall_ffmpeg_init (void       *engine,
		      const char *filename,
		      int         bitrate,
		      int        *width,
		      int        *height,
		      int        *offset_x,
		      int        *offset_y)
{
  EnthrallFFmpeg  *ef;
  AVOutputFormat  *fmt;
  AVFormatContext *oc;
  AVStream        *video_st;
  uint8_t         *video_outbuf;
  AVFrame         *picture_rgb;
  AVFrame         *picture_yuv;
  uint8_t         *picture_buf;
  int              video_outbuf_size;
  int              size;

  ef = (EnthrallFFmpeg *)engine;

  av_register_all();

  fmt = guess_format (NULL, filename, NULL);
  if (!fmt) {
    fmt = guess_format ("avi", NULL, NULL);
  }
  if (!fmt) {
    fprintf (stderr, "Could not find suitable output format\n");
    return 0;
  }

  oc = av_alloc_format_context ();
  if (!oc) {
    fprintf (stderr, "Canot allocate memory for the output format\n");
    return 0;
  }
  oc->oformat = fmt;
  snprintf (oc->filename, sizeof(oc->filename), "%s", filename);

  video_st = NULL;
  /* this test should be useless: avi format can always contain video */
  if (fmt->video_codec != CODEC_ID_NONE) {
    AVCodecContext *c;

    /* add video stream */
    video_st = av_new_stream(oc, 0);
    if (!video_st) {
      fprintf (stderr, "Canot allocate memory for the video stream\n");
      av_free(oc);
      return 0;
    }

    c = video_st->codec;
    c->codec_id = CODEC_ID_FFVHUFF;//fmt->video_codec;
    printf ("codec id : %d %d\n", c->codec_id, CODEC_ID_FFVHUFF);
    c->codec_type = CODEC_TYPE_VIDEO;

    /*************************/
    /* put sample parameters */
    /*************************/

    /* this one should be useless with FFVHUFF */
    c->bit_rate = bitrate * 1000; /* 150 default */

    /* resolution must be a multiple of two */
    c->width = *width;
    c->height = *height;

    /* time base: this is the fundamental unit of time (in seconds) in terms
       of which frame timestamps are represented. for fixed-fps content,
       timebase should be 1/framerate and timestamp increments should be
       identically 1. */
    c->time_base.den = 25;
    c->time_base.num = 1;

    /* this one should be useless with FFVHUFF */
    c->gop_size = 12; /* emit one intra frame every twelve frames at most */

    c->pix_fmt = PIX_FMT_YUV420P;
  }

  if (av_set_parameters(oc, NULL) < 0) {
    fprintf (stderr, "Invalid output format parameters\n");
    av_free(oc);
    return 0;
  }

  dump_format(oc, 0, filename, 1);

  /* this test should be useless as fmt->video_codec != CODEC_ID_NONE */
  if (video_st) {
    AVCodec        *codec;
    AVCodecContext *c;

    c = video_st->codec;

    /* find the video encoder */
    codec = avcodec_find_encoder(c->codec_id);
    if (!codec) {
        fprintf(stderr, "codec not found\n");
	av_free(oc);
	return 0;
    }

    /* open the codec */
    if (avcodec_open(c, codec) < 0) {
        fprintf(stderr, "could not open codec\n");
	av_free(oc);
	return 0;
    }

    video_outbuf = NULL;
    if (!(oc->oformat->flags & AVFMT_RAWPICTURE)) {
        /* allocate output buffer */
        /* XXX: API change will be done */
        /* buffers passed into lav* can be allocated any way you prefer,
           as long as they're aligned enough for the architecture, and
           they're freed appropriately (such as using av_free for buffers
           allocated with av_malloc) */
        video_outbuf_size = 200000;
        video_outbuf = av_malloc(video_outbuf_size);
    }

    /* allocate the input RGBA picture */
    picture_rgb = avcodec_alloc_frame ();
    if (!picture_rgb) {
        fprintf(stderr, "Could not allocate picture\n");
	av_free(oc);
	return 0;
    }

    /* allocate the encoded raw picture */
    picture_yuv = avcodec_alloc_frame ();
    if (!picture_yuv) {
        fprintf(stderr, "Could not allocate picture\n");
	av_free(oc);
	return 0;
    }
    size = avpicture_get_size (c->pix_fmt, c->width, c->height);
    picture_buf = av_malloc(size);
    if (!picture_buf) {
        fprintf (stderr, "Could not allocate memory for picture buffer\n");
        av_free (picture_yuv);
	av_free (oc);
	return 0;
    }
    avpicture_fill((AVPicture *)picture_yuv, picture_buf,
                   c->pix_fmt, c->width, c->height);
  }

  /* open the output file, if needed */
  if (!(fmt->flags & AVFMT_NOFILE)) {
    if (url_fopen (&oc->pb, filename, URL_WRONLY) < 0) {
      fprintf (stderr, "Could not open '%s'\n", filename);
      av_free(oc);
      return 0;
    }
  }

  /* write the stream header, if any */
  av_write_header(oc);

  printf ("size : %d %d\n", video_st->codec->width, video_st->codec->height);

  ef->fmt = fmt;
  ef->oc = oc;
  ef->video_st = video_st;
  ef->video_outbuf = video_outbuf;
  ef->video_outbuf_size = video_outbuf_size;
  ef->picture_rgb = picture_rgb;
  ef->picture_yuv = picture_yuv;
  ef->frame_count = 0;

  return 1;
}

void
enthrall_ffmpeg_finish (void *engine)
{
  EnthrallFFmpeg *ef;
  int             i;

  ef = (EnthrallFFmpeg *)engine;

  if (ef->video_st) {
    avcodec_close(ef->video_st->codec);
  }

  if (ef->picture_rgb) {
    av_free(ef->picture_rgb);
  }

  if (ef->picture_yuv) {
    av_free(ef->picture_yuv->data[0]);
    av_free(ef->picture_yuv);
  }

  if (ef->video_outbuf)
    av_free(ef->video_outbuf);

  av_write_trailer (ef->oc);

  for(i = 0; i < ef->oc->nb_streams; i++) {
    av_freep (&ef->oc->streams[i]->codec);
    av_freep (&ef->oc->streams[i]);
  }

  if (!(ef->fmt->flags & AVFMT_NOFILE)) {
    /* close the output file */
    url_fclose (ef->oc->pb);
  }

  av_free(ef->oc);

  free(engine);
}

void
enthrall_ffmpeg_encode_frame (void     *engine,
                              uint32_t *data)
{
  EnthrallFFmpeg *ef;
  AVCodecContext *c;
  int             out_size;
  int             ret;

  if (!data) return;

  ef = (EnthrallFFmpeg *)engine;

  c = ef->video_st->codec;

  avpicture_fill((AVPicture *)ef->picture_rgb, (uint8_t *)data,
		 PIX_FMT_RGB32, c->width, c->height);
  img_convert((AVPicture*)ef->picture_yuv, c->pix_fmt,
	      (AVPicture *)ef->picture_rgb, PIX_FMT_RGB32,
	      c->width, c->height);


  if (ef->oc->oformat->flags & AVFMT_RAWPICTURE) {
    /* raw video case. The API will change slightly in the near
       futur for that */
    AVPacket pkt;
    av_init_packet(&pkt);

    pkt.flags |= PKT_FLAG_KEY;
    pkt.stream_index= ef->video_st->index;
    pkt.data= (uint8_t *)ef->picture_yuv;
    pkt.size= sizeof(AVPicture);

    ret = av_write_frame(ef->oc, &pkt);
  }
  else {
    /* encode the image */
    out_size = avcodec_encode_video(c, ef->video_outbuf, ef->video_outbuf_size, ef->picture_yuv);
    /* if zero size, it means the image was buffered */
    if (out_size > 0) {
      AVPacket pkt;
      av_init_packet(&pkt);

      if (c->coded_frame->pts != AV_NOPTS_VALUE)
        pkt.pts= av_rescale_q(c->coded_frame->pts, c->time_base, ef->video_st->time_base);
      if(c->coded_frame->key_frame)
	pkt.flags |= PKT_FLAG_KEY;
      pkt.stream_index= ef->video_st->index;
      pkt.data= ef->video_outbuf;
      pkt.size= out_size;

      /* write the compressed frame in the media file */
      {
	AVStream *st;
/*	AVFrac *f; */

	st = ef->oc->streams[pkt.stream_index];
/*	f = &st->pts; */
      }
      ret = av_write_frame(ef->oc, &pkt);
    } else {
      ret = 0;
    }
  }

  if (ret != 0) {
    fprintf(stderr, "Error while writing video frame\n");
    return;
  }

  ef->frame_count++;
}
