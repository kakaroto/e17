/*
 * (c) 1998-2000 Gerd Knorr
 *
 *    capture a image, compress as jpeg and upload to the webserver
 *    q
 *    using ftp the ftp utility
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <X11/Xlib.h>
#include <Imlib2.h>
#include <giblib.h>
#include <curl/types.h>
#include <curl/curl.h>
#include <curl/easy.h>

#include <asm/types.h>
#include "videodev.h"

#include "parseconfig.h"

void log(char *fmt, ...);

char *ftp_host = "www";
char *ftp_user = "webcam";
char *ftp_pass = "xxxxxx";
char *ftp_dir = "public_html/images";
char *ftp_file = "webcam.jpeg";
char *ftp_tmp = "uploading.jpeg";
int ftp_debug = 0;
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
int ftp_timeout = 30;
char *title_font = "arial/8";
char *ttf_dir = "/usr/X11R6/lib/X11/fonts/TrueType";
char *archive_ext = "jpg";
char *grab_archive = NULL;
char *grab_blockfile = NULL;
char *upload_blockfile = NULL;
char *grab_postprocess = NULL;
char *title_text = NULL;
gib_style *title_style = NULL;
gib_style *text_style = NULL;
char *title_style_file = NULL;
char *text_style_file = NULL;
char *overlay_file = NULL;
Imlib_Image overlay_im = NULL;
int overlay_x = 0, overlay_y = 0;
Imlib_Font title_fn, text_fn;

int v_width[5] = { 128, 160, 176, 320, 640 };
int v_height[5] = { 96, 120, 144, 240, 480 };
int v_curr = -1;
int v_force = 0;
int bw_percent = 100;
int delay_correct = 0;
int reinit_device = 0;

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
   if ((grab_data == NULL) || (-1 == (int) grab_data))
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

   if (j == 0)
      j++;

   while (j--)
   {
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
   int x, y, w, h;

   time(&t);
   tm = localtime(&t);
   strftime(line, 254, grab_text, tm);
   if (title_text)
      strftime(title_line, 254, title_text, tm);

   if (message)
      strcat(line, message);
   line[127] = '\0';

   len = strlen(line);

   if (line[len - 1] == '\n')
      line[--len] = '\0';

   if (title_text && title_fn)
   {
      gib_imlib_get_text_size(title_fn, title_line, title_style, &w, &h,
                              IMLIB_TEXT_TO_RIGHT);
      x = width - w - 2;
      y = 2;
      gib_imlib_image_fill_rectangle(image, x - 2, y - 1, w + 4, h + 2, bg_r,
                                     bg_g, bg_b, bg_a);
      gib_imlib_text_draw(image, title_fn, title_style, x, y, title_line,
                          IMLIB_TEXT_TO_RIGHT, title_r, title_g, title_b,
                          title_a);
   }

   if (line && text_fn)
   {
      gib_imlib_get_text_size(text_fn, line, text_style, &w, &h,
                              IMLIB_TEXT_TO_RIGHT);
      x = 2;
      y = height - h - 2;
      gib_imlib_image_fill_rectangle(image, x - 2, y - 1, w + 4, h + 2, bg_r,
                                     bg_g, bg_b, bg_a);
      gib_imlib_text_draw(image, text_fn, text_style, x, y, line,
                          IMLIB_TEXT_TO_RIGHT, text_r, text_g, text_b,
                          text_a);
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

      log("executing postprocessing\n");
      snprintf(buf, sizeof(buf), "%s %s", grab_postprocess, filename);
      system(buf);
   }
}

void
archive_jpeg(Imlib_Image im)
{
   char buffer[1028];
   char date[128];
   time_t t;
   struct tm *tm;
   struct stat st;

   if (grab_archive)
   {
      time(&t);
      tm = localtime(&t);
      strftime(date, 127, "%Y-%m-%d_%H%M%S", tm);

      do
      {
         snprintf(buffer, sizeof(buffer), "%s/webcam_%s.%s", archive_ext, grab_archive,
                  date);
      }
      while (stat(buffer, &st) == 0);
      gib_imlib_save_image(im, buffer);
   }
}

void
log(char *fmt, ...)
{
   va_list args;
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
   fprintf(fp, "%s  ", date);
   va_start(args, fmt);
   vfprintf(fp, fmt, args);
   va_end(args);
   fclose(fp);
}

void
draw_overlay(Imlib_Image image)
{
   int w, h;

   w = gib_imlib_image_get_width(overlay_im);
   h = gib_imlib_image_get_height(overlay_im);
   gib_imlib_blend_image_onto_image(image, overlay_im, 0, 0, 0, w, h,
                                    overlay_x, overlay_y, w, h, 0,
                                    gib_imlib_image_has_alpha(overlay_im), 0);
};

void
bw_res_change(int diff)
{
   int temp = 0;

   if (!diff)
      return;

   if (v_curr == -1)
   {

      while (temp < 5)
      {
         if (grab_buf.height == v_height[temp])
            v_curr = temp;
         temp++;
      }
      if (v_curr == -1)
      {
         bw_percent = 100;
         fprintf(stderr,
                 "You don't appear to be running any of the resolutions\n");
         fprintf(stderr,
                 "req'd by the bandwidth limiter. It has been deactivated.\n");
         log("method bw_percent killed, not at support'd res\n");
      }
   }

   if (diff > (grab_delay * bw_percent) / 100)
   {
      log("bw_res_change Not enough bandwidth.\n");
      if (v_force < -1 && v_curr > 0)
      {
         log("bw_res_change Reducing image resolution.\n");
         grab_buf.height = v_height[--v_curr];
         grab_buf.width = v_width[v_curr];
      }
      v_force--;
   }
   else if (diff < (grab_delay * bw_percent) / 200)
   {
      if (v_force > 1 && v_curr < 5)
      {
         log("bw_res_change Increasing image resolution.\n");
         grab_buf.height = v_height[++v_curr];
         grab_buf.width = v_width[v_curr];
      }
      v_force++;
   }
   else
      v_force = 0;
}


/* upload local to tmp then MV to remote */
void
ftp_upload1(char *local, char *remote, char *tmp)
{
   char buf[2096];
   FILE *infile;
   CURLcode ret;
   CURL *curl_handle;
   struct stat st;
   struct curl_slist *post_commands = NULL;
   char *passwd_string, *url_string;

   infile = fopen(local, "r");

   if (!infile)
   {
      fprintf(stderr, "camE: Couldn't open temp file to upload it\n");
      perror("ftp_upload(): ");
      return;
   }
   fstat(fileno(infile), &st);

   snprintf(buf, sizeof(buf), "rnfr %s", tmp);
   post_commands = curl_slist_append(post_commands, buf);
   snprintf(buf, sizeof(buf), "rnto %s", remote);
   post_commands = curl_slist_append(post_commands, buf);

   /* init the curl session */
   curl_handle = curl_easy_init();

   curl_easy_setopt(curl_handle, CURLOPT_INFILE, infile);
   curl_easy_setopt(curl_handle, CURLOPT_INFILESIZE, st.st_size);

   passwd_string = gib_strjoin(":", ftp_user, ftp_pass, NULL);
   curl_easy_setopt(curl_handle, CURLOPT_USERPWD, passwd_string);

   /* set URL to save to */
   url_string = gib_strjoin("/", "ftp:/", ftp_host, ftp_dir, tmp, NULL);
   curl_easy_setopt(curl_handle, CURLOPT_URL, url_string);

   /* no progress meter please */
   curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1);

   /* shut up completely */
   if (ftp_debug)
      curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1);
   else
      curl_easy_setopt(curl_handle, CURLOPT_MUTE, 1);

   curl_easy_setopt(curl_handle, CURLOPT_POSTQUOTE, post_commands);

   curl_easy_setopt(curl_handle, CURLOPT_UPLOAD, 1);

   /* get it! */
   ret = curl_easy_perform(curl_handle);
   /* TODO check error */
   if (ret)
   {
      fprintf(stderr, "\ncamE: error sending via ftp: ");
      log("EEEE error: ");
      switch (ret)
      {
        case CURLE_URL_MALFORMAT:
           fprintf(stderr, "Badly formatted ftp host or directory\n");
           log("Badly formatted ftp host or directory\n");
           break;
        case CURLE_URL_MALFORMAT_USER:
           fprintf(stderr, "Badly formatted ftp username\n");
           log("Badly formatted ftp username\n");
           break;
        case CURLE_COULDNT_RESOLVE_PROXY:
           fprintf(stderr, "Couldn't resolve proxy\n");
           log("Couldn't resolve proxy\n");
           break;
        case CURLE_COULDNT_RESOLVE_HOST:
           fprintf(stderr, "Unable to resolve ftp host\n");
           log("Unable to resolve ftp host\n");
           break;
        case CURLE_COULDNT_CONNECT:
           fprintf(stderr, "Unable to connect to ftp host\n");
           log("Unable to connect to ftp host\n");
           break;
        case CURLE_FTP_WEIRD_SERVER_REPLY:
           fprintf(stderr, "Wierd server reply detected\n");
           log("Wierd server reply detected\n");
           break;
        case CURLE_FTP_ACCESS_DENIED:
           fprintf(stderr, "Access denied to ftp upload\n");
           log("Access denied to ftp upload\n");
           break;
        case CURLE_FTP_USER_PASSWORD_INCORRECT:
           fprintf(stderr, "Incorrect password for ftp login\n");
           log("Incorrect password for ftp login\n");
           break;
        case CURLE_FTP_WEIRD_PASS_REPLY:
           fprintf(stderr, "Wierd password reply from server\n");
           log("Wierd password reply from server\n");
           break;
        case CURLE_FTP_WEIRD_USER_REPLY:
           fprintf(stderr, "Wierd user reply from server\n");
           log("Wierd user reply from server\n");
           break;
        case CURLE_FTP_WEIRD_PASV_REPLY:
           fprintf(stderr, "Wierd passive reply from server\n");
           log("Wierd passive reply from server\n");
           break;
        case CURLE_FTP_CANT_GET_HOST:
           fprintf(stderr, "No route to host\n");
           log("No route to host\n");
           break;
        case CURLE_FTP_COULDNT_SET_BINARY:
           fprintf(stderr, "Couldn't set binary mode\n");
           log("Couldn't set binary mode\n");
           break;
        case CURLE_PARTIAL_FILE:
           fprintf(stderr, "Only partial file uploaded\n");
           log("Only partial file uploaded\n");
           break;
        case CURLE_FTP_WRITE_ERROR:
           fprintf(stderr, "Write error\n");
           log("Write error\n");
           break;
        case CURLE_FTP_QUOTE_ERROR:
           fprintf(stderr, "Misquoted ftp command - check ftp config\n");
           log("Misquoted ftp command - check ftp config\n");
           break;
        case CURLE_WRITE_ERROR:
           fprintf(stderr, "Write error\n");
           log("Write error\n");
           break;
        case CURLE_MALFORMAT_USER:	/* the user name is illegally specified */
           fprintf(stderr, "Malformatted username\n");
           log("Malformatted username\n");
           break;
        case CURLE_FTP_COULDNT_STOR_FILE:	/* failed FTP upload */
           fprintf(stderr, "Couldn't STOR the file\n");
           log("Couldn't STOR the file\n");
           break;
        case CURLE_READ_ERROR:	/* could open/read from file */
           fprintf(stderr, "Couldn't open temp file\n");
           log("Couldn't open temp file\n");
           break;
        case CURLE_OUT_OF_MEMORY:
           fprintf(stderr, "Out of memory\n");
           log("Out of memory\n");
           break;
        case CURLE_OPERATION_TIMEOUTED:	/* the timeout time was reached */
           fprintf(stderr, "Upload timed out\n");
           log("Upload timed out\n");
           break;
        case CURLE_FTP_PORT_FAILED:	/* FTP PORT operation failed */
           fprintf(stderr, "ftp PORT failed\n");
           log("ftp PORT failed\n");
           break;
        case CURLE_FILE_COULDNT_READ_FILE:
           fprintf(stderr, "Couldn't read temp file\n");
           log("Couldn't read temp file\n");
           break;
        default:
           fprintf(stderr, "unknown error, attempting to continue\n");
           log("unknown error, attempting to continue\n");
           break;
      }
   }

   /* cleanup curl stuff */
   curl_easy_cleanup(curl_handle);
   curl_slist_free_all(post_commands);
   free(url_string);
   free(passwd_string);
   fclose(infile);
}

int
main(int argc, char *argv[])
{
   unsigned char *val;
   Imlib_Image image;
   char filename[100];
   int width, height, i;
   struct stat st;
   pid_t childpid;
   time_t start_shot;
   time_t end_shot;
   int just_shot = 0;
   int new_delay;

   if ((argc >= 2) && (!strcmp(argv[1], "-f")))
   {
      /* don't fork */
   }
   else
   {
      /* fork and die */
      if ((childpid = fork()) < 0)
      {
         fprintf(stderr, "fork (%s)\n", strerror(errno));
         return (2);
      }
      else if (childpid > 0)
         exit(0);       /* parent */
   }

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
   if (-1 != (i = cfg_get_int("ftp", "timeout")))
      ftp_timeout = i;

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
   if (NULL != (val = cfg_get_str("grab", "uploadblockfile")))
      upload_blockfile = val;
   if (NULL != (val = cfg_get_str("grab", "postprocess")))
      grab_postprocess = val;
   if (NULL != (val = cfg_get_str("grab", "title_text")))
      title_text = val;
   if (NULL != (val = cfg_get_str("grab", "archive_ext")))
      archive_ext = val;
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
   if (NULL != (val = cfg_get_str("grab", "overlay_image")))
      overlay_file = val;
   if (-1 != (i = cfg_get_int("grab", "width")))
      grab_width = i;
   if (-1 != (i = cfg_get_int("grab", "height")))
      grab_height = i;
   if (-1 != (i = cfg_get_int("grab", "delay")))
      grab_delay = i;
   if (-1 != (i = cfg_get_int("grab", "correct")))
      delay_correct = 1;
   if (-1 != (i = cfg_get_int("grab", "percent")))
      bw_percent = i;
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
   if (-1 != (i = cfg_get_int("grab", "overlay_x")))
      overlay_x = i;
   if (-1 != (i = cfg_get_int("grab", "overlay_y")))
      overlay_y = i;

   /* print config */
   fprintf(stderr, "camE v0.6 - (c) 1999, 2000 Gerd Knorr, Tom Gilbert\n");
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
   if (title_style_file)
      title_style = gib_style_new_from_ascii(title_style_file);
   if (text_style_file)
      text_style = gib_style_new_from_ascii(text_style_file);
   if (overlay_file)
      overlay_im = imlib_load_image(overlay_file);
   title_fn = imlib_load_font(title_font);
   if (!title_fn)
      fprintf(stderr, "can't load font %s\n", title_font);
   text_fn = imlib_load_font(text_font);
   if (!text_fn)
      fprintf(stderr, "can't load font %s\n", text_font);

   /* go! */
   for (;;)
   {
      just_shot = 0;
      end_shot = 0;
      start_shot = 0;
      if ((grab_blockfile && (stat(grab_blockfile, &st) == -1))
          || !grab_blockfile)
      {
         time(&start_shot);
         if (action_pre_shot)
         {
            log("running pre-shot action\n");
            system(action_pre_shot);
            log("pre-shot action done\n");
         }

         log("* taking shot\n");
         /* Prevent camera lag... */
         image = grab_one(&width, &height);
         imlib_context_set_image(image);
         if (!image)
         {
            fprintf(stderr, "no image captured\n");
            exit(2);
         }

         log("** shot taken\n");
         if (action_post_shot)
         {
            log("running post-shot action\n");
            system(action_post_shot);
            log("post-shot action done\n");
         }
         if (overlay_im)
            draw_overlay(image);
         add_time_text(image, get_message(), width, height);
         gib_imlib_save_image(image, temp_file);
         do_postprocess(temp_file);
         archive_jpeg(image);
         if (ftp_do)
         {
            if ((upload_blockfile && (stat(upload_blockfile, &st) == -1))
                || !upload_blockfile)
            {
               log("*** uploading via ftp\n");
               ftp_upload1(temp_file, ftp_file, ftp_tmp);
               log("shot uploaded\n");
               if (action_post_upload)
               {
                  log("running post upload action\n");
                  system(action_post_upload);
                  log("post upload action done\n");
               }
            }
         }
         else if (scp_target)
         {
            char buf[4096];

            if ((upload_blockfile && (stat(upload_blockfile, &st) == -1))
                || !upload_blockfile)
            {
               log("uploading via scp\n");
               snprintf(buf, sizeof(buf), "scp -BCq %s %s", temp_file,
                        scp_target);
               system(buf);
               log("shot uploaded\n");
               if (action_post_upload)
               {
                  log("running post upload action\n");
                  system(action_post_upload);
                  log("post upload action done\n");
               }
            }
         }
         gib_imlib_free_image_and_decache(image);
         just_shot = 1;
         time(&end_shot);
      }
      new_delay = grab_delay;
      if (just_shot)
      {
         end_shot = end_shot - start_shot;
         if (bw_percent < 100)
            bw_res_change(end_shot);
         if (delay_correct && end_shot)
         {
            new_delay -= end_shot;
            if (new_delay < 0)
               new_delay = 0;
            log("Sleeping %d secs (corrected)\n", new_delay);
         }
         else
         {
            log("Sleeping %d secs\n", grab_delay);
         }
      }
      if (new_delay > 0)
         sleep(new_delay);
   }
   return 0;
}
