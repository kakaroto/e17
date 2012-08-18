#ifndef __EYESIGHT_MODULE_PDF_H__
#define __EYESIGHT_MODULE_PDF_H__


typedef enum
{
  EYESIGHT_DOCUMENT_PDF_PAGE_MODE_USE_NONE,       /**< No page mode */
  EYESIGHT_DOCUMENT_PDF_PAGE_MODE_USE_OUTLINES,   /**< Outlines page mode */
  EYESIGHT_DOCUMENT_PDF_PAGE_MODE_USE_THUMBS,     /**< Thumbs page mode */
  EYESIGHT_DOCUMENT_PDF_PAGE_MODE_FULLSCREEN,     /**< Fullscreen page mode */
  EYESIGHT_DOCUMENT_PDF_PAGE_MODE_USE_OC          /**< OC page mode */
} Eyesight_Document_Pdf_Page_Mode;

typedef enum
{
  EYESIGHT_DOCUMENT_PDF_PAGE_LAYOUT_NONE,               /**< No page layout */
  EYESIGHT_DOCUMENT_PDF_PAGE_LAYOUT_SINGLE_PAGE,        /**< Single page layout */
  EYESIGHT_DOCUMENT_PDF_PAGE_LAYOUT_ONE_COLUMN,         /**< One column page layout */
  EYESIGHT_DOCUMENT_PDF_PAGE_LAYOUT_TWO_COLUMN_LEFT,    /**< Two column left page layout */
  EYESIGHT_DOCUMENT_PDF_PAGE_LAYOUT_TWO_COLUMN_RIGHT,   /**< Two column right page layout */
  EYESIGHT_DOCUMENT_PDF_PAGE_LAYOUT_TWO_PAGE_LEFT,      /**< Two page left page layout */
  EYESIGHT_DOCUMENT_PDF_PAGE_LAYOUT_TWO_PAGE_RIGHT      /**< Two page right page layout */
} Eyesight_Document_Pdf_Page_Layout;


typedef struct
{
  int version_maj;
  int version_min;
  const char *filename;
  char *title;
  char *author;
  char *subject;
  char *keywords;
  char *creator;
  char *producer;
  char *date_creation;
  char *date_modification;
  Eyesight_Document_Pdf_Page_Mode mode;
  Eyesight_Document_Pdf_Page_Layout layout;
  Eina_Bool linearized : 1;
  Eina_Bool printable : 1;
  Eina_Bool changeable : 1;
  Eina_Bool copyable : 1;
  Eina_Bool notable : 1;
} Eyesight_Document_Pdf;


#endif /* __EYESIGHT_MODULE_PDF_H__ */
