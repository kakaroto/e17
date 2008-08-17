#include <stdlib.h>

#include "config.h"

#ifdef HAVE_DVILIB_2_9
#  include <libdvi29.h>
#else
#  include <dvi-2_6.h>
#endif /* HAVE_DVILIB_2_9 */

#include "edvi_enum.h"
#include "edvi_forward.h"
#include "edvi_private.h"


Edvi_Framebuffer *
edvi_framebuffer_new (int width,
                      int height,
                      int aa)
{
  Edvi_Framebuffer *framebuffer;

  framebuffer = (Edvi_Framebuffer *)malloc (sizeof (Edvi_Framebuffer));
  if (!framebuffer)
    return NULL;

  framebuffer->dvi_framebuffer = DVI_fb_create ((long)width * aa, 
                                                (long)height * aa);
  if (!framebuffer->dvi_framebuffer) {
    free (framebuffer);

    return NULL;
  }
  DVI_fb_clear (framebuffer->dvi_framebuffer);

  return framebuffer;
}

void
edvi_framebuffer_delete (Edvi_Framebuffer *framebuffer)
{
  if (!framebuffer)
    return;

  if (framebuffer->dvi_framebuffer)
    DVI_fb_dispose (framebuffer->dvi_framebuffer);
  free (framebuffer);
}

void
edvi_framebuffer_clear (Edvi_Framebuffer *framebuffer)
{
  if (!framebuffer || !framebuffer->dvi_framebuffer)
    return;

  DVI_fb_clear (framebuffer->dvi_framebuffer);
}
