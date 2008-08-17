#ifndef __EPDF_DOCUMENT_H__
#define __EPDF_DOCUMENT_H__


#include "epdf_forward.h"


/**
 * @file epdf_document.h
 *
 * @defgroup Epdf_Document Epdf Document
 *
 * @brief Functions that manage PDF documents
 *
 * Functions that manage PDF documents
 *
 * @ingroup Epdf
 *
 * @{
 */


/**
 * @brief Return a newly created Epdf_Document object
 *
 * @param filename The file name
 * @return A pointer to a newly created Epdf_Document
 *
 * Return a newly created Epdf_Document object built from the
 * PDF file whose name is @p filename. The result must be freed with
 * epdf_document_delete().
 */
Epdf_Document *epdf_document_new (const char *filename);

/**
 * @brief Delete an Epdf_Document object
 *
 * @param document The document to delete
 *
 * Delete the Epdf_Document @p document that has been created
 * with epdf_document_new().
 */
void epdf_document_delete (Epdf_Document *document);

/**
 * @brief Return the count of the pages of the document
 *
 * @param document The document
 * @return The count of the pages
 *
 * Return the count of the pages of the document @p document
 */
int epdf_document_page_count_get (const Epdf_Document *document);

/**
 * @brief Return the page mode of the document
 *
 * @param document The document
 * @return The page mode
 *
 * Return the page mode of the document @p document
 */
Epdf_Document_Page_Mode epdf_document_page_mode_get (const Epdf_Document *document);

/**
 * @brief Unlock the document
 *
 * @param document The document
 * @param password
 * @return 1 if the document has been unlocked, 0 otherwise
 *
 * Unlock the document @p document with the password @p password
 */
unsigned char epdf_document_unlock (Epdf_Document *document, const char *password);

/**
 * @brief Return whether the document is locked
 *
 * @param document The document
 * @return 1 if the document is locked, 0 otherwise
 *
 * Return whether the document @p document is locked
 */
unsigned char epdf_document_is_locked (const Epdf_Document *document);

/**
 * @brief To be implemented
 *
 * @param document The document
 * @param data data
 * @return @c NULL
 *
 * Return NULL
 */
const char *epdf_document_info_get (const Epdf_Document *document, const char *data);

/**
 * @brief Return wheter the document is encrypted
 *
 * @param document The document
 * @return 1 if the document is encrypted, 0 otherwise
 *
 * Return wheter the document @p document is encrypted
 */
unsigned char epdf_document_is_encrypted (const Epdf_Document *document);

/**
 * @brief Return wheter the document is linearized
 *
 * @param document The document
 * @return 1 if the document is linearized, 0 otherwise
 *
 * Return wheter the document @p document is linearized
 */
unsigned char epdf_document_is_linearized (const Epdf_Document *document);

/**
 * @brief Return wheter the document is printable
 *
 * @param document The document
 * @return 1 if the document is printable, 0 otherwise
 *
 * Return wheter the document @p document is printable
 */
unsigned char epdf_document_is_printable (const Epdf_Document *document);

/**
 * @brief Return wheter the document is changeable
 *
 * @param document The document
 * @return 1 if the document is changeable, 0 otherwise
 *
 * Return wheter the document @p document is changeable
 */
unsigned char epdf_document_is_changeable (const Epdf_Document *document);

/**
 * @brief Return wheter the document is copyable
 *
 * @param document The document
 * @return 1 if the document is copyable, 0 otherwise
 *
 * Return wheter the document @p document is copyable
 */
unsigned char epdf_document_is_copyable (const Epdf_Document *document);

/**
 * @brief Return wheter the document is notable
 *
 * @param document The document
 * @return 1 if the document is notable, 0 otherwise
 *
 * Return wheter the document @p document is notable
 */
unsigned char epdf_document_is_notable (const Epdf_Document *document);

/**
 * @brief Return the version of the pdf used in the document
 *
 * @param document The document
 * @return The version
 *
 * Return the version of the pdf used in the document @p document
 */
double epdf_document_pdf_version_get (const Epdf_Document *document);

/**
 * @brief Return a list of the fonts used in the document
 *
 * @param document The document
 * @return The list of the fonts
 *
 * Return a list of the fonts used in the document @p document
 */
Ecore_List *epdf_document_fonts_get (const Epdf_Document *document);

/**
 * @brief Return a list of the fonts used in a page of the document
 *
 * @param document The document
 * @param page_count The page count
 * @return The list of the fonts
 *
 * Return a list of the fonts used in the document @p document in the
 * page number @p page_count
 */
Ecore_List * epdf_document_scan_for_fonts (const Epdf_Document *document,
                                           int                  page_count);


/* Metadata information functions. Used for display */

/**
 * @brief Return the filename as a string
 *
 * @param document The document
 * @return The filename as a string
 *
 * Return the filename of the document @p document as a string
 */
const char *epdf_document_filename_get (const Epdf_Document *document);

/**
 * @brief Return the title as a string
 *
 * @param document The document
 * @return The title as a string
 *
 * Return the title of the document @p document as a string. The
 * result must be freed
 */
char *epdf_document_title_get (const Epdf_Document *document);

/**
 * @brief Return the author as a string
 *
 * @param document The document
 * @return The author as a string
 *
 * Return the author of the document @p document as a string. The
 * result must be freed
 */
char *epdf_document_author_get (const Epdf_Document *document);

/**
 * @brief Return the subject as a string
 *
 * @param document The document
 * @return The subject as a string
 *
 * Return the subject of the document @p document as a string. The
 * result must be freed.
 */
char *epdf_document_subject_get (const Epdf_Document *document);

/**
 * @brief Return the keywords as a string
 *
 * @param document The document
 * @return The keywords as a string
 *
 * Return the keywords of the document @p document as a string. The
 * result must be freed
 */
char *epdf_document_keywords_get (const Epdf_Document *document);

/**
 * @brief Return the creator as a string
 *
 * @param document The document
 * @return The creator as a string
 *
 * Return the creator of the document @p document as a string. The
 * result must be freed
 */
char *epdf_document_creator_get (const Epdf_Document *document);

/**
 * @brief Return the producer as a string
 *
 * @param document The document
 * @return The producer as a string
 *
 * Return the producer of the document @p document as a string. The
 * result must be freed
 */
char *epdf_document_producer_get (const Epdf_Document *document);

/**
 * @brief Return the creation date as a string
 *
 * @param document The document
 * @return The creation date as a string
 *
 * Return the creation date of the document @p document as a
 * string. The result must be freed.
 */
char *epdf_document_creation_date_get (const Epdf_Document *document);

/**
 * @brief Return the modification date as a string
 *
 * @param document The document
 * @return The modification date as a string
 *
 * Return the modification date of the document @p document as a
 * string. The result must be freed.
 */
char *epdf_document_mod_date_get (const Epdf_Document *document);

/**
 * @brief Return whether the document is linearized, as a string
 *
 * @param document The document
 * @return Whether the document is linearized, as a string
 *
 * Return whether the document @p document is linearized, as a
 * string. The result must not be freed.
 */
const char *epdf_document_linearized_get (const Epdf_Document *document);

/**
 * @brief Return the page mode of the document
 *
 * @param document The document
 * @return The page mode
 *
 * Return the page mode of the document @p document, as a string. The
 * result must not be freed.
 */
const char *epdf_document_page_mode_string_get (const Epdf_Document *document);

/**
 * @brief Return the page layout of the document
 *
 * @param document The document
 * @return The page layout
 *
 * Return the page layout of the document @p document, as a string. The
 * result must not be freed.
 */
const char *epdf_document_page_layout_string_get (const Epdf_Document *document);

/**
 * @}
 */


#endif /* __EPDF_DOCUMENT_H__ */
