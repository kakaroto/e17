#ifndef __COLOR
#define __COLOR 1
/* FIXME: DATA32 must become DATA8 */
extern DATA8  _dither_color_lut[256];
extern DATA8  _pal_type;
extern DATA16 _max_colors;

void
__imlib_AllocColorTable(Display *d, Colormap cmap);
char
__imlib_AllocColors332(Display *d, Colormap cmap);
char
__imlib_AllocColors232(Display *d, Colormap cmap);
char
__imlib_AllocColors222(Display *d, Colormap cmap);
char
__imlib_AllocColors221(Display *d, Colormap cmap);
char
__imlib_AllocColors121(Display *d, Colormap cmap);
char
__imlib_AllocColors111(Display *d, Colormap cmap);
char
__imlib_AllocColors1(Display *d, Colormap cmap);
#endif
