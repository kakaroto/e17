/* slideshow.c
 *
 * Copyright (C) 2000 Tom Gilbert
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "feh.h"
#include "filelist.h"
#include "timers.h"
#include "winwidget.h"
#include "options.h"

void
init_slideshow_mode(void)
{
   winwidget w = NULL;
   int success = 0;
   char *s = NULL;
   feh_file *file = NULL, *last = NULL;

   D_ENTER;

   for (file = filelist; file; file = file->next)
   {
      if (last)
      {
         filelist = filelist_remove_file(filelist, last);
         last = NULL;
      }
      current_file = file;
      s = slideshow_create_name(file->filename);
      if ((w = winwidget_create_from_file(file, s, WIN_TYPE_SLIDESHOW)) !=
          NULL)
      {
         free(s);
         success = 1;
         w->file = file;
         if (opt.draw_filename)
            feh_draw_filename(w);
         if (!opt.progressive)
            winwidget_show(w);
         if (opt.slideshow_delay >= 0)
            feh_add_timer(cb_slide_timer, w, opt.slideshow_delay,
                          "SLIDE_CHANGE");
         else if (opt.reload > 0)
            feh_add_unique_timer(cb_reload_timer, w, opt.reload);
         break;
      }
      else
      {
         free(s);
         last = file;
      }
   }
   if (!success)
      show_mini_usage();
   D_RETURN_;
}

void
cb_slide_timer(void *data)
{
   D_ENTER;
   slideshow_change_image((winwidget) data, SLIDE_NEXT);
   D_RETURN_;
}

void
cb_reload_timer(void *data)
{
   winwidget w = (winwidget) data;

   D_ENTER;
   feh_reload_image(w);
   feh_add_unique_timer(cb_reload_timer, w, opt.reload);
   D_RETURN_;
}

void
feh_reload_image(winwidget w)
{
   D_ENTER;

   winwidget_free_image(w);

   if (opt.progressive)
   {
      /* Yeah, we have to do this stuff for progressive loading, so the
         callback knows it's got to create a new image... */
      progwin = w;
      imlib_context_set_progress_function(progressive_load_cb);
      imlib_context_set_progress_granularity(PROGRESS_GRANULARITY);
   }
   if ((feh_load_image(&(w->im), w->file)) != 0)
   {
      w->zoom_mode = 0;
      w->zoom = 0.0;
      if (!opt.progressive)
      {
         imlib_context_set_image(w->im);
         w->im_w = imlib_image_get_width();
         w->im_h = imlib_image_get_height();
         winwidget_render_image(w, 0);
      }
      if (opt.draw_filename)
         feh_draw_filename(w);
   }
   else
      eprintf("Couldn't reload image. Is it still there?");

   D_RETURN_;
}


void
slideshow_change_image(winwidget winwid, int change)
{
   int success = 0;
   feh_file *last = NULL;
   int i = 0, file_num = 0;
   int jmp = 1;

   D_ENTER;

   file_num = filelist_length(filelist);

   /* Without this, clicking a one-image slideshow reloads it. Not very *
      intelligent behaviour :-) */
   if (file_num < 2)
      D_RETURN_;

   /* Ok. I do this in such an odd way to ensure that if the last or first *
      image is not loadable, it will go through in the right direction to *
      find the correct one. Otherwise SLIDE_LAST would try the last file, *
      then loop forward to find a loadable one. */
   if (change == SLIDE_FIRST)
   {
      current_file = filelist_last(filelist);
      change = SLIDE_NEXT;
   }
   else if (change == SLIDE_LAST)
   {
      current_file = filelist;
      change = SLIDE_PREV;
   }

   /* The for loop prevents us looping infinitely */
   for (i = 0; i < file_num; i++)
   {
      winwidget_free_image(winwid);
      switch (change)
      {
        case SLIDE_NEXT:
           current_file = filelist_jump(filelist, current_file, FORWARD, 1);
           break;
        case SLIDE_PREV:
           current_file = filelist_jump(filelist, current_file, BACK, 1);
           break;
        case SLIDE_JUMP_FWD:
           if (file_num < 5)
              jmp = 1;
           else if (file_num < 40)
              jmp = 2;
           else
              jmp = file_num / 20;
           if (!jmp)
              jmp = 2;
           current_file = filelist_jump(filelist, current_file, FORWARD, jmp);
           /* important. if the load fails, we only want to step on ONCE to
              try the next file, not another jmp */
           change = SLIDE_NEXT;
           break;
        case SLIDE_JUMP_BACK:
           if (file_num < 5)
              jmp = 1;
           else if (file_num < 40)
              jmp = 2;
           else
              jmp = file_num / 20;
           if (!jmp)
              jmp = 2;
           current_file = filelist_jump(filelist, current_file, BACK, jmp);
           /* important. if the load fails, we only want to step back ONCE to
              try the previous file, not another jmp */
           change = SLIDE_NEXT;
           break;
        default:
           eprintf("BUG!\n");
           break;
      }

      if (last)
      {
         filelist = filelist_remove_file(filelist, last);
         last = NULL;
      }
      if (opt.progressive)
      {
         /* Yeah, we have to do this stuff for progressive loading, so the
            callback knows it's got to create a new image... */
         progwin = winwid;
         imlib_context_set_progress_function(progressive_load_cb);
         imlib_context_set_progress_granularity(PROGRESS_GRANULARITY);
      }
      winwidget_rename(winwid, slideshow_create_name(current_file->filename));
      if ((feh_load_image(&(winwid->im), current_file)) != 0)
      {
         success = 1;
         winwid->zoom_mode = 0;
         winwid->zoom = 0.0;
         winwid->file = current_file;
         if (!opt.progressive)
         {
            imlib_context_set_image(winwid->im);
            winwid->im_w = imlib_image_get_width();
            winwid->im_h = imlib_image_get_height();
            winwidget_render_image(winwid, 0);
         }
         if (opt.draw_filename)
            feh_draw_filename(winwid);
         break;
      }
      else
         last = current_file;
   }
   if (!success)
   {
      /* We didn't manage to load any files. Maybe the last one in the * show 
         was deleted? */
      eprintf("No more slides in show");
   }
   if (opt.slideshow_delay >= 0)
      feh_add_timer(cb_slide_timer, winwid, opt.slideshow_delay,
                    "SLIDE_CHANGE");
   D_RETURN_;
}

char *
slideshow_create_name(char *filename)
{
   char *s = NULL;
   int len = 0;

   D_ENTER;
   len = strlen(PACKAGE " [slideshow mode] - ") + strlen(filename) + 1;
   s = emalloc(len);
   snprintf(s, len, PACKAGE " [%d of %d] - %s",
            filelist_num(filelist, current_file) + 1,
            filelist_length(filelist), filename);
   D_RETURN(s);
}

void
feh_action_run(winwidget w)
{
   char *sys;

   D_ENTER;
   D(("Running action %s\n", opt.action));

   sys = feh_printf(opt.action, w->file);

   if (opt.verbose)
      fprintf(stderr, "Running action -->%s<--\n", sys);
   system(sys);
   D_RETURN_;
}

char *
feh_printf(char *str, feh_file * file)
{
   char *c;
   char buf[20];
   static char ret[4096];

   D_ENTER;

   ret[0] = '\0';

   for (c = str; *c != '\0'; c++)
   {
      if (*c == '%')
      {
         c++;
         switch (*c)
         {
           case 'f':
              strcat(ret, file->filename);
              break;
           case 'n':
              strcat(ret, file->name);
              break;
           case 'w':
              if (!file->info)
                 feh_file_info_load(file, NULL);
              snprintf(buf, sizeof(buf), "%d", file->info->width);
              strcat(ret, buf);
              break;
           case 'h':
              if (!file->info)
                 feh_file_info_load(file, NULL);
              snprintf(buf, sizeof(buf), "%d", file->info->height);
              strcat(ret, buf);
              break;
           case 's':
              if (!file->info)
                 feh_file_info_load(file, NULL);
              snprintf(buf, sizeof(buf), "%d", file->info->size);
              strcat(ret, buf);
              break;
           case 'p':
              if (!file->info)
                 feh_file_info_load(file, NULL);
              snprintf(buf, sizeof(buf), "%d", file->info->pixels);
              strcat(ret, buf);
              break;
           case 't':
              if (!file->info)
                 feh_file_info_load(file, NULL);
              strcat(ret, file->info->format);
              break;
           default:
              strncat(ret, c, 1);
              break;
         }
      }
      else if (*c == '\\')
      {
         c++;
         switch (*c)
         {
           case 'n':
              strcat(ret, "\n");
              break;
           default:
              strncat(ret, c, 1);
              break;
         }
      }
      else
         strncat(ret, c, 1);
   }
   D_RETURN(ret);
}

void
feh_filelist_image_remove(winwidget winwid, char do_delete)
{
   if (opt.slideshow)
   {
      feh_file *doomed;

      doomed = current_file;
      slideshow_change_image(winwid, SLIDE_NEXT);
      if (do_delete)
         filelist = feh_file_rm_and_free(filelist, doomed);
      else
         filelist = filelist_remove_file(filelist, doomed);
      if (!filelist)
      {
         /* No more images. Game over ;-) */
         winwidget_destroy(winwid);
      }
      winwidget_rename(winwid, slideshow_create_name(winwid->file->filename));
   }
   else if (opt.multiwindow)
   {
      if (do_delete)
         filelist = feh_file_rm_and_free(filelist, winwid->file);
      else
         filelist = filelist_remove_file(filelist, winwid->file);
      winwidget_destroy(winwid);
   }
}
