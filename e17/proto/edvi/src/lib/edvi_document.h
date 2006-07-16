#ifndef __EDVI_DOCUMENT_H__
#define __EDVI_DOCUMENT_H__


#include "edvi_forward.h"


Edvi_Document *edvi_document_new (const char    *filename,
                                  Edvi_Device   *device,
                                  Edvi_Property *property);

void           edvi_document_delete (Edvi_Document *document);

int            edvi_document_page_count_get (Edvi_Document *document);


#endif /* __EDVI_DOCUMENT_H__ */
