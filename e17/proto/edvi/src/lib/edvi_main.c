#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "config.h"

#ifdef HAVE_DVILIB_2_9
#  include <libdvi29.h>
#else
#  include <dvi-2_6.h>
#endif /* HAVE_DVILIB_2_9 */

#include "edvi_enum.h"
#include "edvi_forward.h"
#include "edvi_private.h"


#define VFLIBCAP "vflibcap-tex"


static int _edvi_dpi = EDVI_DEFAULT_DPI;
static int _edvi_aa = EDVI_DEFAULT_AA;
static double _edvi_offset_x = EDVI_DEFAULT_OFFSET_X;
static double _edvi_offset_y = EDVI_DEFAULT_OFFSET_Y;
static unsigned int *_edvi_color_map = NULL;

int
edvi_init (int dpi, char *kpathsea_mode, int aa,
           double offset_x, double offset_y,
           int bg_a, int bg_r, int bg_g, int bg_b,
           int fg_r, int fg_g, int fg_b)
{
  char params[3*256];
  int  aa_nlevels;
  int  i;

  DVI_setup ();

  sprintf(params, "TeX_DPI=%d, TeX_KPATHSEA_MODE=%s, TeX_KPATHSEA_PROGRAM=%s",
	  dpi, kpathsea_mode, "edvi");
  if (DVI_INIT (VFLIBCAP, params) < 0) {
    return 0;
  }

  if (dpi > 0)
    _edvi_dpi = dpi;
  if ((aa >=1) && (aa <= 8))
    _edvi_aa = aa;

  _edvi_offset_x = offset_x;
  _edvi_offset_y = offset_y;

  aa_nlevels = aa * aa + 1;
  _edvi_color_map = (unsigned int *)malloc ( sizeof (unsigned int) * aa_nlevels);
  if (!_edvi_color_map)
    return 0;

  for (i = 0; i < aa_nlevels; i++) {
    int r, g, b;

    r = bg_r + ceil(((double)(fg_r - bg_r) * i) / (double)(aa_nlevels - 1));
    g = bg_g + ceil(((double)(fg_g - bg_g) * i) / (double)(aa_nlevels - 1));
    b = bg_b + ceil(((double)(fg_b - bg_b) * i) / (double)(aa_nlevels - 1));
    _edvi_color_map[i] = ((bg_a << 24) |
                          (r << 16)    |
                          (g << 8)     |
                          (b));
  }

  return 1;
}

void
edvi_shutdown ()
{
  free (_edvi_color_map);
}

int
edvi_dpi_get ()
{
  return _edvi_dpi;
}

int
edvi_aa_get ()
{
  return _edvi_aa;
}

double
edvi_offset_x_get ()
{
  return _edvi_offset_x;
}

double
edvi_offset_y_get ()
{
  return _edvi_offset_y;
}

const unsigned int *
edvi_color_map_get ()
{
  return _edvi_color_map;
}

const char *
edvi_version_get ()
{
  return DVI_version ();
}
