#ifndef __XIMAGE
#define __XIMAGE 1
void    __imlib_SetMaxXImageCount(Display *d, int num);
int     __imlib_GetMaxXImageCount(Display *d);
void    __imlib_SetMaxXImageTotalSize(Display *d, int num);
int     __imlib_GetMaxXImageTotalSize(Display *d);
void    __imlib_FlushXImage(Display *d);
void    __imlib_ConsumeXImage(Display *d, XImage *xim);
XImage *__imlib_ProduceXImage(Display *d, Visual *v, int depth, int w, int h, char *shared);
#endif
