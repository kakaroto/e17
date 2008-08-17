#include <stdlib.h>
#include <string.h>

#include <Ecore_Evas.h>
#include <Epsilon.h>
#include <Epsilon_Plugin.h>
#include <Epdf.h>

Epsilon_Image *
epsilon_generate_thumb (Epsilon * e)
{
   Epsilon_Image *dst = NULL;
   Ecore_Evas    *ee;
   Evas          *evas;
   Evas_Object   *o;
   Epdf_Document *document;
   Epdf_Page     *page;
   int            width;
   int            height;
   const int     *pixels;

   document = epdf_document_new (e->src);
   if (!document)
     return NULL;

   page = epdf_page_new (document);
   if (!page)
     {
        epdf_document_delete (document);
        return NULL;
     }
   epdf_page_page_set (page, 0);
   epdf_page_size_get (page, &width, &height);

   ee = ecore_evas_buffer_new(width, height);
   evas = ecore_evas_get(ee);

   o = evas_object_image_add (evas);
   evas_object_move (o, 0, 0);
   epdf_page_render (page, o);
   evas_object_show (o);

   dst = calloc(1, sizeof(Epsilon_Image));
   if (!dst)
     {
        epdf_page_delete (page);
        epdf_document_delete (document);
        return NULL;
     }

   dst->w = width;
   dst->h = height;
   dst->alpha = 1;
   dst->data = malloc(dst->w * dst->h * sizeof(int));
   if (!dst->data)
     {
        free(dst);
        epdf_page_delete (page);
        epdf_document_delete (document);
        return NULL;
     }

   pixels = ecore_evas_buffer_pixels_get (ee);
   memcpy(dst->data, pixels, dst->w * dst->h * sizeof(int));

   ecore_evas_free(ee);
   epdf_page_delete (page);
   epdf_document_delete (document);

   return dst;
}

Epsilon_Plugin *
epsilon_plugin_init ()
{
   Epsilon_Plugin *plugin;

   plugin = calloc (1, sizeof (Epsilon_Plugin));
   if (!plugin) return NULL;

   plugin->mime_types = ecore_list_new ();
   if (!plugin->mime_types)
     {
        free(plugin);
	return NULL;
     }
   plugin->epsilon_generate_thumb = &epsilon_generate_thumb;

   ecore_list_append (plugin->mime_types, "application/pdf");

   return plugin;
}
