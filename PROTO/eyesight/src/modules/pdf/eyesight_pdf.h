#ifndef __EYESIGHT_PDF_PRIVATE_H__
#define __EYESIGHT_PDF_PRIVATE_H__


typedef struct _Eyesight_Backend_Pdf  Eyesight_Backend_Pdf;

struct _Eyesight_Backend_Pdf
{
  char *filename;
  Eyesight_Document_Pdf *document;
  Evas_Object *obj;

  /* Document */
  struct {
    PDFDoc          *pdfdoc;
    Dict            *dict;
    FontInfoScanner *scanner;
    Eina_List       *toc;
  } doc;

  /* Current page */
  struct {
    ::Page              *page;
    Eyesight_Orientation orientation;
    double               hscale;
    double               vscale;
    Eina_List           *links;
  } page;
};


#endif /* __EYESIGHT_PDF_PRIVATE_H__ */
