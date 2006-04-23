#ifndef __EPDF_POSTSCRIPT_H__
#define __EPDF_POSTSCRIPT_H__


#include "poppler_forward.h"


#ifdef __cplusplus
extern "C" {
#endif


/**
 * Return a newly created Epdf_Postscript object
 *
 * @param document The PDF document
 * @param filename The file name
 * @param first_page The first page that will be printed
 * @param last_page The last page that will be printed
 * @return A pointer to a newly created Epdf_Postscript
 *
 * Return a newly created Epdf_Postscript object built from the
 * PDF document @p document,  whose name will be @p filename. The
 * first and last pages that will be printed are respectively @p
 * first_page and @p last_page. The count of the pages begins from 1
 * and not 0
 */
Epdf_Postscript *epdf_postscript_new (Epdf_Document *document,
                                      const char    *filename,
                                      int            first_page,
                                      int            last_page);

/**
 * Delete an Epdf_Postscript object
 *
 * @param postscript The Epdf_Postscript object to delete
 *
 * Delete the Epdf_Postscript @p postscript that has been created
 * with epdf_postscript_new
 *
 */
void epdf_postscript_delete (Epdf_Postscript *postscript);

/**
 * Set the size of an Epdf_Postscript object
 *
 * @param postscript The postscript to delete
 * @param width The width of the page
 * @param height The height of the page
 *
 * Set the width @p width and the height @p height of the postscipt
 * file @p postscript. The sizes are in 1/72 inch.
 *
 */
void epdf_postscript_size_set (Epdf_Postscript *postscript,
                               int              width,
                               int              height);

/**
 * Set if a Epdf_Postscript object prints in duplex mode or not
 *
 * @param postscript The postscript to print
 * @param duplex The duplex mode
 *
 * Set if the Epdf_Postscript object @p postscript prints in duplex
 * mode or not. Set @p duplex to @c 0 to not print in duplex mode, non
 * 0 otherwise
 *
 */
void epdf_postscript_duplex_set (Epdf_Postscript *postscript,
                                 int              duplex);

/**
 * Print a Epdf_Postscript object
 *
 * @param postscript The postscript to print
 *
 * Print a Postscript file described by @p postscript
 *
 */
void epdf_postscript_print (Epdf_Postscript *postscript);


#ifdef __cplusplus
}
#endif


#endif /* __EPDF_POSTSCRIPT_H__ */
