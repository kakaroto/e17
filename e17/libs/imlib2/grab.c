#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "common.h"
#include "grab.h"

static char         _x_err = 0;

static void
Tmp_HandleXError(Display * d, XErrorEvent * ev)
{
  d = NULL;
  ev = NULL;
  _x_err = 1;
}

char
__imlib_GrabDrawableToRGBA(DATA32 *data, int ox, int oy, int ow, int oh,
			   Display *d, Drawable p, Pixmap m, Visual *v, 
			   Colormap cm, int depth, int x, int y, 
			   int w, int h, char domask, char grab)
{
   XErrorHandler       prev_erh = NULL;
   XWindowAttributes   xatt, ratt;
   char                is_pixmap = 0, created_mask = 0, is_shm = 0, is_mshm = 0;
   int                 i, pixel, mpixel;
   int                 src_x, src_y, src_w, src_h, origx, origy, origw, origh;
   int                 width, height, clipx, clipy, inx, iny;
   XShmSegmentInfo     shminfo, mshminfo;
   XImage             *xim = NULL, *mxim = NULL;
   static char         x_does_shm = -1;
   DATA32             *ptr;
   DATA8               rtab[256], gtab[256], btab[256];
   XColor              cols[256];
   
   origx = x;
   origy = y;
   origw = w;
   origh = h;
   /* FIXME:  hmm - need to co-ordinate this with the app */
   if (grab)
      XGrabServer(d);
   prev_erh = XSetErrorHandler((XErrorHandler) Tmp_HandleXError);
   _x_err = 0;
   /* lets see if its a pixmap or not */
   XGetWindowAttributes(d, p, &xatt);
   XSync(d, False);
   if (_x_err)
      is_pixmap = 1;
   /* reset our error handler */
   XSetErrorHandler((XErrorHandler) prev_erh);  
   if (is_pixmap)
     {
	Window dw;
	
	XGetGeometry(d, p, &dw, &src_x, &src_y, 
		     (unsigned int *)&src_w, (unsigned int *)&src_h,
		     (unsigned int *)&src_x, (unsigned int *)&xatt.depth);
	src_x = 0;
	src_y = 0;
     }
   else
     {
	Window dw;
	XGetWindowAttributes(d, xatt.root, &ratt);
	XTranslateCoordinates(d, p, xatt.root, 0, 0, &src_x, &src_y, &dw);
	src_w = xatt.width;
	src_h = xatt.height;
	if ((xatt.map_state != IsViewable) &&
	    (xatt.backing_store == NotUseful))
	  {
	     if (grab)
		XUngrabServer(d);
	     return 0;
	  }
     }
   
   /* clip to the drawable tree and screen */
   clipx = 0;
   clipy = 0;  
   width = src_w - x;
   height = src_h - y;
   if (width > w)
      width = w;
   if (height > h)
      height = h;
   
   if (!is_pixmap)
     {
	if ((src_x + x + width) > ratt.width)
	   width = ratt.width - (src_x + x);
	if ((src_y + y + height) > ratt.height)
	   height = ratt.height - (src_y + y);
     }
   if (x < 0)
     {
	clipx = -x;
	width += x;
	x = 0;
     }
   if (y < 0)
     {
	clipy = -y;
	height += y;
	y = 0;
     }
   if (!is_pixmap)
     {
	if ((src_x + x) < 0)
	  {
	     clipx -= (src_x + x);
	     width += (src_x + x);
	     x = -src_x;
	  }
	if ((src_y + y) < 0)
	  {
	     clipy -= (src_y + y);
	     height += (src_y + y);
	     y = -src_y;
	  }
     }
   if ((width <= 0) || (height <= 0))
     {
	if (grab)
	   XUngrabServer(d);
	return 0;
     }
   w = width;
   h = height;
   if ((!is_pixmap) && (domask) && (!m))
     {
	int ord, rect_no = 0;
	XRectangle *r = NULL;
	
	r = XShapeGetRectangles(d, p, ShapeBounding, &rect_no, &ord);
	if (r)
	  {
	     if (!((rect_no == 1) && 
		   (r[0].x == 0) && (r[0].y == 0) &&
		   (r[0].width == xatt.width) && (r[0].height == xatt.height)))
	       {
		  XGCValues gcv;
		  GC gc;
		  
		  created_mask = 1;
		  m = XCreatePixmap(d, p, w, h, 1);
		  gcv.foreground = 0;
		  gc = XCreateGC(d, m, GCForeground, &gcv);
		  XFillRectangle(d, m, gc, 0, 0, w, h);
		  XSetForeground(d, gc, 1);
		  for (i = 0; i < rect_no; i++)
		     XFillRectangle(d, m, gc, 
				    r[i].x - x, r[i].y - y, 
				    r[i].width, r[i].height);
		  XFreeGC(d, gc);
	       }
	     XFree(r);
	  }
     }
   /* Create an Ximage (shared or not) */
   if (x_does_shm < 0)
     {
	if (XShmQueryExtension(d))
	   x_does_shm = 1;
	else
	   x_does_shm = 0;
     }
   prev_erh = XSetErrorHandler((XErrorHandler) Tmp_HandleXError);
   _x_err = 0;
   if (x_does_shm)
     {
	xim = XShmCreateImage(d, v, xatt.depth, ZPixmap, NULL,
			      &shminfo, w, h);
	if (!xim)
	   xim = XGetImage(d, p, x, y, w, h, 
			   0xffffffff, ZPixmap);
	else
	  {
	     XSync(d, False);
	     if (_x_err)
	       {
		  XDestroyImage(xim);
		  xim = XGetImage(d, p, x, y, w, h, 
				  0xffffffff, ZPixmap);
		  _x_err = 0;
	       }
	     else
	       {
		  shminfo.shmid = shmget(IPC_PRIVATE, xim->bytes_per_line *
					 xim->height, IPC_CREAT | 0666);
		  if (shminfo.shmid < 0)
		    {
		       XDestroyImage(xim);
		       xim = XGetImage(d, p, x, y, w, h, 
				       0xffffffff, ZPixmap);
		    }
		  else
		    {
		       shminfo.shmaddr = xim->data = shmat(shminfo.shmid, 0, 0);
		       if (xim->data != (char *)-1)
			 {
			    shminfo.readOnly = False;
			    XShmAttach(d, &shminfo);
			    is_shm = 1;
			 }
		       else
			 {
			    shmctl(shminfo.shmid, IPC_RMID, 0);
			    XDestroyImage(xim);
			    xim = XGetImage(d, p, x, y, w, h, 
					    0xffffffff, ZPixmap);
			 }
		    }
	       }
	  }
     }
   else
      xim = XGetImage(d, p, x, y, w, h, 0xffffffff, ZPixmap);
   if (is_shm)
     {
	XShmGetImage(d, p, xim, x, y, 0xffffffff);
	XSync(d, False);
	if (_x_err)
	  {
	     shmdt(shminfo.shmaddr);
	     shmctl(shminfo.shmid, IPC_RMID, 0);
	     XDestroyImage(xim);
	     xim = XGetImage(d, p, x, y, w, h, 
			     0xffffffff, ZPixmap);
	     is_shm = 0;
	  }
     }
   XSetErrorHandler((XErrorHandler) prev_erh);
   if ((m) && (domask))
     {
	prev_erh = XSetErrorHandler((XErrorHandler) Tmp_HandleXError);
	_x_err = 0;
	if (x_does_shm)
	  {
	     mxim = XShmCreateImage(d, v, 1, ZPixmap, NULL,
				   &mshminfo, w, h);
	     if (!mxim)
		mxim = XGetImage(d, p, 0, 0, w, h, 
				 0xffffffff, ZPixmap);
	     else
	       {
		  XSync(d, False);
		  if (_x_err)
		    {
		       XDestroyImage(mxim);
		       xim = XGetImage(d, p, 0, 0, w, h, 
				       0xffffffff, ZPixmap);
		       _x_err = 0;
		    }
		  else
		    {
		       mshminfo.shmid = shmget(IPC_PRIVATE, 
					       mxim->bytes_per_line *
					       mxim->height, IPC_CREAT | 0666);
		       if (mshminfo.shmid < 0)
			 {
			    XDestroyImage(mxim);
			    mxim = XGetImage(d, p, 0, 0, w, h, 
					     0xffffffff, ZPixmap);
			 }
		       else
			 {
			    mshminfo.shmaddr = xim->data = 
			       shmat(mshminfo.shmid, 0, 0);
			    if (mxim->data != (char *)-1)
			      {
				 mshminfo.readOnly = False;
				 XShmAttach(d, &mshminfo);
				 is_mshm = 1;
			      }
			    else
			      {
				 shmctl(mshminfo.shmid, IPC_RMID, 0);
				 XDestroyImage(mxim);
				 mxim = XGetImage(d, p, x, y, w, h, 
						  0xffffffff, ZPixmap);
			      }
			 }
		    }
	       }
	  }
	else
	   mxim = XGetImage(d, m, 0, 0, w, h, 0xffffffff, ZPixmap);
	if (is_mshm)
	  {
	     XShmGetImage(d, p, mxim, 0, 0, 0xffffffff);
	     XSync(d, False);
	     if (_x_err)
	       {
		  shmdt(mshminfo.shmaddr);
		  shmctl(mshminfo.shmid, IPC_RMID, 0);
		  XDestroyImage(mxim);
		  mxim = XGetImage(d, p, 0, 0, w, h, 
				   0xffffffff, ZPixmap);
		  is_mshm = 0;
	       }
	     XSetErrorHandler((XErrorHandler) prev_erh);
	  }
     }
   if ((is_shm) || (is_mshm))
     {
	XSync(d, False);
	if (grab)
	   XUngrabServer(d);
	XSync(d, False);
     }
   else if (grab)
      XUngrabServer(d);
   
   if ((xatt.depth == 1) && (!cm) && (is_pixmap))
     {
	rtab[0] = 0;
	gtab[0] = 0;
	btab[0] = 0;
	rtab[1] = 255;
	gtab[1] = 255;
	btab[1] = 255;
     }
   else if (xatt.depth <= 8)
     {
	if ((!is_pixmap) && (!cm))
	  {
	     cm = xatt.colormap;
	     if (cm == None)
		cm = ratt.colormap;
	  }
	else
	   cm = ratt.colormap;
	
	for (i = 0; i < (1 << xatt.depth); i++)
	  {
	     cols[i].pixel = i;
	     cols[i].flags = DoRed | DoGreen | DoBlue;
	  }
	XQueryColors(d, cm, cols, 1 << xatt.depth);
	for (i = 0; i < (1 << xatt.depth); i++)
	  {
	     rtab[i] = cols[i].red >> 8;
	     gtab[i] = cols[i].green >> 8;
	     btab[i] = cols[i].blue >> 8;
	  }
     }
   if (data)
     {
	DATA32 *src;

	if (origx < 0)
	   inx = -origx;
	else
	   inx = ox;
	if (origy < 0)
	   iny = -origy;
	else
	   iny = oy;
	/* go thru the XImage and convert */
	if (xim->bits_per_pixel == 32)
	   depth = 32;
	switch (depth)
	  {
	  case 0:
	  case 1:
	  case 2:
	  case 3:
	  case 4:
	  case 5:
	  case 6:
	  case 7:
	  case 8:
	     if (mxim)
	       {
		  for (y = 0; y < h; y++)
		    {
		       ptr = data + ((y + iny) * ow) + inx;
		       for (x = 0; x < w; x++)
			 {
			    pixel = XGetPixel(xim, x, y);
			    mpixel = XGetPixel(mxim, x, y);
			    *ptr++ = (0xff000000 >> (mpixel << 31)) | 
			       (btab[pixel & 0xff]) |
			       (gtab[pixel & 0xff] << 8) |
			       (rtab[pixel & 0xff] << 16);
			    
			 }
		    }
	       }
	     else
	       {
		  for (y = 0; y < h; y++)
		    {
		       ptr = data + ((y + iny) * ow) + inx;
		       for (x = 0; x < w; x++)
			 {
			    pixel = XGetPixel(xim, x, y);
			    *ptr++ = 0xff000000 | 
			       (btab[pixel & 0xff]) |
			       (gtab[pixel & 0xff] << 8) |
			       (rtab[pixel & 0xff] << 16);
			 }
		    }
	       }
	     break;
	  case 16:
#undef MP 
#undef RMSK 
#undef GMSK
#undef BMSK
#undef R1SH
#undef G1SH
#undef B1SH
#undef R2SH
#undef G2SH
#undef B2SH
#undef P1
#undef P2
#define MP(x, y) (0xff000000 >> (XGetPixel(mxim, (x), (y)) << 31))
#define RMSK  0xf80000
#define GMSK  0x00fc00
#define BMSK  0x0000f8
#define R1SH(p)  ((p) << 8)
#define G1SH(p)  ((p) << 5)
#define B1SH(p)  ((p) << 3)
#define R2SH(p)  ((p) >> 8)
#define G2SH(p)  ((p) >> 11)
#define B2SH(p)  ((p) >> 13)
#define P1(p) (R1SH(p) & RMSK) | (G1SH(p) & GMSK) | (B1SH(p) & BMSK)
#define P2(p) (R2SH(p) & RMSK) | (G2SH(p) & GMSK) | (B2SH(p) & BMSK)
	     if (mxim)
	       {
		  for (y = 0; y < h; y++)
		    {
		       src = (DATA32 *)(xim->data + (xim->bytes_per_line * y));
		       ptr = data + ((y + iny) * ow) + inx;
		       for (x = 0; x < (w - 1); x += 2)
			 {
			    *ptr++ = MP(x, y) | P1(*src);
			    *ptr++ = MP(x + 1, y) | P2(*src);
			    src++;
			 }
		       if (x == (w - 1))
			 {
			    pixel = XGetPixel(xim, x, y);
			    *ptr++ = MP(x, y) | P1(pixel);
			 }
		    }
	       }
#undef MP		       
#define MP(x, y) (0xff000000)
	     else
	       {
		  for (y = 0; y < h; y++)
		    {
		       src = (DATA32 *)(xim->data + (xim->bytes_per_line * y));
		       ptr = data + ((y + iny) * ow) + inx;
		       for (x = 0; x < (w - 1); x += 2)
			 {
			    *ptr++ = MP(x, y) | P1(*src);
			    *ptr++ = MP(x + 1, y) | P2(*src);
			    src++;
			 }
		       if (x == (w - 1))
			 {
			    pixel = XGetPixel(xim, x, y);
			    *ptr++ = MP(x, y) | P1(pixel);
			 }
		    }
	       }
	     break;
	  case 15:
#undef MP 
#undef RMSK 
#undef GMSK
#undef BMSK
#undef R1SH
#undef G1SH
#undef B1SH
#undef R2SH
#undef G2SH
#undef B2SH
#undef P1
#undef P2
#define MP(x, y) (0xff000000 >> (XGetPixel(mxim, (x), (y)) << 31))
#define RMSK  0xf80000
#define GMSK  0x00f800
#define BMSK  0x0000f8
#define R1SH(p)  ((p) << 9)
#define G1SH(p)  ((p) << 6)
#define B1SH(p)  ((p) << 3)
#define R2SH(p)  ((p) >> 7)
#define G2SH(p)  ((p) >> 10)
#define B2SH(p)  ((p) >> 13)
#define P1(p) (R1SH(p) & RMSK) | (G1SH(p) & GMSK) | (B1SH(p) & BMSK)
#define P2(p) (R2SH(p) & RMSK) | (G2SH(p) & GMSK) | (B2SH(p) & BMSK)
	     if (mxim)
	       {
		  for (y = 0; y < h; y++)
		    {
		       src = (DATA32 *)(xim->data + (xim->bytes_per_line * y));
		       ptr = data + ((y + iny) * ow) + inx;
		       for (x = 0; x < (w - 1); x += 2)
			 {
			    *ptr++ = MP(x, y) | P1(*src);
			    *ptr++ = MP(x + 1, y) | P2(*src);
			    src++;
			 }
		       if (x == (w - 1))
			 {
			    pixel = XGetPixel(xim, x, y);
			    *ptr++ = MP(x, y) | P1(pixel);
			 }
		    }
	       }
#undef MP		       
#define MP(x, y) (0xff000000)
	     else
	       {
		  for (y = 0; y < h; y++)
		    {
		       src = (DATA32 *)(xim->data + (xim->bytes_per_line * y));
		       ptr = data + ((y + iny) * ow) + inx;
		       for (x = 0; x < (w - 1); x += 2)
			 {
			    *ptr++ = MP(x, y) | P1(*src);
			    *ptr++ = MP(x + 1, y) | P2(*src);
			    src++;
			 }
		       if (x == (w - 1))
			 {
			    pixel = XGetPixel(xim, x, y);
			    *ptr++ = MP(x, y) | P1(pixel);
			 }
		    }
	       }
	     break;
	  case 24:
	     if (mxim)
	       {
		  for (y = 0; y < h; y++)
		    {
		       ptr = data + ((y + iny) * ow) + inx;
		       for (x = 0; x < w; x++)
			 {
			    pixel = XGetPixel(xim, x, y);
			    mpixel = XGetPixel(mxim, x, y);
			    *ptr++ = (0xff000000 >> (mpixel << 31)) | 
			       (pixel & 0x00ffffff);
			 }
		    }
	       }
	     else
	       {
		  for (y = 0; y < h; y++)
		    {
		       ptr = data + ((y + iny) * ow) + inx;
		       for (x = 0; x < w; x++)
			 {
			    pixel = XGetPixel(xim, x, y);
			    *ptr++ = 0xff000000 | 
			       (pixel & 0x00ffffff);
			 }
		    }
	       }
	     break;
	  case 32:
	     if (mxim)
	       {
		  for (y = 0; y < h; y++)
		    {
		       src = (DATA32 *)(xim->data + (xim->bytes_per_line * y));
		       ptr = data + ((y + iny) * ow) + inx;
		       for (x = 0; x < w; x++)
			 {
			    mpixel = XGetPixel(mxim, x, y);
			    *ptr++ = (0xff000000 >> (mpixel << 31)) | 
			       ((*src) & 0x00ffffff);
			    src++;
			 }
		    }
	       }
	     else
	       {
		  for (y = 0; y < h; y++)
		    {
		       src = (DATA32 *)(xim->data + (xim->bytes_per_line * y));
		       ptr = data + ((y + iny) * ow) + inx;
		       for (x = 0; x < w; x++)
			 {
			    *ptr++ = 0xff000000 | 
			       ((*src) & 0x00ffffff);
			    src++;
			 }
		    }
	       }
	     break;
	  default:
	     break;
	  }
     }
   /* destroy the Ximage */
   if (is_shm)
     {
	XSync(d, False);
	XShmDetach(d, &shminfo);
	shmdt(shminfo.shmaddr);
	shmctl(shminfo.shmid, IPC_RMID, 0);
     }
   if ((is_mshm) && (mxim))
     {
	XShmDetach(d, &mshminfo);
	shmdt(mshminfo.shmaddr);
	shmctl(mshminfo.shmid, IPC_RMID, 0);
     }
   XDestroyImage(xim);
   if (created_mask)
      XFreePixmap(d, m);
   if (mxim)
      XDestroyImage(mxim);
   return 1;
}
