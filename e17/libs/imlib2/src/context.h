#ifndef __CONTEXT
#define __CONTEXT 1

typedef struct _context Context;

struct _context
{
   int      last_use;
   Display *display;
   Visual  *visual;
   Colormap colormap;
   int      depth;
   Context *next;
   
   DATA8 *palette;
   DATA8  palette_type;
   void  *r_dither;
   void  *g_dither;
   void  *b_dither;
};

void     __imlib_SetMaxContexts(int num);
int      __imlib_GetMaxContexts(void);
void     __imlib_FlushContexts(void);
void     __imlib_FreeContextForDisplay(Display *d);
void     __imlib_FreeContextForColormap(Display *d, Colormap cm);
void     __imlib_FreeContextForVisual(Display *d, Visual *v);
Context *__imlib_FindContext(Display *d, Visual *v, Colormap c, int depth);
Context *__imlib_NewContext(Display *d, Visual *v, Colormap c, int depth);
Context *__imlib_GetContext(Display *d, Visual *v, Colormap c, int depth);

#endif
