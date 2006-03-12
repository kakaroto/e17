#ifndef __POPPLER_FONTINFO_H__
#define __POPPLER_FONTINFO_H__


#include "poppler_forward.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * Return a new Evas_Poppler_Font_Info structure
 *
 * @param font_name The name of the font
 * @param is_embedded Set the font as embedded
 * @param is_subset Set the font as subset
 * @param type The type of the font
 * @return A pointer on a newly created Evas_Poppler_Font_Info
 *
 * Return a newly created Evas_Poppler_Font_Info structure. It must be
 * freed with evas_poppler_font_info_delete.
 */
Evas_Poppler_Font_Info *evas_poppler_font_info_new (const char                 *font_name,
                                                    const char                 *font_path,
                                                    unsigned char               is_embedded,
                                                    unsigned char               is_subset,
                                                    Evas_Poppler_Font_Info_Type type);

/**
 * Delete a Evas_Poppler_Font_Info
 *
 * @param fi The Evas_Poppler_Font_Info to delete
 *
 * Delete a Evas_Poppler_Font_Info structure allocated by evas_poppler_font_info_new
 */
void evas_poppler_font_info_delete (Evas_Poppler_Font_Info *fi);

/**
 * Get the name of a Evas_Poppler_Font_Info
 *
 * @param fi The Evas_Poppler_Font_Info to get the name from
 * @return The name of the font
 *
 * Get the name of a Evas_Poppler_Font_Info. The name must not be freed.
 */
const char *evas_poppler_font_info_font_name_get (Evas_Poppler_Font_Info *fi);

/**
 * Get the path of a Evas_Poppler_Font_Info
 *
 * @param fi The Evas_Poppler_Font_Info to get the name from
 * @return The path of the font
 *
 * Get the path of a Evas_Poppler_Font_Info. The path must not be freed.
 */
const char *evas_poppler_font_info_font_path_get (Evas_Poppler_Font_Info *fi);

/**
 * Whether the font is embedded in the file, or not
 *
 * @param fi The Evas_Poppler_Font_Info
 * @return 1 if the font is embedded, 0 otherwise
 *
 * Whether the font is embedded in the file, or not
 */
unsigned char evas_poppler_font_info_is_embedded_get (Evas_Poppler_Font_Info *fi);

/**
 * Whether the font provided is only a subset of the full
 * font or not. This only has meaning if the font is embedded.
 *
 * @param fi The Evas_Poppler_Font_Info
 * @return 1 if the font is a subset, 0 otherwise
 *
 * Whether the font is a subset, or not
 */
unsigned char evas_poppler_font_info_is_subset_get (Evas_Poppler_Font_Info *fi);

/**
 * The type of the font encoding
 *
 * @param fi The Evas_Poppler_Font_Info
 * @return The type of the font encoding as a Evas_Poppler_Font_Info_Type
 *
 * The type of the font encoding
 */
Evas_Poppler_Font_Info_Type evas_poppler_font_info_type_get (Evas_Poppler_Font_Info *fi);

/**
 * The type of the font encoding as a string
 *
 * @param fi The Evas_Poppler_Font_Info
 * @return The type of the font encoding as a Evas_Poppler_Font_Info_Type
 *
 * The type of the font encoding as a Evas_Poppler_Font_Info_Type
 */

/**
 * The type of the font encoding
 *
 * @param fi The Evas_Poppler_Font_Info
 * @return The type of the font encoding as a Evas_Poppler_Font_Info_Type
 *
 * The type of the font encoding as a string. The functions can be
 * used for displaying the type encoding of the font with printf, for
 * example. The result must not be freed
 */
const char *evas_poppler_font_info_type_name_get (Evas_Poppler_Font_Info *fi);


#ifdef __cplusplus
}
#endif


#endif /* __POPPLER_FONTINFO_H__ */
