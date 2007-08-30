#include <Evas.h>
#include <Ecore_Evas.h>
#include <Epsilon.h>
#include <Epsilon_Plugin.h>
#include <Epdf.h>

Imlib_Image
epsilon_thumb_imlib_standardize ()
{
   Imlib_Image dst = NULL;
   int         dw;
   int         dh;
   int         sw;
   int         sh;
   int         s = 128;

   sw = imlib_image_get_width ();
   sh = imlib_image_get_height ();

   if (sw > sh)
     {
        dw = s;
	dh = (s * sh) / sw;
     }
   else
     {
        dh = s;
	dw = (s * sw) / sh;
     }

   imlib_context_set_cliprect (0, 0, dw, dh);

   if ((dst = imlib_create_cropped_scaled_image (0, 0, sw, sh, dw, dh)))
     {
        imlib_context_set_image (dst);
	imlib_context_set_anti_alias (1);
	imlib_image_set_has_alpha (1);
     }

   return dst;
}

int
clip (int val)
{
   if (val < 0)
     return 0;

   return (val > 255) ? 255 : val;
}

Imlib_Image
epsilon_generate_thumb (Epsilon * e)
{
   Imlib_Image    img = NULL;
   Ecore_Evas    *ee;
   Evas          *evas;
   Evas_Object   *o;
   Epdf_Document *document;
   Epdf_Page     *page;
   int            page_number;
   const int     *pixels;

   document = epdf_document_new (e->src);
   page = epdf_page_new (document, 1);
   ee = ecore_evas_buffer_new(64,64);
   evas = ecore_evas_get(ee);
   o = evas_object_image_add (evas);
   evas_object_move (o, 0, 0);
   epdf_page_render (page, o,
		     EPDF_PAGE_ORIENTATION_PORTRAIT,
		     0, 0, -1, -1, 1.0, 1.0);
   evas_object_show (o);
   ecore_evas_resize (ee, epdf_page_width_get (page), epdf_page_height_get (page));

   pixels = ecore_evas_buffer_pixels_get (ee);
   img = imlib_create_image_using_data (epdf_page_width_get (page),
					epdf_page_height_get (page),
					(DATA32 *)pixels);

   imlib_context_set_image(img);

   epdf_page_delete (page);
   epdf_document_delete (document);

   return img;
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
