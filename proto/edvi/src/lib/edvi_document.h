#ifndef __EDVI_DOCUMENT_H__
#define __EDVI_DOCUMENT_H__


#include "edvi_forward.h"


/**
 * @file edvi_document.h
 *
 * @defgroup Edvi_Document Edvi Document
 *
 * @brief Functions that manage DVI documents
 *
 * Functions that manage DVI documents
 *
 * @ingroup Edvi
 *
 * @{
 */

/**
 * @brief Return a newly created Edvi_Document object
 *
 * @param filename The file name
 * @param device Thee dvi device
 * @param property Thee dvi property
 * @return A pointer to a newly created Edvi_Document
 *
 * Return a newly created Edvi_Document object built from the
 * DVI file whose name is @p filename. The result must be freed with
 * edvi_document_delete().
 */
Edvi_Document *edvi_document_new (const char    *filename,
                                  Edvi_Device   *device,
                                  Edvi_Property *property);

/**
 * @brief Delete an Edvi_Document object
 *
 * @param document The document to delete
 *
 * Delete the Edvi_Document @p document that has been created
 * with edvi_document_new().
 */
void edvi_document_delete (Edvi_Document *document);

/**
 * @brief Return the count of the pages of the document
 *
 * @param document The document
 * @return The count of the pages
 *
 * Return the count of the pages of the document @p document
 */
int edvi_document_page_count_get (const Edvi_Document *document);

/**
 * @}
 */


#endif /* __EDVI_DOCUMENT_H__ */
