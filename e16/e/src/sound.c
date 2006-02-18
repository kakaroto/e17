/*
 * Copyright (C) 2000-2006 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2006 Kim Woelders
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
#include "dialog.h"
#include "e16-ecore_list.h"
#include "emodule.h"

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

typedef struct
{
   char               *name;
   char               *file;
   Sample             *sample;
   unsigned int        ref_count;
} SoundClass;

static struct
{
   char                enable;
   char               *theme;
} Conf_sound;

static int          sound_fd = -1;

Ecore_List         *sound_list = NULL;

#ifdef HAVE_LIBESD
static Sample      *
LoadWav(const char *file)
{
   AFfilehandle        in_file;
   char               *find = NULL;
   Sample             *s;
   int                 in_format, in_width, in_channels, frame_count;
   int                 bytes_per_frame, frames_read;
   double              in_rate;

   find = FindFile(file, Mode.theme.path, 0);
   if (!find)
     {
	DialogOK(_("Error finding sound file"),
		 _("Warning!  Enlightenment was unable to load the\n"
		   "following sound file:\n%s\n"
		   "Enlightenment will continue to operate, but you\n"
		   "may wish to check your configuration settings.\n"), file);
	return NULL;
     }

   in_file = afOpenFile(find, "r", NULL);
   if (!in_file)
     {
	Efree(find);
	return NULL;
     }

   s = Emalloc(sizeof(Sample));
   if (!s)
     {
	Efree(find);
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

   return s;
}

static void
SamplePlay(Sample * s)
{
   int                 size, confirm = 0;

   if ((sound_fd < 0) || (!Conf_sound.enable) || (!s))
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
#endif /* HAVE_LIBESD */

static void
SampleDestroy(Sample * s)
{
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
}

static void
_SclassSampleDestroy(void *data, void *user_data __UNUSED__)
{
   SoundClass         *sclass = data;

   if (!sclass || !sclass->sample)
      return;

   SampleDestroy(sclass->sample);
   sclass->sample = NULL;
}

static SoundClass  *
SclassCreate(const char *name, const char *file)
{
   SoundClass         *sclass;

   sclass = Emalloc(sizeof(SoundClass));
   if (!sclass)
      return NULL;

   if (!sound_list)
      sound_list = ecore_list_new();
   ecore_list_prepend(sound_list, sclass);

   sclass->name = Estrdup(name);
   sclass->file = Estrdup(file);
   sclass->sample = NULL;

   return sclass;
}

static void
SclassDestroy(SoundClass * sclass)
{
   if (!sclass)
      return;
   ecore_list_remove_node(sound_list, sclass);
   if (sclass->name)
      Efree(sclass->name);
   if (sclass->file)
      Efree(sclass->file);
   if (sclass->sample)
      SampleDestroy(sclass->sample);
   Efree(sclass);
}

static void
SclassApply(SoundClass * sclass)
{
   if (!sclass || !Conf_sound.enable)
      return;
#ifdef HAVE_LIBESD
   if (!sclass->sample)
      sclass->sample = LoadWav(sclass->file);
   if (sclass->sample)
      SamplePlay(sclass->sample);
   else
      SclassDestroy(sclass);
#endif
}

static int
_SclassMatchName(const void *data, const void *match)
{
   return strcmp(((const SoundClass *)data)->name, match);
}

static SoundClass  *
SclassFind(const char *name)
{
   return ecore_list_find(sound_list, _SclassMatchName, name);
}

void
SoundPlay(const char *name)
{
   SoundClass         *sclass;

   if (!Conf_sound.enable)
      return;

   sclass = SclassFind(name);
   SclassApply(sclass);
}

static int
SoundFree(const char *name)
{
   SoundClass         *sclass;

   sclass = SclassFind(name);
   SclassDestroy(sclass);

   return sclass != NULL;
}

static void
SoundInit(void)
{
#ifdef HAVE_LIBESD
   int                 fd;
#endif

#ifdef HAVE_LIBESD
   if (!Conf_sound.enable)
      return;

   if (sound_fd != -1)
      return;

   fd = esd_open_sound(NULL);
   if (fd >= 0)
      sound_fd = fd;
   else
     {
	AlertX(_("Error initialising sound"), _("OK"), " ", " ",
	       _("Audio was enabled for Enlightenment but there was an error\n"
		 "communicating with the audio server (Esound). Audio will\n"
		 "now be disabled.\n"));
	Conf_sound.enable = 0;
     }
#else
   Conf_sound.enable = 0;
#endif
}

static void
SoundExit(void)
{
   if (sound_fd < 0)
      return;

   ecore_list_for_each(sound_list, _SclassSampleDestroy, NULL);

   close(sound_fd);
   sound_fd = -1;
}

/*
 * Configuration load/save
 */
#include "conf.h"

static int
SoundConfigLoad(void)
{
   int                 err = 0;
   SoundClass         *sc;
   char                s[FILEPATH_LEN_MAX];
   char                s1[FILEPATH_LEN_MAX];
   char                s2[FILEPATH_LEN_MAX];
   int                 i1, ret;
   FILE               *fs;
   char               *file;

   file = ConfigFileFind("sound.cfg", Mode.theme.path, 1);
   if (!file)
      return 0;

   fs = fopen(file, "r");
   Efree(file);
   if (!fs)
      goto done;

   while (GetLine(s, sizeof(s), fs))
     {
	i1 = -1;
	ret = sscanf(s, "%d", &i1);
	if (ret == 1)
	  {
	     switch (i1)
	       {
	       case CONFIG_VERSION:
	       case CONFIG_OPEN:
		  break;
	       case CONFIG_CLOSE:
		  goto done;
	       }
	  }
	else
	  {
	     s1[0] = s2[0] = '\0';
	     ret = sscanf(s, "%4000s %4000s", s1, s2);
	     if (ret != 2)
	       {
		  Eprintf("Ignoring line: %s\n", s);
		  break;
	       }
	     sc = SclassCreate(s1, s2);
	  }
     }
   if (err)
      ConfigAlertLoad(_("Sound"));

 done:
   if (fs)
      fclose(fs);
   return err;
}

/*
 * Sound module
 */

static void
SoundSighan(int sig, void *prm __UNUSED__)
{
   switch (sig)
     {
     case ESIGNAL_INIT:
	SoundInit();
	break;
     case ESIGNAL_CONFIGURE:
	SoundConfigLoad();
	break;
     case ESIGNAL_START:
	if (!Conf_sound.enable)
	   break;
	SoundPlay("SOUND_STARTUP");
	SoundFree("SOUND_STARTUP");
	break;
     case ESIGNAL_EXIT:
/*      if (Mode.wm.master) */
	SoundExit();
	break;
     }
}

/*
 * Configuration dialog
 */

static char         tmp_audio;

static void
CB_ConfigureAudio(Dialog * d __UNUSED__, int val, void *data __UNUSED__)
{
   if (val < 2)
     {
	Conf_sound.enable = tmp_audio;
	if (Conf_sound.enable)
	   SoundInit();
	else
	   SoundExit();
     }
   autosave();
}

static void
SettingsAudio(void)
{
   Dialog             *d;
   DItem              *table, *di;

   d = DialogFind("CONFIGURE_AUDIO");
   if (d)
     {
	SoundPlay("SOUND_SETTINGS_ACTIVE");
	ShowDialog(d);
	return;
     }
   SoundPlay("SOUND_SETTINGS_AUDIO");

   tmp_audio = Conf_sound.enable;

   d = DialogCreate("CONFIGURE_AUDIO");
   DialogSetTitle(d, _("Audio Settings"));

   table = DialogInitItem(d);
   DialogItemTableSetOptions(table, 2, 0, 0, 0);

   if (Conf.dialogs.headers)
      DialogAddHeader(d, "pix/sound.png",
		      _("Enlightenment Audio\n" "Settings Dialog\n"));

#ifdef HAVE_LIBESD
   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Enable sounds"));
   DialogItemCheckButtonSetPtr(di, &tmp_audio);
#else
   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di,
		     _("Audio not available since EsounD was not\n"
		       "present at the time of compilation."));
#endif

   DialogAddFooter(d, DLG_OAC, CB_ConfigureAudio);

   ShowDialog(d);
}

/*
 * IPC functions
 */

static void
SoundIpc(const char *params, Client * c __UNUSED__)
{
   const char         *p;
   char                cmd[128], prm[4096];
   int                 len;
   SoundClass         *sc;

   cmd[0] = prm[0] = '\0';
   p = params;
   if (p)
     {
	len = 0;
	sscanf(p, "%100s %4000s %n", cmd, prm, &len);
	p += len;
     }

   if (!strncmp(cmd, "cfg", 3))
     {
	SettingsAudio();
     }
   else if (!strncmp(cmd, "del", 3))
     {
	SoundFree(prm);
     }
   else if (!strncmp(cmd, "list", 2))
     {
	ECORE_LIST_FOR_EACH(sound_list, sc) IpcPrintf("%s\n", sc->name);
     }
   else if (!strncmp(cmd, "new", 3))
     {
	SclassCreate(prm, p);
     }
   else if (!strncmp(cmd, "off", 2))
     {
	Conf_sound.enable = 0;
	SoundExit();
     }
   else if (!strncmp(cmd, "on", 2))
     {
	Conf_sound.enable = 1;
	SoundInit();
     }
   else if (!strncmp(cmd, "play", 2))
     {
	SoundPlay(prm);
     }
}

static const IpcItem SoundIpcArray[] = {
   {
    SoundIpc,
    "sound", "snd",
    "Sound functions",
    "  sound add <classname> <filename> Create soundclass\n"
    "  sound del <classname>            Delete soundclass\n"
    "  sound list                       Show all sounds\n"
    "  sound off                        Disable sounds\n"
    "  sound on                         Enable sounds\n"
    "  sound play <classname>           Play sounds\n"}
};
#define N_IPC_FUNCS (sizeof(SoundIpcArray)/sizeof(IpcItem))

static const CfgItem SoundCfgItems[] = {
   CFG_ITEM_BOOL(Conf_sound, enable, 0),
   CFG_ITEM_STR(Conf_sound, theme),
};
#define N_CFG_ITEMS (sizeof(SoundCfgItems)/sizeof(CfgItem))

/*
 * Module descriptor
 */
EModule             ModSound = {
   "sound", "audio",
   SoundSighan,
   {N_IPC_FUNCS, SoundIpcArray},
   {N_CFG_ITEMS, SoundCfgItems}
};
