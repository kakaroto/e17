#include "imlib_wrap.h"

int
imlib_wrap_image_get_width(Imlib_Image im)
{
   imlib_context_set_image(im);
   return imlib_image_get_width();
}

int
imlib_wrap_image_get_height(Imlib_Image im)
{
   imlib_context_set_image(im);
   return imlib_image_get_height();
}

int
imlib_wrap_image_has_alpha(Imlib_Image im)
{
   imlib_context_set_image(im);
   return imlib_image_has_alpha();
}

void
imlib_wrap_free_image_and_decache(Imlib_Image im)
{
   imlib_context_set_image(im);
   imlib_free_image_and_decache();
}

void
imlib_wrap_free_image(Imlib_Image im)
{
   imlib_context_set_image(im);
   imlib_free_image();
}

const char *
imlib_wrap_image_get_filename(Imlib_Image im)
{
   if (im)
   {
      imlib_context_set_image(im);
      return imlib_image_get_filename();
   }
   else
      return NULL;
}

void
imlib_wrap_render_image_on_drawable(Drawable d, Imlib_Image im, int x, int y,
                                     char dither, char blend, char alias)
{
   imlib_context_set_image(im);
   imlib_context_set_drawable(d);
   imlib_context_set_anti_alias(alias);
   imlib_context_set_dither(dither);
   imlib_context_set_blend(blend);
   imlib_context_set_angle(0);
   imlib_render_image_on_drawable(x, y);
}

void
imlib_wrap_render_image_on_drawable_at_size(Drawable d, Imlib_Image im,
                                             int x, int y, int w, int h,
                                             char dither, char blend,
                                             char alias)
{
   imlib_context_set_image(im);
   imlib_context_set_drawable(d);
   imlib_context_set_anti_alias(alias);
   imlib_context_set_dither(dither);
   imlib_context_set_blend(blend);
   imlib_context_set_angle(0);
   imlib_render_image_on_drawable_at_size(x, y, w, h);
}

void
imlib_wrap_render_image_part_on_drawable_at_size(Drawable d, Imlib_Image im,
                                                  int sx, int sy, int sw,
                                                  int sh, int dx, int dy,
                                                  int dw, int dh, char dither,
                                                  char blend, char alias)
{
   imlib_context_set_image(im);
   imlib_context_set_drawable(d);
   imlib_context_set_anti_alias(alias);
   imlib_context_set_dither(dither);
   imlib_context_set_blend(blend);
   imlib_context_set_angle(0);
   imlib_render_image_part_on_drawable_at_size(sx, sy, sw, sh, dx, dy, dw,
                                               dh);
}

void
imlib_wrap_image_fill_rectangle(Imlib_Image im, int x, int y, int w, int h,
                                 int r, int g, int b, int a)
{
   imlib_context_set_image(im);
   imlib_context_set_color(r, g, b, a);
   imlib_image_fill_rectangle(x, y, w, h);
}

void
imlib_wrap_image_fill_polygon(Imlib_Image im, ImlibPolygon poly, int r,
                               int g, int b, int a, unsigned char alias,
                               int cx, int cy, int cw, int ch)
{
   imlib_context_set_image(im);
   imlib_context_set_color(r, g, b, a);
   imlib_context_set_anti_alias(alias);
   imlib_context_set_cliprect(cx, cy, cw, ch);
   imlib_image_fill_polygon(poly);
   imlib_context_set_cliprect(0, 0, 0, 0);
}

void
imlib_wrap_image_draw_polygon(Imlib_Image im, ImlibPolygon poly, int r,
                               int g, int b, int a, unsigned char closed,
                               unsigned char alias, int cx, int cy, int cw,
                               int ch)
{
   imlib_context_set_image(im);
   imlib_context_set_color(r, g, b, a);
   imlib_context_set_anti_alias(alias);
   imlib_context_set_cliprect(cx, cy, cw, ch);
   imlib_image_draw_polygon(poly, closed);
   imlib_context_set_cliprect(0, 0, 0, 0);
}


void
imlib_wrap_image_draw_rectangle(Imlib_Image im, int x, int y, int w, int h,
                                 int r, int g, int b, int a)
{
   imlib_context_set_image(im);
   imlib_context_set_color(r, g, b, a);
   imlib_image_draw_rectangle(x, y, w, h);
}


void
imlib_wrap_text_draw(Imlib_Image im, Imlib_Font fn, font_style * s, int x,
                      int y, char *text, Imlib_Text_Direction dir, int r,
                      int g, int b, int a)
{
   imlib_context_set_image(im);
   imlib_context_set_font(fn);
   imlib_context_set_direction(dir);
   if (s)
   {
      int min_x = 0, min_y = 0;
      font_style_bit *bb;
      geist_list *l;

      /* here we shift the draw to accomodate bits with negative offsets,
       * which would be drawn at negative coords otherwise */
      l = s->bits;
      while (l)
      {
         bb = (font_style_bit *) l->data;
         if (bb)
         {
            if (bb->x_offset < min_x)
               min_x = bb->x_offset;
            if (bb->y_offset < min_y)
               min_y = bb->y_offset;
         }
         l = l->next;
      }
      x -= min_x;
      y -= min_y;

      /* Now draw the bits */
      l = s->bits;
      while (l)
      {
         bb = (font_style_bit *) l->data;
         if (bb)
         {
            if((bb->r + bb->g + bb->b + bb->a) == 0)
               imlib_context_set_color(r,g,b,a);
            else
            imlib_context_set_color(bb->r, bb->g, bb->b, bb->a);
            imlib_text_draw(x +bb->x_offset, y + bb->y_offset, text);
         }
         l = l->next;
      }
   }
   else
   {
      imlib_context_set_color(r, g, b, a);
      imlib_text_draw(x, y, text);
   }
}

char **
imlib_wrap_list_fonts(int *num)
{
   return imlib_list_fonts(num);
}


void
imlib_wrap_get_text_size(Imlib_Font fn, char *text, font_style * s, int *w,
                          int *h, Imlib_Text_Direction dir)
{

   imlib_context_set_font(fn);
   imlib_context_set_direction(dir);
   imlib_get_text_size(text, w, h);
   if (s)
   {
      font_style_bit *b;
      int max_x_off = 0, min_x_off = 0, max_y_off = 0, min_y_off = 0;
      geist_list *l;

      l = s->bits;
      while (l)
      {
         b = (font_style_bit *) l->data;
         if (b)
         {
            if (b->x_offset > max_x_off)
               max_x_off = b->x_offset;
            else if (b->x_offset < min_x_off)
               min_x_off = b->x_offset;
            if (b->y_offset > max_y_off)
               max_y_off = b->y_offset;
            else if (b->y_offset < min_y_off)
               min_y_off = b->y_offset;
         }
         l = l->next;
      }
      if (h)
      {
         *h += max_y_off;
         *h -= min_y_off;
      }
      if (w)
      {
         *w += max_x_off;
         *w -= min_x_off;
      }
   }
}

Imlib_Image imlib_wrap_clone_image(Imlib_Image im)
{
   imlib_context_set_image(im);
   return imlib_clone_image();
}

char *
imlib_wrap_image_format(Imlib_Image im)
{
   imlib_context_set_image(im);
   return imlib_image_format();
}

void
imlib_wrap_blend_image_onto_image(Imlib_Image dest_image,
                                   Imlib_Image source_image, char merge_alpha,
                                   int sx, int sy, int sw, int sh, int dx,
                                   int dy, int dw, int dh, char dither,
                                   char blend, char alias)
{
   imlib_context_set_image(dest_image);
   imlib_context_set_anti_alias(alias);
   imlib_context_set_dither(dither);
   imlib_context_set_blend(blend);
   imlib_context_set_angle(0);
   imlib_blend_image_onto_image(source_image, merge_alpha, sx, sy, sw, sh, dx,
                                dy, dw, dh);
}

void
imlib_wrap_blend_image_onto_image_with_rotation(Imlib_Image dest_image,
                                                 Imlib_Image source_image,
                                                 char merge_alpha, int sx,
                                                 int sy, int sw, int sh,
                                                 int dx, int dy, int dw,
                                                 int dh, double angle,
                                                 char dither, char blend,
                                                 char alias)
{
   imlib_context_set_image(dest_image);
   imlib_context_set_anti_alias(alias);
   imlib_context_set_dither(dither);
   imlib_context_set_blend(blend);
   imlib_context_set_angle(angle);
   imlib_blend_image_onto_image_at_angle(source_image, merge_alpha, sx, sy,
                                         sw, sh, dx, dy, (int) angle,
                                         (int) angle);
   return;
   dw = 0;
   dh = 0;
}

Imlib_Image imlib_wrap_create_cropped_scaled_image(Imlib_Image im, int sx,
                                                    int sy, int sw, int sh,
                                                    int dw, int dh,
                                                    char alias)
{
   imlib_context_set_image(im);
   imlib_context_set_anti_alias(alias);
   return imlib_create_cropped_scaled_image(sx, sy, sw, sh, dw, dh);
}

void
imlib_wrap_apply_color_modifier_to_rectangle(Imlib_Image im, int x, int y,
                                              int w, int h, DATA8 * rtab,
                                              DATA8 * gtab, DATA8 * btab,
                                              DATA8 * atab)
{
   Imlib_Color_Modifier cm;

   imlib_context_set_image(im);
   cm = imlib_create_color_modifier();
   imlib_context_set_color_modifier(cm);
   imlib_set_color_modifier_tables(rtab, gtab, btab, atab);
   imlib_apply_color_modifier_to_rectangle(x, y, w, h);
   imlib_free_color_modifier();
}

void
imlib_wrap_image_set_has_alpha(Imlib_Image im, int alpha)
{
   imlib_context_set_image(im);
   imlib_image_set_has_alpha(alpha);
}

void
imlib_wrap_save_image(Imlib_Image im, char *file)
{
   char *tmp;
   imlib_context_set_image(im);
   tmp = strrchr(file, '.');
   if (tmp)
      imlib_image_set_format(tmp + 1);
   imlib_save_image(file);
}

void
imlib_wrap_free_font(Imlib_Font fn)
{
   imlib_context_set_font(fn);
   imlib_free_font();
}

void
imlib_wrap_image_draw_line(Imlib_Image im, int x1, int y1, int x2, int y2,
                            char make_updates, int r, int g, int b, int a)
{
   imlib_context_set_image(im);
   imlib_context_set_color(r, g, b, a);
   imlib_image_draw_line(x1, y1, x2, y2, make_updates);
}

Imlib_Image imlib_wrap_create_rotated_image(Imlib_Image im, double angle)
{
   imlib_context_set_image(im);
   return (imlib_create_rotated_image(angle));
}

void
imlib_wrap_image_tile(Imlib_Image im)
{
   imlib_context_set_image(im);
   imlib_image_tile();
}

void
imlib_wrap_image_blur(Imlib_Image im, int radius)
{
   imlib_context_set_image(im);
   imlib_image_blur(radius);
}

void
imlib_wrap_image_sharpen(Imlib_Image im, int radius)
{
   imlib_context_set_image(im);
   imlib_image_sharpen(radius);
}

void
imlib_wrap_line_clip_and_draw(Imlib_Image dest, int x0, int y0, int x1,
                               int y1, int cx, int cy, int cw, int ch, int r,
                               int g, int b, int a)
{
   imlib_context_set_cliprect(cx, cy, cw, ch);
   imlib_wrap_image_draw_line(dest, x0, y0, x1, y1, 0, r, g, b, a);
   imlib_context_set_cliprect(0, 0, 0, 0);
}

font_style *
font_style_new(char *name)
{
   font_style *s = NULL;

   s = malloc(sizeof(font_style));

   memset(s, 0, sizeof(font_style));
   if (name)
      s->name = strdup(name);

   return s;
}

void
font_style_free(font_style * s)
{
   if (s)
   {
      if (s->name)
         free(s->name);
      if (s->bits)
      {
         geist_list *l;

         l = s->bits;
         while (l)
         {
            font_style_bit_free((font_style_bit *) l->data);
            l = l->next;
         }
         geist_list_free(s->bits);
      }
      free(s);
   }
}

font_style_bit *
font_style_bit_new(int x_offset, int y_offset, int r, int g, int b, int a)
{
   font_style_bit *sb;

   sb = malloc(sizeof(font_style_bit));
   memset(sb, 0, sizeof(font_style_bit));

   sb->x_offset = x_offset;
   sb->y_offset = y_offset;
   sb->r = r;
   sb->g = g;
   sb->b = b;
   sb->a = a;

    return sb;
}

void
font_style_bit_free(font_style_bit * s)
{
   if (s)
      free(s);
}

font_style *
font_style_new_from_ascii(char *file)
{
   FILE *stylefile;
   char current[4096];
   char *s;
   font_style *ret = NULL;

   stylefile = fopen(file, "r");
   if (stylefile)
   {
      int r = 0, g = 0, b = 0, a = 0, x_off = 0, y_off = 0;

      ret = font_style_new(NULL);
      /* skip initial idenifier line */
      fgets(current, sizeof(current), stylefile);
      while (fgets(current, sizeof(current), stylefile))
      {
         if (!strncmp(current, "#NAME", 5))
         {
            int l;

            l = strlen(current) - 1;
            if (current[l] == '\n')
               current[l] = '\0';
            if (l > 6)
               ret->name = strdup(current + 6);
            continue;
         }
         else
         {
            /* support EFM style bits */
            s = strtok(current, " ");
            if (strlen(s) == 2)
            {
               if (!strcmp(s, "ol"))
               {
                  r = g = b = 0;
                  s = strtok(NULL, " ");
                  x_off = atoi(s);
                  s = strtok(NULL, " ");
                  y_off = atoi(s);
               }
               else if (!strcmp(s, "sh"))
               {
                  r = g = b = 0;
                  s = strtok(NULL, " ");
                  x_off = atoi(s);
                  s = strtok(NULL, " ");
                  y_off = atoi(s);
                  s = strtok(NULL, " ");
                  a = atoi(s);
               }
               else if (!strcmp(s, "fg"))
               {
                  r = g = b = a = 0;
                  s = strtok(NULL, " ");
                  x_off = atoi(s);
                  s = strtok(NULL, " ");
                  y_off = atoi(s);
               }
            }
            else
            {
               /* our own format */
               r = atoi(s);
               s = strtok(NULL, " ");
               g = atoi(s);
               s = strtok(NULL, " ");
               b = atoi(s);
               s = strtok(NULL, " ");
               a = atoi(s);
               s = strtok(NULL, " ");
               x_off = atoi(s);
               s = strtok(NULL, " ");
               y_off = atoi(s);
            }
         }
         ret->bits =
            geist_list_add_end(ret->bits,
                               font_style_bit_new(x_off, y_off, r, g, b, a));
      }
      fclose(stylefile);
   }

   return ret;
}
