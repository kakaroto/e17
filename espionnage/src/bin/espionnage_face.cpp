#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#ifdef HAVE_FACE
# include <LibFace.h>
# include <Face.h>

# include <vector>
#endif

#include "espionnage_private.h"

Eina_List *
face_search(char *data, int width, int height, int stride)
{
#ifdef HAVE_FACE
   libface::LibFace* libFace = new libface::LibFace(libface::DETECT, ".");
   std::vector<libface::Face*> *result;
   unsigned int i;
   Eina_List *back = NULL;

   result = libFace->detectFaces(data, width, height, stride, IPL_DEPTH_8U, 1, 0);
   for (i = 0; i < result->size(); i++)
     {
        libface::Face *face = result->at(i);
        Eina_Rectangle *r;

        r = eina_rectangle_new(face->getX1(), face->getY1(), face->getX2() - face->getX1(), face->getY2() - face->getY1());
        back = eina_list_append(back, r);

        result->clear();
     }

   return back;
#else
   return NULL;
#endif
}

