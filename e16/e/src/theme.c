/*
 * Copyright (C) 2000-2004 Carsten Haitzler, Geoff Harrison and various contributors
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

#define ENABLE_THEME_SANITY_CHECKING 0

static char        *badtheme = NULL;
static char        *badreason = NULL;
static char         mustdel = 0;

#if ENABLE_THEME_SANITY_CHECKING

/* be paranoid and check for files being in theme */
static char
SanitiseThemeDir(char *dir)
{
   char                s[4096];

   return 1;
   Esnprintf(s, sizeof(s), "%s/%s", dir, "borders.cfg");
   if (!isfile(s))
     {
	badreason = _("Theme does not contain a borders.cfg file\n");
	return 0;
     }
   Esnprintf(s, sizeof(s), "%s/%s", dir, "buttons.cfg");
   if (!isfile(s))
     {
	badreason = _("Theme does not contain a buttons.cfg file\n");
	return 0;
     }
   Esnprintf(s, sizeof(s), "%s/%s", dir, "colormodifiers.cfg");
   if (!isfile(s))
     {
	badreason = _("Theme does not contain a colormodifiers.cfg file\n");
	return 0;
     }
   Esnprintf(s, sizeof(s), "%s/%s", dir, "cursors.cfg");
   if (!isfile(s))
     {
	badreason = _("Theme does not contain a cursors.cfg file\n");
	return 0;
     }
   Esnprintf(s, sizeof(s), "%s/%s", dir, "desktops.cfg");
   if (!isfile(s))
     {
	badreason = _("Theme does not contain a desktops.cfg file\n");
	return 0;
     }
   Esnprintf(s, sizeof(s), "%s/%s", dir, "imageclasses.cfg");
   if (!isfile(s))
     {
	badreason = _("Theme does not contain a imageclasses.cfg file\n");
	return 0;
     }
   Esnprintf(s, sizeof(s), "%s/%s", dir, "init.cfg");
   if (!isfile(s))
     {
	badreason = _("Theme does not contain a init.cfg file\n");
	return 0;
     }
   Esnprintf(s, sizeof(s), "%s/%s", dir, "menustyles.cfg");
   if (!isfile(s))
     {
	badreason = _("Theme does not contain a menustyles.cfg file\n");
	return 0;
     }
   Esnprintf(s, sizeof(s), "%s/%s", dir, "slideouts.cfg");
   if (!isfile(s))
     {
	badreason = _("Theme does not contain a slideouts.cfg file\n");
	return 0;
     }
   Esnprintf(s, sizeof(s), "%s/%s", dir, "sound.cfg");
   if (!isfile(s))
     {
	badreason = _("Theme does not contain a sound.cfg file\n");
	return 0;
     }
   Esnprintf(s, sizeof(s), "%s/%s", dir, "tooltips.cfg");
   if (!isfile(s))
     {
	badreason = _("Theme does not contain a tooltips.cfg file\n");
	return 0;
     }
   Esnprintf(s, sizeof(s), "%s/%s", dir, "windowmatches.cfg");
   if (!isfile(s))
     {
	badreason = _("Theme does not contain a windowmatches.cfg file\n");
	return 0;
     }
   Esnprintf(s, sizeof(s), "%s/%s", dir, "menus.cfg");
   if (isfile(s))
     {
	badreason = _("Theme contains a menus.cfg file\n");
	return 0;
     }
   Esnprintf(s, sizeof(s), "%s/%s", dir, "control.cfg");
   if (isfile(s))
     {
	badreason = _("Theme contains a control.cfg file\n");
	return 0;
     }
   Esnprintf(s, sizeof(s), "%s/%s", dir, "keybindings.cfg");
   if (isfile(s))
     {
	badreason = _("Theme contains a keybindings.cfg file\n");
	return 0;
     }
   return 1;
}

#else

#define SanitiseThemeDir(dir) 1

#endif /* ENABLE_THEME_SANITY_CHECKING */

static char        *
append_merge_dir(char *dir, char ***list, int *count)
{
   char                s[FILEPATH_LEN_MAX], ss[FILEPATH_LEN_MAX];
   char              **str = NULL, *def = NULL;
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

   Esnprintf(s, sizeof(s), "%s/themes", EDirUser());
   def = append_merge_dir(s, &list, &count);
   Esnprintf(s, sizeof(s), "%s/themes", EDirRoot());
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
ThemeGetDefault(void)
{
   char                s[FILEPATH_LEN_MAX], ss[FILEPATH_LEN_MAX];
   char               *def = NULL;
   int                 count;

   Esnprintf(ss, sizeof(ss), "%s/themes/DEFAULT", EDirUser());
   count = readlink(ss, s, sizeof(s));
   if ((exists(ss)) && (count > 0))
     {
	s[count] = 0;
	if (isabspath(s))
	   def = duplicate(s);
	else
	  {
	     Esnprintf(ss, sizeof(ss), "%s/themes/%s", EDirUser(), s);
	     def = duplicate(ss);
	  }
	return def;
     }

   Esnprintf(ss, sizeof(ss), "%s/themes/DEFAULT", EDirRoot());
   count = readlink(ss, s, sizeof(s));
   if ((exists(ss)) && (count > 0))
     {
	s[count] = 0;
	if (isabspath(s))
	   def = duplicate(s);
	else
	  {
	     Esnprintf(ss, sizeof(ss), "%s/themes/%s", EDirRoot(), s);
	     def = duplicate(ss);
	  }
	return def;
     }

   return NULL;
}

void
ThemeSetDefault(const char *theme)
{
/* os2 has no symlink,
 * but it doesn't matter since we have ~/.enlightenment/user_theme.cfg
 */
   char                ss[FILEPATH_LEN_MAX];

   Esnprintf(ss, sizeof(ss), "%s/themes/DEFAULT", EDirUser());
   if (exists(ss))
      rm(ss);
   if (theme)
      symlink(theme, ss);
}

static char        *
ThemeExtract(const char *theme)
{
   char                s[FILEPATH_LEN_MAX];
   char                th[FILEPATH_LEN_MAX];
   FILE               *f;
   unsigned char       buf[320];
   const char         *oktheme = NULL;
   char               *name;

   EDBUG(7, "ThemeExtract");

   mustdel = 0;

   /* its a directory - just use it "as is" */
   if (isdir(theme))
     {
	oktheme = theme;
	goto exit;
     }

   /* its a file - check its type */
   if (isfile(theme))
     {
	f = fopen(theme, "r");
	if (!f)
	   goto exit;

	fread(buf, 1, 320, f);
	fclose(f);

	/* make the temp dir */
	name = fileof(theme);
	Esnprintf(th, sizeof(th), "%s/themes/%s", EDirUser(), name);
	Efree(name);
	md(th);

	/* check magic numbers */
	if ((buf[0] == 31) && (buf[1] == 139))
	  {
	     /* gzipped tarball */
	     Esnprintf(s, sizeof(s),
		       "gzip -d -c < %s | (cd %s ; tar -xf -)", theme, th);
	  }
	else if ((buf[257] == 'u') && (buf[258] == 's')
		 && (buf[259] == 't') && (buf[260] == 'a') && (buf[261] == 'r'))
	  {
	     /* vanilla tarball */
	     Esnprintf(s, sizeof(s), "(cd %s ; tar -xf %s)", th, theme);
	  }

	/* exec the untar if tarred */
	system(s);

	oktheme = th;
     }

 exit:
   if (oktheme && SanitiseThemeDir(oktheme))
      EDBUG_RETURN(duplicate(oktheme));

   /* failed */
   ThemeCleanup();

   EDBUG_RETURN(NULL);
}

void
ThemeCleanup(void)
{
   if (!mustdel)
      return;

   /* We don't ever get here because mustdel is never set */
#if 0
   rmrf(themepath);
#endif
}

char               *
FindTheme(const char *theme)
{
   char                s[FILEPATH_LEN_MAX];
   char               *ret = NULL;

   EDBUG(6, "FindTheme");

   if (conf.theme.name)
      Efree(conf.theme.name);
   conf.theme.name = duplicate(theme);
   badreason = _("Unknown\n");

   if (!theme[0])
     {
	Esnprintf(s, sizeof(s), "%s/themes/DEFAULT", EDirRoot());
	EDBUG_RETURN(duplicate(s));
     }

   if (isabspath(theme))
      ret = ThemeExtract(theme);
   if (!ret)
     {
	Esnprintf(s, sizeof(s), "%s/themes/%s", EDirUser(), theme);
	if (exists(s))
	   ret = ThemeExtract(s);
	else
	   badreason = _("Theme file/directory does not exist\n");
	if (!ret)
	  {
	     Esnprintf(s, sizeof(s), "%s/themes/%s", EDirRoot(), theme);
	     if (exists(s))
		ret = ThemeExtract(s);
	     else
		badreason = _("Theme file/directory does not exist\n");
	     if (!ret)
	       {
		  ret = ThemeGetDefault();
		  badtheme = duplicate(theme);
	       }
	  }
     }
   EDBUG_RETURN(ret);
}

void
ThemeBadDialog(void)
{
   if (!badtheme)
      return;

   DialogOK(_("Bad Theme"),
	    _("The theme:\n" "%s\n"
	      "Is a badly formed theme package and is thus not being used.\n"
	      "Enlightenment has fallen back to using the DEFAULT theme.\n"
	      "\n" "The reason this theme is bad is:\n" "%s"),
	    badtheme, badreason);
}
