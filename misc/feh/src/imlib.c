/* imlib.c
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
#include "winwidget.h"
#include "options.h"

Display *disp = NULL;
Visual *vis = NULL;
Screen *scr = NULL;
Colormap cm;
int depth;
Atom wmDeleteWindow;
Imlib_Image *checks = NULL;
XContext xid_context = 0;
Window root = 0;
winwidget progwin = NULL;

void
init_x_and_imlib(void)
{
   int onoff, x, y;

   D_ENTER;

   disp = XOpenDisplay(NULL);
   if (!disp)
      eprintf("Can't open X display");
   vis = DefaultVisual(disp, DefaultScreen(disp));
   depth = DefaultDepth(disp, DefaultScreen(disp));
   cm = DefaultColormap(disp, DefaultScreen(disp));
   root = RootWindow(disp, DefaultScreen(disp));
   scr = ScreenOfDisplay(disp, DefaultScreen(disp));
   xid_context = XUniqueContext();

   imlib_context_set_display(disp);
   imlib_context_set_visual(vis);
   imlib_context_set_colormap(cm);
   imlib_context_set_color_modifier(NULL);
   imlib_context_set_operation(IMLIB_OP_COPY);
   wmDeleteWindow = XInternAtom(disp, "WM_DELETE_WINDOW", False);
   imlib_context_set_dither(1);
   imlib_context_set_blend(0);
   imlib_set_cache_size(0);

   /* Set up the font stuff */
   imlib_add_path_to_font_path(".");
   if (opt.fontpath)
   {
      D(("adding fontpath %s\n", opt.fontpath));
      imlib_add_path_to_font_path(opt.fontpath);
   }
   imlib_add_path_to_font_path(PREFIX "/share/feh/fonts");
   imlib_add_path_to_font_path("./ttfonts");

   /* don't need checks for these */
   if (!(opt.list || opt.customlist || opt.loadables || opt.unloadables))
   {
      checks = imlib_create_image(CHECK_SIZE, CHECK_SIZE);

      if (!checks)
         eprintf
            ("Unable to create a teeny weeny imlib image. I detect problems");

      imlib_context_set_image(checks);
      for (y = 0; y < CHECK_SIZE; y += 8)
      {
         onoff = (y / 8) & 0x1;
         for (x = 0; x < CHECK_SIZE; x += 8)
         {
            if (onoff)
               imlib_context_set_color(144, 144, 144, 255);
            else
               imlib_context_set_color(100, 100, 100, 255);
            imlib_image_fill_rectangle(x, y, 8, 8);
            onoff++;
            if (onoff == 2)
               onoff = 0;
         }
      }
   }
   D_RETURN_;
}

int
feh_load_image_char(Imlib_Image ** im, char *filename)
{
   feh_file *file;
   int i;

   D_ENTER;
   file = filelist_newitem(filename);
   i = feh_load_image(im, file);
   feh_file_free(file);
   D_RETURN(i);
}

int
feh_load_image(Imlib_Image ** im, feh_file * file)
{
   Imlib_Load_Error err;

   D_ENTER;
   D(("filename is %s\n", file->filename));

   if (!file || !file->filename)
      D_RETURN(0);

   D(("File is %p, file->next is %p\n", file, file->next));

   /* Handle URLs */
   if ((!strncmp(file->filename, "http://", 7))
       || (!strncmp(file->filename, "ftp://", 6)))
   {
      char *tmpname = NULL;
      char *tempcpy;

      tmpname = feh_http_load_image(file->filename);
      if (tmpname == NULL)
         D_RETURN(0);
      *im = imlib_load_image_with_error_return(tmpname, &err);
      if (im)
      {
         /* load the info now, in case it's needed after we delete the
            temporary image file */
         tempcpy = file->filename;
         file->filename = tmpname;
         feh_file_info_load(file, *im);
         file->filename = tempcpy;
      }
      if ((opt.slideshow) && (opt.reload == 0))
      {
         /* Http, no reload, slideshow. Let's keep this image on hand... */
         free(file->filename);
         file->filename = estrdup(tmpname);
      }
      else
      {
         /* Don't cache the image if we're doing reload + http (webcams etc) */
         if (!opt.keep_http)
            unlink(tmpname);
      }
      add_file_to_rm_filelist(tmpname);
      free(tmpname);
   }
   else
   {
      *im = imlib_load_image_with_error_return(file->filename, &err);
   }

   if ((err) || (!im))
   {
      if (opt.verbose && !opt.quiet)
      {
         fprintf(stdout, "\n");
         reset_output = 1;
      }
      /* Check error code */
      switch (err)
      {
        case IMLIB_LOAD_ERROR_FILE_DOES_NOT_EXIST:
           if (!opt.quiet)
              weprintf("%s - File does not exist", file->filename);
           break;
        case IMLIB_LOAD_ERROR_FILE_IS_DIRECTORY:
           if (!opt.quiet)
              weprintf("%s - Directory specified for image filename",
                       file->filename);
           break;
        case IMLIB_LOAD_ERROR_PERMISSION_DENIED_TO_READ:
           if (!opt.quiet)
              weprintf("%s - No read access to directory", file->filename);
           break;
        case IMLIB_LOAD_ERROR_NO_LOADER_FOR_FILE_FORMAT:
           if (!opt.quiet)
              weprintf("%s - No Imlib2 loader for that file format",
                       file->filename);
           break;
        case IMLIB_LOAD_ERROR_PATH_TOO_LONG:
           if (!opt.quiet)
              weprintf("%s - Path specified is too long", file->filename);
           break;
        case IMLIB_LOAD_ERROR_PATH_COMPONENT_NON_EXISTANT:
           if (!opt.quiet)
              weprintf("%s - Path component does not exist", file->filename);
           break;
        case IMLIB_LOAD_ERROR_PATH_COMPONENT_NOT_DIRECTORY:
           if (!opt.quiet)
              weprintf("%s - Path component is not a directory",
                       file->filename);
           break;
        case IMLIB_LOAD_ERROR_PATH_POINTS_OUTSIDE_ADDRESS_SPACE:
           if (!opt.quiet)
              weprintf("%s - Path points outside address space",
                       file->filename);
           break;
        case IMLIB_LOAD_ERROR_TOO_MANY_SYMBOLIC_LINKS:
           if (!opt.quiet)
              weprintf("%s - Too many levels of symbolic links",
                       file->filename);
           break;
        case IMLIB_LOAD_ERROR_OUT_OF_MEMORY:
           eprintf("While loading %s - Out of memory", file->filename);
           break;
        case IMLIB_LOAD_ERROR_OUT_OF_FILE_DESCRIPTORS:
           eprintf("While loading %s - Out of file descriptors",
                   file->filename);
           break;
        case IMLIB_LOAD_ERROR_PERMISSION_DENIED_TO_WRITE:
           if (!opt.quiet)
              weprintf("%s - Cannot write to directory", file->filename);
           break;
        case IMLIB_LOAD_ERROR_OUT_OF_DISK_SPACE:
           if (!opt.quiet)
              weprintf("%s - Cannot write - out of disk space",
                       file->filename);
           break;
        case IMLIB_LOAD_ERROR_UNKNOWN:
        default:
           if (!opt.quiet)
              weprintf
                 ("While loading %s - Unknown error. Attempting to continue",
                  file->filename);
           break;
      }
      D(("Load *failed*\n"));
      D_RETURN(0);
   }

   D(("Loaded ok\n"));
   D_RETURN(1);
}

int
progressive_load_cb(Imlib_Image im, char percent, int update_x, int update_y,
                    int update_w, int update_h)
{
   int new = 0, dest_x = 0, dest_y = 0;

   D_ENTER;
   if (!progwin)
   {
      weprintf("progwin does not exist - this should not happen");
      D_RETURN(0);
   }

   imlib_context_set_drawable(progwin->bg_pmap);
   imlib_context_set_anti_alias(0);
   imlib_context_set_dither(1);
   imlib_context_set_image(im);

   /* Is this the first progress return for a new image? */
   /* If so, we have some stuff to set up... */
   if (progwin->im_w == 0)
   {
      progwin->w = progwin->im_w = imlib_image_get_width();
      progwin->h = progwin->im_h = imlib_image_get_height();
      /* do we need to create a window for the image? */
      if (!progwin->win)
      {
         winwidget_create_window(progwin, progwin->w, progwin->h);
         new = 1;
      }

      winwidget_setup_pixmaps(progwin);

      if (!opt.full_screen)
      {
         feh_draw_checks(progwin);
         if (!new)
            winwidget_resize(progwin, progwin->w, progwin->h);
      }
      XSetWindowBackgroundPixmap(disp, progwin->win, progwin->bg_pmap);
      if (new)
         winwidget_show(progwin);
      if (opt.full_screen)
         XClearArea(disp, progwin->win, 0, 0, scr->width, scr->height, False);
   }

   imlib_context_set_image(im);
   imlib_context_set_drawable(progwin->bg_pmap);
   imlib_context_set_anti_alias(0);
   imlib_context_set_dither(1);
   imlib_context_set_blend(0);
   if (imlib_image_has_alpha())
      imlib_context_set_blend(1);

   if (opt.full_screen)
   {
      dest_x = (scr->width - progwin->im_w) >> 1;
      dest_y = (scr->height - progwin->im_h) >> 1;
   }

   imlib_render_image_part_on_drawable_at_size(update_x, update_y, update_w,
                                               update_h, dest_x + update_x,
                                               dest_y + update_y, update_w,
                                               update_h);
   XClearArea(disp, progwin->win, dest_x + update_x, dest_y + update_y,
              update_w, update_h, False);

   D_RETURN(1);
   percent = 0;
}

char *
feh_http_load_image(char *url)
{
   int pid;
   int status;
   char *tmp;
   char *tmpname;
   char num[10];
   static long int i = 1;

   D_ENTER;
   snprintf(num, sizeof(num), "%04ld_", i++);
   /* Massive paranoia ;) */
   if (i > 9998)
      i = 1;

   tmp = tempnam("/tmp", num);
   if (tmp == NULL)
      eprintf("Error creating unique filename:");

   /* Modify tempname to make it a little more useful... */
   tmpname = estrjoin("", tmp, "_feh_", strrchr(url, '/') + 1, NULL);
   free(tmp);

   if ((pid = fork()) < 0)
   {
      weprintf("open url: fork failed:");
      free(tmpname);
      D_RETURN(NULL);
   }
   else if (pid == 0)
   {
      execlp("wget", "wget", "--cache", "0", url, "-O", tmpname, NULL);
      eprintf("url: exec failed: wget:");
   }
   else
   {
      waitpid(pid, &status, 0);

      if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
      {
         weprintf("url: wget failed to load URL %s\n", url);
         free(tmpname);
         D_RETURN(NULL);
      }
   }
   D_RETURN(tmpname);
}

void
feh_draw_filename(winwidget w)
{
   static Imlib_Font fn = NULL;
   int tw = 0, th = 0;
   Imlib_Image *im = NULL;

   D_ENTER;
   if (!fn)
   {
      if (opt.full_screen)
         fn = imlib_load_font("20thcent/16");
      else
         fn = imlib_load_font("20thcent/10");
   }

   if (!fn)
   {
      weprintf("Couldn't load font for filename printing");
      D_RETURN_;
   }

   imlib_context_set_font(fn);
   imlib_context_set_direction(IMLIB_TEXT_TO_RIGHT);
   imlib_context_set_color(0, 0, 0, 255);
   imlib_context_set_blend(1);
   imlib_context_set_drawable(w->bg_pmap);

   /* Work out how high the font is */
   imlib_get_text_size(w->file->filename, &tw, &th);

   im = imlib_create_image(tw, th);
   if (!im)
      eprintf("Couldn't create image. Out of memory?");

   imlib_context_set_image(im);
   imlib_image_fill_rectangle(0, 0, tw, th);
   imlib_context_set_color(255, 255, 255, 255);

   imlib_text_draw(0, 0, w->file->filename);

   imlib_render_image_on_drawable(0, 0);

   imlib_free_image_and_decache();
   imlib_context_set_image(w->im);

   XSetWindowBackgroundPixmap(disp, w->win, w->bg_pmap);
   XClearArea(disp, w->win, 0, 0, tw, th, False);
   D_RETURN_;
}

unsigned char reset_output = 0;

void
feh_display_status(char stat)
{
   static int i = 0;
   static int init_len = 0;
   int j = 0;

   D_ENTER;

   D(
     ("filelist %p, filelist->next %p, filelist->name %s\n", filelist,
      filelist->next, filelist->name));

   if (!init_len)
      init_len = filelist_length(filelist);

   if (i)
   {
      if (reset_output)
      {
         /* There's just been an error message. Unfortunate ;) */
         for (j = 0; j < (((i % 50) + ((i % 50) / 10)) + 7); j++)
            fprintf(stdout, " ");
      }

      if (!(i % 50))
      {
         int len;
         char buf[50];

         len = filelist_length(filelist);
         snprintf(buf, sizeof(buf), " %5d/%d (%d)\n[%3d%%] ", i, init_len,
                  len, ((int) ((float) i / init_len * 100)));
         fprintf(stdout, buf);
      }
      else if ((!(i % 10)) && (!reset_output))
         fprintf(stdout, " ");

      reset_output = 0;
   }
   else
      fprintf(stdout, "[  0%%] ");

   fprintf(stdout, "%c", stat);
   fflush(stdout);
   i++;
   D_RETURN_;
}
