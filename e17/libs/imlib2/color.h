#ifndef __COLOR
#define __COLOR 1
/* FIXME: DATA32 must become DATA8 */
extern DATA8 _dither_color_lut[256];
extern DATA8  _pal_type;

void
AllocColorTable(Display *d, Colormap cmap);
char
AllocColors332(Display *d, Colormap cmap);
char
AllocColors232(Display *d, Colormap cmap);
char
AllocColors222(Display *d, Colormap cmap);
char
AllocColors221(Display *d, Colormap cmap);
char
AllocColors121(Display *d, Colormap cmap);
char
AllocColors111(Display *d, Colormap cmap);
char
AllocColors1(Display *d, Colormap cmap);
#endif
