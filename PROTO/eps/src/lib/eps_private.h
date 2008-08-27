#ifndef __EPS_PRIVATE_H__
#define __EPS_PRIVATE_H__


struct _Eps_Document
{
   SpectreDocument *ps_doc;
};


struct _Eps_Page
{
   SpectreDocument      *document;
   SpectrePage          *page;
   SpectreRenderContext *rc;
};


#endif /* __EPS_PRIVATE_H__ */
