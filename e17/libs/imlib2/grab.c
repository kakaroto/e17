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

DATA32 *
GrabDrawableToRGBA(Display *d, Drawable p, Pixmap m, Visual *v, Colormap cm, 
		   int depth, int x, int y, int w, int h, char domask)
{
   XErrorHandler       prev_erh = NULL;
   XWindowAttributes   xatt, ratt;
   char                is_pixmap = 0, created_mask = 0, is_shm = 0, is_mshm = 0;
   int                 i, pixel, mpixel;
   int                 src_x, src_y, src_w, src_h, ow, oh, ox, oy;
   int                 width, height, clipx, clipy, inx, iny;
   XShmSegmentInfo     shminfo, mshminfo;
   XImage             *xim = NULL, *mxim = NULL;
   static char         x_does_shm = -1;
   DATA32             *data, *ptr;
   DATA8               rtab[256], gtab[256], btab[256];
   XColor              cols[256];
   
   ox = x;
   oy = y;
   ow = w;
   oh = h;
   /* FIXME:  hmm - need to co-ordinate this with the app */
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
	if ((xatt.map_state != IsViewable) &&
	    (xatt.backing_store == NotUseful))
	  {
	     XUngrabServer(d);
	     return NULL;
	  }
     }
   
   /* clip to the drawable tree and screen */
   clipx = 0;
   clipy = 0;  
   width = xatt.width - x;
   height = xatt.height - y;
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
	XUngrabServer(d);
	return NULL;
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
	XUngrabServer(d);
	XSync(d, False);
     }
   else
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
   data = malloc(ow * oh * sizeof(DATA32));
   if (data)
     {
	inx = x - ox;
	iny = y - oy;
	/* go thru the XImage and convert */
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
			       (btab[pixel & 0xff] << 16) |
			       (gtab[pixel & 0xff] << 8) |
			       (rtab[pixel & 0xff]);
			    
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
			       (btab[pixel & 0xff] << 16) |
			       (gtab[pixel & 0xff] << 8) |
			       (rtab[pixel & 0xff]);
			 }
		    }
	       }
	     break;
	  case 16:
	     if (mxim)
	       {
		  for (y = 0; y < h; y++)
		    {
		       DATA8 r, g, b;
		       
		       ptr = data + ((y + iny) * ow) + inx;
		       for (x = 0; x < w; x++)
			 {
			    pixel = XGetPixel(xim, x, y);
			    r = (pixel >> 8) & 0xf8;
			    g = (pixel >> 3) & 0xfc;
			    b = (pixel << 3) & 0xf8;
			    mpixel = XGetPixel(mxim, x, y);
			    *ptr++ = (0xff000000 >> (mpixel << 31)) | 
			       (b << 16) |
			       (g << 8) |
			       (r);
			 }
		    }
	       }
	     else
	       {
		  for (y = 0; y < h; y++)
		    {
		       DATA8 r, g, b;
		       
		       ptr = data + ((y + iny) * ow) + inx;
		       for (x = 0; x < w; x++)
			 {
			    pixel = XGetPixel(xim, x, y);
			    r = (pixel >> 8) & 0xf8;
			    g = (pixel >> 3) & 0xfc;
			    b = (pixel << 3) & 0xf8;
			    *ptr++ = 0xff000000 | 
			       (b << 16) |
			       (g << 8) |
			       (r);
			 }
		    }
	       }
	     break;
	  case 15:
	     if (mxim)
	       {
		  for (y = 0; y < h; y++)
		    {
		       DATA8 r, g, b;
		       
		       ptr = data + ((y + iny) * ow) + inx;
		       for (x = 0; x < w; x++)
			 {
			    pixel = XGetPixel(xim, x, y);
			    r = (pixel >> 7) & 0xf8;
			    g = (pixel >> 2) & 0xf8;
			    b = (pixel << 3) & 0xf8;
			    mpixel = XGetPixel(mxim, x, y);
			    *ptr++ = (0xff000000 >> (mpixel << 31)) | 
			       (b << 16) |
			       (g << 8) |
			       (r);
			 }
		    }
	       }
	     else
	       {
		  for (y = 0; y < h; y++)
		    {
		       DATA8 r, g, b;
		       
		       ptr = data + ((y + iny) * ow) + inx;
		       for (x = 0; x < w; x++)
			 {
			    pixel = XGetPixel(xim, x, y);
			    r = (pixel >> 7) & 0xf8;
			    g = (pixel >> 2) & 0xf8;
			    b = (pixel << 3) & 0xf8;
			    *ptr++ = 0xff000000 | 
			       (b << 16) |
			       (g << 8) |
			       (r);
			 }
		    }
	       }
	     break;
	  case 24:
	  case 32:
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
			       ((pixel & 0x000000ff) << 16) |
			       ((pixel & 0x0000ff00)) |
			       ((pixel & 0x00ff0000) >> 16);
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
			       ((pixel & 0x000000ff) << 16) |
			       ((pixel & 0x0000ff00)) |
			       ((pixel & 0x00ff0000) >> 16);
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
   return data;
}
