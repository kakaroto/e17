/*
 * (c) 1998-2000 Gerd Knorr
 *
 *    capture a image, compress as jpeg and upload to the webserver
 *    using ftp the ftp utility
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <X11/Xlib.h>
#include <Imlib2.h>
#include "geist_list.h"
#include "imlib_wrap.h"

#include <asm/types.h>
#include "videodev.h"

#include "ftp.h"
#include "parseconfig.h"

void log(char *entry);

char *ftp_host = "www";
char *ftp_user = "webcam";
char *ftp_pass = "xxxxxx";
char *ftp_dir = "public_html/images";
char *ftp_file = "webcam.jpeg";
char *ftp_tmp = "uploading.jpeg";
char *temp_file = "/tmp/webcam.jpg";
int ftp_passive = 1;
int ftp_do = 1;
char *scp_target = NULL;
char *grab_device = "/dev/video0";
char *grab_text = "webcam %Y-%m-%d %H:%M:%S";	/* strftime */
char *action_pre_shot = NULL;
char *action_post_shot = NULL;
char *action_post_upload = NULL;
char *grab_infofile = NULL;
char *logfile = NULL;
int grab_width = 320;
int grab_height = 240;
int grab_delay = 3;
int grab_quality = 75;
int lag_reduce = 5;
int text_r = 255;
int text_g = 255;
int text_b = 255;
int text_a = 255;
char *text_font = "arial/8";
int title_r = 255;
int title_g = 255;
int title_b = 255;
int title_a = 255;
int bg_r = 0;
int bg_g = 0;
int bg_b = 0;
int bg_a = 150;
int close_dev = 0;
char *title_font = "arial/8";
char *ttf_dir = "/usr/X11R6/lib/X11/fonts/TrueType";
char *grab_archive = NULL;
char *grab_blockfile = NULL;
char *grab_postprocess = NULL;
char *title_text = NULL;
font_style *title_style = NULL;
font_style *text_style = NULL;
char *title_style_file = NULL;
char *text_style_file = NULL;

/* these work for v4l only, not v4l2 */
int grab_input = 0;
int grab_norm = VIDEO_MODE_PAL;

static struct video_mmap grab_buf;
static int grab_fd = -1;
static int grab_size = 0;
static unsigned char *grab_data = NULL;
Imlib_Image convert_rgb_to_imlib2(unsigned char *mem, int width, int height);

void
close_device()
{
   if (munmap(grab_data, grab_size) != 0)
   {
      perror("munmap()");
      exit(1);
   }
   grab_data = NULL;
   if (close(grab_fd))
      perror("close(grab_fd) ");
   grab_fd = -1;
}

void
grab_init()
{
   struct video_capability grab_cap;
   struct video_channel grab_chan;
   struct video_mbuf vid_mbuf;

   if ((grab_fd = open(grab_device, O_RDWR)) == -1)
   {
      fprintf(stderr, "open %s: %s\n", grab_device, strerror(errno));
      exit(1);
   }
   if (ioctl(grab_fd, VIDIOCGCAP, &grab_cap) == -1)
   {
      fprintf(stderr, "%s: no v4l device\n", grab_device);
      exit(1);
   }

   /* set image source and TV norm */
   grab_chan.channel = grab_input;
   if (ioctl(grab_fd, VIDIOCGCHAN, &grab_chan) == -1)
   {
      perror("ioctl VIDIOCGCHAN");
      exit(1);
   }
   grab_chan.channel = grab_input;
   grab_chan.norm = grab_norm;
   if (ioctl(grab_fd, VIDIOCSCHAN, &grab_chan) == -1)
   {
      perror("ioctl VIDIOCSCHAN");
      exit(1);
   }

   /* try to setup mmap-based capture */
   grab_buf.format = VIDEO_PALETTE_RGB24;
   grab_buf.frame = 0;
   grab_buf.width = grab_width;
   grab_buf.height = grab_height;

   ioctl(grab_fd, VIDIOCGMBUF, &vid_mbuf);

   /*   grab_size = grab_buf.width * grab_buf.height * 3; */
   grab_size = vid_mbuf.size;
   grab_data =
      mmap(0, grab_size, PROT_READ | PROT_WRITE, MAP_SHARED, grab_fd, 0);
   if ((grab_data == NULL) || (-1 == (int)grab_data))
   {
      fprintf(stderr,
              "couldn't mmap vidcam. your card doesn't support that?\n");
      exit(1);
   }
}

Imlib_Image
grab_one(int *width, int *height)
{
   Imlib_Image im;
   int i = 0;
   int j = lag_reduce;
   if (j == 0) j++;

   while (j--)
   {
      if (grab_fd == -1)
         grab_init();
      if (ioctl(grab_fd, VIDIOCMCAPTURE, &grab_buf) == -1)
      {
         perror("ioctl VIDIOCMCAPTURE");
         return NULL;
      }
      if (ioctl(grab_fd, VIDIOCSYNC, &i) == -1)
      {
         perror("ioctl VIDIOCSYNC");
         return NULL;
      }
   }
   im = convert_rgb_to_imlib2(grab_data, grab_buf.width, grab_buf.height);
   if (close_dev)
      close_device();
   if (im)
   {
      imlib_context_set_image(im);
      imlib_image_attach_data_value("quality", NULL, grab_quality, NULL);
   }
   *width = grab_buf.width;
   *height = grab_buf.height;
   return im;
}

char *
get_message(void)
{
   static char buffer[4096];
   FILE *fp;

   fp = fopen(grab_infofile, "r");
   if (fp)
   {
      fgets(buffer, sizeof(buffer), fp);
      fclose(fp);
      return buffer;
   }
   return NULL;
}

/* ---------------------------------------------------------------------- */

void
add_time_text(Imlib_Image image, char *message, int width, int height)
{
   time_t t;
   struct tm *tm;
   char line[255], title_line[255];
   int len;
   char *msg;
   Imlib_Font title_fn, text_fn;
   int x, y, w, h;

   time(&t);
   tm = localtime(&t);
   strftime(line, 254, grab_text, tm);
   if (title_text)
      strftime(title_line, 254, title_text, tm);

   msg = get_message();
   if (msg)
      strcat(line, msg);
   line[127] = '\0';

   len = strlen(line);

   if (line[len - 1] == '\n')
      line[--len] = '\0';

   if (title_text)
   {
      title_fn = imlib_load_font(title_font);
      if (title_fn)
      {
         imlib_wrap_get_text_size(title_fn, title_line, title_style, &w, &h,
                                  IMLIB_TEXT_TO_RIGHT);
         x = width - w - 2;
         y = 2;
         imlib_wrap_image_fill_rectangle(image, x - 2, y - 1, w + 4, h + 2,
                                         bg_r, bg_g, bg_b, bg_a);
         imlib_wrap_text_draw(image, title_fn, title_style, x, y, title_line,
                              IMLIB_TEXT_TO_RIGHT, title_r, title_g, title_b,
                              title_a);
      }
   }

   if (line)
   {
      text_fn = imlib_load_font(text_font);
      if (text_fn)
      {
         imlib_wrap_get_text_size(text_fn, line, text_style, &w, &h, IMLIB_TEXT_TO_RIGHT);
         x = 2;
         y = height - h - 2;
         imlib_wrap_image_fill_rectangle(image, x - 2, y - 1, w + 4, h + 2,
                                         bg_r, bg_g, bg_b, bg_a);
         imlib_wrap_text_draw(image, text_fn, text_style, x, y, line,
                              IMLIB_TEXT_TO_RIGHT, text_r, text_g, text_b,
                              text_a);
      }
   }
}

Imlib_Image
convert_rgb_to_imlib2(unsigned char *mem, int width, int height)
{
   Imlib_Image im;
   DATA32 *data, *dest;
   unsigned char *src;
   int i;

   im = imlib_create_image(width, height);
   imlib_context_set_image(im);
   data = imlib_image_get_data();

   dest = data;
   src = mem;
   i = width * height;
   while (i--)
   {
      *dest = (src[2] << 16) | (src[1] << 8) | src[0] | 0xff000000;
      dest++;
      src += 3;
   }

   imlib_image_put_back_data(data);

   return im;
}

/* ---------------------------------------------------------------------- */

void
do_postprocess(char *filename)
{
   if (grab_postprocess)
   {
      char buf[4096];

      log("executing postprocessing");
      snprintf(buf, sizeof(buf), "%s %s", grab_postprocess, filename);
      system(buf);
   }
}

void
archive_jpeg(Imlib_Image im)
{
   static int num = 0;
   char buffer[1028];
   char date[128];
   time_t t;
   struct tm *tm;
   struct stat st;

   if (grab_archive)
   {
      time(&t);
      tm = localtime(&t);
      strftime(date, 127, "%Y-%m-%d", tm);
      do
      {
         snprintf(buffer, sizeof(buffer), "%s/webcam_%s_%05d.jpg",
                  grab_archive, date, num++);
      }
      while (stat(buffer, &st) == 0);
      imlib_wrap_save_image(im, buffer);
   }
}

void
log(char *entry)
{
   time_t t;
   struct tm *tm;
   char date[128];
   FILE *fp;

   if (!logfile)
      return;

   fp = fopen(logfile, "a");
   if (!fp)
   {
      fprintf(stderr, "can't open log file %s\n", logfile);
      exit(2);
   }

   time(&t);
   tm = localtime(&t);
   strftime(date, 127, "%d/%m %H:%M:%S", tm);
   fprintf(fp, "%s  %s\n", date, entry);
   fclose(fp);
}

int
main(int argc, char *argv[])
{
   unsigned char *val;
   Imlib_Image image;
   char filename[100];
   int width, height, i;
   struct stat st;

   /* read config */
   sprintf(filename, "%s/%s", getenv("HOME"), ".camErc");
   fprintf(stderr, "reading config file: %s\n", filename);
   cfg_parse_file(filename);

   if (NULL != (val = cfg_get_str("ftp", "host")))
      ftp_host = val;
   if (NULL != (val = cfg_get_str("ftp", "user")))
      ftp_user = val;
   if (NULL != (val = cfg_get_str("ftp", "pass")))
      ftp_pass = val;
   if (NULL != (val = cfg_get_str("ftp", "dir")))
      ftp_dir = val;
   if (NULL != (val = cfg_get_str("ftp", "file")))
      ftp_file = val;
   if (NULL != (val = cfg_get_str("ftp", "tmp")))
      ftp_tmp = val;
   if (-1 != (i = cfg_get_int("ftp", "passive")))
      ftp_passive = i;
   if (-1 != (i = cfg_get_int("ftp", "debug")))
      ftp_debug = i;
   if (-1 != (i = cfg_get_int("ftp", "do")))
      ftp_do = i;

   if (NULL != (val = cfg_get_str("scp", "target")))
      scp_target = val;

   if (NULL != (val = cfg_get_str("grab", "device")))
      grab_device = val;
   if (NULL != (val = cfg_get_str("grab", "text")))
      grab_text = val;
   if (NULL != (val = cfg_get_str("grab", "infofile")))
      grab_infofile = val;
   if (NULL != (val = cfg_get_str("grab", "action_pre_shot")))
      action_pre_shot = val;
   if (NULL != (val = cfg_get_str("grab", "action_post_shot")))
      action_post_shot = val;
   if (NULL != (val = cfg_get_str("grab", "action_post_upload")))
      action_post_upload = val;
   if (NULL != (val = cfg_get_str("grab", "archive")))
      grab_archive = val;
   if (NULL != (val = cfg_get_str("grab", "blockfile")))
      grab_blockfile = val;
   if (NULL != (val = cfg_get_str("grab", "postprocess")))
      grab_postprocess = val;
   if (NULL != (val = cfg_get_str("grab", "title_text")))
      title_text = val;
   if (NULL != (val = cfg_get_str("grab", "logfile")))
      logfile = val;
   if (NULL != (val = cfg_get_str("grab", "ttf_dir")))
      ttf_dir = val;
   if (NULL != (val = cfg_get_str("grab", "title_font")))
      title_font = val;
   if (NULL != (val = cfg_get_str("grab", "text_font")))
      text_font = val;
   if (NULL != (val = cfg_get_str("grab", "temp_file")))
      temp_file = val;
   if (NULL != (val = cfg_get_str("grab", "title_style")))
      title_style_file = val;
   if (NULL != (val = cfg_get_str("grab", "text_style")))
      text_style_file = val;
   if (-1 != (i = cfg_get_int("grab", "width")))
      grab_width = i;
   if (-1 != (i = cfg_get_int("grab", "height")))
      grab_height = i;
   if (-1 != (i = cfg_get_int("grab", "delay")))
      grab_delay = i;
   if (-1 != (i = cfg_get_int("grab", "quality")))
      grab_quality = i;
   if (-1 != (i = cfg_get_int("grab", "input")))
      grab_input = i;
   if (-1 != (i = cfg_get_int("grab", "norm")))
      grab_norm = i;
   if (-1 != (i = cfg_get_int("grab", "text_r")))
      text_r = i;
   if (-1 != (i = cfg_get_int("grab", "text_g")))
      text_g = i;
   if (-1 != (i = cfg_get_int("grab", "text_b")))
      text_b = i;
   if (-1 != (i = cfg_get_int("grab", "text_a")))
      text_a = i;
   if (-1 != (i = cfg_get_int("grab", "title_r")))
      title_r = i;
   if (-1 != (i = cfg_get_int("grab", "title_g")))
      title_g = i;
   if (-1 != (i = cfg_get_int("grab", "title_b")))
      title_b = i;
   if (-1 != (i = cfg_get_int("grab", "title_a")))
      title_a = i;
   if (-1 != (i = cfg_get_int("grab", "bg_r")))
      bg_r = i;
   if (-1 != (i = cfg_get_int("grab", "bg_g")))
      bg_g = i;
   if (-1 != (i = cfg_get_int("grab", "bg_b")))
      bg_b = i;
   if (-1 != (i = cfg_get_int("grab", "bg_a")))
      bg_a = i;
   if (-1 != (i = cfg_get_int("grab", "close_dev")))
      close_dev = i;
   if (-1 != (i = cfg_get_int("grab", "lag_reduce")))
      lag_reduce = i;

   /* print config */
   fprintf(stderr, "camE v0.5 - (c) 1999, 2000 Gerd Knorr, Tom Gilbert\n");
   fprintf(stderr,
           "grabber config: size %dx%d, input %d, norm %d, "
           "jpeg quality %d\n", grab_width, grab_height, grab_input,
           grab_norm, grab_quality);
   if (ftp_do)
      fprintf(stderr, "ftp config:\n  %s@%s:%s\n  %s => %s\n", ftp_user,
              ftp_host, ftp_dir, ftp_tmp, ftp_file);

   /* init everything */
   grab_init();

   imlib_context_set_direction(IMLIB_TEXT_TO_RIGHT);
   imlib_add_path_to_font_path(ttf_dir);
   imlib_add_path_to_font_path(".");
   imlib_context_set_operation(IMLIB_OP_COPY);
   if(title_style_file)
      title_style = font_style_new_from_ascii(title_style_file);
   if(text_style_file)
      text_style = font_style_new_from_ascii(text_style_file);

   if (ftp_do)
   {
      log("connecting to ftp");
      ftp_init(ftp_passive);
      ftp_connect(ftp_host, ftp_user, ftp_pass, ftp_dir);
   }

   /* go! */
   for (;;)
   {
      if (grab_blockfile && (stat(grab_blockfile, &st) == -1))
      {
         if (ftp_do && !ftp_connected)
         {
            log("reconnecting ftp");
            ftp_connect(ftp_host, ftp_user, ftp_pass, ftp_dir);
         }
         if (action_pre_shot)
         {
            log("running pre-shot action");
            system(action_pre_shot);
         }

         log("* taking shot");
         /* Prevent camera lag... */
         image = grab_one(&width, &height);
         imlib_context_set_image(image);
         if (!image)
         {
            fprintf(stderr, "no image captured\n");
            exit(2);
         }

         log("** shot taken");
         if (action_post_shot)
         {
            log("running post-shot action");
            system(action_post_shot);
         }
         add_time_text(image, get_message(), width, height);
         imlib_wrap_save_image(image, temp_file);
         do_postprocess(temp_file);
         archive_jpeg(image);
         if (ftp_do)
         {
            log("*** uploading via ftp");
            ftp_upload(temp_file, ftp_file, ftp_tmp);
         }
         else if (scp_target)
         {
            char buf[4096];

            log("uploading via scp");
            snprintf(buf, sizeof(buf), "scp -BCq %s %s", temp_file,
                     scp_target);
            system(buf);
         }
         if (ftp_do || scp_target)
            log("shot uploaded");
         if (action_post_upload)
         {
            log("running post upload action");
            system(action_post_upload);
         }
         imlib_wrap_free_image_and_decache(image);
         log("sleeping");
      }
      if (grab_delay > 0)
         sleep(grab_delay);
   }
   return 0;
}
