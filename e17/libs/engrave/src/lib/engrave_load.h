#ifndef ENGRAVE_LOAD_H
#define ENGRAVE_LOAD_H

/**
 * @file engrave_load.h Engrave loading functions
 * @brief Contains the functions to load either an EDC or EET into Engrave.
 */

/**
 * @defgroup Engrave_Load Engrave_Load: Contains the functions to load EDC
 * and EET files into Engrave.
 * 
 * @{
 */

Engrave_File *engrave_load_edc(char *file, char *imdir, char *fontdir);
Engrave_File *engrave_load_eet(char *filename);

/**
 * @}
 */

#endif

