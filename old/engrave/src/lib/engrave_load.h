#ifndef ENGRAVE_LOAD_H
#define ENGRAVE_LOAD_H

/**
 * @file engrave_load.h Engrave loading functions
 * @brief Contains the functions to load either an EDC or EDJ into Engrave.
 */

/**
 * @defgroup Engrave_Load Engrave_Load: Contains the functions to load EDC
 * and EDJ files into Engrave.
 * 
 * @{
 */

EAPI Engrave_File *engrave_load_edc(const char *file, const char *imdir, 
                                    const char *fontdir);
EAPI Engrave_File *engrave_load_edj(const char *filename);

/**
 * @}
 */

#endif

