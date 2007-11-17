#include "config.h"
#include "epplet.h"

extern Display     *disp;

GLXContext
Epplet_bind_double_GL(Epplet_gadget da, int red, int green, int blue,
		      int aux_buffers, int alpha, int depth, int stencil,
		      int accum_red, int accum_green, int accum_blue,
		      int accum_alpha)
{
   XVisualInfo        *vi;
   GLXContext          cx;
   Window              win;

   /* This sets up our MINIMUM request list for glx values. If all
    * the following minimums are not available, then a NULL is
    * returned for cx. You also might get a LARGER value then you
    * specify. */

   int                 attributeListDbl[] = { GLX_RGBA, GLX_DOUBLEBUFFER,
      GLX_RED_SIZE, red, GLX_GREEN_SIZE, green, GLX_BLUE_SIZE, blue,
      GLX_ALPHA_SIZE, alpha, GLX_AUX_BUFFERS, aux_buffers,
      GLX_DEPTH_SIZE, depth, GLX_STENCIL_SIZE, stencil,
      GLX_ACCUM_RED_SIZE, accum_red, GLX_ACCUM_GREEN_SIZE, accum_green,
      GLX_ACCUM_BLUE_SIZE, accum_blue, GLX_ACCUM_ALPHA_SIZE, accum_alpha,
      None
   };

   win = Epplet_get_drawingarea_window(da);
   vi = glXChooseVisual(disp, DefaultScreen(disp), attributeListDbl);
   cx = glXCreateContext(disp, vi, 0, GL_TRUE);
   glXMakeCurrent(disp, win, cx);

   return cx;
}

GLXContext
Epplet_bind_single_GL(Epplet_gadget da, int red, int green, int blue,
		      int aux_buffers, int alpha, int depth, int stencil,
		      int accum_red, int accum_green, int accum_blue,
		      int accum_alpha)
{
   XVisualInfo        *vi;
   GLXContext          cx;
   Window              win;
   int                 attributeListSgl[] = { GLX_RGBA,
      GLX_RED_SIZE, red, GLX_GREEN_SIZE, green, GLX_BLUE_SIZE, blue,
      GLX_ALPHA_SIZE, alpha, GLX_AUX_BUFFERS, aux_buffers,
      GLX_DEPTH_SIZE, depth, GLX_STENCIL_SIZE, stencil,
      GLX_ACCUM_RED_SIZE, accum_red, GLX_ACCUM_GREEN_SIZE, accum_green,
      GLX_ACCUM_BLUE_SIZE, accum_blue, GLX_ACCUM_ALPHA_SIZE, accum_alpha,
      None
   };

   win = Epplet_get_drawingarea_window(da);
   vi = glXChooseVisual(disp, DefaultScreen(disp), attributeListSgl);
   cx = glXCreateContext(disp, vi, 0, GL_TRUE);
   glXMakeCurrent(disp, win, cx);

   return cx;
}

GLXContext
Epplet_default_bind_GL(Epplet_gadget da)
{
   return Epplet_bind_double_GL(da, 1, 1, 1, 0, 0, 8, 0, 0, 0, 0, 0);
}

void
Epplet_unbind_GL(GLXContext cx)
{
   glXDestroyContext(disp, cx);
}
