#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <Eet.h>
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_File.h>
#include <Ecore_Evas.h>
#include <Edje.h>
#include <Emotion.h>

#include "config.h"

static void video_resize_cb(void *data, Evas_Object *obj, void *event_info);
static void video_stopped_cb(void *data, Evas_Object *obj, void *event_info);
static void video_resize(void);
static void args_parse(void);
static void help_show(void);
static int signal_exit(void *data, int ev_type, void *ev);
static int frame_grab(void *data);

char *module_filename = "xine";
Ecore_Evas *ee = NULL, *ee_im = NULL, *ee_im2 = NULL;
Evas *evas = NULL, *evas_im = NULL, *evas_im2 = NULL;
Evas_Object *video = NULL, *bg = NULL, *im = NULL, *im2 = NULL;
char *file = NULL;
char *outfile = NULL;
double fps = 10.0;
int outw = 80;
int outh = 60;
int frnum = 0;
int pos = 0;
int endpos = 3;
int firstsize = 1;
int oneframe = 0;
double len = 0.0;
double snip = 5.0 / 2.0;
double p1 = 0.0, p2 = 0.0, p3 = 0.0;
Eet_File *ef = NULL;

int
main(int argc, char **argv)
{
   if (!ecore_init()) return -1;
   ecore_app_args_set(argc, (const char **)argv);
   ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, signal_exit, NULL);
   if (!ecore_evas_init()) return -1;

   args_parse();

   ee = ecore_evas_buffer_new(outw, outh);
   if (!ee)
     {
	fprintf(stderr, "Error. cannot create buffer engine canvas for image save.\n");
	exit(-1);
     }
   evas = ecore_evas_get(ee);

   video = emotion_object_add(evas);
   emotion_object_module_option_set(video, "audio", "off");
   emotion_object_init(video, module_filename);
   emotion_object_file_set(video, file);
   evas_object_smart_callback_add(video, "frame_resize", video_resize_cb, NULL);
   evas_object_smart_callback_add(video, "decode_stop", video_stopped_cb, NULL);
   evas_object_move(video, 0, 0);
   evas_object_resize(video, outw, outh);
   evas_object_show(video);
   emotion_object_play_set(video, 1);
   emotion_object_audio_mute_set(video, 1);
   video_resize();

   frame_grab(NULL);
   ecore_timer_add(1.0 / fps, frame_grab, NULL);
   ecore_main_loop_begin();

   if (ef) eet_close(ef);

   evas_object_del(video);
   ecore_evas_free(ee);
   ecore_evas_shutdown();
   ecore_shutdown();
   return 0;
}

static void
video_resize_cb(void *data, Evas_Object *obj, void *event_info)
{
   video_resize();
}

static void
video_stopped_cb(void *data, Evas_Object *obj, void *event_info)
{
   pos = 4;
}

static void
video_resize(void)
{
   Evas_Coord w2, h2;
   int iw, ih;
   double ratio;

   emotion_object_size_get(video, &iw, &ih);
   if ((iw == 0) || (ih == 0)) return;
   ratio = emotion_object_ratio_get(video);
   if (ratio > 0.0) iw = (ih * ratio) + 0.5;

   if ((firstsize) && (iw > 1))
     {
	outw = (outh * iw) / ih;
	ecore_evas_resize(ee, outw, outh);
	firstsize = 0;
     }
   w2 = outw;
   h2 = (outw * ih) / iw;
   if (h2 > outh)
     {
	h2 = outh;
	w2 = (outh * iw) / ih;
     }
   evas_object_move(video, (outw - w2) / 2, (outh - h2) / 2);
   evas_object_resize(video, w2, h2);
   evas_object_resize(bg, outw, outh);
}

static void
args_parse(void)
{
   int argc;
   char **argv;
   int i;

   ecore_app_args_get(&argc, &argv);
   for (i = 1; i < argc; i++)
     {
	if ((!strcmp(argv[i], "-h")) ||
	    (!strcmp(argv[i], "-help")) ||
	    (!strcmp(argv[i], "--help")))
	  {
	     help_show();
	     exit(-1);
	  }
	else if (((!strcmp(argv[i], "-og")) ||
		  (!strcmp(argv[i], "-out-geometry")) ||
		  (!strcmp(argv[i], "--out-geometry"))) && (i < (argc - 1)))
	  {
	     int n;
	     char buf[16], buf2[16];

	     n = sscanf(argv[i +1], "%10[^x]x%10s", buf, buf2);
	     if (n == 2)
	       {
		  outw = atoi(buf);
		  outh = atoi(buf2);
	       }
	     i++;
	  }
	else if ((!strcmp(argv[i], "-fps")) && (i < (argc - 1)))
	  {
	     fps = atof(argv[i + 1]);
	     if (fps < 0.1) fps = 0.1;
	     i++;
	  }
	else if (!file)
	  {
	     file = argv[i];
	  }
	else if (!outfile)
	  {
	     outfile = argv[i];
	  }
     }
   if ((!file) || (!outfile))
     {
	help_show();
	exit(-1);
     }
}

static void
help_show(void)
{
   printf("Usage: edje_thumb INPUT_VIDEO OUT_FILE [OPTIONS]\n"
	  "\n"
	  "Where required parameters are:\n"
	  "  INPUT_VIDEO   the video file to look at\n"
	  "  OUT_FILE      the output file\n"
	  "\n"
	  "Where optional OPTIONS are:\n"
	  "  -h          this help\n"
	  "  -og WxH     output file geometry\n"
	  "  -fps N      frames per second if capturing an animation\n"
	  );
}

static int
signal_exit(void *data, int ev_type, void *ev)
{
   ecore_main_loop_quit();
   return 1;
}

static int
frame_grab(void *data)
{
   char buf[4096];
   const void *pixels;
   double p;

   if (pos == 0)
     {
	len = emotion_object_play_length_get(video);
	if (len <= 0.0) return 1;
	p1 = (len * 1.0) / 4.0;
	p2 = (len * 2.0) / 4.0;
	p3 = (len * 3.0) / 4.0;
	if (len < 120.0)
	  {
	     p2 = (len * 1.0) / 3.0;
	     p3 = (len * 2.0) / 3.0;
	     pos = 2;
	     endpos = 3;
	  }
	else if (len < 60.0)
	  {
	     p3 = (len * 1.0) / 2.0;
	     pos = 3;
	     endpos = 3;
	  }
	else if (len < 30.0)
	  {
	     p1 = (len * 1.0) / 2.0;
	     pos = 1;
	     endpos = 3;
	     oneframe = 1;
	  }
	else
	  {
	     p1 = (len * 1.0) / 4.0;
	     p2 = (len * 2.0) / 4.0;
	     p3 = (len * 3.0) / 4.0;
	     pos = 1;
	     endpos = 3;
	  }
	emotion_object_position_set(video, p1 - snip);
     }
   p = emotion_object_position_get(video);
   printf("fr %i pos %i p %3.3f\n", frnum, pos, p);
   if (!ef) ef = eet_open(outfile, EET_FILE_MODE_WRITE);
   if ((pos == 1) && (p <= (p1 - snip))) return 1;
   else if ((pos == 2) && (p <= (p2 - snip))) return 1;
   else if ((pos == 3) && (p <= (p3 - snip))) return 1;
   if (ef)
     {
	pixels = ecore_evas_buffer_pixels_get(ee);
	snprintf(buf, sizeof(buf), "v/%i", frnum);
	eet_data_image_write(ef, buf, pixels, outw, outh, 0, 0, 50, 1);
	frnum++;
     }
   if (pos == 1)
     {
	if ((p - p1) >= snip)
	  {
	     emotion_object_position_set(video, p2 - snip);
	     if (pos == endpos)
	       ecore_main_loop_quit();
	     else
	       pos = 2;
	  }
     }
   else if (pos == 2)
     {
	if ((p - p2) >= snip)
	  {
	     emotion_object_position_set(video, p3 - snip);
	     if (pos == endpos)
	       ecore_main_loop_quit();
	     else
	       pos = 3;
	  }
     }
   else if (pos == 3)
     {
	if ((p - p3) >= snip)
	  {
	     if (pos == endpos)
	       ecore_main_loop_quit();
	     else
	       ecore_main_loop_quit();
	  }
     }
   else if (pos == 4)
     {
	ecore_main_loop_quit();
     }
   if ((frnum >= 1) && (oneframe))
     {
	ecore_main_loop_quit();
     }
   return 1;
}
