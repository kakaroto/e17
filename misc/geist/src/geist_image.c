#include "geist.h"
#include "geist_imlib.h"
#include "geist_image.h"

static gboolean 
img_load_cancel_cb(GtkWidget * widget, gpointer data);

typedef struct _cb_data cb_data;
struct _cb_data 
{
	geist_object *obj;
	GtkWidget *dialog;
};

geist_object *
geist_image_new(void)
{
   geist_image *img;

   D_ENTER(5);

   img = emalloc(sizeof(geist_image));
   geist_image_init(img);

   geist_object_set_state(GEIST_OBJECT(img), VISIBLE);

   D_RETURN(5, (geist_object *) img);
}

void
geist_image_init(geist_image * img)
{
   geist_object *obj;

   D_ENTER(5);
   memset(img, 0, sizeof(geist_image));
   obj = (geist_object *) img;
   geist_object_init(obj);
   obj->free = geist_image_free;
   obj->render = geist_image_render;
   obj->render_partial = geist_image_render_partial;
   obj->get_rendered_image = geist_image_get_rendered_image;
   obj->duplicate = geist_image_duplicate;
   obj->resize_event = geist_image_resize;
   geist_object_set_type(obj, GEIST_TYPE_IMAGE);
   obj->sizemode = SIZEMODE_ZOOM;
   obj->alignment = ALIGN_CENTER;
   obj->display_props = geist_image_display_props;

   
   D_RETURN_(5);
}

geist_object *
geist_image_new_from_file(int x, int y, char *filename)
{
   geist_image *img;
   geist_object *obj;
   char *txt;

   D_ENTER(5);

   obj = geist_image_new();
   img = (geist_image *) obj;

   if (!(geist_image_load_file(img, filename)))
   {
      geist_image_free(obj);
      D_RETURN(5, NULL);
   }

   img->filename = estrdup(filename);

   efree(obj->name);
   if ((txt = strrchr(img->filename, '/') + 1) != NULL)
      obj->name = estrdup(txt);
   else
      obj->name = estrdup(txt);

   obj->x = x;
   obj->y = y;
   obj->rendered_x = 0;
   obj->rendered_y = 0;

   D_RETURN(5, (geist_object *) img);
}

void
geist_image_free(geist_object * obj)
{
   geist_image *img;

   D_ENTER(5);

   img = (geist_image *) obj;

   if (!img)
      D_RETURN_(5);

   if (img->filename)
      efree(img->filename);
   if (img->im)
      geist_imlib_free_image(img->im);

   efree(img);

   D_RETURN_(5);
}

void
geist_image_render(geist_object * obj, Imlib_Image dest)
{
   geist_image *im;

   D_ENTER(5);

   if (!geist_object_get_state(obj, VISIBLE))
      D_RETURN_(5);

   im = (geist_image *) obj;
   if (!im->im)
      D_RETURN_(5);


   /*
   dw = geist_imlib_image_get_width(dest);
   dh = geist_imlib_image_get_height(dest);
   sw = geist_imlib_image_get_width(im->im);
   sh = geist_imlib_image_get_height(im->im);

   D(3, ("Rendering image %p with filename %s\n", obj, im->filename));
   geist_imlib_blend_image_onto_image(dest, im->im, 0, 0, 0, sw, sh, obj->x,
                                      obj->y, sw, sh, 1,
                                      geist_imlib_image_has_alpha(im->im),
                                      obj->alias);
    */

   /* just render to the full size of the object */
   geist_image_render_partial(obj, dest, obj->x, obj->y, obj->w, obj->h);
   D_RETURN_(5);
}

void
geist_image_render_partial(geist_object * obj, Imlib_Image dest, int x, int y,
                           int w, int h)
{
   geist_image *im;
   int sw, sh, dw, dh, sx, sy, dx, dy;

   D_ENTER(5);

   if (!geist_object_get_state(obj, VISIBLE))
      D_RETURN_(5);

   im = GEIST_IMAGE(obj);
   if (!im->im)
      D_RETURN_(5);

   if (obj->rendered_x < 0)
      sx = x - obj->x;
   else
      sx = x - (obj->x + obj->rendered_x);
   if (obj->rendered_y < 0)
      sy = y - obj->y;
   else
      sy = y - (obj->y + obj->rendered_y);

   if (sx < 0)
      sx = 0;
   if (sy < 0)
      sy = 0;

   if (obj->rendered_w > obj->w)
      sw = obj->w - sx;
   else
      sw = obj->rendered_w - sx;

   if (obj->rendered_h > obj->h)
      sh = obj->h - sy;
   else
      sh = obj->rendered_h - sy;

   if (sw > w)
      sw = w;
   if (sh > h)
      sh = h;

   if (obj->rendered_x < 0)
      dx = obj->x + sx;
   else
      dx = (obj->x + obj->rendered_x) + sx;
   if (obj->rendered_y < 0)
      dy = obj->y + sy;
   else
      dy = (obj->y + obj->rendered_y) + sy;
   dw = sw;
   dh = sh;

   D(5,
     ("Rendering image from:\nx: %d y: %d\nobj->x: %d obj->y %d\narea:\nsx: %d sy: %d\nsw: %d sh: %d\ndx: %d dy: %d\ndw: %d dh: %d\n",
      x, y, obj->x, obj->y, sx, sy, sw, sh, dx, dy, dw, dh));

   D(3, ("Rendering partial image %s\n", im->filename));
   geist_imlib_blend_image_onto_image(dest, im->im, 0, sx, sy, sw, sh, dx, dy,
                                      dw, dh, 1,
                                      geist_imlib_image_has_alpha(im->im),
                                      obj->alias);

   D_RETURN_(5);
}


int
geist_image_load_file(geist_image * img, char *filename)
{
   geist_object *obj;
   int ret;

   D_ENTER(5);

   if (img->im)
      geist_imlib_free_image(img->im);

   ret = geist_imlib_load_image(&img->im, filename);

   if (ret)
   {
      obj = (geist_object *) img;

      obj->w = obj->rendered_w = geist_imlib_image_get_width(img->im);
      obj->h = obj->rendered_h = geist_imlib_image_get_height(img->im);
   }

   D_RETURN(5, ret);
}

Imlib_Image
geist_image_get_rendered_image(geist_object * obj)
{
   D_ENTER(3);

   D_RETURN(3, GEIST_IMAGE(obj)->im);
}

geist_object *
geist_image_duplicate(geist_object * obj)
{
   geist_object *ret;
   geist_image *img;

   D_ENTER(3);

   img = GEIST_IMAGE(obj);

   ret = geist_image_new_from_file(obj->x, obj->y, img->filename);
   ret->rendered_x = obj->rendered_x;
   ret->rendered_y = obj->rendered_y;
   ret->h = obj->h;
   ret->w = obj->w;
   if (ret)
   {
      ret->state = obj->state;
      ret->alias = obj->alias;
      ret->name =
         g_strjoin(" ", "Copy of", obj->name ? obj->name : "Untitled object",
                   NULL);
   }

   D_RETURN(3, ret);
}

void
geist_image_resize(geist_object * obj, int x, int y)
{
   geist_image *img;

   D_ENTER(5);

   img = GEIST_IMAGE(obj);

   D(5, ("resize to %d,%d\n", x, y));
   geist_object_resize_object(obj, x, y);

   D_RETURN_(5);
}


static gboolean
img_load_cb(GtkWidget * widget, gpointer data)
{
   geist_object *obj = ((cb_data*) data)->obj;
   char *path;

   D_ENTER(3);

   path =
      gtk_file_selection_get_filename
		   (GTK_FILE_SELECTION(((cb_data*)data)->dialog));
   
   if (path)
   {  
	  geist_object_dirty(obj);
	  geist_image_load_file(GEIST_IMAGE(obj),path);
      geist_document_render_updates(GEIST_OBJECT_DOC(obj));
      geist_object_dirty(obj);
   }
   gtk_widget_destroy((GtkWidget *) ((cb_data*)data)->dialog);
   efree(data);
   D_RETURN(3, TRUE);
}

static gboolean 
img_load_cancel_cb(GtkWidget * widget, gpointer data)
{
   gtk_widget_destroy((GtkWidget *) data);
   return TRUE;
}



gboolean
geist_image_select_file_cb(GtkWidget *widget, gpointer *data)
{
	cb_data *sel_cb_data = NULL;
	geist_object *obj = GEIST_OBJECT(data);
	GtkWidget *file_sel = gtk_file_selection_new("Select an Image");

	sel_cb_data = emalloc(sizeof(cb_data));
	sel_cb_data->obj = obj;
	sel_cb_data->dialog = (gpointer)file_sel;
				
   gtk_file_selection_show_fileop_buttons(GTK_FILE_SELECTION(file_sel));
   gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(file_sel)->ok_button),
                      "clicked", GTK_SIGNAL_FUNC(img_load_cb),
                      (gpointer) sel_cb_data);
   gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(file_sel)->cancel_button),
                      "clicked", GTK_SIGNAL_FUNC(img_load_cancel_cb),
                      (gpointer) file_sel);
   gtk_widget_show(file_sel);
   return TRUE;
}


GtkWidget *
geist_image_display_props(geist_object *obj)
{
  char *buf;
  GtkWidget *image_props;
  GtkWidget *table;
  GtkWidget *sel_file_btn;
  GtkWidget *file_entry;
  GtkWidget *antialias_checkb;
  
  image_props = gtk_hbox_new(FALSE, 0);
  
  table = gtk_table_new ( 4, 2, FALSE);
  gtk_widget_show (table);
  gtk_container_add (GTK_CONTAINER (image_props), table);
  gtk_container_set_border_width (GTK_CONTAINER (image_props), 5);
  
  sel_file_btn = gtk_button_new_with_label ("Change file");
  gtk_table_attach(GTK_TABLE(table), sel_file_btn, 0, 1, 1 , 2, GTK_FILL | GTK_EXPAND, 0, 2, 2); 
  gtk_container_set_border_width (GTK_CONTAINER (sel_file_btn), 10);
  gtk_widget_show (sel_file_btn);

  file_entry = gtk_entry_new ();
  gtk_table_attach(GTK_TABLE(table), file_entry, 1, 2, 1, 2, GTK_FILL | GTK_EXPAND, 0, 2, 2);  
  gtk_widget_show (file_entry);


  antialias_checkb = gtk_check_button_new_with_label ("antialias");
  gtk_table_attach(GTK_TABLE(table), antialias_checkb, 0, 1, 4, 5, GTK_FILL | GTK_EXPAND, 0, 2, 2);
  gtk_widget_show (antialias_checkb);
  gtk_container_set_border_width (GTK_CONTAINER (antialias_checkb), 10);

  
  buf = (char *) geist_imlib_image_get_filename (geist_object_get_rendered_image(obj));
  gtk_entry_set_text (GTK_ENTRY(file_entry), buf);
  

  gtk_signal_connect(GTK_OBJECT(sel_file_btn), "clicked",
		  			 GTK_SIGNAL_FUNC(geist_image_select_file_cb), (gpointer) obj);
  
  
  return(image_props);
}
