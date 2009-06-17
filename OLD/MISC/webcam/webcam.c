/*
 * (c) 1998,99 Gerd Knorr
 *
 *    capture a image, compress as jpeg and upload to the webserver
 *    using ftp
 *
 */

/*
 *
 * (c) 2000 Mandrake <mandrake@mandrake.net>
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

#include <asm/types.h>
#include "videodev.h"	/* change this to "videodev2.h" for v4l2 */

#include "jpeglib.h"

/* ---------------------------------------------------------------------- */
/* configuration                                                          */

#define FTP_HOST          "www"
#define FTP_USER          "webcam"
#define FTP_PASS          "xxxxxx"
#define FTP_DIRECTORY     "public_html/images"
#define FTP_FILE          "webcam.jpeg"
#define FTP_TMPFILE       "uploading.jpeg"
#define FTP_INIT_ONCE     "pass"		/* call once at start */
#define FTP_INIT_CONN     "bin","umask 022"	/* per connection */

#define JPEG_FILE         "/home/httpd/html/webcam.jpg"
#define JPEG_QUALITY      75

#define GRAB_DEVICE       "/dev/video0"
#define GRAB_WIDTH        320
#define GRAB_HEIGHT       240
#define GRAB_TEXT         "%m/%d/%y %H:%M"        /* strftime */

#ifdef VIDIOCGCAP
/* these work for v4l only, not v4l2 */
# define GRAB_SOURCE      1
# define GRAB_NORM        VIDEO_MODE_NTSC
#endif

#define GRAB_DELAY        55

/* ---------------------------------------------------------------------- */

void swap_rgb24(char *mem, int n);
static int ftp_debug = 0;

/* ---------------------------------------------------------------------- */
/* jpeg stuff                                                             */

	int
write_jpeg(char *filename, char *data, int width, int height)
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	FILE *fp;
	int i;
	unsigned char *line;

	if (NULL == (fp = fopen(filename,"w"))) {
		fprintf(stderr,"can't open %s for writing: %s\n",
				filename,strerror(errno));
		return -1;
	}

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	jpeg_stdio_dest(&cinfo, fp);
	cinfo.image_width = width;
	cinfo.image_height = height;
	cinfo.input_components = 3;
	cinfo.in_color_space = JCS_RGB;
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, JPEG_QUALITY, TRUE);
	jpeg_start_compress(&cinfo, TRUE);

	for (i = 0, line = data; i < height; i++, line += width*3)
		jpeg_write_scanlines(&cinfo, &line, 1);

	jpeg_finish_compress(&(cinfo));
	jpeg_destroy_compress(&(cinfo));
	fclose(fp);

	return 0;
}

/* ---------------------------------------------------------------------- */
/* FTP stuff                                                              */

static int ftp_pty, ftp_pid,ftp_connected;

static char pty_name[32];
static char tty_name[32];

void ftp_send(char *command);
int  ftp_recv();

int open_pty()
{
	static char s1[] = "pqrs";
	static char s2[] = "0123456789abcdef";

	char *p1,*p2;
	int pty;

	for (p1 = s1; *p1; p1++) {
		for (p2 = s2; *p2; p2++) {
			sprintf(pty_name,"/dev/pty%c%c",*p1,*p2);
			sprintf(tty_name,"/dev/tty%c%c",*p1,*p2);
			if (-1 == access(tty_name,R_OK|W_OK))
				continue;
			if (-1 != (pty = open(pty_name,O_RDWR)))
				return pty;
		}
	}
	return -1;
}

	void
ftp_init()
{
	static char *argv[] = { "ftp", "-n", NULL };
	static char *init_commands[] = { FTP_INIT_ONCE, NULL };
	int i;

	if (-1 == (ftp_pty = open_pty())) {
		fprintf(stderr,"can't grab pty\n");
		exit(1);
	}
	switch (ftp_pid = fork()) {
		case -1:
			perror("fork");
			exit(1);
		case 0:
			/* child */
			close(ftp_pty);
			close(0); close(1); close(2);
			setsid();
			open(tty_name,O_RDWR); dup(0); dup(0);
			execvp(argv[0],argv);
			perror("execvp");
			exit(1);
		default:
			/* parent */
			break;
	}
	ftp_recv();

	/* initialisation */
	for (i = 0; init_commands[i] != NULL; i++) {
		ftp_send(init_commands[i]);
		ftp_recv();
	}
	return;
}

	void
ftp_send(char *command)
{
	char line[128];
	int length;

	length = sprintf(line,"%s\n",command);
	if (ftp_debug)
		fprintf(stderr,">> %s",line);
	if (length != write(ftp_pty,line,length)) {
		fprintf(stderr,"ftp: write error\n");
		exit(1);
	}
}

	int
ftp_recv()
{
	char line[512],*p,*n;
	int length, done, status, ret=0;
	fd_set set;

	for (done = 0; !done;) {
		FD_ZERO(&set);
		FD_SET(ftp_pty,&set);
		select(ftp_pty+1,&set,NULL,NULL,NULL);

		switch (length = read(ftp_pty,line,511)) {
			case -1:
				perror("ftp: read error");
				exit(1);
			case 0:
				fprintf(stderr,"ftp: EOF\n");
				exit(1);
		}
		line[length] = 0;

		for (p=line; p && *p; p = n) {
			/* split into lines */
			if (NULL != (n = strchr(p,'\n')) || NULL != (n = strchr(p,'\r')))
				*(n++) = 0;
			else
				n = NULL;
			if (ftp_debug)
				fprintf(stderr,"<< %s\n",p);

			/* prompt? */
			if (NULL != strstr(p,"ftp>")) {
				done = 1;
			}

			/* line dropped ? */
			if (NULL != strstr(p,"closed connection")) {
				fprintf(stderr,"ftp: lost connection\n");
				ftp_connected = 0;
			}
			if (NULL != strstr(p,"Not connected")) {
				if (ftp_connected)
					fprintf(stderr,"ftp: lost connection\n");
				ftp_connected = 0;
			}

			/* status? */
			if (1 == sscanf(p,"%d",&status)) {
				ret = status;
			}
		}
	}
	return ret;
}

	void
ftp_connect()
{
	int i,delay = 0;
	static char *init_commands[] = { FTP_INIT_CONN, NULL };

	for (;;) {
		/* Wiederholungsversuche mit wachsendem Intervall, 10 min max. */
		if (delay) {
			fprintf(stderr,"ftp: connect failed, sleeping %d sec\n",delay);
			sleep(delay);
			delay *= 2;
			if (delay > 600)
				delay = 600;
		} else {
			delay = 5;
		}

		/* (re-) connect */
		ftp_send("close");
		ftp_recv();
		ftp_send("open " FTP_HOST);
		if (220 != ftp_recv())
			continue;

		fprintf(stderr,"ftp: connected to " FTP_HOST "\n");
		ftp_connected = 1;

		/* login */
		ftp_send("user " FTP_USER " " FTP_PASS);
		if (230 != ftp_recv()) {
			if (!ftp_connected)
				continue;
			fprintf(stderr,"ftp: login incorrect\n");
			exit(1);
		}

		/* set directory */
		ftp_send("cd " FTP_DIRECTORY);
		if (250 != ftp_recv()) {
			if (!ftp_connected)
				continue;
			fprintf(stderr,"ftp: login incorrect\n");
			exit(1);
		}

		/* initialisation */
		for (i = 0; init_commands[i] != NULL; i++) {
			ftp_send(init_commands[i]);
			ftp_recv();
		}

		/* ok */
		break;
	}
}

	void
ftp_upload()
{
	ftp_send("put " JPEG_FILE " " FTP_TMPFILE);
	ftp_recv();
	ftp_send("rename " FTP_TMPFILE " " FTP_FILE);
	ftp_recv();
}


/* ---------------------------------------------------------------------- */
/* capture stuff  - v4l2                                                  */

#ifdef VIDIOC_QUERYCAP

static struct v4l2_capability    grab_cap;
static struct v4l2_format        grab_pix;
static int                       grab_fd, grab_size;
static unsigned char            *grab_data;

	void
grab_init()
{
	if (-1 == (grab_fd = open(GRAB_DEVICE,O_RDWR))) {
		perror("open " GRAB_DEVICE);
		exit(1);
	}
	if (-1 == ioctl(grab_fd,VIDIOC_QUERYCAP,&grab_cap)) {
		fprintf(stderr,"wrong device\n");
		exit(1);
	}
	if (-1 == ioctl(grab_fd, VIDIOC_G_FMT, &grab_pix)) {
		perror("ioctl VIDIOC_G_FMT");
		exit(1);
	}
	grab_pix.pixelformat = V4L2_PIX_FMT_BGR24;
	grab_pix.depth  = 24;
	grab_pix.width  = GRAB_WIDTH;
	grab_pix.height = GRAB_HEIGHT;
	if (-1 == ioctl(grab_fd, VIDIOC_S_FMT, &grab_pix)) {
		perror("ioctl VIDIOC_S_FMT");
		exit(1);
	}
	grab_size = grab_pix.width * grab_pix.height * ((grab_pix.depth+7)/8);
	fprintf(stderr,"grabber: using %dx%dx%d => %d byte\n",
			grab_pix.width,grab_pix.height,grab_pix.depth,grab_size);
	if (NULL == (grab_data = malloc(grab_size))) {
		fprintf(stderr,"out of virtual memory\n");
		exit(1);
	}
}

	unsigned char*
grab_one(int *width, int *height)
{
	int rc;

	for (;;) {
		rc = read(grab_fd,grab_data,grab_size);
		if (rc == grab_size) {
			swap_rgb24(grab_data,grab_pix.width*grab_pix.height);
			*width  = grab_pix.width;
			*height = grab_pix.height;
			return grab_data;
		}
		fprintf(stderr,"grabber: read: %d != %d\n",grab_size,rc);
		if (-1 == rc)
			perror("grabber: read");
	}
}

#endif


/* ---------------------------------------------------------------------- */
/* capture stuff  -  old v4l (bttv)                                       */

#ifdef VIDIOCGCAP

static struct video_capability   grab_cap;
static struct video_mmap         grab_buf;
static struct video_channel	 grab_chan;
static int                       grab_fd, grab_size;
static unsigned char            *grab_data;

	void
grab_init()
{
	if (-1 == (grab_fd = open(GRAB_DEVICE,O_RDWR))) {
		perror("open " GRAB_DEVICE);
		exit(1);
	}
	if (-1 == ioctl(grab_fd,VIDIOCGCAP,&grab_cap)) {
		fprintf(stderr,"wrong device\n");
		exit(1);
	}

	/* set image source and TV norm */
	grab_chan.channel = GRAB_SOURCE;
	if (-1 == ioctl(grab_fd,VIDIOCGCHAN,&grab_chan)) {
		perror("ioctl VIDIOCGCHAN");
		exit(1);
	}
	grab_chan.channel = GRAB_SOURCE;    
	grab_chan.norm    = GRAB_NORM;
	if (-1 == ioctl(grab_fd,VIDIOCSCHAN,&grab_chan)) {
		perror("ioctl VIDIOCSCHAN");
		exit(1);
	}

	grab_buf.format = VIDEO_PALETTE_RGB24;
	grab_buf.frame  = 0;
	grab_buf.width  = GRAB_WIDTH;
	grab_buf.height = GRAB_HEIGHT;
	grab_size = grab_buf.width * grab_buf.height * 3;
	grab_data = mmap(0,grab_size,PROT_READ|PROT_WRITE,MAP_SHARED,grab_fd,0);
	if (-1 == (int)grab_data) {
		perror("mmap");
		exit(1);
	}
}

	unsigned char*
grab_one(int *width, int *height)
{
	for (;;) {
		if (-1 == ioctl(grab_fd,VIDIOCMCAPTURE,&grab_buf)) {
			perror("ioctl VIDIOCMCAPTURE");
		} else {
			if (-1 == ioctl(grab_fd,VIDIOCSYNC,&grab_buf)) {
				perror("ioctl VIDIOCSYNC");
			} else {
				swap_rgb24(grab_data,grab_buf.width*grab_buf.height);
				*width  = grab_buf.width;
				*height = grab_buf.height;
				return grab_data;
			}
		}
		sleep(1);
	}
}

#endif

/* ---------------------------------------------------------------------- */

#if 0
# define CHAR_HEIGHT  8
# define CHAR_WIDTH   8
# define CHAR_START   1
# include "font_8x8.h"
#else
# define CHAR_HEIGHT  11
# define CHAR_WIDTH   6
# define CHAR_START   4
# include "font_6x11.h"
#endif

	void
add_text(char *image, int width, int height)
{
	time_t      t;
	struct tm  *tm;
	char        line[128],*ptr;
	int         i,x,y,f,len;
	char        line2[128];
	char        localtext[128];
	FILE       *camstring;

	time(&t);
	tm = localtime(&t);
	len = strftime(line,127,GRAB_TEXT,tm);


	sprintf(line2,"%s/.camstring",getenv("HOME"));
	camstring = fopen(line2,"r");
	fgets(localtext,128,camstring);
	fclose(camstring);
	localtext[strlen(localtext) -1 ] = 0;
	sprintf(line2,"%s : %s",line,localtext);
	fprintf(stderr,"%s\n",line2);
	len = strlen(line2);


	for (y = 0; y < CHAR_HEIGHT; y++) {
		ptr = image + 3 * width * (height-CHAR_HEIGHT-2+y) + 12;
		for (x = 0; x < len; x++) {
			f = fontdata[line2[x] * CHAR_HEIGHT + y];
			for (i = CHAR_WIDTH-1; i >= 0; i--) {
				if (f & (CHAR_START << i)) {

					int average;
					/*
					ptr[0] = 255;
					ptr[1] = 255;
					ptr[2] = 255;
					*/

					ptr[0] = 0;
					ptr[1] = 0;
					ptr[2] = 0;
				}
				ptr += 3;
			}
		}
	}
	for (y = 0; y < CHAR_HEIGHT; y++) {
		ptr = image + 3 * width * (height-CHAR_HEIGHT-3+y) + 10;
		for (x = 0; x < len; x++) {
			f = fontdata[line2[x] * CHAR_HEIGHT + y];
			for (i = CHAR_WIDTH-1; i >= 0; i--) {
				if (f & (CHAR_START << i)) {

					int average;
					/*
					ptr[0] = 255;
					ptr[1] = 255;
					ptr[2] = 255;
					*/

					ptr[0] = 255;
					ptr[1] = 255;
					ptr[2] = 255;
				}
				ptr += 3;
			}
		}
	}
}

	void
swap_rgb24(char *mem, int n)
{
	char  c;
	char *p = mem;
	int   i = n;

	while (--i) {
		c = p[0]; p[0] = p[2]; p[2] = c;
		p += 3;
	}
}

	int
main(int argc, char *argv[])
{
	unsigned char *image;
	int width, height;

	if (argc > 1)
		ftp_debug = 1;
	fprintf(stderr,"webcam config: size %dx%d, jpeg quality %d\n",
			GRAB_WIDTH,GRAB_HEIGHT,JPEG_QUALITY);
	/*
	   fprintf(stderr,"ftp config:\n  " FTP_USER "@" FTP_HOST ":" FTP_DIRECTORY
	   "\n  " FTP_TMPFILE " => " FTP_FILE "\n");
	 */

	grab_init();

	/*
	   ftp_init();
	   ftp_connect();
	 */

	for (;;) {
		/*
		   if (!ftp_connected)
		   ftp_connect();
		 */
		image = grab_one(&width,&height);
		add_text(image,width,height);
		write_jpeg(JPEG_FILE,image,width,height);
		/*
		   ftp_upload();
		 */
#if GRAB_DELAY > 0
		sleep(GRAB_DELAY);
#endif
		printf("5 seconds from camshot...\n");
		sleep(5);
	}
	return 0;
}
