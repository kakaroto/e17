#ifndef __EDVI_CORE_H__
#define __EDVI_CORE_H__


int edvi_init (int dpi, char *kpathsea_mode, int aa,
               double offset_x, double offset_y,
               int bg_a, int bg_r, int bg_g, int bg_b,
               int fg_r, int fg_g, int fg_b);

void edvi_shutdown ();

int edvi_dpi_get ();

int edvi_aa_get ();

double edvi_offset_x_get ();

double edvi_offset_y_get ();

unsigned int *edvi_color_map_get ();

const char *edvi_version ();


#endif /* __EDVI_CORE_H__ */
