#include "E.h"

char               *
append_merge_dir(char *dir, char ***list, int *count)
{
   char                s[FILEPATH_LEN_MAX], ss[FILEPATH_LEN_MAX], **str = NULL,
                      *def = NULL;
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
		    }
		  else
		    {
		       ok = 0;

		       Esnprintf(s, sizeof(s), "%s/%s", dir, str[i]);
		       if ((isdir(s)) && (SanitiseThemeDir(s)))
			  ok = 1;
		       else if ((isfile(s)) && (FileExtension(s)) &&
				(!strcmp(FileExtension(s), "etheme")))
			  ok = 1;
		       if (ok)
			 {
			    (*count)++;
			    (*list) = Erealloc(*list, (*count) * sizeof(char *));

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
   char                s[FILEPATH_LEN_MAX], **list = NULL, *def = NULL,
                      *def2 = NULL;
   int                 count = 0;

   Esnprintf(s, sizeof(s), "%s/themes", UserEDir());
   def = append_merge_dir(s, &list, &count);
   Esnprintf(s, sizeof(s), "%s/themes", ENLIGHTENMENT_ROOT);
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
   if (!def)
     {
	Esnprintf(ss, sizeof(ss), "%s/themes/DEFAULT", ENLIGHTENMENT_ROOT);
	count = readlink(ss, s, sizeof(s));
	if ((exists(ss)) && (count > 0))
	  {
	     s[count] = 0;
	     if (s[0] == '/')
		def = duplicate(s);
	     else
	       {
		  Esnprintf(ss, sizeof(ss), "%s/themes/%s", ENLIGHTENMENT_ROOT, s);
		  def = duplicate(ss);
	       }
	  }
     }
   return def;
}

void
SetDefaultTheme(char *theme)
{
   char                ss[FILEPATH_LEN_MAX];

   Esnprintf(ss, sizeof(ss), "%s/themes/DEFAULT", UserEDir());
   if (exists(ss))
      rm(ss);
   if (theme)
      symlink(theme, ss);
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
	     Esnprintf(th, sizeof(th), "%s/themes/%s",
		       UserEDir(), themename);
	     Efree(themename);
	     md(th);
	     /* check magic numbers */
	     if ((buf[0] == 31) && (buf[1] == 139))
	       {
		  /*gzipped tarball */
		  Esnprintf(s, sizeof(s),
			  "gzip -d -c < %s | (cd %s ; tar -xf -)", theme, th);
	       }
	     else if ((buf[257] == 'u') && (buf[258] == 's') && (buf[259] == 't') &&
		      (buf[260] == 'a') && (buf[261] == 'r'))
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

		  Esnprintf(sss, sizeof(sss), "/bin/rm -rf %s", themepath);
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
   if (!theme[0])
     {
	Esnprintf(s, sizeof(s), "%s/themes/DEFAULT", ENLIGHTENMENT_ROOT);
	EDBUG_RETURN(duplicate(s));
     }
   if (theme[0] == '/')
      ret = ExtractTheme(theme);
   if (!ret)
     {
	Esnprintf(s, sizeof(s), "%s/themes/%s", UserEDir(), theme);
	if (exists(s))
	   ret = ExtractTheme(s);
	if (!ret)
	  {
	     Esnprintf(s, sizeof(s), "%s/themes/%s", ENLIGHTENMENT_ROOT, theme);
	     if (exists(s))
		ret = ExtractTheme(s);
	     if (!ret)
		ret = GetDefaultTheme();
	  }
     }
   EDBUG_RETURN(ret);
}
