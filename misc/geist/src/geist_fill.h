
#ifndef GEIST_FILL_H
#define GEIST_FILL_H

#include "geist.h"
#include "geist_object.h"
#include "geist_document.h"
#include "geist_list.h"

#define GEIST_FILL(O) ((geist_fill *)O)

struct __geist_fill
{
   int r,g,b,a;
};

geist_fill *geist_fill_new(void);
void geist_fill_free(geist_fill *f);
void geist_fill_render(geist_fill * fill, Imlib_Image dest, int x, int y, int w, int h);
geist_fill *geist_fill_new_coloured(int r, int g, int b, int a);
#endif
