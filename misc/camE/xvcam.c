/* View streams from a webcam with Xv */
/* Parts from an app used to test Trident drivers, and camE */
/* $Id$ */
/* gcc xvcam.c -o xvcam -I/usr/X11R6/include -L/usr/X11R6/lib -lX11 -lXext -lXv */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <fcntl.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xv.h>
#include <X11/extensions/Xvlib.h>
#include <X11/extensions/XShm.h>

#include <linux/videodev.h>

#define GUID_I420_PLANAR 0x30323449


extern int XShmGetEventBase(Display *);
extern XvImage *XvShmCreateImage(Display *, XvPortID, int, char *, int, int,
                                 XShmSegmentInfo *);

int grab_fd, grab_size;
static struct video_mmap grab_buf;
int yuv_width = 320;
int yuv_height = 240;
static unsigned char *grab_data = NULL;

void
deinit()
{
   munmap(grab_data, grab_size);
   grab_data = NULL;
   close(grab_fd);
}

void
init()
{
   struct video_capability grab_cap;
   struct video_channel grab_chan;
   struct video_mbuf vid_mbuf;

   atexit(deinit);

   if ((grab_fd = open("/dev/video0", O_RDWR)) == -1) {
      fprintf(stderr, "Couldn't open /dev/video0: %s\n", strerror(errno));
      exit(-1);
   }

   if (ioctl(grab_fd, VIDIOCGCAP, &grab_cap) == -1) {
      perror("ioctl VIDIOCGCAP");
      exit(-1);
   }

   grab_chan.channel = 0;

   if (ioctl(grab_fd, VIDIOCGCHAN, &grab_chan) == -1) {
      perror("ioctl VIDOCGCHAN");
      exit(-1);
   }

   grab_chan.norm = 0;

   if (ioctl(grab_fd, VIDIOCSCHAN, &grab_chan) == -1) {
      perror("ioctl VIDIOCSCHAN");
      exit(-1);
   }

   grab_buf.format = VIDEO_PALETTE_YUV420P;
   grab_buf.frame = 0;
   grab_buf.width = yuv_width;
   grab_buf.height = yuv_height;

   ioctl(grab_fd, VIDIOCGMBUF, &vid_mbuf);

   grab_size = vid_mbuf.size;
   grab_data =
       mmap(0, grab_size, PROT_READ | PROT_WRITE, MAP_SHARED, grab_fd, 0);

   if ((grab_data == NULL) || (-1 == (int) grab_data)) {
      fprintf(stderr, "Couldn't mmap\n");
      exit(1);
   }

   /* Useless? probably. */
   setpriority(PRIO_PROCESS, 0, 20);
   nice(20);
}


void
grab_image()
{
   int i = 0;
   fd_set fds;

   FD_ZERO(&fds);
   FD_SET(0, &fds);
   FD_SET(grab_fd, &fds);

   /* Maybe a little nicer to the cpu ? */
   select(grab_fd + 1, &fds, NULL, NULL, NULL); 

   if (ioctl(grab_fd, VIDIOCMCAPTURE, &grab_buf) == -1) {
      perror("ioctl VIDIOCMCAPTURE");
      return;
   }

   if (ioctl(grab_fd, VIDIOCSYNC, &i) == -1) {
      perror("ioctrl VIDIOCSYNC");
      return;
   }
   return;
}

int
main(int argc, char **argv)
{
   int xv_port = -1, i, d, screen, CompletionType;
   unsigned int ud, width, height;
   long frames;
   unsigned int ver, rel, req, ev, err, adapt;
   Display *dpy;
   Window window, _dw;
   XSizeHints hint;
   XSetWindowAttributes xswa;
   XWindowAttributes attribs;
   XVisualInfo vinfo;
   XEvent event;
   GC gc;
   XvAdaptorInfo *ai;
   XvImage *yuv_image;
   XShmSegmentInfo yuv_shminfo;
   Atom wmDeleteWindow;

   dpy = XOpenDisplay(NULL);
   screen = DefaultScreen(dpy);

   XGetWindowAttributes(dpy, DefaultRootWindow(dpy), &attribs);
   XMatchVisualInfo(dpy, screen, attribs.depth, TrueColor, &vinfo);
   wmDeleteWindow = XInternAtom(dpy, "WM_DELETE_WINDOW", False);

   hint.x = 1;
   hint.y = 1;
   hint.width = yuv_width;
   hint.height = yuv_height;
   hint.flags = PPosition | PSize;

   xswa.colormap = XCreateColormap(dpy, DefaultRootWindow(dpy), vinfo.visual, 
                                   AllocNone);
   xswa.event_mask = StructureNotifyMask | ExposureMask;
   xswa.background_pixel = 0;
   xswa.border_pixel = 0;

   window = XCreateWindow(dpy, DefaultRootWindow(dpy), 0, 0, yuv_width, 
                          yuv_height, 0, vinfo.depth, InputOutput, vinfo.visual, 
                          CWBackPixel | CWBorderPixel | CWColormap | 
                          CWEventMask, &xswa);

   XSelectInput(dpy, window, StructureNotifyMask);
   XSetStandardProperties(dpy, window, "xvcam", "xvcam", None, NULL, 0,
                          &hint);

   XSetWMProtocols(dpy, window, &wmDeleteWindow, 1);
   XMapWindow(dpy, window);

   if (XShmQueryExtension(dpy))
      CompletionType = XShmGetEventBase(dpy) + ShmCompletion;
   else
      exit(-1);

   if (Success !=
       XvQueryExtension(dpy, &ver, &rel, &req, &ev, &err))
      fprintf(stderr, "Couldn't do Xv stuff\n");

   if (Success !=
       XvQueryAdaptors(dpy, DefaultRootWindow(dpy), &adapt, &ai))
      fprintf(stderr, "Couldn't do Xv stuff\n");

   for (i = 0; i < (int) adapt; i++) {
      xv_port = ai[i].base_id;
   }
   
   if (adapt > 0)
      XvFreeAdaptorInfo(ai);
   
   gc = XCreateGC(dpy, window, 0, 0);

   yuv_image = XvShmCreateImage(dpy, xv_port, GUID_I420_PLANAR, 0, yuv_width,
                                yuv_height, &yuv_shminfo);
   yuv_shminfo.shmid = shmget(IPC_PRIVATE, yuv_image->data_size, 
                              IPC_CREAT | 0777);
   yuv_shminfo.shmaddr = (char *) shmat(yuv_shminfo.shmid, 0, 0);
   yuv_image->data = yuv_shminfo.shmaddr;
   yuv_shminfo.readOnly = False;

   if (!XShmAttach(dpy, &yuv_shminfo)) {
      printf("XShmAttach go boom boom!\n");
      exit(-1);
   }

   init();

   while (1) {
      grab_image();
      memcpy(yuv_image->data, grab_data, yuv_image->data_size);

      XGetGeometry(dpy, window, &_dw, &d, &d, &width, &height, &ud, &ud);
      XvShmPutImage(dpy, xv_port, window, gc, yuv_image, 0, 0,
                    yuv_image->width, yuv_image->height, 0, 0, width, height,
                    True);
      frames++;

      while (XPending(dpy)) {
         XNextEvent(dpy, &event);
            if (event.type == ClientMessage) {
               if (event.xclient.format == 32 &&
                   event.xclient.data.l[0] == (signed) wmDeleteWindow)
                  exit(0);
            }
      }
               
   }
   return 0;
}
