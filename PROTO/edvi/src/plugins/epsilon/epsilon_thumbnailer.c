#include <stdlib.h>
#include <string.h>

#include <Ecore_Evas.h>
#include <Epsilon.h>
#include <Epsilon_Plugin.h>
#include <Edvi.h>

Epsilon_Image *
epsilon_generate_thumb (Epsilon * e)
{
   Epsilon_Image *dst = NULL;
   Ecore_Evas    *ee;
   Evas          *evas;
   Evas_Object   *o;
   Edvi_Device   *device;
   Edvi_Property *property;
   Edvi_Document *document;
   Edvi_Page     *page;
   int            width;
   int            height;
   const int     *pixels;
   char          *param_kpathsea_mode  = "cx";

   if (!edvi_init (300, param_kpathsea_mode, 4,
                   1.0, 1.0,
                   0, 255, 255, 255, 0, 0, 0))
    return NULL;

   device = edvi_device_new (edvi_dpi_get (), edvi_dpi_get ());
   if (!device)
     goto no_device;

   property = edvi_property_new ();
   if (!property)
     goto no_property;

   edvi_property_property_set (property, EDVI_PROPERTY_DELAYED_FONT_OPEN);

   document = edvi_document_new (e->src, device, property);
   if (!document)
     goto no_document;
   page = edvi_page_new (document);
   if (!page)
     goto no_page;
   edvi_page_page_set (page, 0);
   edvi_page_size_get (page, &width, &height);

   ee = ecore_evas_buffer_new(width, height);
   evas = ecore_evas_get(ee);

   o = evas_object_image_add (evas);
   evas_object_move (o, 0, 0);
   edvi_page_render (page, device, o);
   evas_object_show (o);

   dst = calloc(1, sizeof(Epsilon_Image));
   if (!dst)
     goto no_dst;

   dst->w = width;
   dst->h = height;
   dst->alpha = 1;
   dst->data = malloc(dst->w * dst->h * sizeof(int));
   if (!dst->data)
     goto no_dst_data;

   pixels = ecore_evas_buffer_pixels_get (ee);
   memcpy(dst->data, pixels, dst->w * dst->h * sizeof(int));

   edvi_page_delete (page);
   edvi_document_delete (document);
   edvi_property_delete (property);
   edvi_device_delete (device);
   edvi_shutdown ();

   ecore_evas_free(ee);
   edvi_page_delete (page);
   edvi_document_delete (document);
   edvi_property_delete (property);
   edvi_device_delete (device);
   edvi_shutdown ();

   return dst;

 no_dst_data:
   free(dst);
 no_dst:
   edvi_page_delete (page);
 no_page:
   edvi_document_delete (document);
 no_document:
   edvi_property_delete (property);
 no_property:
   edvi_device_delete (device);
 no_device:
   edvi_shutdown ();
   return NULL;
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

   ecore_list_append (plugin->mime_types, "application/dvi");

   return plugin;
}
