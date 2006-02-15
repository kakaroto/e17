#ifndef __POPPLER_DOCUMENT_H__
#define __POPPLER_DOCUMENT_H__


#include "poppler_forward.h"


#ifdef __cplusplus
extern "C" {
#endif


/**
 * Return a newly created Evas_Poppler_Document object
 *
 * @param filename The file name
 * @return A pointer to a newly created Evas_Poppler_Document
 *
 * Return a newly created Evas_Poppler_Document object built from the
 * PDF file whose name is @p filename. The result must be freed with
 * evas_poppler_document_delete
 */
Evas_Poppler_Document *evas_poppler_document_new (const char *filename);

/**
 * Delete an Evas_Poppler_Document object
 *
 * @param document The document to delete
 *
 * Delete the Evas_Poppler_Document @p document that has been created
 * withevas_poppler_document_new
 * 
 */
void evas_poppler_document_delete (Evas_Poppler_Document *document);

/**
 * Return a newly created Evas_Poppler_Page object
 *
 * @param document The document to delete
 * @param index The number of the page
 * @return The page 
 *
 * Return the page number @p index of the document @p document. The
 * result must be freed with evas_poppler_page_delete
 * 
 */
Evas_Poppler_Page *evas_poppler_document_page_get (Evas_Poppler_Document *document,
                                                   int                    index);

/**
 * Return the count of the pages of the document
 *
 * @param document The document
 * @return The count of the pages
 *
 * Return the count of the pages of the document @p document
 * 
 */
int evas_poppler_document_page_count_get (Evas_Poppler_Document *document);

/**
 * Return the page mode of the document
 *
 * @param document The document
 * @return The page mode
 *
 * Return the page mode of the document @p document
 * 
 */
Evas_Poppler_Document_Page_Mode evas_poppler_document_page_mode_get (Evas_Poppler_Document *document);

/**
 * Unlock the document
 *
 * @param document The document
 * @param password
 * @return 1 if the document has been unlocked, 0 otherwise
 *
 * Unlock the document @p document with the password @p password
 * 
 */
unsigned char evas_poppler_document_unlock (Evas_Poppler_Document *document, const char *password);

/**
 * Return whether the document is locked
 *
 * @param document The document
 * @return 1 if the document is locked, 0 otherwise
 *
 * Return whether the document @p document is locked
 * 
 */
unsigned char evas_poppler_document_is_locked (Evas_Poppler_Document *document);

/**
 * Return 
 *
 * @param document The document
 * @return 
 *
 * Return 
 * 
 */
const char *evas_poppler_document_info_get (const char *data);

/**
 * Return wheter the document is encrypted
 *
 * @param document The document
 * @return 1 if the document is encrypted, 0 otherwise
 *
 * Return wheter the document @p document is encrypted
 * 
 */
unsigned char evas_poppler_document_is_encrypted (Evas_Poppler_Document *document);

/**
 * Return wheter the document is linearized
 *
 * @param document The document
 * @return 1 if the document is linearized, 0 otherwise
 *
 * Return wheter the document @p document is linearized
 * 
 */
unsigned char evas_poppler_document_is_linearized (Evas_Poppler_Document *document);

/**
 * Return wheter the document is printable
 *
 * @param document The document
 * @return 1 if the document is printable, 0 otherwise
 *
 * Return wheter the document @p document is printable
 * 
 */
unsigned char evas_poppler_document_is_printable (Evas_Poppler_Document *document);

/**
 * Return wheter the document is changeable
 *
 * @param document The document
 * @return 1 if the document is changeable, 0 otherwise
 *
 * Return wheter the document @p document is changeable
 * 
 */
unsigned char evas_poppler_document_is_changeable (Evas_Poppler_Document *document);

/**
 * Return wheter the document is copyable
 *
 * @param document The document
 * @return 1 if the document is copyable, 0 otherwise
 *
 * Return wheter the document @p document is copyable
 * 
 */
unsigned char evas_poppler_document_is_copyable (Evas_Poppler_Document *document);

/**
 * Return wheter the document is notable
 *
 * @param document The document
 * @return 1 if the document is notable, 0 otherwise
 *
 * Return wheter the document @p document is notable
 * 
 */
unsigned char evas_poppler_document_is_notable (Evas_Poppler_Document *document);

/**
 * Return the version of the pdf used in the document
 *
 * @param document The document
 * @return The version
 *
 * Return the version of the pdf used in the document @p document
 * 
 */
double evas_poppler_document_pdf_version_get (Evas_Poppler_Document *document);

/**
 * Return a list of the fonts used in the document
 *
 * @param document The document
 * @return The list of the fonts
 *
 * Return a list of the fonts used in the document @p document
 * 
 */
Ecore_List *evas_poppler_document_fonts_get (Evas_Poppler_Document *document);

/**
 * Return a list of the fonts used in a page of the document
 *
 * @param document The document
 * @return The list of the fonts
 *
 * Return a list of the fonts used in the document @p document in the
 * page number @p page_count
 * 
 */
Ecore_List * evas_poppler_document_scan_for_fonts (Evas_Poppler_Document *document,
                                                   int                    page_count);


/* Metadata information functions. Used for display */

/**
 * Return the filename as a string
 *
 * @param document The document
 * @return The filename as a string
 *
 * Return the filename of the document @p document as a string
 * 
 */
char *evas_poppler_document_filename_get (Evas_Poppler_Document *document);

/**
 * Return the title as a string
 *
 * @param document The document
 * @return The title as a string
 *
 * Return the title of the document @p document as a string
 * 
 */
char *evas_poppler_document_title_get (Evas_Poppler_Document *document);

/**
 * Return the author as a string
 *
 * @param document The document
 * @return The author as a string
 *
 * Return the author of the document @p document as a string
 * 
 */
char *evas_poppler_document_author_get (Evas_Poppler_Document *document);

/**
 * Return the subject as a string
 *
 * @param document The document
 * @return The subject as a string
 *
 * Return the subject of the document @p document as a string
 * 
 */
char *evas_poppler_document_subject_get (Evas_Poppler_Document *document);

/**
 * Return the keywords as a string
 *
 * @param document The document
 * @return The keywords as a string
 *
 * Return the keywords of the document @p document as a string
 * 
 */
char *evas_poppler_document_keywords_get (Evas_Poppler_Document *document);

/**
 * Return the creator as a string
 *
 * @param document The document
 * @return The creator as a string
 *
 * Return the creator of the document @p document as a string
 * 
 */
char *evas_poppler_document_creator_get (Evas_Poppler_Document *document);

/**
 * Return the producer as a string
 *
 * @param document The document
 * @return The producer as a string
 *
 * Return the producer of the document @p document as a string
 * 
 */
char *evas_poppler_document_producer_get (Evas_Poppler_Document *document);

/**
 * Return the creation date as a string
 *
 * @param document The document
 * @return The creation date as a string
 *
 * Return the creation date of the document @p document as a
 * string. The result must be freed.
 * 
 */
char *evas_poppler_document_creation_date_get (Evas_Poppler_Document *document);

/**
 * Return the modification date as a string
 *
 * @param document The document
 * @return The modification date as a string
 *
 * Return the modification date of the document @p document as a
 * string. The result must be freed.
 * 
 */
char *evas_poppler_document_mod_date_get (Evas_Poppler_Document *document);

/**
 * Return whether the document is linearized, as a string
 *
 * @param document The document
 * @return Whether the document is linearized, as a string
 *
 * Return whether the document @p document is linearized, as a
 * string. The result must not be freed.
 * 
 */
const char *evas_poppler_document_linearized_get (Evas_Poppler_Document *document);

/**
 * Return the page mode of the document
 *
 * @param document The document
 * @return The page mode
 *
 * Return the page mode of the document @p document, as a string. The
 * result must not be freed.
 * 
 */
const char *evas_poppler_document_page_mode_string_get (Evas_Poppler_Document *document);

/**
 * Return the page layout of the document
 *
 * @param document The document
 * @return The page layout
 *
 * Return the page layout of the document @p document, as a string. The
 * result must not be freed.
 * 
 */
const char *evas_poppler_document_page_layout_string_get
(Evas_Poppler_Document *document);


#ifdef __cplusplus
}
#endif


#endif /* __POPPLER_DOCUMENT_H__ */
