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
#include "feh_list.h"
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
   feh_list *l = NULL, *last = NULL;
   feh_file *file = NULL;

   D_ENTER;

   mode = "slideshow";

   for (l = filelist; l; l = l->next)
   {
      file = FEH_FILE(l->data);
      if (last)
      {
         filelist = feh_file_remove_from_list(filelist, last);
         last = NULL;
      }
      current_file = l;
      s = slideshow_create_name(file);
      if ((w = winwidget_create_from_file(l, s, WIN_TYPE_SLIDESHOW)) != NULL)
      {
         free(s);
         success = 1;
         if (opt.draw_filename)
            feh_draw_filename(w);
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
         last = l;
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
   feh_reload_image(w, 0);
   feh_add_unique_timer(cb_reload_timer, w, opt.reload);
   D_RETURN_;
}

void
feh_reload_image(winwidget w, int resize)
{
   Imlib_Progress_Function pfunc = NULL;

   D_ENTER;

   if (!w->file)
   {
      weprintf("couldn't reload, this image has no file associated with it.");
      D_RETURN_;
   }

   winwidget_free_image(w);

   /* for now, we do reloads *without* progressive loading */
   /*
      if (opt.progressive)
      pfunc = progressive_load_cb;
    */

   if ((winwidget_loadimage(w, FEH_FILE(w->file->data), pfunc)) != 0)
   {
      if (!pfunc)
      {
         w->mode = MODE_NORMAL;
         if ((w->im_w != feh_imlib_image_get_width(w->im))
             || (w->im_h != feh_imlib_image_get_height(w->im)))
            w->had_resize = 1;
         if (w->has_rotated)
         {
            Imlib_Image temp;

            temp = feh_imlib_create_rotated_image(w->im, 0.0);
            w->im_w = feh_imlib_image_get_width(temp);
            w->im_h = feh_imlib_image_get_height(temp);
         }
         else
         {
            w->im_w = feh_imlib_image_get_width(w->im);
            w->im_h = feh_imlib_image_get_height(w->im);
         }
         winwidget_render_image(w, resize, 1);
      }
      if (opt.draw_filename)
         feh_draw_filename(w);
   }
   else
      weprintf("Couldn't reload image. Is it still there?");

   D_RETURN_;
}


void
slideshow_change_image(winwidget winwid, int change)
{
   Imlib_Progress_Function pfunc = NULL;
   int success = 0;
   feh_list *last = NULL;
   int i = 0, file_num = 0;
   int jmp = 1;
   char *s;

   D_ENTER;

   file_num = feh_list_length(filelist);

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
      current_file = feh_list_last(filelist);
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
           current_file = feh_list_jump(filelist, current_file, FORWARD, 1);
           break;
        case SLIDE_PREV:
           current_file = feh_list_jump(filelist, current_file, BACK, 1);
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
           current_file = feh_list_jump(filelist, current_file, FORWARD, jmp);
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
           current_file = feh_list_jump(filelist, current_file, BACK, jmp);
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
         filelist = feh_file_remove_from_list(filelist, last);
         last = NULL;
      }
      s = slideshow_create_name(FEH_FILE(current_file->data));

      winwidget_rename(winwid, s);
      free(s);

      if (opt.progressive)
         pfunc = progressive_load_cb;
      if ((winwidget_loadimage(winwid, FEH_FILE(current_file->data), pfunc))
          != 0)
      {
         success = 1;
         winwid->mode = MODE_NORMAL;
         winwid->file = current_file;
         if (!opt.progressive)
         {
            if ((winwid->im_w != feh_imlib_image_get_width(winwid->im))
                || (winwid->im_h != feh_imlib_image_get_height(winwid->im)))
               winwid->had_resize = 1;
            winwid->im_w = feh_imlib_image_get_width(winwid->im);
            winwid->im_h = feh_imlib_image_get_height(winwid->im);
            winwidget_render_image(winwid, 1, 1);
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
      /* We didn't manage to load any files. Maybe the last one in the show 
         was deleted? */
      eprintf("No more slides in show");
   }
   if (opt.slideshow_delay >= 0)
      feh_add_timer(cb_slide_timer, winwid, opt.slideshow_delay,
                    "SLIDE_CHANGE");
   D_RETURN_;
}

char *
slideshow_create_name(feh_file * file)
{
   char *s = NULL;
   int len = 0;

   D_ENTER;
   if (!opt.title)
   {
      len =
         strlen(PACKAGE " [slideshow mode] - ") + strlen(file->filename) + 1;
      s = emalloc(len);
      snprintf(s, len, PACKAGE " [%d of %d] - %s",
               feh_list_num(filelist, current_file) + 1,
               feh_list_length(filelist), file->filename);
   }
   else
   {
      s = estrdup(feh_printf(opt.title, file));
   }

   D_RETURN(s);
}

void
feh_action_run(feh_file * file)
{
   char *sys;

   D_ENTER;
   D(("Running action %s\n", opt.action));

   sys = feh_printf(opt.action, file);

   if (opt.verbose && !opt.list && !opt.customlist)
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
		   case 'P':
			  strcat(ret, PACKAGE);
			  break;
		   case 'v':
			  strcat(ret, VERSION);	
			  break;
		   case 'm':
			  strcat(ret, mode);
			  break;
		   case 'l':
			  snprintf(buf, sizeof(buf), "%d", feh_list_length(filelist));
			  strcat(ret, buf);
			  break;
		   case 'u':
			  snprintf(buf, sizeof(buf), "%d", current_file != NULL ?
					   feh_list_num(filelist, current_file) + 1 : 0);
			  strcat(ret, buf);
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
   if (winwid->type == WIN_TYPE_SLIDESHOW)
   {
      char *s;
      feh_list *doomed;

      doomed = current_file;
      slideshow_change_image(winwid, SLIDE_NEXT);
      if (do_delete)
         filelist = feh_file_rm_and_free(filelist, doomed);
      else
         filelist = feh_file_remove_from_list(filelist, doomed);
      if (!filelist)
      {
         /* No more images. Game over ;-) */
         winwidget_destroy(winwid);
      }
      s = slideshow_create_name(FEH_FILE(winwid->file->data));
      winwidget_rename(winwid, s);
      free(s);
   }
   else if ((winwid->type == WIN_TYPE_SINGLE)
            || (winwid->type == WIN_TYPE_THUMBNAIL_VIEWER))
   {
      if (do_delete)
         filelist = feh_file_rm_and_free(filelist, winwid->file);
      else
         filelist = feh_file_remove_from_list(filelist, winwid->file);
      winwidget_destroy(winwid);
   }
}
