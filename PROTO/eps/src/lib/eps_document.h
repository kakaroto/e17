#ifndef __EPS_DOCUMENT_H__
#define __EPS_DOCUMENT_H__


#include "eps_forward.h"


/**
 * @file eps_document.h
 *
 * @defgroup Eps_Document Eps Document
 *
 * @brief Functions that manage Postscipt documents
 *
 * Functions that manage Postscipt documents
 *
 * @ingroup Eps
 *
 * @{
 */

/**
 * @brief Return a newly created Eps_Document object.
 *
 * @param filename The file name.
 * @return A pointer to a newly created Eps_Document.
 *
 * Return a newly created Eps_Document object built from the
 * Postscript file whose name is @p filename. The result must
 * be freed with eps_document_delete().
 */
Eps_Document *eps_document_new (const char *filename);

/**
 * @brief Delete an Eps_Document object.
 *
 * @param document The document to delete.
 *
 * Delete the Eps_Document @p document that has been created
 * with eps_document_new().
 */
void eps_document_delete (Eps_Document *document);

/**
 * @brief Return the count of the pages of the document.
 *
 * @param document The document.
 * @return The count of the pages.
 *
 * Return the count of the pages of the document @p document.
 */
int eps_document_page_count_get (const Eps_Document *document);

/**
 * @brief Return the title of the document as a string.
 *
 * @param document The document.
 * @return The title as a string.
 *
 * Return the title of the document @p document as a string. The
 * result must not be freed.
 */
const char *eps_document_title_get (const Eps_Document *document);

/**
 * @brief Return the author of the document as a string.
 *
 * @param document The document.
 * @return The author as a string.
 *
 * Return the author of the document @p document as a string. The
 * result must not be freed
 */
const char *eps_document_author_get (const Eps_Document *document);

/**
 * @brief Return the 'For:' header field of the document as a string.
 *
 * @param document The document.
 * @return The 'For:' header field.
 *
 * Return the 'For:' header field of the document @p document as
 * a string. The result must not be freed.
 */
const char *eps_document_for_get (const Eps_Document *document);

/**
 * @brief Return the format of the document as a string.
 *
 * @param document The document.
 * @return The format of the document.
 *
 * Return the format of the document @p document as a string,
 * like 'PS-Adobe-2.0'. The result must not be freed.
 */
const char *eps_document_format_get (const Eps_Document *document);

/**
 * @brief Return the creation date of the document as a string.
 *
 * @param document The document.
 * @return The creation date as a string.
 *
 * Return the creation date of the document @p document as a
 * string. The result must not be freed.
 */
const char *eps_document_creation_date_get (const Eps_Document *document);

/**
 * @brief Return whether the document is encapsulated or not.
 *
 * @param document The document.
 * @return 1 if the document is an eps, 0 otherwise.
 *
 * Return 1 if the document is an Ecapsulated Poscript file, 0
 * otherwise.
 */
int eps_document_is_eps_get (const Eps_Document *document);

/**
 * @brief Return the language level of the document.
 *
 * @param document The document.
 * @return The language level of the document.
 *
 * Return the language level of the document.
 */
int eps_document_language_level_get (const Eps_Document *document);

/**
 * @}
 */


#endif /* __EPS_DOCUMENT_H__ */
