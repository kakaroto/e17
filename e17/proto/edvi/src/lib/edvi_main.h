#ifndef __EDVI_MAIN_H__
#define __EDVI_MAIN_H__


/**
 * @file edvi_main.h
 * @defgroup Edvi_Main Edvi main functions
 * @brief Functions that manage initialization, shutdown, and other miscellaneous functions
 * @ingroup Edvi
 *
 * Functions that manage initialisations of Edvi
 */


/**
 * @brief Initialise Edvi
 *
 * @param dpi Dot per inch
 * @param kpathsea_mode Kpathsea mode
 * @param aa Anti-aliasing
 * @param offset_x Horizontal offset
 * @param offset_y Vertical offset
 * @param bg_a Alpha component of the background color
 * @param bg_r Red component of the background color
 * @param bg_g Green component of the background color
 * @param bg_b Blue component of the background color
 * @param fg_r Red component of the foreground color
 * @param fg_g Green component of the foreground color
 * @param fg_b Blue component of the foreground color
 *
 * Initialise Edvi. It set the dot per inch used by the GhostScript
 * interpreter, the Kpathsea mode and program. A colormap is also
 * initialized. The colors count is given by @p aa * @p aa + 1. The
 * offset if used to shift the character in the document.
 *
 * @ingroup Edvi_Main
 */
int edvi_init (int dpi,
               char *kpathsea_mode,
               int aa,
               double offset_x,
               double offset_y,
               int bg_a,
               int bg_r,
               int bg_g,
               int bg_b,
               int fg_r,
               int fg_g,
               int fg_b);

/**
 * @brief Shutdown Edvi
 *
 * Shutdown the Edvi library. This function must be called
 * if edvi_init() has been called.
 *
 * @ingroup Edvi_Main
 */
void edvi_shutdown ();

/**
 * @brief Return the dpi.
 *
 * @return The dpi.
 *
 * Return the dot per inch that was passed to edvi_init().
 *
 * @ingroup Edvi_Main
 */
int edvi_dpi_get ();

/**
 * @brief Return the anti-aliasing.
 *
 * @return The anti-aliasing.
 *
 * Return the anti-aliasing that was passed to edvi_init().
 *
 * @ingroup Edvi_Main
 */
int edvi_aa_get ();

/**
 * @brief Return the horizontal offset.
 *
 * @return The horizontal offset.
 *
 * Return the horizontal offset that was passed to edvi_init().
 *
 * @ingroup Edvi_Main
 */
double edvi_offset_x_get ();

/**
 * @brief Return the vertical offset.
 *
 * @return The vertical offset.
 *
 * Return the vertical offset that was passed to edvi_init().
 *
 * @ingroup Edvi_Main
 */
double edvi_offset_y_get ();

/**
 * @brief Return the colormap.
 *
 * @return The colormap.
 *
 * Return the colormap that was initialized by edvi_init(). The
 * number of colors is aa * aa + 1, where aa is the value returned
 * by edvi_aa_get(). A color is a 4 bytes long data with the alpha
 * component as higher byte and with the blue component as the lower
 * byte (ARGB data). The return array must not be freed.
 *
 * @ingroup Edvi_Main
 */
const unsigned int *edvi_color_map_get ();

/**
 * @brief Return the version of dvilib.
 *
 * @return The version of dvilib.
 *
 * Return the version of dvilib as a string.
 *
 * @ingroup Edvi_Main
 */
const char *edvi_version_get ();


#endif /* __EDVI_MAIN_H__ */
