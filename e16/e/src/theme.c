/*
 * Copyright (C) 2000-2003 Carsten Haitzler, Geoff Harrison and various contributors
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
#include "E.h"
#ifdef __EMX__
#include <process.h>
extern char        *__XOS2RedirRoot(const char *);

#endif

char               *
append_merge_dir(char *dir, char ***list, int *count)
{
   char                s[FILEPATH_LEN_MAX], ss[FILEPATH_LEN_MAX], **str =
      NULL, *def = NULL;
   char                already, *tmp, *tmp2, ok;
   int                 i, j, num;

   str = ls(dir, &num);
   if (str)
     {
	for (i = 0; i < num; i++)
	  {
	     already = 0;
	     for (j = 0; (j < (*count)) && (!already); j++)
	       {
		  tmp = fileof((*list)[j]);
		  tmp2 = fileof(str[i]);
		  if ((tmp != NULL) && (tmp2 != NULL) && (!strcmp(tmp, tmp2)))
		     already = 1;
		  if (tmp)
		     Efree(tmp);
		  if (tmp2)
		     Efree(tmp2);
	       }
	     if (!already)
	       {
		  if (!strcmp(str[i], "DEFAULT"))
		    {
		       Esnprintf(ss, sizeof(ss), "%s/%s", dir, str[i]);
#ifndef __EMX__
		       if (readlink(ss, s, sizeof(s)) > 0)
			 {
			    if (s[0] == '/')
			       def = duplicate(s);
			    else
			      {
				 Esnprintf(s, sizeof(s), "%s/%s", dir, s);
				 def = duplicate(s);
			      }
			 }
#else
		       if (isdir(ss))
			 {
			    def = duplicate(ss);
			    (*count)++;
			    (*list) =
			       Erealloc(*list, (*count) * sizeof(char *));

			    (*list)[(*count) - 1] = duplicate(ss);
			 }
#endif
		    }
		  else
		    {
		       ok = 0;

		       Esnprintf(s, sizeof(s), "%s/%s", dir, str[i]);
		       if ((isdir(s)) && (SanitiseThemeDir(s)))
			  ok = 1;
		       else if ((isfile(s)) && (FileExtension(s))
				&& (!strcmp(FileExtension(s), "etheme")))
			  ok = 1;
		       if (ok)
			 {
			    (*count)++;
			    (*list) =
			       Erealloc(*list, (*count) * sizeof(char *));

			    (*list)[(*count) - 1] = duplicate(s);
			 }
		    }
	       }
	  }
	freestrlist(str, num);
     }
   return def;
}

char              **
ListThemes(int *number)
{
   char                s[FILEPATH_LEN_MAX], **list = NULL, *def = NULL, *def2 =
      NULL;
   int                 count = 0;

   Esnprintf(s, sizeof(s), "%s/themes", UserEDir());
   def = append_merge_dir(s, &list, &count);
#ifndef __EMX__
   Esnprintf(s, sizeof(s), "%s/themes", ENLIGHTENMENT_ROOT);
#else
   Esnprintf(s, sizeof(s), "%s/themes", __XOS2RedirRoot(ENLIGHTENMENT_ROOT));
#endif
   def2 = append_merge_dir(s, &list, &count);

   if ((def) && (def2))
     {
	Efree(def2);
	def2 = NULL;
     }
   else if ((!def) && (def2))
      def = def2;
   else if ((!def) && (!def2))
      fprintf(stderr, "errrrrrrrr no default theme?\n");

   if (def)
      Efree(def);

   *number = count;
   return list;
}

char               *
GetDefaultTheme(void)
{
   char                s[FILEPATH_LEN_MAX], ss[FILEPATH_LEN_MAX];
   char               *def = NULL;
   int                 count;

   Esnprintf(ss, sizeof(ss), "%s/themes/DEFAULT", UserEDir());
#ifndef __EMX__
   count = readlink(ss, s, sizeof(s));
   if ((exists(ss)) && (count > 0))
     {
	s[count] = 0;
	if (s[0] == '/')
	   def = duplicate(s);
	else
	  {
	     Esnprintf(ss, sizeof(ss), "%s/themes/%s", UserEDir(), s);
	     def = duplicate(ss);
	  }
     }
#else
   if (isdir(ss))
      def = duplicate(ss);
#endif
   if (!def)
     {
#ifndef __EMX__
	Esnprintf(ss, sizeof(ss), "%s/themes/DEFAULT", ENLIGHTENMENT_ROOT);
#else
	Esnprintf(ss, sizeof(ss), "%s/themes/DEFAULT",
		  __XOS2RedirRoot(ENLIGHTENMENT_ROOT));
#endif
#ifndef __EMX__
	count = readlink(ss, s, sizeof(s));
	if ((exists(ss)) && (count > 0))
	  {
	     s[count] = 0;
	     if (s[0] == '/')
		def = duplicate(s);
	     else
	       {
		  Esnprintf(ss, sizeof(ss), "%s/themes/%s", ENLIGHTENMENT_ROOT,
			    s);
		  def = duplicate(ss);
	       }
	  }
#else
	if (isdir(ss))
	   def = duplicate(ss);
#endif
     }
   return def;
}

void
SetDefaultTheme(char *theme)
{
#ifndef __EMX__
/* os2 has no symlink,
 * but it doesn't matter since we have ~/.enlightenment/user_theme.cfg
 */
   char                ss[FILEPATH_LEN_MAX];

   Esnprintf(ss, sizeof(ss), "%s/themes/DEFAULT", UserEDir());
   if (exists(ss))
      rm(ss);
   if (theme)
      symlink(theme, ss);
#endif
}

char               *
ExtractTheme(char *theme)
{
   char                s[FILEPATH_LEN_MAX];
   char                th[FILEPATH_LEN_MAX];
   FILE               *f;
   unsigned char       buf[320];

   EDBUG(7, "ExtractTheme");
   mustdel = 0;
   /* its a directory - just use it "as is" */
   if (isdir(theme))
     {
	if (SanitiseThemeDir(theme))
	  {
	     EDBUG_RETURN(duplicate(theme));
	  }
	else
	  {
	     EDBUG_RETURN(NULL);
	  }
     }
   /* its a file - check its type */
   if (isfile(theme))
     {
	f = fopen(theme, "r");
	if (f)
	  {
	     char               *themename;

	     fread(buf, 1, 320, f);
	     fclose(f);
	     /* make the temp dir */

	     themename = fileof(theme);
	     Esnprintf(th, sizeof(th), "%s/themes/%s", UserEDir(), themename);
	     Efree(themename);
	     md(th);
	     /* check magic numbers */
	     if ((buf[0] == 31) && (buf[1] == 139))
	       {
		  /*gzipped tarball */
		  Esnprintf(s, sizeof(s),
			    "gzip -d -c < %s | (cd %s ; tar -xf -)", theme, th);
	       }
	     else if ((buf[257] == 'u') && (buf[258] == 's')
		      && (buf[259] == 't') && (buf[260] == 'a')
		      && (buf[261] == 'r'))
	       {
		  /*vanilla tarball */
		  Esnprintf(s, sizeof(s), "(cd %s ; tar -xf %s)", th, theme);
	       }
	     /* exec the untar if tarred */
	     system(s);
	     /* we made a temp dir - flag for deletion */
	     if (SanitiseThemeDir(th))
	       {
		  EDBUG_RETURN(duplicate(th));
	       }
	     else if (mustdel)
	       {
		  char                sss[FILEPATH_LEN_MAX];

#ifndef __EMX__
		  Esnprintf(sss, sizeof(sss), "/bin/rm -rf %s", themepath);
#else
		  Esnprintf(sss, sizeof(sss), "rm.exe -rf %s", themepath);
#endif
		  system(sss);
		  mustdel = 0;
		  EDBUG_RETURN(NULL);
	       }
	  }
     }
   EDBUG_RETURN(NULL);
}

char               *
FindTheme(char *theme)
{
   char                s[FILEPATH_LEN_MAX];
   char               *ret = NULL;

   EDBUG(6, "FindTheme");
   badreason = _("Unknown\n");
   if (!theme[0])
     {
#ifndef __EMX__
	Esnprintf(s, sizeof(s), "%s/themes/DEFAULT", ENLIGHTENMENT_ROOT);
#else
	Esnprintf(s, sizeof(s), "%s/themes/DEFAULT",
		  __XOS2RedirRoot(ENLIGHTENMENT_ROOT));
#endif
	EDBUG_RETURN(duplicate(s));
     }
#ifndef __EMX__
   if (theme[0] == '/')
#else
   if (_fnisabs(theme))
#endif
      ret = ExtractTheme(theme);
   if (!ret)
     {
	Esnprintf(s, sizeof(s), "%s/themes/%s", UserEDir(), theme);
	if (exists(s))
	   ret = ExtractTheme(s);
	else
	   badreason = _("Theme file/directory does not exist\n");
	if (!ret)
	  {
#ifndef __EMX__
	     Esnprintf(s, sizeof(s), "%s/themes/%s", ENLIGHTENMENT_ROOT, theme);
#else
	     Esnprintf(s, sizeof(s), "%s/themes/%s",
		       __XOS2RedirRoot(ENLIGHTENMENT_ROOT), theme);
#endif
	     if (exists(s))
		ret = ExtractTheme(s);
	     else
		badreason = _("Theme file/directory does not exist\n");
	     if (!ret)
	       {
		  ret = GetDefaultTheme();
		  badtheme = duplicate(theme);
	       }
	  }
     }
   EDBUG_RETURN(ret);
}

void
BadThemeDialog(void)
{
   char                s[1024];

   if (!badtheme)
      return;

   Esnprintf(s, sizeof(s),
	     _("The theme:\n" "%s\n"
	       "Is a badly formed theme package and is thus not being used.\n"
	       "Enlightenment has fallen back to using the DEFAULT theme.\n"
	       "\n" "The reason this theme is bad is:\n" "%s"), badtheme,
	     badreason);
   DIALOG_OK(_("Bad Theme"), s);
}
