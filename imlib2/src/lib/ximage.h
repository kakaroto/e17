#ifndef __XIMAGE
#define __XIMAGE 1

#ifdef BUILD_X11

void           __imlib_SetMaxXImageCount(Display *d, int num);
__hidden int     __imlib_GetMaxXImageCount(Display *d);
__hidden void    __imlib_SetMaxXImageTotalSize(Display *d, int num);
__hidden int     __imlib_GetMaxXImageTotalSize(Display *d);
__hidden void    __imlib_FlushXImage(Display *d);
__hidden void    __imlib_ConsumeXImage(Display *d, XImage *xim);
__hidden XImage *__imlib_ProduceXImage(Display *d, Visual *v, int depth, int w, int h, char *shared);

#endif

#endif
