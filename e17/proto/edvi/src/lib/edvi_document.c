#include <stdlib.h>

#include <libdvi29.h>

#include "edvi_enum.h"
#include "edvi_forward.h"
#include "edvi_private.h"

Edvi_Document *
edvi_document_new (const char    *filename,
                   Edvi_Device   *device,
                   Edvi_Property *property)
{
  Edvi_Document *document;

  if (!filename || !device || !property)
    return NULL;
  
  document = (Edvi_Document *)malloc (sizeof (Edvi_Document));
  if (!document)
    return NULL;

  document->dvi_document = DVI_CREATE(device->dvi_device, (char *)filename, property->dvi_property);
  if (!document->dvi_document) {
    free (document);
    return NULL;
  }

  if (DVI_OPEN_FONT (document->dvi_document, device->dvi_device) < 0) {
    DVI_DISPOSE(document->dvi_document, document->device->dvi_device);
    free (document);
    return NULL;
  }

  document->device = device;

  return document;
}

void
edvi_document_delete (Edvi_Document *document)
{
  if (!document)
    return;

  DVI_DISPOSE(document->dvi_document, document->device->dvi_device);
  free (document);
  document = NULL;
}

int
edvi_document_page_count_get (Edvi_Document *document)
{
  if (!document || !document->dvi_document)
    return 0;

  return document->dvi_document->pages;
}
