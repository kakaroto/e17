#ifndef __UPDATES
#define __UPDATES 1
typedef struct _imlibupdate ImlibUpdate;

struct _imlibupdate
{
   int x, y, w, h;
   ImlibUpdate *next;
};

ImlibUpdate *__imlib_AddUpdate(ImlibUpdate *u, int x, int y, int w, int h);
ImlibUpdate *__imlib_MergeUpdate(ImlibUpdate *u, int w, int h);
void __imlib_FreeUpdates(ImlibUpdate *u);

#endif
