#ifndef __EPDF_FONTINFO_H__
#define __EPDF_FONTINFO_H__


#include "epdf_forward.h"


/**
 * @file epdf_fontinfo.h
 *
 * @defgroup Epdf_Font_Info Epdf Font Info
 *
 * @brief Functions that manage font informations in a PDF document
 *
 * Functions that manage font informations in a PDF document
 *
 * @ingroup Epdf
 *
 * @{
 */


/**
 * @brief Return a new Epdf_Font_Info structure
 *
 * @param font_name The name of the font
 * @param font_path The path of the font
 * @param is_embedded Set the font as embedded
 * @param is_subset Set the font as subset
 * @param type The type of the font
 * @return A pointer on a newly created Epdf_Font_Info
 *
 * Return a newly created Epdf_Font_Info structure. It must be
 * freed with epdf_font_info_delete.
 */
Epdf_Font_Info *epdf_font_info_new (const char         *font_name,
                                    const char         *font_path,
                                    unsigned char       is_embedded,
                                    unsigned char       is_subset,
                                    Epdf_Font_Info_Type type);

/**
 * @brief Delete a Epdf_Font_Info
 *
 * @param fi The Epdf_Font_Info to delete
 *
 * Delete a Epdf_Font_Info structure allocated by epdf_font_info_new
 */
void epdf_font_info_delete (Epdf_Font_Info *fi);

/**
 * @brief Get the name of a Epdf_Font_Info
 *
 * @param fi The Epdf_Font_Info to get the name from
 * @return The name of the font
 *
 * Get the name of a Epdf_Font_Info. The name must not be freed.
 */
const char *epdf_font_info_font_name_get (const Epdf_Font_Info *fi);

/**
 * @brief Get the path of a Epdf_Font_Info
 *
 * @param fi The Epdf_Font_Info to get the name from
 * @return The path of the font
 *
 * Get the path of a Epdf_Font_Info. The path must not be freed.
 */
const char *epdf_font_info_font_path_get (const Epdf_Font_Info *fi);

/**
 * @brief Whether the font is embedded in the file, or not
 *
 * @param fi The Epdf_Font_Info
 * @return 1 if the font is embedded, 0 otherwise
 *
 * Whether the font is embedded in the file, or not
 */
unsigned char epdf_font_info_is_embedded_get (const Epdf_Font_Info *fi);

/**
 * @brief Whether the font provided is only a subset of the full
 * font or not. This only has meaning if the font is embedded.
 *
 * @param fi The Epdf_Font_Info
 * @return 1 if the font is a subset, 0 otherwise
 *
 * Whether the font is a subset, or not
 */
unsigned char epdf_font_info_is_subset_get (const Epdf_Font_Info *fi);

/**
 * @brief The type of the font encoding
 *
 * @param fi The Epdf_Font_Info
 * @return The type of the font encoding as a Epdf_Font_Info_Type
 *
 * The type of the font encoding as a Epdf_Font_Info_Type
 */
Epdf_Font_Info_Type epdf_font_info_type_get (const Epdf_Font_Info *fi);

/**
 * @brief The type of the font encoding
 *
 * @param fi The Epdf_Font_Info
 * @return The type of the font encoding as a Epdf_Font_Info_Type
 *
 * The type of the font encoding as a string. The functions can be
 * used for displaying the type encoding of the font with printf, for
 * example. The result must not be freed
 */
const char *epdf_font_info_type_name_get (const Epdf_Font_Info *fi);

/**
 * @}
 */


#endif /* __EPDF_FONTINFO_H__ */
