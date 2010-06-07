#ifndef __EPDF_MUPDF_PRIVATE_H__
#define __EPDF_MUPDF_PRIVATE_H__


struct _Epdf_Page
{
   Epdf_Document        *doc;
   pdf_page             *page;
   fz_pixmap            *image;
   int                   index;
   Epdf_Page_Orientation orientation;
};

struct _Epdf_Document
{
   char            *filename;
   char            *doctitle;
   pdf_xref        *xref;
   pdf_outline     *outline;
   fz_renderer     *rast;
   int              pagecount;
   float            zoom;
   int              rotate;
   unsigned int     locked;
};

struct _Epdf_Index_Item
{
   char       *title;
   pdf_link   *link;
   Eina_List  *children;
};


#endif /* __EPDF_MUPDF_PRIVATE_H__ */
