/*
 * Copyright (C) 2000-2004 Carsten Haitzler, Geoff Harrison and various contributors
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

#ifdef HAVE_LIBESD
#include <esd.h>
#include <audiofile.h>

#ifdef WORDS_BIGENDIAN
#define SWAP_SHORT( x ) x = ( ( x & 0x00ff ) << 8 ) | ( ( x >> 8 ) & 0x00ff )
#define SWAP_LONG( x ) x = ( ( ( x & 0x000000ff ) << 24 ) |\
      ( ( x & 0x0000ff00 ) << 8 ) |\
      ( ( x & 0x00ff0000 ) >> 8 ) |\
      ( ( x & 0xff000000 ) >> 24 ) )
#endif
#endif

typedef struct
{
   char               *file;
   int                 rate;
   int                 format;
   int                 samples;
   unsigned char      *data;
   int                 id;
} Sample;

struct _soundclass
{
   char               *name;
   char               *file;
   Sample             *sample;
   unsigned int        ref_count;
};

static int          sound_fd = -1;

static Sample      *
LoadWav(char *file)
{
#ifdef HAVE_LIBESD
   AFfilehandle        in_file;
   char               *find = NULL;
   Sample             *s;
   int                 in_format, in_width, in_channels, frame_count;
   int                 bytes_per_frame, frames_read;
   double              in_rate;

#endif

   EDBUG(5, "LoadWav");
#ifdef HAVE_LIBESD
   find = FindFile(file);
   if (!find)
     {
	DialogOK(_("Error finding sound file"),
		 _("Warning!  Enlightenment was unable to load the\n"
		   "following sound file:\n%s\n"
		   "Enlightenment will continue to operate, but you\n"
		   "may wish to check your configuration settings.\n"), file);
	EDBUG_RETURN(NULL);
     }
   in_file = afOpenFile(find, "r", NULL);
   if (!in_file)
     {
	Efree(find);
	EDBUG_RETURN(NULL);
     }
   s = Emalloc(sizeof(Sample));
   if (!s)
     {
	Efree(find);
	afCloseFile(in_file);
	EDBUG_RETURN(NULL);
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
   s->file = Estrdup(find);
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
   s->data = Emalloc(frame_count * bytes_per_frame);
   frames_read = afReadFrames(in_file, AF_DEFAULT_TRACK, s->data, frame_count);
   afCloseFile(in_file);
   Efree(find);
   EDBUG_RETURN(s);
#else
   file = NULL;
   EDBUG_RETURN(NULL);
#endif
}

static void
SamplePlay(Sample * s)
{
#ifdef HAVE_LIBESD
   int                 size, confirm = 0;
#endif

   EDBUG(5, "SamplePlay");
#ifdef HAVE_LIBESD
   if ((sound_fd < 0) || (!conf.sound) || (!s))
      EDBUG_RETURN_;
   if (!s->id)
     {
	if (sound_fd >= 0)
	  {
	     if (s->data)
	       {
		  size = s->samples;
		  s->id = esd_sample_getid(sound_fd, s->file);
		  if (s->id < 0)
		    {
		       s->id =
			  esd_sample_cache(sound_fd, s->format, s->rate, size,
					   s->file);
		       write(sound_fd, s->data, size);
		       confirm = esd_confirm_sample_cache(sound_fd);
		       if (confirm != s->id)
			  s->id = 0;
		    }
		  Efree(s->data);
		  s->data = NULL;
	       }
	  }
     }
   if (s->id > 0)
      esd_sample_play(sound_fd, s->id);
#else
   s = NULL;
#endif
   EDBUG_RETURN_;
}

static void
DestroySample(Sample * s)
{
   EDBUG(5, "DestroySample");

#ifdef HAVE_LIBESD
   if ((s->id) && (sound_fd >= 0))
     {
/*      Why the hell is this symbol not in esd? */
/*      it's in esd.h - evil evil evil */
/*      esd_sample_kill(sound_fd,s->id); */
	esd_sample_free(sound_fd, s->id);
     }
#endif
   if (s->data)
      Efree(s->data);
   if (s->file)
      Efree(s->file);
   if (s)
      Efree(s);
   EDBUG_RETURN_;
}

SoundClass         *
SclassCreate(const char *name, const char *file)
{
   SoundClass         *sclass;

   EDBUG(6, "SclassCreate");
   sclass = Emalloc(sizeof(SoundClass));
   if (!sclass)
      EDBUG_RETURN(NULL);
   sclass->name = Estrdup(name);
   sclass->file = Estrdup(file);
   sclass->sample = NULL;
   AddItem(sclass, sclass->name, 0, LIST_TYPE_SCLASS);
   EDBUG_RETURN(sclass);
}

static void
SclassApply(SoundClass * sclass)
{
#ifdef HAVE_LIBESD
   char               *f;
#endif

   EDBUG(4, "SclassApply");
   if (!sclass)
      EDBUG_RETURN_;
#ifdef HAVE_LIBESD
   if ((!sclass->sample) && (conf.sound))
     {
	f = FindFile(sclass->file);
	if (f)
	  {
	     sclass->sample = LoadWav(f);
	     Efree(f);
	  }
     }
   if ((conf.sound) && (sclass->sample))
      SamplePlay(sclass->sample);
#endif
   EDBUG_RETURN_;
}

static void
SclassDestroy(SoundClass * sclass)
{
   EDBUG(5, "SclassDestroy");
   if (!sclass)
      EDBUG_RETURN_;
   RemoveItem(sclass->name, 0, LIST_FINDBY_NAME, LIST_TYPE_SCLASS);
   if (sclass->name)
      Efree(sclass->name);
   if (sclass->file)
      Efree(sclass->file);
   if (sclass->sample)
      DestroySample(sclass->sample);
   Efree(sclass);
   EDBUG_RETURN_;
}

const char         *
SclassGetName(SoundClass * sclass)
{
   return sclass->name;
}

int
SoundPlay(const char *name)
{
   SoundClass         *sclass;

   sclass = FindItem(name, 0, LIST_FINDBY_NAME, LIST_TYPE_SCLASS);
   SclassApply(sclass);
   return sclass != NULL;
}

int
SoundFree(const char *name)
{
   SoundClass         *sclass;

   sclass = FindItem(name, 0, LIST_FINDBY_NAME, LIST_TYPE_SCLASS);
   SclassDestroy(sclass);
   return sclass != NULL;
}

void
SoundInit(void)
{
#ifdef HAVE_LIBESD
   int                 fd;
#endif

   EDBUG(5, "SoundInit");
#ifdef HAVE_LIBESD
   if (!conf.sound)
      EDBUG_RETURN_;
   if (sound_fd != -1)
      EDBUG_RETURN_;
   fd = esd_open_sound(NULL);
   if (fd >= 0)
      sound_fd = fd;
   else
     {
	AlertX(_("Error initialising sound"), _("OK"), " ", " ",
	       _("Audio was enabled for Enlightenment but there was an error\n"
		 "communicating with the audio server (Esound). Audio will\n"
		 "now be disabled.\n"));
	conf.sound = 0;
     }
#else
   conf.sound = 0;
#endif
   EDBUG_RETURN_;
}

void
SoundExit(void)
{
   SoundClass        **lst;
   int                 num, i;

   EDBUG(6, "SoundExit");
   if (sound_fd >= 0)
     {
	lst = (SoundClass **) ListItemType(&num, LIST_TYPE_SCLASS);
	if (lst)
	  {
	     for (i = 0; i < num; i++)
	       {
		  if (lst[i]->sample)
		     DestroySample(lst[i]->sample);
		  lst[i]->sample = NULL;
	       }
	     Efree(lst);
	  }
	close(sound_fd);
	sound_fd = -1;
     }
   EDBUG_RETURN_;
}
