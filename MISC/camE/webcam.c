/*
 * (c) 1998-2000 Gerd Knorr
 * (c) 2000-2004 Tom Gilbert
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <signal.h>
#include <X11/Xlib.h>
#include <Imlib2.h>
#include <giblib.h>
#include <curl/types.h>
#include <curl/curl.h>
#include <curl/easy.h>

#include <sys/types.h>
#include <linux/videodev.h>

#include <net/if.h>

#include "parseconfig.h"
#include "pwc-ioctl.h"

#define VERSION "1.9"

void camlog(char *fmt,
         ...);

char *ftp_host = "www";
char *ftp_user = "webcam";
char *ftp_pass = "xxxxxx";
char *ftp_dir = "public_html/images";
char *ftp_file = "webcam.jpeg";
char *ftp_tmp = "uploading.jpeg";
int ftp_debug = 0;
char *temp_file = "/tmp/webcam.jpg";
pid_t childpid = 0;
int ftp_passive = 1;
int ftp_do = 1;
int ftp_upload_every = 1; /* default to upload every shot */
int ftp_keepalive = 1;
int ftp_delete_first = 0;
char *scp_target = NULL;
char *grab_device = "/dev/video0";
char *grab_text = "";           /* strftime */
char *action_pre_shot = NULL;
char *action_post_shot = NULL;
char *action_post_upload = NULL;
char *grab_infofile = NULL;
char *logfile = NULL;
int grab_width = 320;
int grab_height = 240;
float grab_delay = 3.0;
int grab_quality = 75;
int lag_reduce = 5;
int scp_timeout = 30;
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
int cam_contrast = 50;
int cam_brightness = 50;
int cam_hue = 50;
int cam_colour = 50;
int cam_whiteness = 50;
int cam_framerate = 10;
char *title_font = "arial/8";
char *ttf_dir = "/usr/X11R6/lib/X11/fonts/TrueType";
char *archive_ext = "jpg";
char *grab_archive = NULL;
int archive_subdirs = 0;    /* default to archive without    */
                            /* subdirs                       */
int archive_shot_every = 1;     /* default to archive every shot */
char *archive_thumbnails_dir = NULL;
int archive_thumbnails_create = 0; /* default is not to create archive thumbnails */
int archive_thumbnails_width = 120;
int archive_thumbnails_height = 90;
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
char *ftp_interface = "-";
char *watch_interface = NULL;
char *offline_image = NULL;
int interface_active = 0;
int device_palette;
char *pwc_wb_mode = "auto";
int pwc_wb_red = 50;
int pwc_wb_blue = 50;
int pwc_backlight_mode = 1;

int crop = 0;
int crop_x = 0;
int crop_y = 0;
int crop_width = 0;
int crop_height = 0;

int scale = 0;
int scale_width = 0;
int scale_height = 0;

int v_width[5] = { 128, 160, 176, 320, 640 };
int v_height[5] = { 96, 120, 144, 240, 480 };
int v_curr = -1;
int v_force = 0;
int bw_percent = 100;
int delay_correct = 0;
int reinit_device = 0;
int flip_horizontal = 0;
int flip_vertical = 0;
int orientation = 0;

int connections = 0;
CURL *curl_handle = NULL;

struct video_picture cam_pic;

/* these work for v4l only, not v4l2 */
int grab_input = 0;
int grab_norm = VIDEO_MODE_PAL;

static struct video_mmap grab_buf;
static int grab_fd = -1;
static int grab_size = 0;
static unsigned char *grab_data = NULL;
Imlib_Image convert_yuv420p_to_imlib2(unsigned char *mem,
                                      int width,
                                      int height);
int execvp_with_timeout(int timeout,
                        char *file,
                        char **argv);
void alarm_handler(int sig);
int save_image(Imlib_Image image,
               char *file);


struct timeval add_timevals( struct timeval t1, struct timeval t2 )
{
  struct timeval tr;
  tr.tv_usec = t1.tv_usec + t2.tv_usec;
  tr.tv_sec = t1.tv_sec + t2.tv_sec + tr.tv_usec/1000000;
  tr.tv_usec = tr.tv_usec%1000000;
  return tr;
}

struct timeval sub_timevals( struct timeval t1, struct timeval t2 )
{
  struct timeval tr;
  tr.tv_usec = t1.tv_usec - t2.tv_usec;
  tr.tv_sec = t1.tv_sec - t2.tv_sec + t1.tv_usec/1000000;
  while( tr.tv_usec<0 ){
    tr.tv_sec-=1;
    tr.tv_usec+=1000000;
  }
  return tr;
}

float float_timeval( struct timeval t )
{
  float f = 1e-6 * t.tv_usec;
  f += t.tv_sec;
  return f;
}

struct timeval timeval_float( float f )
{
  struct timeval tr;
  tr.tv_sec = (time_t) floor(f);
  f -= tr.tv_sec;
  tr.tv_usec = (suseconds_t) round(1e6*f);
  return tr;
}

void
close_device()
{
  if (munmap(grab_data, grab_size) != 0) {
    perror("munmap()");
    exit(1);
  }
  grab_data = NULL;
  if (close(grab_fd))
    perror("close(grab_fd) ");
  grab_fd = -1;
}

int
try_palette(int fd,
            int pal,
            int depth)
{
  cam_pic.palette = pal;
  cam_pic.depth = depth;
  if (ioctl(fd, VIDIOCSPICT, &cam_pic) < 0)
    return 0;
  if (ioctl(fd, VIDIOCGPICT, &cam_pic) < 0)
    return 0;
  if (cam_pic.palette == pal)
    return 1;
  return 0;
}

int
find_palette(int fd,
             struct video_mmap *vid)
{
  if (try_palette(fd, VIDEO_PALETTE_RGB24, 24)) {
    camlog("negotiated palette RGB24\n");
    return VIDEO_PALETTE_RGB24;
  }
  if (try_palette(fd, VIDEO_PALETTE_YUV420P, 16)) {
    camlog("negotiated palette YUV420P\n");
    return VIDEO_PALETTE_YUV420P;
  }
  if (try_palette(fd, VIDEO_PALETTE_YUV420, 16)) {
    camlog("negotiated palette YUV420\n");
    return VIDEO_PALETTE_YUV420;
  }
  fprintf(stderr,
          "No supported palette found, please report your device to the author\n");
  exit(2);
  return 0;
}

void
grab_init()
{
  struct video_capability grab_cap;
  struct video_channel grab_chan;
  struct video_mbuf vid_mbuf;
  struct pwc_probe probe;
  int IsPhilips = 0;
  int type;

  if ((grab_fd = open(grab_device, O_RDWR)) == -1) {
    fprintf(stderr, "open %s: %s\n", grab_device, strerror(errno));
    exit(1);
  }
  if (ioctl(grab_fd, VIDIOCGCAP, &grab_cap) == -1) {
    fprintf(stderr, "%s: no v4l device\n", grab_device);
    exit(1);
  }

  if (ioctl(grab_fd, VIDIOCGPICT, &cam_pic) < 0)
    perror("getting pic info");
  cam_pic.contrast = 65535 * ((float) cam_contrast / 100);
  cam_pic.brightness = 65535 * ((float) cam_brightness / 100);
  cam_pic.hue = 65535 * ((float) cam_hue / 100);
  cam_pic.colour = 65535 * ((float) cam_colour / 100);
  cam_pic.whiteness = 65535 * ((float) cam_whiteness / 100);
  if (ioctl(grab_fd, VIDIOCSPICT, &cam_pic) < 0)
    perror("setting cam pic");
  device_palette = find_palette(grab_fd, &grab_buf);

  grab_buf.format = device_palette;
  grab_buf.frame = 0;
  grab_buf.width = grab_width;
  grab_buf.height = grab_height;

  ioctl(grab_fd, VIDIOCGMBUF, &vid_mbuf);
  camlog("%s detected\n", grab_cap.name);

  if (sscanf(grab_cap.name, "Philips %d webcam", &type) == 1)
    IsPhilips = 1;
  else {
    /* No match yet; try the PROBE */
    if (ioctl(grab_fd, VIDIOCPWCPROBE, &probe) == 0) {
      if (!strcmp(grab_cap.name, probe.name)) {
        IsPhilips = 1;
        type = probe.type;
      }
    }
  }

  /* special philips features */
  if (IsPhilips) {
    struct video_window vwin;
    int shutter = -1;
    int gain = -1;
    struct pwc_whitebalance wb;

    /* philips cam detected, maybe enable special features */
    camlog("enabling pwc-specific features\n");

    ioctl(grab_fd, VIDIOCGWIN, &vwin);
    if (vwin.flags & PWC_FPS_MASK) {
      /* Set new framerate */
      vwin.flags &= ~PWC_FPS_FRMASK;
      vwin.flags |= (cam_framerate << PWC_FPS_SHIFT);
    }

    /* Turning off snapshot mode */
    vwin.flags &= ~(PWC_FPS_SNAPSHOT);

    if (ioctl(grab_fd, VIDIOCSWIN, &vwin) < 0)
      perror("trying to set extra pwc flags");

    if (ioctl(grab_fd, VIDIOCPWCSAGC, &gain) < 0)
      perror("trying to set gain");
    if (ioctl(grab_fd, VIDIOCPWCSSHUTTER, &shutter) < 0)
      perror("trying to set shutter");

    wb.mode = PWC_WB_AUTO;
    wb.manual_red = 50;
    wb.manual_blue = 50;
    if (!strcasecmp(pwc_wb_mode, "auto")) {
      wb.mode = PWC_WB_AUTO;
    } else if (!strcasecmp(pwc_wb_mode, "indoor")) {
      wb.mode = PWC_WB_INDOOR;
    } else if (!strcasecmp(pwc_wb_mode, "outdoor")) {
      wb.mode = PWC_WB_OUTDOOR;
    } else if (!strcasecmp(pwc_wb_mode, "fluorescent")) {
      wb.mode = PWC_WB_FL;
    } else if (!strcasecmp(pwc_wb_mode, "manual")) {
      wb.mode = PWC_WB_MANUAL;
      wb.manual_red = 65535 * ((float) pwc_wb_red / 100);
      wb.manual_blue = 65535 * ((float) pwc_wb_blue / 100);
    } else {
      camlog("unknown pwc white balance mode '%s' ignored\n", pwc_wb_mode);
    }

    if (ioctl(grab_fd, VIDIOCPWCSAWB, &wb) < 0)
      perror("trying to set pwc white balance mode");

    if (ioctl(grab_fd, VIDIOCPWCSBACKLIGHT, &pwc_backlight_mode) < 0)
      perror("trying to set backlight compensation mode");
  }

  /* set image source and TV norm */
  grab_chan.channel = grab_input;
  if (ioctl(grab_fd, VIDIOCGCHAN, &grab_chan) == -1) {
    perror("ioctl VIDIOCGCHAN");
    exit(1);
  }
  grab_chan.channel = grab_input;
  grab_chan.norm = grab_norm;
  if (ioctl(grab_fd, VIDIOCSCHAN, &grab_chan) == -1) {
    perror("ioctl VIDIOCSCHAN");
    exit(1);
  }

  /*   grab_size = grab_buf.width * grab_buf.height * 3; */
  grab_size = vid_mbuf.size;
  grab_data =
    mmap(0, grab_size, PROT_READ | PROT_WRITE, MAP_SHARED, grab_fd, 0);
  if ((grab_data == NULL) || (MAP_FAILED == grab_data)) {
    fprintf(stderr,
            "couldn't mmap vidcam. your card doesn't support that?\n");
    exit(1);
  }
}

/* This is a really simplistic approach. Speedups are welcomed. */
Imlib_Image
convert_yuv420p_to_imlib2(unsigned char *src,
                          int width,
                          int height)
{
  int line, col;
  int y, u, v, yy, vr = 0, ug = 0, vg = 0, ub = 0;
  int r, g, b;
  unsigned char *sy, *su, *sv;
  Imlib_Image im;
  DATA32 *data, *dest;

  im = imlib_create_image(width, height);
  imlib_context_set_image(im);
  data = imlib_image_get_data();
  dest = data;

  sy = src;
  su = sy + (width * height);
  sv = su + (width * height / 4);

  for (line = 0; line < height; line++) {
    for (col = 0; col < width; col++) {
      y = *sy++;
      yy = y << 8;
      u = *su - 128;
      ug = 88 * u;
      ub = 454 * u;
      v = *sv - 128;
      vg = 183 * v;
      vr = 359 * v;

      if ((col & 1) == 0) {
        su++;
        sv++;
      }

      r = (yy + vr) >> 8;
      g = (yy - ug - vg) >> 8;
      b = (yy + ub) >> 8;

      if (r < 0)
        r = 0;
      if (r > 255)
        r = 255;
      if (g < 0)
        g = 0;
      if (g > 255)
        g = 255;
      if (b < 0)
        b = 0;
      if (b > 255)
        b = 255;

      *dest = (r << 16) | (g << 8) | b | 0xff000000;
      dest++;
    }
    if ((line & 1) == 0) {
      su -= width / 2;
      sv -= width / 2;
    }
  }
  imlib_image_put_back_data(data);
  return im;
}

/* This is a really simplistic approach. Speedups are welcomed. */
Imlib_Image
convert_yuv420i_to_imlib2(unsigned char *src,
                          int width,
                          int height)
{
  int line, col, linewidth;
  int y, u, v, yy, vr = 0, ug = 0, vg = 0, ub = 0;
  int r, g, b;
  unsigned char *sy, *su, *sv;
  Imlib_Image im;
  DATA32 *data, *dest;

  im = imlib_create_image(width, height);
  imlib_context_set_image(im);
  data = imlib_image_get_data();
  dest = data;

  linewidth = width + (width >> 1);
  sy = src;
  su = sy + 4;
  sv = su + linewidth;

  /* 
     The biggest problem is the interlaced data, and the fact that odd
     add even lines have V and U data, resp. 
   */

  for (line = 0; line < height; line++) {
    for (col = 0; col < width; col++) {
      y = *sy++;
      yy = y << 8;
      if ((col & 1) == 0) {
        /* only at even colums we update the u/v data */
        u = *su - 128;
        ug = 88 * u;
        ub = 454 * u;
        v = *sv - 128;
        vg = 183 * v;
        vr = 359 * v;

        su++;
        sv++;
      }
      if ((col & 3) == 3) {
        sy += 2;        /* skip u/v */
        su += 4;        /* skip y */
        sv += 4;        /* skip y */
      }
      r = (yy + vr) >> 8;
      g = (yy - ug - vg) >> 8;
      b = (yy + ub) >> 8;

      if (r < 0)
        r = 0;
      if (r > 255)
        r = 255;
      if (g < 0)
        g = 0;
      if (g > 255)
        g = 255;
      if (b < 0)
        b = 0;
      if (b > 255)
        b = 255;

      *dest = (r << 16) | (g << 8) | b | 0xff000000;
      dest++;
    }
    if (line & 1) {
      su += linewidth;
      sv += linewidth;
    } else {
      su -= linewidth;
      sv -= linewidth;
    }
  }
  imlib_image_put_back_data(data);
  return im;
}

Imlib_Image
convert_rgb24_to_imlib2(unsigned char *mem,
                        int width,
                        int height)
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
  while (i--) {
    *dest = (src[2] << 16) | (src[1] << 8) | src[0] | 0xff000000;
    dest++;
    src += 3;
  }

  imlib_image_put_back_data(data);

  return im;
}


Imlib_Image
grab_one(int *width,
         int *height)
{
  Imlib_Image im;
  int i = 0;
  int j = lag_reduce;

  if (grab_fd == -1)
    grab_init();

  if (j == 0)
    j++;

  while (j--) {
    if (ioctl(grab_fd, VIDIOCMCAPTURE, &grab_buf) == -1) {
      perror("ioctl VIDIOCMCAPTURE");
      return NULL;
    }
    if (ioctl(grab_fd, VIDIOCSYNC, &i) == -1) {
      perror("ioctl VIDIOCSYNC");
      return NULL;
    }
  }
  switch (device_palette) {
    case VIDEO_PALETTE_YUV420P:
      im =
        convert_yuv420p_to_imlib2(grab_data, grab_buf.width, grab_buf.height);
      break;
    case VIDEO_PALETTE_YUV420:
      im =
        convert_yuv420i_to_imlib2(grab_data, grab_buf.width, grab_buf.height);
      break;
    case VIDEO_PALETTE_RGB24:
      im =
        convert_rgb24_to_imlib2(grab_data, grab_buf.width, grab_buf.height);
      break;
    default:
      fprintf(stderr, "eeek");
      exit(2);
  }
  if (close_dev)
    close_device();
  if (im) {
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
  if (fp) {
    fgets(buffer, sizeof(buffer), fp);
    fclose(fp);
    return buffer;
  }
  return NULL;
}

/* ---------------------------------------------------------------------- */

void
add_time_text(Imlib_Image image,
              char *message,
              int width,
              int height)
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

  if (title_text && title_fn) {
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

  if (line && text_fn) {
    gib_imlib_get_text_size(text_fn, line, text_style, &w, &h,
                            IMLIB_TEXT_TO_RIGHT);
    x = 2;
    y = height - h - 2;
    gib_imlib_image_fill_rectangle(image, x - 2, y - 1, w + 4, h + 2, bg_r,
                                   bg_g, bg_b, bg_a);
    gib_imlib_text_draw(image, text_fn, text_style, x, y, line,
                        IMLIB_TEXT_TO_RIGHT, text_r, text_g, text_b, text_a);
  }
}

/* ---------------------------------------------------------------------- */

void
do_postprocess(char *filename)
{
  if (grab_postprocess) {
    char buf[4096];

    camlog("executing postprocessing\n");
    snprintf(buf, sizeof(buf), "%s %s", grab_postprocess, filename);
    system(buf);
  }
}

void
archive_jpeg(Imlib_Image im)
{
  char buffer[PATH_MAX];
  char date[128];
  char year[5];
  char month[3];
  char day[3];
  time_t t;
  struct tm *tm;
  static int shot_counter = 0;
  Imlib_Image thumbnail_image;

  shot_counter++;

  if (grab_archive && archive_shot_every
      && shot_counter >= archive_shot_every) {
    time(&t);
    tm = localtime(&t);

    if (archive_subdirs) {
      strftime(date, 128, "%H%M%S", tm);
      strftime(year, 5, "%Y", tm);
      strftime(month, 3, "%m", tm);
      strftime(day, 3, "%d", tm);

      snprintf(buffer, sizeof(buffer), "%s/%s", grab_archive, year);
      if (access(buffer, F_OK) == -1) {
        mkdir(buffer, 0777);
        camlog("Created new archive subdir %s\n", buffer);
      }

      snprintf(buffer, sizeof(buffer), "%s/%s/%s", grab_archive, year,
               month);
      if (access(buffer, F_OK) == -1) {
        mkdir(buffer, 0777);
        camlog("Created new archive subdir %s\n", buffer);
      }

      snprintf(buffer, sizeof(buffer), "%s/%s/%s/%s", grab_archive, year,
               month, day);
      if (access(buffer, F_OK) == -1) {
        mkdir(buffer, 0777);
        camlog("Created new archive subdir %s\n", buffer);
      }

      snprintf(buffer, sizeof(buffer), "%s/%s/%s/%s/%s.%s", grab_archive,
               year, month, day, date, archive_ext);
    } else {
      strftime(date, 127, "%Y-%m-%d_%H%M%S", tm);
      snprintf(buffer, sizeof(buffer), "%s/webcam_%s.%s", grab_archive, date,
              archive_ext);
    }
    save_image(im, buffer);
    shot_counter = 0;
    /* 
     * if archive thumbnails are enabled we save them here,
     * same filenames just in "archive_thumbnails_directory" directory
     * files are saved in the same format as archive
     */
    if (archive_thumbnails_create && archive_thumbnails_width
        && archive_thumbnails_height) {
      if (archive_subdirs) {
        snprintf(buffer, sizeof(buffer), "%s/%s", archive_thumbnails_dir, year);
        if (access(buffer, F_OK) == -1) {
          mkdir(buffer, 0777);
          camlog("Created new archive subdir %s\n", buffer);
        }

        snprintf(buffer, sizeof(buffer), "%s/%s/%s", archive_thumbnails_dir,
                 year, month);
        if (access(buffer, F_OK) == -1) {
          mkdir(buffer, 0777);
          camlog("Created new archive subdir %s\n", buffer);
        }

        snprintf(buffer, sizeof(buffer), "%s/%s/%s/%s",
                 archive_thumbnails_dir, year, month, day);
        if (access(buffer, F_OK) == -1) {
          mkdir(buffer, 0777);
          camlog("Created new archive subdir %s\n", buffer);
        }

        snprintf(buffer, sizeof(buffer), "%s/%s/%s/%s/%s.%s",
                 archive_thumbnails_dir, year, month, day, date, archive_ext);
      } else {
        snprintf(buffer, sizeof(buffer), "%s/webcam_%s.%s",
          archive_thumbnails_dir, date, archive_ext);
      }
      thumbnail_image =
        gib_imlib_create_cropped_scaled_image(im, 0, 0,
                                              gib_imlib_image_get_width(im),
                                              gib_imlib_image_get_height(im),
                                              archive_thumbnails_width,
                                              archive_thumbnails_height, 1);
      save_image(thumbnail_image, buffer);
      gib_imlib_free_image_and_decache(thumbnail_image);
    }
  }
}

void
camlog(char *fmt,
    ...)
{
  va_list args;
  time_t t;
  struct tm *tm;
  char date[128];
  FILE *fp;

  if (!logfile) {
    va_start(args, fmt);
    fprintf(stderr, "camE: ");
    vfprintf(stderr, fmt, args);
    va_end(args);
    return;
  }

  fp = fopen(logfile, "a");
  if (!fp) {
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

int
save_image(Imlib_Image image,
           char *file)
{
  Imlib_Load_Error err;

  gib_imlib_save_image_with_error_return(image, file, &err);
  if ((err) || (!image)) {
    switch (err) {
      case IMLIB_LOAD_ERROR_FILE_DOES_NOT_EXIST:
        camlog("Error saving image %s - File does not exist", file);
        break;
      case IMLIB_LOAD_ERROR_FILE_IS_DIRECTORY:
        camlog("Error saving image %s - Directory specified for image filename",
            file);
        break;
      case IMLIB_LOAD_ERROR_PERMISSION_DENIED_TO_READ:
        camlog("Error saving image %s - No read access to directory", file);
        break;
      case IMLIB_LOAD_ERROR_UNKNOWN:
      case IMLIB_LOAD_ERROR_NO_LOADER_FOR_FILE_FORMAT:
        camlog("Error saving image %s - No Imlib2 loader for that file format",
            file);
        break;
      case IMLIB_LOAD_ERROR_PATH_TOO_LONG:
        camlog("Error saving image %s - Path specified is too long", file);
        break;
      case IMLIB_LOAD_ERROR_PATH_COMPONENT_NON_EXISTANT:
        camlog("Error saving image %s - Path component does not exist", file);
        break;
      case IMLIB_LOAD_ERROR_PATH_COMPONENT_NOT_DIRECTORY:
        camlog("Error saving image %s - Path component is not a directory",
            file);
        break;
      case IMLIB_LOAD_ERROR_PATH_POINTS_OUTSIDE_ADDRESS_SPACE:
        camlog("Error saving image %s - Path points outside address space",
            file);
        break;
      case IMLIB_LOAD_ERROR_TOO_MANY_SYMBOLIC_LINKS:
        camlog("Error saving image %s - Too many levels of symbolic links",
            file);
        break;
      case IMLIB_LOAD_ERROR_OUT_OF_MEMORY:
        camlog("Error saving image %s - Out of memory", file);
        break;
      case IMLIB_LOAD_ERROR_OUT_OF_FILE_DESCRIPTORS:
        gib_eprintf("While loading %s - Out of file descriptors", file);
        break;
      case IMLIB_LOAD_ERROR_PERMISSION_DENIED_TO_WRITE:
        camlog("Error saving image %s - Cannot write to directory", file);
        break;
      case IMLIB_LOAD_ERROR_OUT_OF_DISK_SPACE:
        camlog("Error saving image %s - Cannot write - out of disk space", file);
        break;
      default:
        camlog
          ("Error saving image %s - Unknown error (%d). Attempting to continue",
           file, err);
        break;
    }
    return 0;
  }
  return 1;
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
bw_res_change(struct timeval diff)
{
  int temp = 0;

  if (v_curr == -1) {

    while (temp < 5) {
      if (grab_buf.height == v_height[temp])
        v_curr = temp;
      temp++;
    }
    if (v_curr == -1) {
      bw_percent = 100;
      fprintf(stderr,
              "You don't appear to be running any of the resolutions\n");
      fprintf(stderr,
              "req'd by the bandwidth limiter. It has been deactivated.\n");
      camlog("method bw_percent killed, not at support'd res\n");
    }
  }

  if (float_timeval(diff) > (grab_delay * bw_percent) / 100) {
    camlog("bw_res_change Not enough bandwidth.\n");
    if (v_force < -1 && v_curr > 0) {
      camlog("bw_res_change Reducing image resolution.\n");
      grab_buf.height = v_height[--v_curr];
      grab_buf.width = v_width[v_curr];
    }
    v_force--;
  } else if (float_timeval(diff) < (grab_delay * bw_percent) / 200) {
    if (v_force > 1 && v_curr < 5) {
      camlog("bw_res_change Increasing image resolution.\n");
      grab_buf.height = v_height[++v_curr];
      grab_buf.width = v_width[v_curr];
    }
    v_force++;
  } else
    v_force = 0;
}


/* upload local to tmp then MV to remote */
void
ftp_upload(char *local,
           char *remote,
           char *tmp)
{
  char buf[2096];
  FILE *infile;
  CURLcode ret;
  struct stat st;
  static struct curl_slist *post_commands = NULL;
  static char *passwd_string = NULL;
  static char *url_string = NULL;

  infile = fopen(local, "r");

  if (!infile) {
    camlog("camE: Couldn't open temp file to upload it\n");
    perror("ftp_upload(): ");
    return;
  }
  fstat(fileno(infile), &st);

  if (!post_commands) {
    if (ftp_delete_first) {
      snprintf(buf, sizeof(buf), "dele %s", remote);
      post_commands = curl_slist_append(post_commands, buf);
    }
    snprintf(buf, sizeof(buf), "rnfr %s", tmp);
    post_commands = curl_slist_append(post_commands, buf);
    snprintf(buf, sizeof(buf), "rnto %s", remote);
    post_commands = curl_slist_append(post_commands, buf);
  }

  /* init the curl session */
  if (connections < 1) {
    curl_handle = curl_easy_init();
    connections++;

    if (passwd_string)
      free(passwd_string);
    passwd_string = gib_strjoin(":", ftp_user, ftp_pass, NULL);
    curl_easy_setopt(curl_handle, CURLOPT_USERPWD, passwd_string);

    /* set URL to save to */
    if (url_string)
      free(url_string);
    url_string = gib_strjoin("/", "ftp:/", ftp_host, ftp_dir, tmp, NULL);
    curl_easy_setopt(curl_handle, CURLOPT_URL, url_string);

    /* no progress meter please */
    curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1);

    /* shut up completely */
    if (ftp_debug)
      curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1);
    else
      curl_easy_setopt(curl_handle, CURLOPT_MUTE, 1);

    curl_easy_setopt(curl_handle, CURLOPT_UPLOAD, 1);

    if (!ftp_passive)
      curl_easy_setopt(curl_handle, CURLOPT_FTPPORT, ftp_interface);

    curl_easy_setopt(curl_handle, CURLOPT_POSTQUOTE, post_commands);
  }

  curl_easy_setopt(curl_handle, CURLOPT_INFILE, infile);
  curl_easy_setopt(curl_handle, CURLOPT_INFILESIZE, st.st_size);

  /* get it! */
  ret = curl_easy_perform(curl_handle);
  /* TODO check error */
  if (ret) {
    fprintf(stderr, "\ncamE: error sending via ftp: ");
    camlog("camE error: ");
    switch (ret) {
      case CURLE_URL_MALFORMAT:
        fprintf(stderr, "Badly formatted ftp host or directory\n");
        camlog("Badly formatted ftp host or directory\n");
        break;
      case CURLE_URL_MALFORMAT_USER:
        fprintf(stderr, "Badly formatted ftp username\n");
        camlog("Badly formatted ftp username\n");
        break;
      case CURLE_COULDNT_RESOLVE_PROXY:
        fprintf(stderr, "Couldn't resolve proxy\n");
        camlog("Couldn't resolve proxy\n");
        break;
      case CURLE_COULDNT_RESOLVE_HOST:
        fprintf(stderr, "Unable to resolve ftp host\n");
        camlog("Unable to resolve ftp host\n");
        break;
      case CURLE_COULDNT_CONNECT:
        fprintf(stderr, "Unable to connect to ftp host\n");
        camlog("Unable to connect to ftp host\n");
        break;
      case CURLE_FTP_WEIRD_SERVER_REPLY:
        fprintf(stderr, "Wierd server reply detected\n");
        camlog("Wierd server reply detected\n");
        break;
      case CURLE_FTP_ACCESS_DENIED:
        fprintf(stderr, "Access denied to ftp upload\n");
        camlog("Access denied to ftp upload\n");
        break;
      case CURLE_FTP_USER_PASSWORD_INCORRECT:
        fprintf(stderr, "Incorrect password for ftp login\n");
        camlog("Incorrect password for ftp login\n");
        break;
      case CURLE_FTP_WEIRD_PASS_REPLY:
        fprintf(stderr, "Wierd password reply from server\n");
        camlog("Wierd password reply from server\n");
        break;
      case CURLE_FTP_WEIRD_USER_REPLY:
        fprintf(stderr, "Wierd user reply from server\n");
        camlog("Wierd user reply from server\n");
        break;
      case CURLE_FTP_WEIRD_PASV_REPLY:
        fprintf(stderr, "Wierd passive reply from server\n");
        camlog("Wierd passive reply from server\n");
        break;
      case CURLE_FTP_CANT_GET_HOST:
        fprintf(stderr, "No route to host\n");
        camlog("No route to host\n");
        break;
      case CURLE_FTP_COULDNT_SET_BINARY:
        fprintf(stderr, "Couldn't set binary mode\n");
        camlog("Couldn't set binary mode\n");
        break;
      case CURLE_PARTIAL_FILE:
        fprintf(stderr, "Only partial file uploaded\n");
        camlog("Only partial file uploaded\n");
        break;
      case CURLE_FTP_WRITE_ERROR:
        fprintf(stderr, "Write error\n");
        camlog("Write error\n");
        break;
      case CURLE_FTP_QUOTE_ERROR:
        fprintf(stderr, "Misquoted ftp command - check ftp config\n");
        camlog("Misquoted ftp command - check ftp config\n");
        break;
      case CURLE_WRITE_ERROR:
        fprintf(stderr, "Write error\n");
        camlog("Write error\n");
        break;
      case CURLE_MALFORMAT_USER: /* the user name is illegally specified */
        fprintf(stderr, "Malformatted username\n");
        camlog("Malformatted username\n");
        break;
      case CURLE_FTP_COULDNT_STOR_FILE: /* failed FTP upload */
        fprintf(stderr, "Couldn't STOR the file\n");
        camlog("Couldn't STOR the file\n");
        break;
      case CURLE_READ_ERROR: /* could open/read from file */
        fprintf(stderr, "Couldn't open temp file\n");
        camlog("Couldn't open temp file\n");
        break;
      case CURLE_OUT_OF_MEMORY:
        fprintf(stderr, "Out of memory\n");
        camlog("Out of memory\n");
        break;
      case CURLE_OPERATION_TIMEOUTED: /* the timeout time was reached */
        fprintf(stderr, "Upload timed out\n");
        camlog("Upload timed out\n");
        break;
      case CURLE_FTP_PORT_FAILED: /* FTP PORT operation failed */
        fprintf(stderr, "ftp PORT failed\n");
        camlog("ftp PORT failed\n");
        break;
      case CURLE_FILE_COULDNT_READ_FILE:
        fprintf(stderr, "Couldn't read temp file\n");
        camlog("Couldn't read temp file\n");
        break;
      default:
        fprintf(stderr, "unknown error, attempting to continue\n");
        camlog("unknown error, attempting to continue\n");
        break;
    }
  }

  /* cleanup curl stuff */
  if (!ftp_keepalive) {
    curl_easy_cleanup(curl_handle);
    connections--;
    curl_slist_free_all(post_commands);
    post_commands = NULL;
  }
  fclose(infile);
}

int
check_interface(char *watch_interface)
{
  if (watch_interface == NULL)
    return 1;
  return if_nametoindex(watch_interface);
}

void
version(void)
{
  printf("camE version %s\n", VERSION);
  exit(0);
}

void
usage(void)
{
  printf("usage: camE [OPTION]\n");
  printf("       -c FILE    Use config file FILE\n");
  printf("       -f         Don't fork to background\n");
  printf("       -d         Detach after forking\n");
  printf("       -s         Single shot\n");
  printf("       -h -v      This message\n");
  exit(0);
}

int
do_upload(char *file)
{
  struct stat st;
  int upload_successful = 1;
  static int shot_counter = 0;

  shot_counter++;

  if (ftp_do && (shot_counter >= ftp_upload_every)) {
    if ((upload_blockfile && (stat(upload_blockfile, &st) == -1))
        || !upload_blockfile) {
      camlog("*** uploading via ftp\n");
      ftp_upload(file, ftp_file, ftp_tmp);
      camlog("shot uploaded\n");
      if (action_post_upload) {
        camlog("running post upload action\n");
        system(action_post_upload);
        camlog("post upload action done\n");
      }
    }
    shot_counter = 0;
  } else if (scp_target && (shot_counter >= ftp_upload_every)) {
    char target_buf[2048];
    char cmd_buf[4096];
    char *scp_args[] = { "scp", "-BCq", NULL, NULL, NULL };
    char *ssh_args[] = { "ssh", "-n", "-q", NULL, NULL, NULL };

    if (!upload_blockfile
        || (upload_blockfile && (stat(upload_blockfile, &st) == -1))) {
      camlog("uploading via scp\n");
      snprintf(target_buf, sizeof(target_buf), "%s:%s/%s", scp_target,
               ftp_dir, ftp_tmp);
      snprintf(cmd_buf, sizeof(cmd_buf), "mv %s/%s %s/%s", ftp_dir, ftp_tmp,
               ftp_dir, ftp_file);
      scp_args[2] = file;
      scp_args[3] = target_buf;
      if ((upload_successful =
           execvp_with_timeout(scp_timeout, "scp", scp_args))) {
        ssh_args[3] = scp_target;
        ssh_args[4] = cmd_buf;
        if ((upload_successful =
             execvp_with_timeout(scp_timeout, "ssh", ssh_args))) {
          camlog("shot uploaded\n");

          if (action_post_upload) {
            camlog("running post upload action\n");
            system(action_post_upload);
            camlog("post upload action done\n");
          }
        }
      }
    }
    shot_counter = 0;
  }
  return upload_successful;
}

int
main(int argc,
     char *argv[])
{
  unsigned char *val;
  Imlib_Image image, tmp_image;
  char filename[100];
  int width, height, i;
  struct stat st;
  pid_t childpid;
  float f;
  struct timeval start_shot;
  struct timeval end_shot;
  struct timeval new_delay;
  int just_shot = 0, upload_successful = 1;
  FILE *fp;
  int ch;
  int dont_fork = 0;
  int single_shot = 0;
  int offline_done = 1;
  char *config_file = NULL;
  int detach = 0;
  int ret;

  while ((ch = getopt(argc, argv, "c:fsdhv")) != EOF) {
    switch (ch) {
      case 'c':
        config_file = strdup(optarg);
        break;
      case 'f':
        if (detach) {
          fprintf(stderr, "-f does not make sense with -d specified.\n");
          exit(1);
        }
        dont_fork = 1;
        break;
      case 'd':
        if (dont_fork) {
          fprintf(stderr, "-d does not make sense with -f specified.\n");
          exit(1);
        }
        detach = 1;
        break;
      case 's':
        single_shot = 1;
        break;
      case 'h':
        usage();
        break;
      case 'v':
        version();
        break;
      case '?':
      default:
        usage();
        break;
    }
  }

  if (!dont_fork && !single_shot) {
    /* fork and die */
    if (detach) {
      ret = daemon(1,0);
      if (ret != 0)
	fprintf(stderr, "daemon (%s)\n", strerror(errno));
    } else if ((childpid = fork()) < 0) {
      fprintf(stderr, "fork (%s)\n", strerror(errno));
      return (2);
    } else if (childpid > 0)
      exit(0);          /* parent */
  }

  /* read config */
  if (config_file) {
    cfg_parse_file(config_file);
  } else {
    sprintf(filename, "%s/%s", getenv("HOME"), ".camErc");
    memset(filename, '\0', sizeof(filename));
    snprintf(filename, sizeof(filename)-1, "%s/%s", getenv("HOME"), ".camErc");
    cfg_parse_file(filename);
  }

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
  if (-1 != (i = cfg_get_int("ftp", "upload_every")))
    ftp_upload_every = i;
  if (-1 != (i = cfg_get_int("ftp", "keepalive")))
    ftp_keepalive = i;
  if (-1 != (i = cfg_get_int("ftp", "timeout")))
    ftp_timeout = i;
  if (NULL != (val = cfg_get_str("ftp", "interface")))
    ftp_interface = val;
  if (-1 != (i = cfg_get_int("ftp", "delete_first")))
    ftp_delete_first = i;

  if (NULL != (val = cfg_get_str("scp", "target")))
    scp_target = val;
  if (-1 != (i = cfg_get_int("scp", "timeout")))
    scp_timeout = i;

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
  if (NULL != (val = cfg_get_str("grab", "archive_thumbnails_dir")))
    archive_thumbnails_dir = val;
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
  if (NULL != (val = cfg_get_str("grab", "watch_interface")))
    watch_interface = val;
  if (NULL != (val = cfg_get_str("grab", "offline_image")))
    offline_image = val;
  if (NULL != (val = cfg_get_str("grab", "pwc_wb_mode")))
    pwc_wb_mode = val;
  if (-1 != (i = cfg_get_int("grab", "width")))
    grab_width = i;
  if (-1 != (i = cfg_get_int("grab", "height")))
    grab_height = i;
  if (-1 != (f = cfg_get_float("grab", "delay")))
    grab_delay = f;
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
  if (-1 != (i = cfg_get_int("grab", "colour")))
    cam_colour = i;
  if (-1 != (i = cfg_get_int("grab", "brightness")))
    cam_brightness = i;
  if (-1 != (i = cfg_get_int("grab", "contrast")))
    cam_contrast = i;
  if (-1 != (i = cfg_get_int("grab", "hue")))
    cam_hue = i;
  if (-1 != (i = cfg_get_int("grab", "whiteness")))
    cam_whiteness = i;
  if (-1 != (i = cfg_get_int("grab", "framerate")))
    cam_framerate = i;
  if (-1 != (i = cfg_get_int("grab", "crop")))
    crop = i;
  if (-1 != (i = cfg_get_int("grab", "crop_width")))
    crop_width = i;
  if (-1 != (i = cfg_get_int("grab", "crop_height")))
    crop_height = i;
  if (-1 != (i = cfg_get_int("grab", "crop_x")))
    crop_x = i;
  if (-1 != (i = cfg_get_int("grab", "crop_y")))
    crop_y = i;
  if (-1 != (i = cfg_get_int("grab", "scale")))
    scale = i;
  if (-1 != (i = cfg_get_int("grab", "scale_width")))
    scale_width = i;
  if (-1 != (i = cfg_get_int("grab", "scale_height")))
    scale_height = i;
  if (-1 != (i = cfg_get_int("grab", "archive_shot_every")))
    archive_shot_every = i;
  if (-1 != (i = cfg_get_int("grab", "archive_subdirs")))
    archive_subdirs = i;
  if (-1 != (i = cfg_get_int("grab", "archive_thumbnails_create")))
    archive_thumbnails_create = i;
  if (-1 != (i = cfg_get_int("grab", "archive_thumbnails_width")))
    archive_thumbnails_width = i;
  if (-1 != (i = cfg_get_int("grab", "archive_thumbnails_height")))
    archive_thumbnails_height = i;
  if (-1 != (i = cfg_get_int("grab", "pwc_wb_red")))
    pwc_wb_red = i;
  if (-1 != (i = cfg_get_int("grab", "pwc_wb_blue")))
    pwc_wb_blue = i;
  if (-1 != (i = cfg_get_int("grab", "pwc_backlight_mode")))
    pwc_backlight_mode = i;
  if (-1 != (i = cfg_get_int("grab", "flip_horizontal")))
    flip_horizontal = 1;
  if (-1 != (i = cfg_get_int("grab", "flip_vertical")))
    flip_vertical = 1;
  if (-1 != (i = cfg_get_int("grab", "orientation")))
    orientation = i;

  if (cam_framerate > 60)
    cam_framerate = 60;
  if (cam_framerate < 1)
    cam_framerate = 1;

  /* clear logfile */
  if (logfile) {
    fp = fopen(logfile, "w");
    if (fp)
      fclose(fp);
  }

  /* print config */
  camlog("camE " VERSION " - (c) 1999, 2000 Gerd Knorr, Tom Gilbert\n");
  camlog("grabber config: size %dx%d, input %d, norm %d, " "jpeg quality %d\n",
      grab_width, grab_height, grab_input, grab_norm, grab_quality);

  imlib_context_set_direction(IMLIB_TEXT_TO_RIGHT);
  imlib_add_path_to_font_path(ttf_dir);
  imlib_add_path_to_font_path(".");
  imlib_context_set_operation(IMLIB_OP_COPY);
  imlib_set_cache_size(0);
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
  if (offline_image) {
    Imlib_Image image = imlib_load_image(offline_image);

    if (!image) {
      fprintf(stderr, "can't load offline image %s, ignoring\n",
              offline_image);
      offline_image = NULL;
    } else {
      imlib_context_set_image(image);
      imlib_free_image_and_decache();
    }
  }

  /* init everything */
  grab_init();

  /* go! */
  do {
    just_shot = 0;
    upload_successful = 1;
    end_shot.tv_sec = 0;
    end_shot.tv_usec = 0;
    start_shot.tv_sec = 0;
    start_shot.tv_usec = 0;

    if (((grab_blockfile && (stat(grab_blockfile, &st) == -1))
         || !grab_blockfile) && check_interface(watch_interface)) {
      offline_done = 0;
      gettimeofday(&start_shot, 0);
      if (action_pre_shot) {
        camlog("running pre-shot action\n");
        system(action_pre_shot);
        camlog("pre-shot action done\n");
      }

      camlog("* taking shot\n");
      /* Prevent camera lag... */
      image = grab_one(&width, &height);
      imlib_context_set_image(image);
      if (!image) {
        fprintf(stderr, "no image captured\n");
        exit(2);
      }


      if (crop) {
        if (!crop_width)
          crop_width = grab_width;
        if (!crop_height)
          crop_height = grab_height;
        tmp_image =
          gib_imlib_create_cropped_scaled_image(image, crop_x, crop_y,
                                                crop_width, crop_height,
                                                crop_width, crop_height, 1);
        gib_imlib_free_image_and_decache(image);
        image = tmp_image;
        imlib_context_set_image(image);
	
	/* Set new values for width and height, else the image's 
	   text might not show up in the correct position. */
	width = crop_width;
	height = crop_height;
      }

      if (scale) {
        if (!scale_width)
          scale_width = grab_width;
        if (!scale_height)
          scale_height = grab_height;

        tmp_image =
          gib_imlib_create_cropped_scaled_image(image, 0, 0, scale_width,
                                                scale_height, scale_width,
                                                scale_height, 1);
        gib_imlib_blend_image_onto_image(tmp_image, image, 1, 0, 0,
                                         grab_width, grab_height, 0, 0,
                                         scale_width, scale_height, 1, 0, 0);
        gib_imlib_free_image_and_decache(image);
        image = tmp_image;
        imlib_context_set_image(image);

	/* Set new values for width and height, else the image's 
	   text might not show up in the correct position. */
	width = scale_width;
	height = scale_height;
      }

      camlog("** shot taken\n");
      if (action_post_shot) {
        camlog("running post-shot action\n");
        system(action_post_shot);
        camlog("post-shot action done\n");
      }

      if (flip_horizontal) {
        imlib_image_flip_horizontal();
      }
      if (flip_vertical) {
        imlib_image_flip_vertical();
      }
      
      if (orientation && orientation > 0 && orientation < 4) {
        imlib_image_orientate(orientation);
	/* Changing orientation flips width and height, so we must swap them. */
	int swap_dimensions;
	if(orientation == 1 || orientation == 3) {
	  swap_dimensions = height;
	  height = width;
	  width = swap_dimensions;
	}
      }
      
      if (overlay_im)
        draw_overlay(image);
      add_time_text(image, get_message(), width, height);
      save_image(image, temp_file);
      do_postprocess(temp_file);
      archive_jpeg(image);
      gib_imlib_free_image_and_decache(image);
      upload_successful = do_upload(temp_file);
      just_shot = 1;
      gettimeofday(&end_shot,0);
    } else if (offline_image && !offline_done
               && ((upload_blockfile && (stat(upload_blockfile, &st) == -1))
                   || !upload_blockfile) && check_interface(watch_interface)) {
      /* blockfile was just created */
      camlog("uploading offline image\n");
      offline_done = do_upload(offline_image);
      camlog("OFFLINE\n");
    }

    if (!single_shot) {
      new_delay = timeval_float(grab_delay);
      if (just_shot && upload_successful) {
        end_shot = sub_timevals(end_shot, start_shot);
        if (bw_percent < 100)
          bw_res_change(end_shot);
        if (delay_correct) {
          new_delay = sub_timevals(new_delay, end_shot);
          if (float_timeval(new_delay) < 0) {
            new_delay.tv_sec = 0;
            new_delay.tv_usec = 0;
          }
          camlog("Sleeping %d secs (corrected)\n", new_delay);
        } else {
          camlog("Sleeping %d secs\n", grab_delay);
        }
      }
      if (upload_successful && (float_timeval(new_delay) > 0))
        select( 0, 0, 0, 0, &new_delay );
    }
  } while(!single_shot);
  return 0;
}

int
execvp_with_timeout(int timeout,
                    char *file,
                    char **argv)
{
  int status, ret;

  signal(SIGALRM, alarm_handler);
  alarm(timeout);

  if ((childpid = fork()) < 0) {
    fprintf(stderr, "fork (%s)\n", strerror(errno));
    exit(2);
  } else if (childpid == 0) {
    /* child */
    execvp(file, argv);
    fprintf(stderr, "execvp %s (%s)\n", file, strerror(errno));
    exit(2);
  } else if (childpid > 0) {
    /* parent */
    ret = waitpid(childpid, &status, 0);
    alarm(0);
    childpid = 0;
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
      camlog("exec failed for %s\n", file);
      return 0;
    }
  }
  return 1;
}

void
alarm_handler(int sig)
{
  signal(sig, SIG_IGN);
  camlog("timeout reached, abandoning\n");
  if (childpid) {
    kill(childpid, SIGTERM);
  }
}
