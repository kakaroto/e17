#include "util.h"
#include "callbacks.h"
#include "gtk_util.h"
#include <gdk/gdkx.h>
#include "interface.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define EBONY_IMAGE_CACHE 4
#define EBONY_FONT_CACHE 0

/** Parse the ebony previously modified bg dbs 
 * Return a GList 
 */
GList *
parse_ebony_bg_list_db(void)
{
   char buf[PATH_MAX], dbname[PATH_MAX];
   E_DB_File *db;
   GList *result = NULL;

   snprintf(dbname, PATH_MAX, "%s/.ebony.db", getenv("HOME"));

   db = e_db_open(dbname);
   if (db)
   {
      int count, i;
      char *s;

      if (e_db_int_get(db, "/bg/count", &count))
      {
         for (i = 0; (i < count) && (i < MAX_RECENT_BG_COUNT); i++)
         {
            snprintf(buf, PATH_MAX, "/bg/%d/filename", i);
            s = e_db_str_get(db, buf);
            result = g_list_append(result, s);
         }
      }
   }
   e_db_close(db);
   return (result);
}

/* write_ebony_bg_list_db - Write the GList to the db clearing out old
 * entries
 * @l - a Glist from the bg_list gtk_list
 * This DESTROYS the list it's passed.
 */
void
write_ebony_bg_list_db(GList * l)
{
   char buf[PATH_MAX], dbname[PATH_MAX];
   E_DB_File *db;

   snprintf(dbname, PATH_MAX, "%s/.ebony.db", getenv("HOME"));
   db = e_db_open(dbname);
   if (db)
   {
      int i, key_count = 0;
      char **keys;

      keys = e_db_dump_key_list(dbname, &key_count);
      if (keys)
      {
         for (i = 0; i < key_count; i++)
         {
            e_db_data_del(db, keys[i]);
            free(keys[i]);
         }
         free(keys);
      }

      if (l)
      {
         GList *ll = NULL;

         for (ll = l, i = 0; ll && i < MAX_RECENT_BG_COUNT;
              ll = ll->next, i++)
         {
            snprintf(buf, PATH_MAX, "/bg/%d/filename", i);
            e_db_str_set(db, buf, (char *) ll->data);
            if (ll->data)
               free((char *) ll->data);
         }
         e_db_int_set(db, "/bg/count", i);
      }
      e_db_close(db);
      e_db_flush();
      g_list_free(l);
   }
}

char *
get_dirpath_from_filename(const char *filename)
{
   char *result, *str;
   int i;

   result = strdup(filename);
   i = strlen(filename);
   while (i--)
   {
      if (result[i] == '/')
         break;
   }
   result[i] = '\0';
   str = strdup(result);
   result[i] = 'e';

   free(result);
   return (str);
}

/**
 * get_shortname_for - chop the absolute path off of a bg filename
 * @filename the file to chop
 * Return the shortened filename, caller should free this
 */
char *
get_shortname_for(const char *filename)
{
   char *tmp, *result = NULL;
   int length, i;

   tmp = strdup(filename);
   length = strlen(filename);

   /* start from the back, find the first slash */
   for (i = length; tmp[i] != '/'; i--);
   /* chop the slash */
   i++;

   result = strdup(&tmp[i]);

   free(tmp);
   return (result);
}

/**
 * update_background - e_bg_resize on the image, and allows layers to swap
 * @_bg - the bg to update
 */
void
update_background(E_Background _bg)
{
   int w, h;

   if (!_bg)
      return;
   w = _bg->geom.w;
   h = _bg->geom.h;
   _bg->geom.w = 0;
   _bg->geom.h = 0;
   e_bg_set_scale(_bg, export_info.screen.w * export_info.xinerama.h,
                  export_info.screen.h * export_info.xinerama.v);
   e_bg_set_layer(_bg, 0);
   if ((bl) && (bl->obj))
      outline_evas_object(bl->obj);
   DRAW();
}

/**
 * move_layer_up - move the currently selected bl "up" one layer
 * @_bl - the background layer to move up
 * Return 1 on success, 0 on failure
 */
int
move_layer_up(E_Background_Layer _bl)
{
   Evas_List *l, *ll;

   if (!bg)
      return (0);
   if (!_bl)
      return (0);
   for (l = bg->layers; l && l->data != _bl; l = l->next);
   if (!l)
      return (0);               /* not in the list */
   ll = l->next;
   if (!ll)
      return (0);               /* top layer */
   bg->layers = evas_list_remove(bg->layers, _bl);
   bg->layers = evas_list_append_relative(bg->layers, _bl, ll->data);
   display_layer_values(_bl);
   update_background(bg);
   return (1);
}

/**
 * move_layer_down - move the currently selected bl "down" one layer
 * @_bl - the background layer to move down
 * Return 1 on success, 0 on failure
 */
int
move_layer_down(E_Background_Layer _bl)
{
   Evas_List *l, *ll;

   if (!bg)
      return (0);
   if (!_bl)
      return (0);
   for (l = bg->layers; l && l->data != _bl; l = l->next);
   if (!l)
      return (0);               /* not in the list */
   ll = l->prev;
   if (!ll)
      return (0);               /* bottom layer */
   bg->layers = evas_list_remove(bg->layers, _bl);
   bg->layers = evas_list_prepend_relative(bg->layers, _bl, ll->data);
   display_layer_values(_bl);
   update_background(bg);
   return (1);
}

/**
 * outline_evas_object - outline the evas object in the current layer
 * @_o - the object to outline
 */
void
outline_evas_object(Evas_Object * _o)
{
   double x, y, w, h;

   /* int colors[] = { 255, 255, 255, 255 }; */
   /* int colors[] = { 0, 0, 0, 90 }; */

   Evas_Object *o;

   if (!_o)
      return;
   evas_object_geometry_get(_o, &x, &y, &w, &h);
   o = evas_object_name_find(evas, "top_line");
   if (o)
   {
      evas_object_line_xy_set(o, x, y, x + w, y);
      evas_object_layer_set(o, 100);
   }
   o = evas_object_name_find(evas, "bottom_line");
   if (o)
   {
      evas_object_line_xy_set(o, x, y + h, x + w, y + h);
      evas_object_layer_set(o, 100);
   }
   o = evas_object_name_find(evas, "right_line");
   if (o)
   {
      evas_object_line_xy_set(o, x + w, y, x + w, y + h);
      evas_object_layer_set(o, 100);
   }
   o = evas_object_name_find(evas, "left_line");
   if (o)
   {
      evas_object_line_xy_set(o, x, y, x, y + h);
      evas_object_layer_set(o, 100);
   }
}

/**
 * fill_background_images - load Imlib2 Image into memory for saving
 * @_bg - the E_Background to be "filled"
 */
void
fill_background_images(E_Background _bg)
{
   Evas_List *l;
   E_Background_Layer _bl;

   if (!_bg)
      return;
   for (l = _bg->layers; l; l = l->next)
   {
      _bl = (E_Background_Layer) l->data;
      if ((_bl->type == E_BACKGROUND_TYPE_IMAGE) && (_bl->inlined))
      {
         char buf[PATH_MAX];

         snprintf(buf, PATH_MAX, "%s:%s", _bg->file, _bl->file);
         _bl->image = imlib_load_image(buf);
      }
   }
}

/**
 * clear_bg_db_keys - clears out all the keys and data in the E_Background
 * @_bg - the bg requesting all of its data to be deleted
 */
void
clear_bg_db_keys(E_Background _bg)
{
   E_DB_File *db;
   char **keys;
   int i, key_count;

   if (!_bg)
      return;
   if (!_bg->file)
      return;
   db = e_db_open(_bg->file);
   if (db)
   {
      keys = e_db_dump_key_list(_bg->file, &key_count);
      if (keys)
      {
         for (i = 0; i < key_count; i++)
         {
            e_db_data_del(db, keys[i]);
            free(keys[i]);
         }
         free(keys);
      }
      e_db_close(db);
      e_db_flush();
   }
}

void
redraw_gradient_object(void)
{
   Evas_List *l;
   E_Background_Gradient g;

   if (!bl)
      return;
   if (bl->obj)
      evas_object_del(bl->obj);

   bl->obj = evas_object_gradient_add(evas);
   for (l = bl->gradient.colors; l; l = l->next)
   {
      g = (E_Background_Gradient) l->data;
      evas_object_gradient_color_add(bl->obj, g->r, g->g, g->b, g->a,
                                     g->dist);
   }
   evas_object_gradient_angle_set(bl->obj, bl->gradient.angle);
   evas_object_show(bl->obj);
   return;
   UN(g);
   UN(l);
}

void
setup_evas(Display * disp, Window win, Visual * vis, Colormap cm, int w,
           int h)
{
   Evas_Object *o;
   Evas_Engine_Info_Software_X11 *einfo;

   int colors[] = { 255, 255, 255, 255 };

   evas = evas_new();
   evas_output_method_set(evas, evas_render_method_lookup("software_x11"));
   evas_output_size_set(evas, w, h);
   evas_output_viewport_set(evas, 0, 0, w, h);

   einfo = (Evas_Engine_Info_Software_X11 *) evas_engine_info_get(evas);
   einfo->info.drawable = win;
   einfo->info.display = disp;
   einfo->info.visual = vis;
   einfo->info.colormap = cm;
   einfo->info.depth = DefaultDepth(disp, DefaultScreen(disp));
   einfo->info.rotation = 0;
   einfo->info.debug = 0;
   evas_engine_info_set(evas, (Evas_Engine_Info *) einfo);

   evas_object_image_cache_set(evas, (1024 * 1024) * EBONY_IMAGE_CACHE);
   evas_object_font_cache_set(evas, (1024 * 1024) * EBONY_FONT_CACHE);
   evas_object_font_path_append(evas, PACKAGE_DATA_DIR "/fnt");

   ebony_base_bg = e_bg_load(PACKAGE_DATA_DIR "/pixmaps/base.bg.db");
   if (ebony_base_bg)
   {
      e_bg_add_to_evas(ebony_base_bg, evas);
      e_bg_set_layer(ebony_base_bg, -20);
      e_bg_resize(ebony_base_bg, w, h);
      e_bg_show(ebony_base_bg);
   }
   else
   {
      fprintf(stderr, "Unable to load %s\n",
              PACKAGE_DATA_DIR "/pixmaps/base.bg.db");
   }

   o = evas_object_line_add(evas);
   evas_object_name_set(o, "top_line");
   evas_object_color_set(o, colors[0], colors[1], colors[2], colors[3]);
   evas_object_show(o);

   o = evas_object_line_add(evas);
   evas_object_name_set(o, "bottom_line");
   evas_object_color_set(o, colors[0], colors[1], colors[2], colors[3]);
   evas_object_show(o);

   o = evas_object_line_add(evas);
   evas_object_name_set(o, "left_line");
   evas_object_color_set(o, colors[0], colors[1], colors[2], colors[3]);
   evas_object_show(o);

   o = evas_object_line_add(evas);
   evas_object_name_set(o, "right_line");
   evas_object_color_set(o, colors[0], colors[1], colors[2], colors[3]);
   evas_object_show(o);
}

void
rebuild_bg_ref(void)
{
   GtkWidget *w;

   if (!bg_ref)
   {
      bg_ref = create_win_bg();
      gtk_widget_show(bg_ref);
      w = gtk_object_get_data(GTK_OBJECT(bg_ref), "evas");
      if (w)
         gtk_widget_realize(w);

      /* setup the evas stuffs */
      setup_evas(GDK_WINDOW_XDISPLAY(w->window),
                 GDK_WINDOW_XWINDOW(w->window),
                 GDK_VISUAL_XVISUAL(gtk_widget_get_visual(win_ref)),
                 GDK_COLORMAP_XCOLORMAP(gtk_widget_get_colormap(win_ref)),
                 w->allocation.width, w->allocation.height);

      w = gtk_object_get_data(GTK_OBJECT(bg_ref), "xscale");
      if (w)
         gtk_signal_connect(GTK_OBJECT
                            (gtk_range_get_adjustment(GTK_RANGE(w))),
                            "value_changed",
                            GTK_SIGNAL_FUNC(on_scale_scroll_request), NULL);
      w = gtk_object_get_data(GTK_OBJECT(bg_ref), "yscale");
      if (w)
         gtk_signal_connect(GTK_OBJECT
                            (gtk_range_get_adjustment(GTK_RANGE(w))),
                            "value_changed",
                            GTK_SIGNAL_FUNC(on_scale_scroll_request), NULL);
   }
   w = gtk_object_get_data(GTK_OBJECT(bg_ref), "_ebony_statusbar");
   if (w)
      ebony_status = w;
}

/**
 * e_bg_resize_scaled
 * @_bg - the E_Background to be resized and scaled
 * @width - the width of the requested size
 * @height - the height of the requested size
 * @scale - the percentage that the background should scale to
 * I haven't tested zooming in(scale > 1.0), though it should work with 
 * a little effort.
 */
static void
e_bg_resize_scaled(E_Background _bg, int width, int height, double scale)
{
   Evas_List *l;
   int i;

   if (!_bg)
      return;
   if (!_bg->evas)
      return;

   evas_object_move(_bg->base_obj, _bg->x, _bg->y);
   evas_object_resize(_bg->base_obj, (double) width * scale,
                      (double) height * scale);
   for (i = 0, l = _bg->layers; l; l = l->next, i++)
   {
      E_Background_Layer bl;
      double x, y, w, h, fw, fh;
      int iw, ih;

      bl = (E_Background_Layer) l->data;
      iw = ih = 0;
      fw = fh = 0.0;

      x = _bg->x;
      y = _bg->y;

      w = bl->size.w * (double) width;
      h = bl->size.h * (double) height;

      /* absolutely sized object */
      if (bl->size.absolute.w)
         w = bl->size.w;
      if (bl->size.absolute.h)
         h = bl->size.h;

      /* object is an image, resize and calculate fill */
      if (bl->type == E_BACKGROUND_TYPE_IMAGE)
      {
         evas_object_image_size_get(bl->obj, &iw, &ih);
         if (bl->size.orig.w)
            w = (double) iw *bl->size.w;

         if (bl->size.orig.h)
            h = (double) ih *bl->size.h;

         x += (scale * (((double) width - w) * bl->pos.x));
         y += (scale * (((double) height - h) * bl->pos.y));

         w = (int) (scale * (double) w);
         h = (int) (scale * (double) h);

         if (bl->fill.orig.w)
            fw = (double) iw *bl->fill.w;

         else
            fw = bl->fill.w * width;


         if (bl->fill.orig.h)
            fh = (double) ih *bl->fill.h;

         else
            fh = bl->fill.h * height;

         fw = (int) (scale * (double) fw);
         fh = (int) (scale * (double) fh);
      }
      else
      {

         x += (scale * (((double) width - w) * bl->pos.x));
         y += (scale * (((double) height - h) * bl->pos.y));

         w = (int) (scale * (double) w);
         h = (int) (scale * (double) h);
      }
      /* adjust for absolute positioning */
      x += (scale * (double) bl->abs.x);
      y += (scale * (double) bl->abs.y);

      evas_object_move(bl->obj, x, y);
      evas_object_resize(bl->obj, w, h);

      switch (bl->type)
      {
        case E_BACKGROUND_TYPE_IMAGE:
           evas_object_image_fill_set(bl->obj,
                                      (double) _bg->geom.sx * bl->scroll.x,
                                      (double) _bg->geom.sy * bl->scroll.y,
                                      fw, fh);
           break;
        case E_BACKGROUND_TYPE_GRADIENT:
           /* FIXME Necessary to call again ? */
           evas_object_gradient_angle_set(bl->obj, bl->gradient.angle);
           break;
        case E_BACKGROUND_TYPE_SOLID:
           break;
        default:
      }
   }
}

/**
 * e_bg_set_scale - resize the background to the requested size
 * @_bg - the E_Background to resize
 * @width - the requested width for the bg
 * @height - the requested height for the bg
 * If width or height are 0 the bg will fill the canvas, otherwise it scales
 * the background to the closest fit and scales the objects in the canvas
 * accordingly
 */
void
e_bg_set_scale(E_Background _bg, int width, int height)
{
   int ww = 0, wh = 0;
   int ew = 0, eh = 0;
   double vx = 0.0, vy = 0.0;
   double scale = 1.0;

   if (!_bg)
      return;
   evas_output_size_get(evas, &ww, &wh);

   /* fill the evas */
   if (height <= 0 || width <= 0)
   {
      width = ew = ww;
      height = eh = wh;
      vx = vy = 0;
      export_info.screen.w = 0;
      export_info.screen.h = 0;
      export_info.xinerama.v = export_info.xinerama.h = 1;
   }
   else if ((width > ww) || (height > wh))
   {
      double scalex = 0.0, scaley = 0.0;

      scalex = ((double) ww / (double) width);
      scaley = ((double) wh / (double) height);

      if (scalex > scaley)
         scale = scaley;
      else
         scale = scalex;
      ew = scale * (double) width;
      eh = scale * (double) height;
   }
   else                         /* height < wh && width < ww */
   {
      ew = width;
      eh = height;
   }
   _bg->x = (((double) (ww - ew)) * 0.5);
   _bg->y = (((double) (wh - eh)) * 0.5);
   e_bg_move(_bg, _bg->x, _bg->y);
   e_bg_resize_scaled(_bg, width, height, scale);
   if (bl)
      outline_evas_object(bl->obj);
}

char *
filesize_as_string(char *filename)
{
   char *str = NULL;
   struct stat file;

   if (!stat(filename, &file))
   {
      char buf[PATH_MAX];
      int depth = 0;
      float remainder = 0.0;
      int bytes = (int) file.st_size;

      char *types[] = {
         "Bytes",
         "KB",
         "MB",
         "GB",
         "TB"
      };

      while (bytes > 1024)
      {
         int c;

         c = bytes % 1024;
         remainder += ((float) c / 1024.0);
         bytes = bytes / 1024;
         depth++;
      }
      remainder += (float) bytes;
      snprintf(buf, PATH_MAX, "%0.2f %s", remainder, types[depth]);
      str = strdup(buf);
   }
   else
   {
      fprintf(stderr, "Error stating %s\n", filename);

   }
   return (str);
}
