#ifndef __COLOR
#define __COLOR 1
/* FIXME: DATA32 must become DATA8 */
extern DATA16 _max_colors;

int __imlib_XActualDepth(Display *d, Visual *v);
Visual *__imlib_BestVisual(Display *d, int screen, int *depth_return);
DATA8 * __imlib_AllocColorTable(Display *d, Colormap cmap, DATA8 *type_return);
DATA8 * __imlib_AllocColors332(Display *d, Colormap cmap);
DATA8 * __imlib_AllocColors232(Display *d, Colormap cmap);
DATA8 * __imlib_AllocColors222(Display *d, Colormap cmap);
DATA8 * __imlib_AllocColors221(Display *d, Colormap cmap);
DATA8 * __imlib_AllocColors121(Display *d, Colormap cmap);
DATA8 * __imlib_AllocColors111(Display *d, Colormap cmap);
DATA8 * __imlib_AllocColors1(Display *d, Colormap cmap);
#endif
