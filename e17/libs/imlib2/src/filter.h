#ifndef __FILTER
#define __FILTER 1

typedef struct _imlib_filter ImlibFilter;
typedef struct _imlib_filter_color ImlibFilterColor;
typedef struct _imlib_filter_pixel ImlibFilterPixel;

struct _imlib_filter_pixel
{
   int xoff, yoff;
   int a, r, g, b;
};

struct _imlib_filter_color
{
   int size, entries;
   int div, cons;
   ImlibFilterPixel *pixels;
};

struct _imlib_filter
{
   ImlibFilterColor alpha, red, green, blue;
};

ImlibFilter *
__imlib_CreateFilter(int size);
void
__imlib_FreeFilter(ImlibFilter *fil);
void
__imlib_FilterSet(ImlibFilterColor *fil, int x, int y,
		  int a, int r, int g, int b);
void
__imlib_FilterSetColor(ImlibFilterColor * fil, int x, int y,
                       int a, int r, int g, int b);
void
__imlib_FilterDivisors(ImlibFilter *fil, int a, int r, int g, int b);
void
__imlib_FilterConstants(ImlibFilter *fil, int a, int r, int g, int b);
void
__imlib_FilterImage(ImlibImage *im, ImlibFilter *fil);
#endif
