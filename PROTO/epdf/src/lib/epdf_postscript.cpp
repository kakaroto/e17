#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <string.h>

#include <PDFDoc.h>
#include <PSOutputDev.h>

#include "Epdf.h"
#include "epdf_private.h"


Epdf_Postscript *
epdf_postscript_new (const Epdf_Document *document,
                     const char          *filename,
                     int                  first_page,
                     int                  last_page)
{
  Epdf_Postscript *ps;

  if (!document || !filename)
    return NULL;

  ps = (Epdf_Postscript *)malloc (sizeof (Epdf_Postscript));
  if (!ps)
    return NULL;

  ps->pdfdoc = document->pdfdoc;
  ps->filename = strdup (filename);
  ps->first_page = (first_page < 1) ? 1 : first_page;
  ps->last_page = (last_page > epdf_document_page_count_get (document))
    ? epdf_document_page_count_get (document)
    : last_page;

  ps->width = -1.0;
  ps->height = -1.0;
  ps->duplex = 1;

  return ps;
}

void
epdf_postscript_delete (Epdf_Postscript *postscript)
{
  if (!postscript)
    return;

  if (postscript->filename)
    free (postscript->filename);
  free (postscript);
}

void
epdf_postscript_size_set (Epdf_Postscript *postscript,
                          int              width,
                          int              height)
{
  if (!postscript)
    return;

  postscript->width = width;
  postscript->height = height;
}

void
epdf_postscript_duplex_set (Epdf_Postscript *postscript,
                            int              duplex)
{
  if (!postscript)
    return;

  postscript->duplex = duplex;
}

void
epdf_postscript_print (const Epdf_Postscript *postscript)
{
  PSOutputDev *ps_dev;

  if (!postscript)
    return;

  // FIXME: fix postscript title
  ps_dev = new PSOutputDev (postscript->filename,
                            postscript->pdfdoc->getXRef(),
                            postscript->pdfdoc->getCatalog(),
			    "PS title",
                            postscript->first_page,
                            postscript->last_page,
                            psModePS,
                            (int)postscript->width,
                            (int)postscript->height,
                            postscript->duplex,
                            0, 0, 0, 0, 0);
  if (!ps_dev)
    return;

  if (ps_dev->isOk ()) {
    for (int page = postscript->first_page; page <= postscript->last_page; page++)
      postscript->pdfdoc->displayPage (ps_dev,
                                       page,
                                       72.0, 72.0,
                                       0, 0, 1, 0);
  }

  delete ps_dev;
}
