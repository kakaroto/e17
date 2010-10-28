#ifndef __EYESIGHT_PDF_H__
#define __EYESIGHT_PDF_H__


typedef struct _Eyesight_Backend_Pdf Eyesight_Backend_Pdf;

struct _Eyesight_Backend_Pdf
{
  char *filename;
  Evas_Object *obj;

  /* Document */
  struct {
    PDFDoc          *pdfdoc;
    Eina_Bool        locked;
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


#endif /* __EYESIGHT_PDF_H__ */
