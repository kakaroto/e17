#ifndef __EDVI_PRIVATE_H__
#define __EDVI_PRIVATE_H__


#include "config.h"

#ifdef HAVE_DVILIB_2_9
#  include <libdvi29.h>
#else
#  include <dvi-2_6.h>
#endif /* HAVE_DVILIB_2_9 */

#include <Evas.h>


struct _Edvi_Device
{
  DVI_DEVICE   dvi_device;
};

struct _Edvi_Property
{
  DVI_PROPERTY dvi_property;
};

struct _Edvi_Framebuffer
{
  DVI_FRAME_BUFFER dvi_framebuffer;
};

struct _Edvi_Page
{
  Edvi_Document        *doc;
  int                   page;
  double                mag;
  int                   shrink;
  double                offset_x;
  double                offset_y;
  Edvi_Page_Format      format;
  Edvi_Page_Orientation orientation;
};

struct _Edvi_Document
{
  DVI          dvi_document;
  Edvi_Device *device;
};


void edvi_device_evas_object_set (Edvi_Device *device, Evas_Object *o);

void edvi_device_framebuffer_set (Edvi_Device *device, Edvi_Framebuffer *fb);

void edvi_device_offset_set (Edvi_Device *device, double offset_x, double offset_y);

void edvi_device_draw_page_finished (int is_finished);


#endif /* __EDVI_PRIVATE_H__ */
