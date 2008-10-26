#include "common.h"
#ifdef BUILD_X11
#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>
#include <X11/Xutil.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "ximage.h"

/* static private variables */
static signed char  x_does_shm = -1;
static int          list_num = 0;
static XImage     **list_xim = NULL;
static XShmSegmentInfo **list_si = NULL;
static Display    **list_d = NULL;
static char        *list_used = NULL;
static int          list_mem_use = 0;
static int          list_max_mem = 1024 * 1024 * 1024;
static int          list_max_count = 0;

/* temporary X error catcher we use later */
static char         _x_err = 0;

/* the fucntion we use for catching the error */
static void
TmpXError(Display * d, XErrorEvent * ev)
{
   _x_err = 1;
   return;
   d = NULL;
   ev = NULL;
}

/* "safe" realloc allowing handling of out-of-memory situations */
static void        *
_safe_realloc(void *ptr, size_t size, int *err)
{
   void               *ptr_new;

   ptr_new = realloc(ptr, size);
   if (!ptr_new)
     {
        *err = 1;
        return ptr;
     }

   return ptr_new;
}

void
__imlib_SetMaxXImageCount(Display * d, int num)
{
   list_max_count = num;
   __imlib_FlushXImage(d);
}

int
__imlib_GetMaxXImageCount(Display * d)
{
   return list_max_count;
   d = NULL;
}

void
__imlib_SetMaxXImageTotalSize(Display * d, int num)
{
   list_max_mem = num;
   __imlib_FlushXImage(d);
}

int
__imlib_GetMaxXImageTotalSize(Display * d)
{
   return list_max_mem;
   d = NULL;
}

void
__imlib_FlushXImage(Display * d)
{
   int                 i;
   XImage             *xim;
   char                did_free = 1;

   while (((list_mem_use > list_max_mem) || (list_num > list_max_count)) &&
          (did_free))
     {
        did_free = 0;
        for (i = 0; i < list_num; i++)
          {
             if (list_used[i] == 0)
               {
                  int                 j;

                  xim = list_xim[i];
                  list_mem_use -= xim->bytes_per_line * xim->height;
                  if (list_si[i])
                     XShmDetach(d, list_si[i]);
                  XDestroyImage(xim);
                  if (list_si[i])
                    {
                       shmdt(list_si[i]->shmaddr);
                       shmctl(list_si[i]->shmid, IPC_RMID, 0);
                       free(list_si[i]);
                    }
                  list_num--;
                  for (j = i; j < list_num; j++)
                    {
                       list_xim[j] = list_xim[j + 1];
                       list_si[j] = list_si[j + 1];
                       list_used[j] = list_used[j + 1];
                       list_d[j] = list_d[j + 1];
                    }
                  if (list_num == 0)
                    {
                       if (list_xim)
                          free(list_xim);
                       if (list_si)
                          free(list_si);
                       if (list_used)
                          free(list_used);
                       if (list_d)
                          free(list_d);
                       list_xim = NULL;
                       list_si = NULL;
                       list_used = NULL;
                       list_d = NULL;
                    }
                  else
                    {
                       list_xim =
                          realloc(list_xim, sizeof(XImage *) * list_num);
                       list_si =
                          realloc(list_si,
                                  sizeof(XShmSegmentInfo *) * list_num);
                       list_used = realloc(list_used, sizeof(char) * list_num);
                       list_d = realloc(list_d, sizeof(Display *) * list_num);
                    }
                  did_free = 1;
               }
          }
     }
}

/* free (consume == opposite of produce) the XImage (mark as unused) */
void
__imlib_ConsumeXImage(Display * d, XImage * xim)
{
   int                 i;

   /* march through the XImage list */
   for (i = 0; i < list_num; i++)
     {
        /* find a match */
        if (list_xim[i] == xim)
          {
             /* we have a match = mark as unused */
             list_used[i] = 0;
             /* flush the XImage list to get rud of stuff we dont want */
             __imlib_FlushXImage(d);
             /* return */
             return;
          }
     }
}

/* create a new XImage or find it on our list of currently available ones so */
/* we dont need to create a new one */
XImage             *
__imlib_ProduceXImage(Display * d, Visual * v, int depth, int w, int h,
                      char *shared)
{
   XImage             *xim;
   int                 i, err;

   /* if we havent check the shm extension before - see if its there */
   if (x_does_shm < 0)
     {
        /* if its there set dose_xhm flag */
        if (XShmQueryExtension(d))
           x_does_shm = 1;
        /* clear the flag - no shm at all */
        else
           x_does_shm = 0;
     }
   /* find a cached XImage (to avoid server to & fro) that is big enough */
   /* for our needs and the right depth */
   *shared = 0;
   /* go thru the current image list */
   for (i = 0; i < list_num; i++)
     {
        int                 depth_ok = 0;

        /* if the image has the same depth, width and height - recycle it */
        /* as long as its not used */
        if (list_xim[i]->depth == depth)
           depth_ok = 1;
        if (depth_ok &&
            (list_xim[i]->width >= w) && (list_xim[i]->height >= h) &&
            /*   (list_d[i] == d) && */
            (!list_used[i]))
          {
             /* mark it as used */
             list_used[i] = 1;
             /* if its shared set shared flag */
             if (list_si[i])
                *shared = 1;
             /* return it */
             return list_xim[i];
          }
     }

   /* can't find a usable XImage on the cache - create one */
   /* add the new XImage to the XImage cache */
   list_num++;
   err = 0;
   list_xim = _safe_realloc(list_xim, sizeof(XImage *) * list_num, &err);
   list_si = _safe_realloc(list_si, sizeof(XShmSegmentInfo *) * list_num, &err);
   list_used = _safe_realloc(list_used, sizeof(char) * list_num, &err);
   list_d = _safe_realloc(list_d, sizeof(Display *) * list_num, &err);
   if (err)
     {
        /* failed to allocate memory */
        list_num--;
        return NULL;
     }
   list_si[list_num - 1] = malloc(sizeof(XShmSegmentInfo));
   if (!list_si[list_num - 1])
     {
        /* failed to allocate memory */
        list_num--;
        return NULL;
     }

   /* work on making a shared image */
   xim = NULL;
   /* if the server does shm */
   if (x_does_shm)
     {
        /* try create an shm image */
        xim = XShmCreateImage(d, v, depth, ZPixmap, NULL,
                              list_si[list_num - 1], w, h);
        /* if it succeeds */
        if (xim)
          {
             /* add to list */
             list_xim[list_num - 1] = xim;
             /* get an shm id of this image */
             list_si[list_num - 1]->shmid =
                shmget(IPC_PRIVATE, xim->bytes_per_line * xim->height,
                       IPC_CREAT | 0777);
             /* if the get succeeds */
             if (list_si[list_num - 1]->shmid != -1)
               {
                  /* set the params for the shm segment */
                  list_si[list_num - 1]->readOnly = False;
                  list_si[list_num - 1]->shmaddr = xim->data =
                     shmat(list_si[list_num - 1]->shmid, 0, 0);
                  /* get the shm addr for this data chunk */
                  if (xim->data != (char *)-1)
                    {
                       XErrorHandler       ph;

                       /* setup a temporary error handler */
                       _x_err = 0;
                       ph = XSetErrorHandler((XErrorHandler) TmpXError);
                       /* ask X to attach to the shared mem segment */
                       XShmAttach(d, list_si[list_num - 1]);
                       /* wait for X to reply and do this */
                       XSync(d, False);
                       /* reset the error handler */
                       XSetErrorHandler((XErrorHandler) ph);
                       /* if we attached without an error we're set */
                       if (!_x_err)
                         {
                            /* mark the image as used */
                            list_used[list_num - 1] = 1;
                            /* incrument our memory count */
                            list_mem_use += xim->bytes_per_line * xim->height;
                            /* set shared flag */
                            *shared = 1;
                         }
                       /* attach by X failed... must be remote client */
                       else
                         {
                            /* flag shm foevere to not work - remote */
                            x_does_shm = 0;
                            /* destroy our previous image */
                            XDestroyImage(xim);
                            /* detach */
                            shmdt(list_si[list_num - 1]->shmaddr);
                            /* remove the shm id */
                            shmctl(list_si[list_num - 1]->shmid, IPC_RMID, 0);
                            /* flag out xim as NULL */
                            xim = NULL;
                         }
                    }
                  /* get failed - out of shm id's or shm segment too big ? */
                  else
                    {
                       /* destroy previous image */
                       XDestroyImage(xim);
                       /* remove the shm id we created */
                       shmctl(list_si[list_num - 1]->shmid, IPC_RMID, 0);
                       /* flag xim as NULL */
                       xim = NULL;
                    }
               }
             /* couldnt create SHM image ? */
             else
               {
                  /* destroy previous image */
                  XDestroyImage(xim);
                  /* flag xim as NULL */
                  xim = NULL;
               }
          }
     }
   /* ok if xim == NULL it all failed - fall back to XImages */
   if (!xim)
     {
        /* get rid of out shm info struct */
        free(list_si[list_num - 1]);
        /* flag it as NULL ot indicate a normal XImage */
        list_si[list_num - 1] = NULL;
        /* create a normal ximage */
        xim = XCreateImage(d, v, depth, ZPixmap, 0, NULL, w, h, 32, 0);
        /* allocate data for it */
        if (xim)
           xim->data = malloc(xim->bytes_per_line * xim->height);
        if (!xim || !xim->data)
          {
             /* failed to create XImage or allocate data memory */
             if (xim)
                XDestroyImage(xim);
             list_num--;
             return NULL;
          }
        /* add xim to our list */
        list_xim[list_num - 1] = xim;
        /* incriment our memory count */
        list_mem_use += xim->bytes_per_line * xim->height;
        /* mark image as used */
        list_used[list_num - 1] = 1;
        /* remember what display that XImage was for */
        list_d[list_num - 1] = d;
     }
   /* flush unused images from the image list */
   __imlib_FlushXImage(d);

   /* set the byte order of the XImage to the byte_order of the Xclient */
   /* (rather than the Xserver) */
#ifdef WORDS_BIGENDIAN
   xim->byte_order = MSBFirst;
#else
   xim->byte_order = LSBFirst;
#endif

   /* return out image */
   return xim;
}

#endif /* BUILD_X11 */
