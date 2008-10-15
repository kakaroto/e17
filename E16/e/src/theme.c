/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2008 Kim Woelders
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
#include "emodule.h"
#include "file.h"
#include "session.h"

#define ENABLE_THEME_SANITY_CHECKING 0

#if ENABLE_THEME_SANITY_CHECKING
static char        *badtheme = NULL;
static char        *badreason = NULL;
#endif

static const char  *const theme_files[] = {
#if ENABLE_THEME_SANITY_CHECKING
   "borders.cfg",
   "buttons.cfg",
   "cursors.cfg",
   "desktops.cfg",
   "imageclasses.cfg",
#endif
   "init.cfg",
#if ENABLE_THEME_SANITY_CHECKING
   "menustyles.cfg",
   "slideouts.cfg",
   "tooltips.cfg",
#endif
   NULL
};

/* Check for files being in theme */
static int
SanitiseThemeDir(const char *dir)
{
   const char         *tf;
   int                 i;
   char                s[4096];

   for (i = 0; (tf = theme_files[i]) != NULL; i++)
     {
	Esnprintf(s, sizeof(s), "%s/%s", dir, tf);
	if (isfile(s))
	   continue;
#if 0
	Esnprintf(s, sizeof(s), _("Theme %s does not contain a %s file\n"), dir,
		  tf);
	badreason = Estrdup(s);
#endif
	return -1;
     }
   return 0;
}

static const char  *
ThemeCheckPath(const char *path)
{
   char                s1[FILEPATH_LEN_MAX];

   Esnprintf(s1, sizeof(s1), "%s/epplets/epplets.cfg", path);
   if (exists(s1))
      return path;		/* OK */

   return NULL;			/* Not OK */
}

char               *
ThemePathName(const char *path)
{
   const char         *p;
   char               *s;

   if (!path)
      return NULL;
   p = strrchr(path, '/');
   if (!p)
      return Estrdup(path);	/* Name only */
   if (strcmp(p + 1, "e16"))
      return Estrdup(p + 1);	/* Regular path */

   /* <path>/<themename>/e16 */
   s = strdup(path);
   s[p - path] = '\0';
   p = strrchr(s, '/');
   if (!p)
      return Estrdup(path);	/* Should not happen */
   p++;
   memmove(s, p, strlen(p) + 1);
   return s;
}

static char        *
append_merge_dir(char *dir, char ***list, int *count)
{
   char                s[FILEPATH_LEN_MAX], ss[FILEPATH_LEN_MAX];
   char              **str = NULL, *def = NULL;
   char                already, *tmp, *tmp2;
   int                 i, j, num, len;

   str = E_ls(dir, &num);
   if (!str)
      return NULL;

   for (i = 0; i < num; i++)
     {
	already = 0;
	for (j = 0; (j < (*count)) && (!already); j++)
	  {
	     tmp = fileof((*list)[j]);
	     tmp2 = fileof(str[i]);
	     if ((tmp != NULL) && (tmp2 != NULL) && (!strcmp(tmp, tmp2)))
		already = 1;
	     Efree(tmp);
	     Efree(tmp2);
	  }

	if (already)
	   continue;

	Esnprintf(ss, sizeof(ss), "%s/%s", dir, str[i]);

	if (!strcmp(str[i], "DEFAULT"))
	  {
	     memset(s, 0, sizeof(s));
	     len = readlink(ss, s, sizeof(s) - 1);
	     if (len > 0)
	       {
		  s[len] = '\0';	/* Redundant due to memset */
		  if (s[0] == '/')
		     def = Estrdup(s);
		  else
		    {
		       Esnprintf(s, sizeof(s), "%s/%s", dir, s);
		       def = Estrdup(s);
		    }
	       }
	  }
	else
	  {
	     if (isdir(ss))
	       {
		  if (!SanitiseThemeDir(ss))
		     goto got_one;
		  Esnprintf(ss, sizeof(ss), "%s/%s/e16", dir, str[i]);
		  if (!SanitiseThemeDir(ss))
		     goto got_one;
		  continue;
	       }
	     else if (isfile(ss))
	       {
		  if (strcmp(fileext(ss), "etheme"))
		     continue;
	       }
	     else
	       {
		  continue;
	       }

	   got_one:
	     (*count)++;
	     *list = EREALLOC(char *, *list, *count);

	     (*list)[(*count) - 1] = Estrdup(ss);
	  }
     }
   StrlistFree(str, num);

   return def;
}

char              **
ThemesList(int *number)
{
   char                paths[4096], *p, *s;
   char              **list;
   int                 count;

   Esnprintf(paths, sizeof(paths), "%s/themes:%s/themes:%s", EDirUser(),
	     EDirRoot(), (Conf.theme.extra_path) ? Conf.theme.extra_path : "");

   count = 0;
   list = NULL;
   for (p = paths; p; p = s)
     {
	s = strchr(p, ':');
	if (s)
	   *s++ = '\0';

	if (*p == ':')
	   continue;

	append_merge_dir(p, &list, &count);
     }

   *number = count;
   return list;
}

static char        *
ThemeGetPath(const char *path)
{
   const char         *s;
   char               *ss, s1[FILEPATH_LEN_MAX], s2[FILEPATH_LEN_MAX];
   int                 len;

   /* We only attempt to dereference a DEFAULT link */
   s = strstr(path, "/DEFAULT");
   if (s == NULL)
      return Estrdup(path);

   len = readlink(path, s1, sizeof(s1) - 1);
   if (len < 0)
      return Estrdup(path);

   s1[len] = '\0';
   if (isabspath(s1))
      return Estrdup(s1);

   Esnprintf(s2, sizeof(s2) - strlen(s1), "%s", path);
   ss = strstr(s2, "/DEFAULT");
   if (ss == NULL)
      return NULL;

   strcpy(ss + 1, s1);
   if (isdir(s2))
      return Estrdup(s2);

   /* We should never get here */
   return NULL;
}

static char        *
ThemeGetDefault(void)
{
   static const char  *const dts[] = {
      "DEFAULT", "winter", "BrushedMetal-Tigert", "ShinyMetal"
   };
   char                s[FILEPATH_LEN_MAX];
   const char         *path;
   char              **lst;
   int                 i, num;

   /* First, try out some defaults */
   num = sizeof(dts) / sizeof(char *);
   for (i = 0; i < num; i++)
     {
	Esnprintf(s, sizeof(s), "%s/themes/%s", EDirUser(), dts[i]);
	path = ThemeCheckPath(s);
	if (path)
	   return ThemeGetPath(path);

	Esnprintf(s, sizeof(s), "%s/themes/%s", EDirRoot(), dts[i]);
	path = ThemeCheckPath(s);
	if (path)
	   return ThemeGetPath(path);
     }

   /* Then, try out all installed themes */
   path = NULL;
   lst = ThemesList(&num);
   if (lst)
     {
	for (i = 0; i < num; i++)
	  {
	     path = ThemeCheckPath(lst[i]);
	     if (path)
		break;
	  }
	StrlistFree(lst, num);
     }
   if (path)
      return ThemeGetPath(path);

   return NULL;
}

static void
ThemeCleanup(void)
{
   /* TBD */
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

   /* its a directory - just use it "as is" */
   if (isdir(theme))
     {
	oktheme = theme;
	goto done;
     }

   /* its a file - check its type */
   if (isfile(theme))
     {
	f = fopen(theme, "r");
	if (!f)
	   goto done;

	fread(buf, 1, 320, f);
	fclose(f);

	/* make the temp dir */
	name = fileof(theme);
	Esnprintf(th, sizeof(th), "%s/themes/%s", EDirUser(), name);
	Efree(name);
	E_md(th);

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
	else
	   goto done;

	/* exec the untar if tarred */
	system(s);

	oktheme = th;
     }

 done:
   if (oktheme && !SanitiseThemeDir(oktheme))
      return Estrdup(oktheme);

   /* failed */
   ThemeCleanup();

   return NULL;
}

static char        *
ThemeFind(const char *theme)
{
   char                paths[4096], tdir[4096], *p, *s;
   char               *ret;

#if ENABLE_THEME_SANITY_CHECKING
   badreason = _("Unknown\n");
#endif

   if (!theme || !theme[0])
      return ThemeGetDefault();

   ret = NULL;
   if (isabspath(theme))
      ret = ThemeExtract(theme);
   if (ret)
      return ret;

   Esnprintf(paths, sizeof(paths), "%s/themes:%s/themes:%s", EDirUser(),
	     EDirRoot(), (Conf.theme.extra_path) ? Conf.theme.extra_path : "");

   for (p = paths; p; p = s)
     {
	s = strchr(p, ':');
	if (s)
	   *s++ = '\0';

	if (*p == ':')
	   continue;

	Esnprintf(tdir, sizeof(tdir), "%s/%s", p, theme);
	ret = ThemeExtract(tdir);
	if (ret)
	   return ret;
	Esnprintf(tdir, sizeof(tdir), "%s/%s/e16", p, theme);
	ret = ThemeExtract(tdir);
	if (ret)
	   return ret;
     }

   if (strcmp(theme, "-"))
      ret = ThemeGetDefault();
#if ENABLE_THEME_SANITY_CHECKING
   badtheme = Estrdup(theme);
#endif

   return ret;
}

#if 0
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
#endif

/*
 * Theme module
 */

void
ThemePathFind(void)
{
   char               *theme;

   /*
    * Conf.theme.name is read from the configuration.
    * Mode.theme.path may be assigned on the command line.
    */
   theme = (Mode.theme.path) ? Mode.theme.path : Conf.theme.name;
   theme = ThemeFind(theme);

   if (!theme)
     {
	Alert(_("No themes were found in the default theme directory:\n"
		" %s/themes/\n"
		"or in the user theme directory:\n"
		" %s/themes/\n"
		"Proceeding from here is mostly pointless.\n"),
	      EDirRoot(), EDirUser());
     }

   Efree(Conf.theme.name);
   Conf.theme.name = ThemePathName(theme);

   Efree(Mode.theme.path);
   Mode.theme.path = (theme) ? theme : Estrdup("-");
}

static void
ThemesSighan(int sig, void *prm __UNUSED__)
{
   switch (sig)
     {
     case ESIGNAL_CONFIGURE:
	break;
     case ESIGNAL_EXIT:
	ThemeCleanup();
	break;
     }
}

static void
ThemesIpc(const char *params)
{
   const char         *p;
   char                cmd[128], prm[128];
   int                 len;

   cmd[0] = prm[0] = '\0';
   p = params;
   if (p)
     {
	len = 0;
	sscanf(p, "%100s %100s %n", cmd, prm, &len);
	p += len;
     }

   if (!p || cmd[0] == '?')
     {
	IpcPrintf("Name: %s\n", Conf.theme.name);
	IpcPrintf("Full: %s\n", Mode.theme.path);
	IpcPrintf("Default: %s\n", ThemeGetDefault());
	IpcPrintf("Path: %s/themes:%s/themes:%s\n", EDirUser(), EDirRoot(),
		  (Conf.theme.extra_path) ? Conf.theme.extra_path : "");
     }
   else if (!strncmp(cmd, "list", 2))
     {
	char              **lst;
	int                 i, num;

	lst = ThemesList(&num);
	if (!lst)
	   return;
	for (i = 0; i < num; i++)
	   IpcPrintf("%s\n", lst[i]);
	StrlistFree(lst, num);
     }
   else if (!strcmp(cmd, "use"))
     {
	/* FIXME - ThemeCheckIfValid(s) */
	SessionExit(EEXIT_THEME, prm);
     }
}

static const IpcItem ThemeIpcArray[] = {
   {
    ThemesIpc,
    "theme", "th",
    "Theme commands",
    "  theme             Show current theme\n"
    "  theme list        Show all themes\n"
    "  theme use <name>  Switch to theme <name>\n"}
   ,
};
#define N_IPC_FUNCS (sizeof(ThemeIpcArray)/sizeof(IpcItem))

static const CfgItem ThemeCfgItems[] = {
   CFG_ITEM_STR(Conf.theme, name),
   CFG_ITEM_STR(Conf.theme, extra_path),
   CFG_ITEM_STR(Conf.theme, ttfont_path),
   CFG_ITEM_BOOL(Conf.theme, use_alt_font_cfg, 1),
   CFG_ITEM_STR(Conf.theme, font_cfg),
};
#define N_CFG_ITEMS (sizeof(ThemeCfgItems)/sizeof(CfgItem))

/*
 * Module descriptor
 */
extern const EModule ModTheme;
const EModule       ModTheme = {
   "theme", "th",
   ThemesSighan,
   {N_IPC_FUNCS, ThemeIpcArray},
   {N_CFG_ITEMS, ThemeCfgItems}
};
