#ifndef __XIMAGE
#define __XIMAGE 1
void    SetMaxXImageCount(Display *d, int num);
int     GetMaxXImageCount(Display *d);
void    SetMaxXImageTotalSize(Display *d, int num);
int     GetMaxXImageTotalSize(Display *d);
void    FlushXImage(Display *d);
void    ConsumeXImage(Display *d, XImage *xim);
XImage *ProduceXImage(Display *d, Visual *v, int depth, int w, int h, char *shared);
#endif
