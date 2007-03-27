#ifndef ENGRAVE_SPECTRUM_H
#define ENGRAVE_SPECTRUM_H

/**
 * @file engrave_spectrum.h Engrave_Spectrum block 
 * @brief Contains all of the functions to maniuplate Engrave_Spectrum blocks
 */

/**
 * @defgroup Engrave_Spectrum Engrave_Spectrum: Functions to work with engrave spectrum objects
 *
 * @{
 */

/**
 * The Engrave_Spectrum typedef
 */
typedef struct _Engrave_Spectrum Engrave_Spectrum;
typedef struct _Engrave_Spectrum_Color Engrave_Spectrum_Color;

/**
 * @brief Stores the needed spectrum information.
 */
struct _Engrave_Spectrum
{
  char *name; /**< The spectrum alias */
  void * parent; /**< The spectrum parent */
  Evas_List * colors; /**< The spectrum colors */
};

/**
 * @brief Stores the spectrum color information.
 */
struct _Engrave_Spectrum_Color
{
  int r;
  int g;
  int b;
  int a;
  int d;
};

EAPI Engrave_Spectrum *engrave_spectrum_new();
EAPI void engrave_spectrum_free(Engrave_Spectrum *es);

EAPI Engrave_Spectrum_Color *engrave_spectrum_color_new(int r, int g, int b, int a, int d);
EAPI void engrave_spectrum_color_free(Engrave_Spectrum_Color *esc);

EAPI void engrave_spectrum_parent_set(Engrave_Spectrum *es, void *parent);

EAPI void engrave_spectrum_color_add(Engrave_Spectrum *spec, Engrave_Spectrum_Color * col);
EAPI void engrave_spectrum_color_del(Engrave_Spectrum *spec, Engrave_Spectrum_Color * col);

EAPI const char * engrave_spectrum_name_get(Engrave_Spectrum *es);
EAPI void engrave_spectrum_name_set(Engrave_Spectrum *es, const char *name);
EAPI int engrave_spectrum_color_count(Engrave_Spectrum *es);
EAPI Engrave_Spectrum_Color * engrave_spectrum_color_nth(Engrave_Spectrum *es, int n);


/**
 * @}
 */

#endif

