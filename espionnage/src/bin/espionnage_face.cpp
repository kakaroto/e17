#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#ifdef HAVE_FACE
# include <LibFace.h>
# include <Face.h>

# include <vector>
#endif

#include "espionnage_private.h"

#ifdef HAVE_FACE
static void
_face_free_cb(void *data)
{
   libface::LibFace* libFace = static_cast<libface::LibFace *> (data);

   delete libFace;
}
#endif

Eina_List *
face_search(Ecore_Thread *thread, char *data, int width, int height, int stride)
{
#ifdef HAVE_FACE
   libface::LibFace* libFace = NULL;
#ifdef HAVE_FACE2
   std::vector<libface::Face*> *result;
#else
   std::vector<libface::Face> result;
#endif
   unsigned int i;
   Eina_List *back = NULL;

   libFace = static_cast<libface::LibFace*> (ecore_thread_local_data_find(thread, "libface"));
   if (!libFace)
     {
        libFace = new libface::LibFace(libface::DETECT, ".");
        ecore_thread_local_data_add(thread, "libface", libFace, _face_free_cb, 1);
     }

   result = libFace->detectFaces(data, width, height, stride, IPL_DEPTH_8U, 1, 0);
#ifdef HAVE_FACE2
   for (i = 0; i < result->size(); i++)
#else
   for (i = 0; i < result.size(); i++)
#endif
     {
#ifdef HAVE_FACE2
        libface::Face *face = result->at(i);
#else
        libface::Face *face = &result.at(i);
#endif
        Eina_Rectangle *r;

        r = eina_rectangle_new(face->getX1(), face->getY1(), face->getX2() - face->getX1(), face->getY2() - face->getY1());
        back = eina_list_append(back, r);
     }

#ifdef HAVE_FACE2
   result->clear();
#else
   result.clear();
#endif

   return back;
#else
   return NULL;
#endif
}

