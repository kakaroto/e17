#include <stdlib.h>
#include <stdio.h>

#include <libspectre/spectre.h>

#include "eps_private.h"
#include "eps_document.h"


Eps_Document *
eps_document_new (const char *filename)
{
   SpectreDocument *sd;
   Eps_Document    *document;
   SpectreStatus    status;

   if (!filename || (filename[0] == '\0'))
     return NULL;

   document = (Eps_Document *)malloc (sizeof (Eps_Document));
   if (!document)
     return NULL;

   sd = spectre_document_new ();
   if (!sd) {
      free (document);
      return NULL;
   }

   spectre_document_load (sd, filename);
   status = spectre_document_status (sd);
   if (status != SPECTRE_STATUS_SUCCESS) {
      printf ("[eps] %s\n", spectre_status_to_string (status));
      spectre_document_free (sd);
      return NULL;
   }

   document->ps_doc = sd;

   return document;
}

void
eps_document_delete (Eps_Document *document)
{
   if (!document)
     return;

   spectre_document_free (document->ps_doc);
   free (document);
}

int
eps_document_page_count_get (const Eps_Document *document)
{
   unsigned int  page_count;
   SpectreStatus status;

   if (!document)
     return 0;

   page_count = spectre_document_get_n_pages (document->ps_doc);
   status = spectre_document_status (document->ps_doc);
   if (status != SPECTRE_STATUS_SUCCESS) {
      printf ("[eps] %s\n", spectre_status_to_string (status));
      return 0;
   }

   return page_count;
}

const char *
eps_document_title_get (const Eps_Document *document)
{
   const char   *title;
   SpectreStatus status;

   if (!document)
     return NULL;

   title = spectre_document_get_title (document->ps_doc);
   status = spectre_document_status (document->ps_doc);
   if (status != SPECTRE_STATUS_SUCCESS) {
      printf ("[eps] %s\n", spectre_status_to_string (status));
      return NULL;
   }

   return title;
}

const char *
eps_document_author_get (const Eps_Document *document)
{
   const char   *author;
   SpectreStatus status;

   if (!document)
     return NULL;

   author = spectre_document_get_creator (document->ps_doc);
   status = spectre_document_status (document->ps_doc);
   if (status != SPECTRE_STATUS_SUCCESS) {
      printf ("[eps] %s\n", spectre_status_to_string (status));
      return NULL;
   }

   return author;
}

const char *
eps_document_for_get (const Eps_Document *document)
{
   const char   *for_;
   SpectreStatus status;

   if (!document)
     return NULL;

   for_ = spectre_document_get_for (document->ps_doc);
   status = spectre_document_status (document->ps_doc);
   if (status != SPECTRE_STATUS_SUCCESS) {
      printf ("[eps] %s\n", spectre_status_to_string (status));
      return NULL;
   }

   return for_;
}

const char *
eps_document_format_get (const Eps_Document *document)
{
   const char   *format;
   SpectreStatus status;

   if (!document)
     return NULL;

   format = spectre_document_get_format (document->ps_doc);
   status = spectre_document_status (document->ps_doc);
   if (status != SPECTRE_STATUS_SUCCESS) {
      printf ("[eps] %s\n", spectre_status_to_string (status));
      return NULL;
   }

   return format;
}

const char *
eps_document_creation_date_get (const Eps_Document *document)
{
   const char   *date;
   SpectreStatus status;

   if (!document)
     return NULL;

   date = spectre_document_get_creation_date (document->ps_doc);
   status = spectre_document_status (document->ps_doc);
   if (status != SPECTRE_STATUS_SUCCESS) {
      printf ("[eps] %s\n", spectre_status_to_string (status));
      return NULL;
   }

   return date;
}

int
eps_document_is_eps_get (const Eps_Document *document)
{
   int           is_eps;
   SpectreStatus status;

   if (!document)
     return 0;

   is_eps = spectre_document_is_eps (document->ps_doc);
   status = spectre_document_status (document->ps_doc);
   if (status != SPECTRE_STATUS_SUCCESS) {
      printf ("[eps] %s\n", spectre_status_to_string (status));
      return 0;
   }

   return is_eps;
}

int
eps_document_language_level_get (const Eps_Document *document)
{
   int           ll;
   SpectreStatus status;

   if (!document)
     return 0;

   ll = spectre_document_get_language_level (document->ps_doc);
   status = spectre_document_status (document->ps_doc);
   if (status != SPECTRE_STATUS_SUCCESS) {
      printf ("[eps] %s\n", spectre_status_to_string (status));
      return 0;
   }

   return ll;
}
