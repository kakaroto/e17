#include "geist_document.h"

geist_document *
geist_document_new(int w, int h)
{
   geist_document *d = NULL;

   D_ENTER(3);

   d = emalloc(sizeof(geist_document));

   memset(d, 0, sizeof(geist_document));

   d->w = w;
   d->h = h;

   d->im = imlib_create_image(w, h);
   d->pmap = XCreatePixmap(disp, root, w, h, depth);

   D_RETURN(3, d);
}

void
geist_document_free(geist_document * document)
{
   geist_list *l;

   D_ENTER(3);

   for (l = document->layers; l; l = l->next)
      geist_layer_free((geist_layer *) l->data);

   geist_list_free(document->layers);

   if (document->im)
      geist_imlib_free_image(document->im);
   if (document->pmap)
      XFreePixmap(disp, document->pmap);

   free(document);

   D_RETURN_(3);
}

void
geist_document_render(geist_document * document)
{
   geist_list *l;

   D_ENTER(3);

   geist_imlib_image_fill_rectangle(document->im, 0, 0, document->w,
                                    document->h, 255, 255, 255, 255);

   for (l = document->layers; l; l = l->next)
      geist_layer_render((geist_layer *) l->data, document->im);

   geist_imlib_render_image_on_drawable(document->pmap, document->im, 0, 0, 1,
                                        1, 0);

   D_RETURN_(3);
}

void
geist_document_add_object(geist_document * doc, geist_object * obj)
{
   geist_list *top;

   D_ENTER(3);

   top = geist_list_last(doc->layers);
   geist_layer_add_object(((geist_layer *) top->data), obj);

   D_RETURN_(3);
}

void
geist_document_add_layer(geist_document * doc)
{
   D_ENTER(3);

   doc->layers = geist_list_add_end(doc->layers, geist_layer_new());

   D_RETURN_(3);
}

geist_object *
geist_document_find_clicked_object(geist_document * doc, int x, int y)
{
   geist_list *l;
   geist_object *ret = NULL;
   D_ENTER(3);

   for (l = doc->layers; l; l = l->next)
   {
      ret = geist_layer_find_clicked_object((geist_layer *) l->data, x, y);
   }
   
  D_RETURN(3, ret);

}
