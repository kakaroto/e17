#ifndef __EPDF_MAIN_H__
#define __EPDF_MAIN_H__


/**
 * @file epdf_main.h
 *
 * @defgroup Epdf_Main Epdf main functions
 *
 * @brief Functions that manage miscellaneous functions
 *
 * Miscellaenous functions
 *
 * @ingroup Epdf
 */

EAPI int epdf_init (void);

EAPI int epdf_shutdown();

/**
 * @brief Return the version of Poppler.
 *
 * @return The version of Poppler.
 *
 * Return the version of Poppler as a string.
 *
 * @ingroup Epdf_Main
 */
EAPI const char *epdf_backend_version_get (void);

/**
 * @brief Return the state of fonts antialiasing in Poppler
 *
 * @return EINA_TRUE if fonts antialiasing enabled, EINA_FALSE otherwise.
 *
 * Return EINA_TRUE if the state of fonts antialiasing is enabled in
 * Poppler, EINA_FALSE otherwise.
 *
 * @ingroup Epdf_Main
 */
EAPI Eina_Bool epdf_fonts_antialias_get (void);

/**
 * @brief Sets fonts antialiasing mode in Poppler
 *
 * @param on EINA_TRUE to enable fonts antialiasing, EINA_FALSE to disable
 *
 * Set @p on to EINA_TRUE to enable fonts antialiasing in Poppler,
 * EINA_FALSE to disable it.
 */
EAPI void epdf_fonts_antialias_set (Eina_Bool on);

/**
 * @brief Return the state of lines antialiasing in Poppler
 *
 * @return EINA_TRUE if lines antialiasing enabled, EINA_FALSE otherwise.
 *
 * Return EINA_TRUE if the state of lines antialiasing is enabled in
 * Poppler, EINA_FALSE otherwise.
 *
 * @ingroup Epdf_Main
 */
EAPI Eina_Bool epdf_lines_antialias_get (void);

/**
 * @brief Sets lines antialiasing mode in Poppler
 *
 * @param on EINA_TRUE to enable lines antialiasing, EINA_FALSE to disable
 *
 * Set @p on to EINA_TRUE to enable lines antialiasing in Poppler,
 * EINA_FALSE to disable it.
 */
EAPI void epdf_lines_antialias_set (Eina_Bool on);


#endif /* __EPDF_MAIN_H__ */
