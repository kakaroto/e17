/* main.c

Copyright (C) 1999,2000 Tom Gilbert.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies of the Software and its documentation and acknowledgment shall be
given in the documentation and software packages that this Software was
used.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#include "gozer.h"

static gib_list *gozer_calculate_lines(Imlib_Font fn, gib_style *style, 
      char *text, int wrap_width);

Imlib_Image
gozer_render(gozeroptions *opt)
{
   Imlib_Font fn;
   Imlib_Image image, bg_image = NULL;
   gib_list *lines;
   const gib_list *l;
   int w = 0, h = 0, ww, hh, x = 0, y = 0, bgw = 0, bgh = 0;
   DATA8 atab[256];
   gib_style *style = NULL;
   char *p, *pp;

   fn = imlib_load_font(opt->font);
   if (!fn) {
      weprintf("failed to load font %s", opt->font);
      return NULL;
   }

   if (opt->stylefile)
   {
      style = gib_style_new_from_ascii(opt->stylefile);
      if (!style)
         weprintf("failed to load style %s", opt->stylefile);
   }

   if (opt->bg_image)
   {
      if (gib_imlib_load_image(&bg_image, opt->bg_image))
      {
         bgw = gib_imlib_image_get_width(bg_image);
         bgh = gib_imlib_image_get_height(bg_image);
      }
      else
      {
         weprintf("failed to load background image \"%s\"", opt->bg_image);
      }
   }

   lines = gozer_calculate_lines(fn, style, opt->text, opt->wrap_width);

   l = lines;
   while (l)
   {
      p = (char *) l->data;
      gib_imlib_get_text_size(fn, p, style, &ww, &hh, IMLIB_TEXT_TO_RIGHT);
      if (ww > w)
         w = ww;
      h += hh;
      if (l->next)
         h += opt->line_spacing;
      l = l->next;
   }

   /* If we have an offset, increase the size */
   if (opt->x)
      w += opt->x;
   if (opt->y)
      h += opt->y;

   if (bg_image && opt->bg_resize)
      image = imlib_create_image(bgw, bgh);
   else
      image = imlib_create_image(w, h);

   if (!image) {
      weprintf("couldn't create imlib image for text area.");
      return NULL;
   }

   /* make image transparent (HACK - imlib2 should do this nicely) */
   gib_imlib_image_set_has_alpha(image, 1);
   memset(atab, 0, sizeof(atab));
   gib_imlib_apply_color_modifier_to_rectangle(image, 0, 0, w, h, NULL, NULL,
                                               NULL, atab);

   gib_imlib_image_fill_rectangle(image, 0, 0, w, h, opt->bg_r, opt->bg_g,
                                  opt->bg_b, opt->bg_a);
   if (bg_image)
   {
      if (opt->bg_scale)
      {
         gib_imlib_blend_image_onto_image(image, bg_image, 1, 0, 0, bgw, bgh,
                                          0, 0, w, h, 0, 1, 1);
      }
      else if (opt->bg_tile)
      {
         gib_imlib_image_tile(bg_image);
         gib_imlib_blend_image_onto_image(image, bg_image, 1, 0, 0, bgw, bgh,
                                          0, 0, w, h, 0, 1, 1);
      }
      else
      {
         gib_imlib_blend_image_onto_image(image, bg_image, 1, 0, 0, bgw, bgh,
                                          0, 0, bgw, bgh, 0, 1, 1);
      }
   }
   l = lines;
   x = opt->x;
   y = opt->y;
   while (l)
   {
      p = (char *) l->data;
      gib_imlib_get_text_size(fn, p, style, &ww, &hh, IMLIB_TEXT_TO_RIGHT);
      switch (opt->justification)
      {
        case JUST_LEFT:
           x = opt->x;
           gib_imlib_text_draw(image, fn, style, x, y, p, IMLIB_TEXT_TO_RIGHT,
                               opt->fn_r, opt->fn_g, opt->fn_b, opt->fn_a);
           break;
        case JUST_CENTER:
           x = ((((opt->bg_resize && bgw) ? bgw : w) - ww) / 2);
           /* y = ((((opt->bg_resize && bgw) ? bgh : h) - hh) / 2); */
           gib_imlib_text_draw(image, fn, style, x, y, p, IMLIB_TEXT_TO_RIGHT,
                               opt->fn_r, opt->fn_g, opt->fn_b, opt->fn_a);
           break;
        case JUST_RIGHT:
           x = (((opt->bg_resize && bgw) ? bgw : w) - ww);
           gib_imlib_text_draw(image, fn, style, x, y, p, IMLIB_TEXT_TO_RIGHT,
                               opt->fn_r, opt->fn_g, opt->fn_b, opt->fn_a);

           break;
        case JUST_BLOCK:
        {
           const gib_list *ll;
           gib_list *words = gib_string_split(p, " ");

           if (words)
           {
              int wordcnt, word_spacing, line_w;
              int t_width, m_width, space_width, offset = 0;

              wordcnt = gib_list_length(words);
              gib_imlib_get_text_size(fn, p, style, &line_w, NULL,
                                      IMLIB_TEXT_TO_RIGHT);
              gib_imlib_get_text_size(fn, "M M", style, &t_width, NULL,
                                      IMLIB_TEXT_TO_RIGHT);
              gib_imlib_get_text_size(fn, "M", style, &m_width, NULL,
                                      IMLIB_TEXT_TO_RIGHT);
              space_width = t_width - (2 * m_width);

              if (wordcnt > 1)
                 word_spacing = (w - line_w) / (wordcnt - 1);
              else
                 word_spacing = (w - line_w);

              ll = words;
              while (ll)
              {
                 pp = (char *) ll->data;
                 if (strcmp(pp, " "))
                 {
                    int wordw;

                    gib_imlib_text_draw(image, fn, style, x + offset, y, pp,
                                        IMLIB_TEXT_TO_RIGHT, opt->fn_r,
                                        opt->fn_g, opt->fn_b, opt->fn_a);
                    gib_imlib_get_text_size(fn, pp, style, &wordw, NULL,
                                            IMLIB_TEXT_TO_RIGHT);
                    offset += (wordw + space_width + word_spacing);
                 }
                 ll = ll->next;
              }
              gib_list_free_and_data(words);
           }
           break;
        }

        default:
           break;
      }
      y += hh + opt->line_spacing;
      l = l->next;
   }

   if (bg_image) {
      imlib_context_set_image(bg_image);
      imlib_free_image();
   }

   imlib_context_set_font(fn);
   imlib_free_font();

   gib_style_free(style);
   gib_list_free_and_data(lines);

   return image;
}

static gib_list *
gozer_calculate_lines(Imlib_Font fn, gib_style * style, 
      char *text, int wrap_width)
{
   gib_list *lines = NULL, *list = NULL;
   gib_list *l = NULL;
   char delim[2] = { '\n', '\0' };
   int w, line_width;
   int tw, th;
   char *p, *pp;
   char *line = NULL;
   char *temp;
   int space_width = 0, m_width = 0, t_width = 0, new_width = 0;

   lines = gib_string_split(text, delim);

   if (wrap_width)
   {
      gib_imlib_get_text_size(fn, "M M", style, &t_width, NULL,
                              IMLIB_TEXT_TO_RIGHT);
      gib_imlib_get_text_size(fn, "M", style, &m_width, NULL,
                              IMLIB_TEXT_TO_RIGHT);
      space_width = t_width - (2 * m_width);
      w = wrap_width;
      l = lines;
      while (l)
      {
         line_width = 0;
         p = (char *) l->data;
         /* quick check to see if whole line fits okay */
         gib_imlib_get_text_size(fn, p, style, &tw, &th, IMLIB_TEXT_TO_RIGHT);
         if (tw <= w)
            list = gib_list_add_end(list, estrdup(p));
         else if (strlen(p) == 0)
            list = gib_list_add_end(list, estrdup(""));
         else if (!strcmp(p, " "))
            list = gib_list_add_end(list, estrdup(" "));
         else
         {
            const gib_list *ll;
            gib_list *words = gib_string_split(p, " ");

            if (words)
            {
               ll = words;
               while (ll)
               {
                  pp = (char *) ll->data;
                  if (strcmp(pp, " "))
                  {
                     gib_imlib_get_text_size(fn, pp, style, &tw, &th,
                                             IMLIB_TEXT_TO_RIGHT);
                     if (line_width == 0)
                        new_width = tw;
                     else
                        new_width = line_width + space_width + tw;
                     if (new_width <= w)
                     {
                        /* add word to line */
                        if (line)
                        {
                           int len;

                           len = strlen(line) + strlen(pp) + 2;
                           temp = emalloc(len);
                           snprintf(temp, len, "%s %s", line, pp);
                           free(line);
                           line = temp;
                        }
                        else
                           line = estrdup(pp);
                        line_width = new_width;
                     }
                     else if (line_width == 0)
                     {
                        /* can't fit single word in :/
                           increase width limit to width of word and jam the bastard
                           in anyhow */
                        w = tw;
                        line = estrdup(pp);
                        line_width = new_width;
                     }
                     else
                     {
                        /* finish this line, start next and add word there */
                        if (line)
                        {
                           list = gib_list_add_end(list, estrdup(line));
                           free(line);
                           line = NULL;
                        }
                        line = estrdup(pp);
                        line_width = tw;
                     }
                  }
                  ll = ll->next;
               }
               if (line)
               {
                  /* finish last line */
                  list = gib_list_add_end(list, estrdup(line));
                  free(line);
                  line = NULL;
                  line_width = 0;
               }
               gib_list_free_and_data(words);
            }
         }
         l = l->next;
      }
      gib_list_free_and_data(lines);
      lines = list;
   }

   return lines;
}
