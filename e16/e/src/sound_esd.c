/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2007 Kim Woelders
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "E.h"
#if defined(HAVE_SOUND) && defined(HAVE_LIBESD)
#include "sound.h"
#include <esd.h>
#include <audiofile.h>

struct _sample
{
   char               *file;	/* We should not need this */
   int                 rate;
   int                 format;
   int                 samples;
   unsigned char      *data;
   int                 id;
};

static int          sound_fd = -1;

static Sample      *
_esd_Load(const char *file)
{
   AFfilehandle        in_file;
   Sample             *s;
   int                 in_format, in_width, in_channels, frame_count;
   int                 bytes_per_frame, frames_read;
   double              in_rate;

   in_file = afOpenFile(file, "r", NULL);
   if (!in_file)
      return NULL;

   s = EMALLOC(Sample, 1);
   if (!s)
     {
	afCloseFile(in_file);
	return NULL;
     }

   frame_count = afGetFrameCount(in_file, AF_DEFAULT_TRACK);
   in_channels = afGetChannels(in_file, AF_DEFAULT_TRACK);
   in_rate = afGetRate(in_file, AF_DEFAULT_TRACK);
   afGetSampleFormat(in_file, AF_DEFAULT_TRACK, &in_format, &in_width);
#ifdef WORDS_BIGENDIAN
   afSetVirtualByteOrder(in_file, AF_DEFAULT_TRACK, AF_BYTEORDER_BIGENDIAN);
#else
   afSetVirtualByteOrder(in_file, AF_DEFAULT_TRACK, AF_BYTEORDER_LITTLEENDIAN);
#endif
   s->file = Estrdup(file);
   s->rate = 44100;
   s->format = ESD_STREAM | ESD_PLAY;
   s->samples = 0;
   s->data = NULL;
   s->id = 0;

   if (in_width == 8)
      s->format |= ESD_BITS8;
   else if (in_width == 16)
      s->format |= ESD_BITS16;
   bytes_per_frame = (in_width * in_channels) / 8;
   if (in_channels == 1)
      s->format |= ESD_MONO;
   else if (in_channels == 2)
      s->format |= ESD_STEREO;
   s->rate = (int)in_rate;

   s->samples = frame_count * bytes_per_frame;
   s->data = EMALLOC(unsigned char, frame_count * bytes_per_frame);

   frames_read = afReadFrames(in_file, AF_DEFAULT_TRACK, s->data, frame_count);
   afCloseFile(in_file);

   return s;
}

static void
_esd_Destroy(Sample * s)
{
   if (s->id && sound_fd >= 0)
     {
/*      Why the hell is this symbol not in esd? */
/*      it's in esd.h - evil evil evil */
/*      esd_sample_kill(sound_fd,s->id); */
	esd_sample_free(sound_fd, s->id);
     }
   if (s->data)
      Efree(s->data);
   if (s->file)
      Efree(s->file);
   if (s)
      Efree(s);
}

static void
_esd_Play(Sample * s)
{
   int                 size, confirm = 0;

   if (sound_fd < 0 || !s)
      return;

   if (!s->id && s->data)
     {
	size = s->samples;
	s->id = esd_sample_getid(sound_fd, s->file);
	if (s->id < 0)
	  {
	     s->id =
		esd_sample_cache(sound_fd, s->format, s->rate, size, s->file);
	     write(sound_fd, s->data, size);
	     confirm = esd_confirm_sample_cache(sound_fd);
	     if (confirm != s->id)
		s->id = 0;
	  }
	Efree(s->data);
	s->data = NULL;
     }
   if (s->id > 0)
      esd_sample_play(sound_fd, s->id);
}

static int
_esd_Init(void)
{
   if (sound_fd >= 0)
      return 0;

   sound_fd = esd_open_sound(NULL);

   return sound_fd < 0;
}

static void
_esd_Exit(void)
{
   if (sound_fd < 0)
      return;

   close(sound_fd);
   sound_fd = -1;
}

extern const SoundOps SoundOps_esd;
const SoundOps      SoundOps_esd = {
   _esd_Init, _esd_Exit, _esd_Load, _esd_Destroy, _esd_Play,
};

#endif /* HAVE_SOUND && HAVE_LIBESD */
