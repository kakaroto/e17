/* imlib.c

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

#include "feh.h"
#include "feh_list.h"
#include "filelist.h"
#include "winwidget.h"
#include "options.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

Display *disp = NULL;
Visual *vis = NULL;
Screen *scr = NULL;
Colormap cm;
int depth;
Atom wmDeleteWindow;
XContext xid_context = 0;
Window root = 0;
winwidget progwin = NULL;

void
init_x_and_imlib(void)
{
   Imlib_Context ctxt;
   D_ENTER(4);

   disp = XOpenDisplay(NULL);
   if (!disp)
      eprintf("Can't open X display. It *is* running, yeah?");
   vis = DefaultVisual(disp, DefaultScreen(disp));
   depth = DefaultDepth(disp, DefaultScreen(disp));
   cm = DefaultColormap(disp, DefaultScreen(disp));
   root = RootWindow(disp, DefaultScreen(disp));
   scr = ScreenOfDisplay(disp, DefaultScreen(disp));
   xid_context = XUniqueContext();

   ctxt = imlib_context_new();
   imlib_context_push(ctxt);
   imlib_context_set_display(disp);
   imlib_context_set_visual(vis);
   imlib_context_set_colormap(cm);
   imlib_context_set_color_modifier(NULL);
   imlib_context_set_progress_function(NULL);
   imlib_context_set_operation(IMLIB_OP_COPY);
   wmDeleteWindow = XInternAtom(disp, "WM_DELETE_WINDOW", False);

   /* Initialise random numbers */
   srand(getpid() * time(NULL) % ((unsigned int) -1));

   /* Set up the font stuff */
   imlib_add_path_to_font_path(".");
   imlib_add_path_to_font_path(PREFIX "/share/feh/fonts");
   imlib_add_path_to_font_path("./ttfonts");

   D_RETURN_(4);
}

int
feh_load_image_char(Imlib_Image * im, char *filename)
{
   feh_file *file;
   int i;

   D_ENTER(4);
   file = feh_file_new(filename);
   i = feh_load_image(im, file);
   feh_file_free(file);
   D_RETURN(4, i);
}

int
feh_load_image(Imlib_Image * im, feh_file * file)
{
   Imlib_Load_Error err;

   D_ENTER(4);
   D(3, ("filename is %s, image is %p\n", file->filename, im));

   if (!file || !file->filename)
      D_RETURN(4, 0);

   /* Handle URLs */
   if ((!strncmp(file->filename, "http://", 7))
       || (!strncmp(file->filename, "ftp://", 6)))
   {
      char *tmpname = NULL;
      char *tempcpy;

      tmpname = feh_http_load_image(file->filename);
      if (tmpname == NULL)
         D_RETURN(4, 0);
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
        case IMLIB_LOAD_ERROR_UNKNOWN:
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
           if (!opt.quiet)
              weprintf("While loading %s - Out of memory", file->filename);
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
        default:
           if (!opt.quiet)
              weprintf
                 ("While loading %s - Unknown error (%d). Attempting to continue",
                  file->filename, err);
           break;
      }
      D(3, ("Load *failed*\n"));
      D_RETURN(4, 0);
   }

   D(3, ("Loaded ok\n"));
   D_RETURN(4, 1);
}

char *
feh_http_load_image(char *url)
{
   char *tmp;
   char *tmpname;
   char *tmpname_timestamper = NULL;
   char *basename;
   char cppid[10];
   static long int i = 1;
   char *newurl = NULL;
   char randnum[20];
   int rnum;
   pid_t ppid;
   char *path = NULL;

   D_ENTER(4);

   if(opt.keep_http)
   {
      if(opt.output_dir)
         path = opt.output_dir;
      else
         path = "";
   }
   else
      path = "/tmp/";

   basename = strrchr(url, '/') + 1;
   tmpname =
         feh_unique_filename(path, basename);
   
   if (opt.wget_timestamp)
   {
      char cppid[10];
      pid_t ppid;
      ppid = getpid();
      snprintf(cppid, sizeof(cppid), "%06ld", ppid);
      tmpname_timestamper =
         estrjoin("", "/tmp/feh_", cppid, "_", basename, NULL);
   }

   if (opt.wget_timestamp)
   {
      newurl = estrdup(url);
   }
   else
   {
      rnum = rand();
      snprintf(randnum, sizeof(randnum), "%d", rnum);
      newurl = estrjoin("?", url, randnum, NULL);
   }
   D(3, ("newurl: %s\n", newurl));

   if (opt.builtin_http)
   {
      /* state for HTTP header parser */
#define SAW_NONE    1
#define SAW_ONE_CM  2
#define SAW_ONE_CJ  3
#define SAW_TWO_CM  4
#define IN_BODY     5

#define OUR_BUF_SIZE 1024
#define EOL "\015\012"

      int sockno = 0;
      int size;
      int body = SAW_NONE;
      struct sockaddr_in addr;
      struct hostent *hptr;
      char *hostname;
      char *get_string;
      char *host_string;
      char *query_string;
      char *get_url;
      static char buf[OUR_BUF_SIZE];
      char ua_string[] = "User-Agent: feh image viewer";
      char accept_string[] = "Accept: image/*";
      FILE *fp;

      D(4, ("using builtin http collection\n"));
      fp = fopen(tmpname, "w");
      if (!fp)
      {
         weprintf("couldn't write to file %s:", tmpname);
         free(tmpname);
         D_RETURN(4, NULL);
      }

      hostname = feh_strip_hostname(newurl);
      if (!hostname)
      {
         weprintf("couldn't work out hostname from %s:", newurl);
         free(tmpname);
         free(newurl);
         D_RETURN(4, NULL);
      }

      D(4, ("trying hostname %s\n", hostname));

      if (!(hptr = feh_gethostbyname(hostname)))
      {
         weprintf("error resolving host %s:", hostname);
         free(hostname);
         free(tmpname);
         free(newurl);
         D_RETURN(4, NULL);
      }

      /* Copy the address of the host to socket description. */
      memcpy(&addr.sin_addr, hptr->h_addr, hptr->h_length);

      /* Set port and protocol */
      addr.sin_family = AF_INET;
      addr.sin_port = htons(80);

      if ((sockno = socket(PF_INET, SOCK_STREAM, 0)) == -1)
      {
         weprintf("error opening socket:");
         free(tmpname);
         free(hostname);
         free(newurl);
         D_RETURN(4, NULL);
      }
      if (connect(sockno, (struct sockaddr *) &addr, sizeof(addr)) == -1)
      {
         weprintf("error connecting socket:");
         free(tmpname);
         free(hostname);
         free(newurl);
         D_RETURN(4, NULL);
      }

      get_url = strchr(newurl, '/') + 2;
      get_url = strchr(get_url, '/');

      get_string = estrjoin(" ", "GET", get_url, "HTTP/1.0", NULL);
      host_string = estrjoin(" ", "Host:", hostname, NULL);
      query_string =
         estrjoin(EOL, get_string, host_string, accept_string, ua_string, "",
                  "", NULL);
      /* At this point query_string looks something like
         **
         **    GET /dir/foo.jpg?123456 HTTP/1.0^M^J
         **    Host: www.example.com^M^J
         **    Accept: image/ *^M^J
         **    User-Agent: feh image viewer^M^J
         **    ^M^J
         **
         ** Host: is required by HTTP/1.1 and very important for some sites,
         ** even with HTTP/1.0
         **
         ** -- BEG
       */
      if ((send(sockno, query_string, strlen(query_string), 0)) == -1)
      {
         free(get_string);
         free(host_string);
         free(query_string);
         free(tmpname);
         free(hostname);
         free(newurl);
         weprintf("error sending over socket:");
         D_RETURN(4, NULL);
      }
      free(get_string);
      free(host_string);
      free(query_string);
      free(hostname);
      free(newurl);

      while ((size = read(sockno, &buf, OUR_BUF_SIZE)))
      {
         if (body == IN_BODY)
         {
            fwrite(buf, 1, size, fp);
         }
         else
         {
            int i;

            for (i = 0; i < size; i++)
            {
               /* We are looking for ^M^J^M^J, but will accept
                  ** ^J^J from broken servers. Stray ^Ms will be
                  ** ignored.
                  **
                  ** TODO:
                  ** Checking the headers for a
                  **    Content-Type: image/ *
                  ** header would help detect problems with results.
                  ** Maybe look at the response code too? But there is
                  ** no fundamental reason why a 4xx or 5xx response
                  ** could not return an image, it is just the 3xx
                  ** series we need to worry about.
                  **
                  ** Also, grabbing the size from the Content-Length
                  ** header and killing the connection after that
                  ** many bytes where read would speed up closing the
                  ** socket.
                  ** -- BEG
                */

               switch (body)
               {

                 case IN_BODY:
                    fwrite(buf + i, 1, size - i, fp);
                    i = size;
                    break;

                 case SAW_ONE_CM:
                    if (buf[i] == '\012')
                    {
                       body = SAW_ONE_CJ;
                    }
                    else
                    {
                       body = SAW_NONE;
                    }
                    break;

                 case SAW_ONE_CJ:
                    if (buf[i] == '\015')
                    {
                       body = SAW_TWO_CM;
                    }
                    else
                    {
                       if (buf[i] == '\012')
                       {
                          body = IN_BODY;
                       }
                       else
                       {
                          body = SAW_NONE;
                       }
                    }
                    break;

                 case SAW_TWO_CM:
                    if (buf[i] == '\012')
                    {
                       body = IN_BODY;
                    }
                    else
                    {
                       body = SAW_NONE;
                    }
                    break;

                 case SAW_NONE:
                    if (buf[i] == '\015')
                    {
                       body = SAW_ONE_CM;
                    }
                    else
                    {
                       if (buf[i] == '\012')
                       {
                          body = SAW_ONE_CJ;
                       }
                    }
                    break;

               }                            /* switch */
            }                               /* for i */
         }
      }                                     /* while read */
      close(sockno);
      fclose(fp);
   }
   else
   {
      int pid;
      int status;

      if ((pid = fork()) < 0)
      {
         weprintf("open url: fork failed:");
         free(tmpname);
         free(newurl);
         D_RETURN(4, NULL);
      }
      else if (pid == 0)
      {
         char *quiet = NULL;

         if (!opt.verbose)
            quiet = estrdup("-q");

         if (opt.wget_timestamp)
         {
            execlp("wget", "wget", "-N", "-O", tmpname_timestamper, newurl,
                   quiet, NULL);
         }
         else
         {
            execlp("wget", "wget", "--cache", "0", newurl, "-O", tmpname,
                   quiet, NULL);
         }
         eprintf("url: exec failed: wget:");
      }
      else
      {
         waitpid(pid, &status, 0);

         if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
         {
            weprintf("url: wget failed to load URL %s\n", url);
            free(tmpname);
            free(newurl);
            D_RETURN(4, NULL);
         }
         if (opt.wget_timestamp)
         {
            char cmd[2048];

            snprintf(cmd, sizeof(cmd), "/bin/cp %s %s", tmpname_timestamper,
                     tmpname);
            system(cmd);
         }
      }
   }

   free(newurl);
   D_RETURN(4, tmpname);
}

struct hostent *
feh_gethostbyname(const char *name)
{
   struct hostent *hp;
   unsigned long addr;

   D_ENTER(3);
   addr = (unsigned long) inet_addr(name);
   if ((int) addr != -1)
      hp = gethostbyaddr((char *) &addr, sizeof(addr), AF_INET);
   else
      hp = gethostbyname(name);
   D_RETURN(3, hp);
}

char *
feh_strip_hostname(char *url)
{
   char *ret;
   char *start;
   char *finish;
   int len;

   D_ENTER(3);

   start = strchr(url, '/');
   if (!start)
      D_RETURN(3, NULL);

   start += 2;

   finish = strchr(start, '/');
   if (!finish)
      D_RETURN(3, NULL);

   len = finish - start;

   ret = emalloc(len + 1);
   strncpy(ret, start, len);
   ret[len] = '\0';
   D_RETURN(3, ret);
}

void
feh_draw_filename(winwidget w)
{
   static Imlib_Font fn = NULL;
   int tw = 0, th = 0;
   Imlib_Image im = NULL;

   D_ENTER(4);
   if (!fn)
   {
      if (opt.full_screen)
         fn = feh_imlib_load_font("20thcent/16");
      else
         fn = feh_imlib_load_font("20thcent/10");
   }

   if (!fn)
   {
      weprintf("Couldn't load font for filename printing");
      D_RETURN_(4);
   }

   /* Work out how high the font is */
   feh_imlib_get_text_size(fn, FEH_FILE(w->file->data)->filename, &tw, &th,
                           IMLIB_TEXT_TO_RIGHT);

   im = imlib_create_image(tw, th);
   if (!im)
      eprintf("Couldn't create image. Out of memory?");

   feh_imlib_image_fill_rectangle(im, 0, 0, tw, th, 0, 0, 0, 255);

   feh_imlib_text_draw(im, fn, 0, 0, FEH_FILE(w->file->data)->filename,
                       IMLIB_TEXT_TO_RIGHT, 255, 255, 255, 255);

   feh_imlib_render_image_on_drawable(w->bg_pmap, im, 0, 0, 1, 0, 0);

   feh_imlib_free_image_and_decache(im);

   XSetWindowBackgroundPixmap(disp, w->win, w->bg_pmap);
   XClearArea(disp, w->win, 0, 0, tw, th, False);
   D_RETURN_(4);
}

unsigned char reset_output = 0;

void
feh_display_status(char stat)
{
   static int i = 0;
   static int init_len = 0;
   int j = 0;

   D_ENTER(5);

   D(5, ("filelist %p, filelist->next %p\n", filelist, filelist->next));

   if (!init_len)
      init_len = feh_list_length(filelist);

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

         len = feh_list_length(filelist);
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
   D_RETURN_(5);
}
