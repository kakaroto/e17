/* esdplay.c - part of esdplay
 * Copyright (C) 1998 Simon Kågedal
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License in the file COPYING for more details.
 */

#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <audiofile.h>
#include <esd.h>

static char *program_name = NULL;

#if 0
static int
play_file (const char *filename)
{

  char buf[ESD_BUF_SIZE];
  int buf_frames;
  int frames_read = 0, bytes_written = 0;

  /* input from libaudiofile... */

  AFfilehandle in_file;
  int in_format, in_width, in_channels, frame_count;
  double in_rate;
  int bytes_per_frame;

  /* output to esound... */
  
  int out_sock, out_bits, out_channels, out_rate;
  int out_mode = ESD_STREAM, out_func = ESD_PLAY;
  esd_format_t out_format;

  in_file = afOpenFile(filename, "r", NULL);
  if (!in_file)
    return 1;

  frame_count = afGetFrameCount (in_file, AF_DEFAULT_TRACK);
  in_channels = afGetChannels (in_file, AF_DEFAULT_TRACK);
  in_rate = afGetRate (in_file, AF_DEFAULT_TRACK);
  afGetSampleFormat (in_file, AF_DEFAULT_TRACK, &in_format, &in_width);

  afSetVirtualByteOrder (in_file, AF_DEFAULT_TRACK, AF_BYTEORDER_LITTLEENDIAN);

  /*  printf ("frames: %i channels: %i rate: %f format: %i width: %i\n",
   *	  frame_count, in_channels, in_rate, in_format, in_width);
   */

  /* convert audiofile parameters to EsounD parameters */

  if (in_width == 8)
    out_bits = ESD_BITS8;
  else if (in_width == 16)
    out_bits = ESD_BITS16;
  else
    {
      fputs ("only sample widths of 8 and 16 supported\n", stderr);
      return 1;
    }

  bytes_per_frame = (in_width  * in_channels) / 8;

  if (in_channels == 1)
    out_channels = ESD_MONO;
  else if (in_channels == 2)
    out_channels = ESD_STEREO;
  else
    {
      fputs ("only 1 or 2 channel samples supported\n", stderr);
      return 1;
    }

  out_format = out_bits | out_channels | out_mode | out_func;

  out_rate = (int) in_rate;

  out_sock = esd_play_stream_fallback (out_format, out_rate, NULL, (char *) filename);
  if (out_sock <= 0)
    return 1;

  /* play */

  buf_frames = ESD_BUF_SIZE / bytes_per_frame;

  while ((frames_read = afReadFrames(in_file, AF_DEFAULT_TRACK, 
				    buf, buf_frames)))
    {
      bytes_written += frames_read * bytes_per_frame;
      if (write (out_sock, buf, frames_read * bytes_per_frame) <= 0)
	return 1;
    }

  close (out_sock);

  if (afCloseFile (in_file))
    return 1;

  printf("bytes_written = %d\n", bytes_written);

  return 0;
}
#else 

static int
play_file (const char *filename)
{
    esd_play_file( program_name, filename, 1 );
	return 0;
}

#endif


static void
usage_exit (int ret_code)
{
  printf ("Usage: %s [OPTION] FILE\n"
	  "Plays the FILE on EsounD.\n\n"
	  "  -s, --server=HOSTNAME\tset EsounD server\n"
	  "  -h, --help\tdisplay this help and exit\n"
	  "  -v, --version\toutput version information and exit\n",
	  program_name);
  if(ret_code)
	  exit(ret_code);
  else 
  	exit (0);
}

int 
main (int argc, char *argv[])
{
  int option_index = 0, c = 0;

  char *server = NULL;

  struct option opts[] = {
    { "server", required_argument, NULL, 's' },
    { "help", no_argument, NULL, 'h' },
    { "version", no_argument, NULL, 'v' },
    { 0, 0, 0, 0 }
  };

  program_name = argv [0];

  /* parse options
   */

  while (1)
    {
      c = getopt_long(argc, argv, "s:hv", opts, &option_index);

      if (c == -1)
	break;

      switch (c)
	{
	case 's':
	  server = strdup (optarg);
	  break;

	case 'h':
	  usage_exit (0);
	  
	case 'v':
	  /* fputs ("esdplay " VERSION "\n", stdout); */
	  fputs ("esdplay\n", stdout);
	  exit (0);

	case '?':
	  /* `getopt_long' already printed an error message. */
	  fprintf(stderr,"Try `%s --help' for more information.\n", 
		  program_name);
	  exit (1);

	default:
	  abort();
	}
    }

  if (optind != argc-1)
    usage_exit (1);

  return play_file (argv[optind]);
}

