#include <Engrave.h>

/**
 * @file engrave_font.h Engrave_Font block 
 * @brief Contains all of the functions to maniuplate Engrave_Font blocks
 */

/**
 * @defgroup Engrave_Font Engrave_Font: Functions to work with engrave font objects
 *
 * @{
 */

/**
 * engrave_font_new - create a new Engrave_Font object.
 * @param path: The path to the font
 * @param name: The name for the font
 *
 * @return Returns a pointer to a newly allocated Engrave_Font on success,
 * or NULL on failure.
 */
Engrave_Font *
engrave_font_new(char *path, char *name)
{
    Engrave_Font *ef;
    ef = NEW(Engrave_Font, 1);
    if (!ef) return NULL;

    ef->name = (name ? strdup(name) : NULL);
    ef->path = (path ? strdup(path) : NULL);
    return ef;
}

/**
 * @}
 */

