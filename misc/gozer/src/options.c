/* options.c

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

void
default_options(gozeroptions *opt)
{
   char *home, *rcfile;
   struct stat ignored;

   /* Set default options */
   memset(opt, 0, sizeof(gozeroptions));

   opt->line_spacing = 1;
   opt->bg_a = 255;
   opt->bg_g = 255;
   opt->bg_b = 255;
   opt->bg_r = 255;
   opt->fn_a = 255;
   opt->justification = JUST_LEFT;
   opt->font = estrdup("helmetr/16");
   opt->pipe = 0;
   opt->bg_image = NULL;
   opt->bg_resize = 0;
   opt->bg_tile = 0;
   opt->bg_scale = 0;
   opt->quality = 75;

   home = getenv("HOME");
   if (!home)
      weprintf("D'oh! Please define HOME in your environment!"
               "It would really help me out in loading your rc file...\n");
   rcfile = estrjoin("/", home, ".gozerrc", NULL);
   if (stat(rcfile, &ignored) < 0)
	  gozer_parse_rc_file("/etc/gozerrc", 0, opt);
   else
	  gozer_parse_rc_file(rcfile, 0, opt);
   free(rcfile);
}

void
gozer_parse_rc_file(char *file, int user_defined, gozeroptions *opt)
{
   FILE *fp = NULL;
   char s[1024], s1[1024], s2[1024];

   if ((fp = fopen(file, "r")) == NULL)
   {
      if (user_defined)
         weprintf("couldn't load the specified rcfile %s:", opt->rc_file);
      return;
   }

   /* Oooh. We have an options file :) */
   for (; fgets(s, sizeof(s), fp);)
   {
      s1[0] = '\0';
      s2[0] = '\0';
      sscanf(s, "%s %[^\n]\n", (char *) &s1, (char *) &s2);
      if (!(*s1) || (!*s2) || (*s1 == '\n') || (*s1 == '#'))
         continue;
      if (!strcasecmp(s1, "fontpath"))
         gib_imlib_parse_fontpath(s2);
      else if (!strcasecmp(s1, "background"))
      {
         free(opt->background_col);
         opt->background_col = estrdup(s2);
      }
      else if (!strcasecmp(s1, "foreground"))
      {
         free(opt->font_col);
         opt->font_col = estrdup(s2);
      }
      else if (!strcasecmp(s1, "font"))
      {
         free(opt->font);
         opt->font = estrdup(s2);
      }
      else if (!strcasecmp(s1, "justification"))
      {
         if (!strcasecmp(s2, "left"))
            opt->justification = JUST_LEFT;
         else if (!strcasecmp(s2, "right"))
            opt->justification = JUST_RIGHT;
         else if (!strcasecmp(s2, "center"))
            opt->justification = JUST_CENTER;
         else if (!strcasecmp(s2, "block"))
            opt->justification = JUST_BLOCK;
         else
            weprintf("unknown justification setting %s ignored\n", s2);
      }
      else if (!strcasecmp(s1, "line-spacing"))
         opt->line_spacing = atoi(s2);
      else if (!strcasecmp(s1, "style"))
      {
         free(opt->stylefile);
         opt->stylefile = estrdup(s2);
      }
      else if (!strcasecmp(s1, "text"))
      {
         free(opt->text);
         opt->text = estrdup(s2);
      }
      else if (!strcasecmp(s1, "textfile"))
      {
         free(opt->textfile);
         opt->textfile = estrdup(s2);
      }
      else if (!strcasecmp(s1, "wrap"))
         opt->wrap_width = atoi(s2);
      else
         weprintf("unrecognised option %s in rc file %s\n", s2, opt->rc_file);
   }
   fclose(fp);
}
