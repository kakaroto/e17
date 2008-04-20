#ifndef __EPDF_MAIN_H__
#define __EPDF_MAIN_H__


/**
 * @file epdf.h
 * @defgroup Epdf_Main Epdf main functions
 * @brief Functions that manage initialization, shutdown, and other miscellaneous functions
 * @ingroup Epdf
 *
 * Miscellaenous functions
 */

/**
 * @brief Return the version of Poppler.
 *
 * @return The version of Poppler.
 *
 * Return the version of Poppler as a string.
 *
 * @ingroup Epdf_Main
 */
const char *epdf_poppler_version_get (void);


#endif /* __EPDF_MAIN_H__ */
