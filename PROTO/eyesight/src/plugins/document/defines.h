#ifndef DEFINES_H_
#define DEFINES_H_

#include <stdlib.h>

#if defined PDF
/* Defines for PDF plugin */
#include <Epdf.h>
#include <esmart_pdf.h>

// Functions
#define doc_add(X)               esmart_pdf_add(X)
#define doc_init(X)              esmart_pdf_init(X)
#define doc_file_set(X, Y)       esmart_pdf_file_set(X, Y)
#define doc_file_get(X)          esmart_pdf_file_get(X)
#define doc_page_set(X, Y)       esmart_pdf_page_set(X, Y)
#define doc_page_get(X)          esmart_pdf_page_get(X)
#define doc_size_get(X, Y, Z)    esmart_pdf_size_get(X, Y, Z)
#define doc_document_get(X)      esmart_pdf_pdf_document_get(X)
#define doc_page_obj_get(X)      esmart_pdf_pdf_page_get(X)
#define doc_page_count_get(X)    epdf_document_page_count_get(X)
#define doc_render(X)            esmart_pdf_render(X)

#define doc_scale_set(X, Y)      esmart_pdf_scale_set(X, Y, Y)

static double inline
doc_scale_get(Evas_Object *o)
{
   double scale;
   esmart_pdf_scale_get(o, &scale, NULL);
   return scale;
}

// Types
typedef Epdf_Document         Doc_Document;
typedef Epdf_Page             Doc_Page;

#elif defined DVI
/* Defines for DVI plugin */
#include <Edvi.h>
#include <esmart_dvi.h>

// Functions
#define doc_add(X)               esmart_dvi_add(X)
#define doc_init(X)              esmart_dvi_init(X)
#define doc_file_set(X, Y)       esmart_dvi_file_set(X, Y)
#define doc_file_get(X)          esmart_dvi_file_get(X)
#define doc_page_set(X, Y)       esmart_dvi_page_set(X, Y)
#define doc_page_get(X)          esmart_dvi_page_get(X)
#define doc_size_get(X, Y, Z)    esmart_dvi_size_get(X, Y, Z)
#define doc_document_get(X)      esmart_dvi_dvi_document_get(X)
#define doc_page_obj_get(X)      esmart_dvi_dvi_page_get(X)
#define doc_page_count_get(X)    edvi_document_page_count_get(X)
#define doc_render(X)            esmart_dvi_render(X)

#define doc_scale_set(X, Y)      esmart_dvi_mag_set(X, Y)
#define doc_scale_get(X)         esmart_dvi_mag_get(X)

// Types
typedef Edvi_Document         Doc_Document;
typedef Edvi_Page             Doc_Page;

#elif defined PS
/* Defines for PDF plugin */
#include <Eps.h>
#include <esmart_ps.h>

// Functions
#define doc_add(X)               esmart_ps_add(X)
#define doc_init(X)              esmart_ps_init(X)
#define doc_file_set(X, Y)       esmart_ps_file_set(X, Y)
#define doc_file_get(X)          esmart_ps_file_get(X)
#define doc_page_set(X, Y)       esmart_ps_page_set(X, Y)
#define doc_page_get(X)          esmart_ps_page_get(X)
#define doc_size_get(X, Y, Z)    esmart_ps_size_get(X, Y, Z)
#define doc_document_get(X)      esmart_ps_ps_document_get(X)
#define doc_page_obj_get(X)      esmart_ps_ps_page_get(X)
#define doc_page_count_get(X)    eps_document_page_count_get(X)
#define doc_render(X)            esmart_ps_render(X)

#define doc_scale_set(X, Y)      esmart_ps_scale_set(X, Y, Y)

static double inline
doc_scale_get(Evas_Object *o)
{
   double scale;
   esmart_ps_scale_get(o, &scale, NULL);
   return scale;
}

// Types
typedef Eps_Document         Doc_Document;
typedef Eps_Page             Doc_Page;

#endif /* PDF & DVI*/
#endif /* DEFINES_H_ */
