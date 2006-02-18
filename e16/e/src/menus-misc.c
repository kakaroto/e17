/*
 * Copyright (C) 2000-2006 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2006 Kim Woelders
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
#include "backgrounds.h"
#include "borders.h"
#include "desktops.h"
#include "ewins.h"
#include "groups.h"
#include "iclass.h"
#include "menus.h"
#include <errno.h>
#include <sys/stat.h>

static Menu        *MenuCreateFromFlatFile(const char *name, Menu * parent,
					   MenuStyle * ms, const char *file);
static Menu        *MenuCreateFromDirectory(const char *name, Menu * parent,
					    MenuStyle * ms, const char *dir);

static MenuItem    *
MenuItemCreateFromBackground(const char *bgid, const char *file)
{
   MenuItem           *mi;
   Background         *bg;
   ImageClass         *ic;
   char                thumb[1024], buf[1024];

   bg = BrackgroundCreateFromImage(bgid, file, thumb, sizeof(thumb));
   if (!bg)
      return NULL;

   ic = ImageclassCreateSimple("`", thumb);

   Esnprintf(buf, sizeof(buf), "bg use %s", bgid);

   mi = MenuItemCreate(NULL, ic, buf, NULL);

   return mi;
}

static int
MenuLoadFromDirectory(Menu * m)
{
   Progressbar        *p = NULL;
   Menu               *mm;
   int                 i, num;
   const char         *dir;
   char              **list, s[4096], ss[4096], cs[4096];
   const char         *ext;
   MenuItem           *mi;
   struct stat         st;
   const char         *chmap =
      "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-_";
   FILE               *f;
   time_t              lastmod;

   dir = MenuGetData(m);
   lastmod = moddate(dir);
   if (lastmod <= MenuGetTimestamp(m))
      return 0;
   MenuSetTimestamp(m, lastmod);

   MenuEmpty(m, 0);

   cs[0] = '\0';		/* FIXME - Check this */

   if (stat(dir, &st) >= 0)
     {
	int                 aa, bb, cc;

	aa = (int)st.st_ino;
	bb = filedev_map((int)st.st_dev);
	cc = 0;
	if (st.st_mtime > st.st_ctime)
	   cc = st.st_mtime;
	else
	   cc = st.st_ctime;
	Esnprintf(cs, sizeof(cs),
		  "%s/cached/img/.%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
		  EDirUserCache(), chmap[(aa >> 0) & 0x3f],
		  chmap[(aa >> 6) & 0x3f], chmap[(aa >> 12) & 0x3f],
		  chmap[(aa >> 18) & 0x3f], chmap[(aa >> 24) & 0x3f],
		  chmap[(aa >> 28) & 0x3f], chmap[(bb >> 0) & 0x3f],
		  chmap[(bb >> 6) & 0x3f], chmap[(bb >> 12) & 0x3f],
		  chmap[(bb >> 18) & 0x3f], chmap[(bb >> 24) & 0x3f],
		  chmap[(bb >> 28) & 0x3f], chmap[(cc >> 0) & 0x3f],
		  chmap[(cc >> 6) & 0x3f], chmap[(cc >> 12) & 0x3f],
		  chmap[(cc >> 18) & 0x3f], chmap[(cc >> 24) & 0x3f],
		  chmap[(cc >> 28) & 0x3f]);
	/* cached dir listing - use it */
	if (exists(cs))
	  {
	     f = fopen(cs, "r");
	     if (!f)
		return 1;
	     while (fgets(s, sizeof(s), f))
	       {
		  s[strlen(s) - 1] = 0;
		  word(s, 1, ss);
		  if (!strcmp(ss, "BG"))
		    {
		       char                s2[4096], s3[512];

		       word(s, 2, s2);
		       word(s, 3, s3);
		       Esnprintf(s, sizeof(s), "%s/%s", dir, s2);
		       mi = MenuItemCreateFromBackground(s3, s);
		       if (mi)
			  MenuAddItem(m, mi);
		    }
		  else if (!strcmp(ss, "EXE"))
		    {
		       word(s, 2, ss);
		       Esnprintf(s, sizeof(s), "exec %s/%s", dir, ss);
		       mi = MenuItemCreate(NULL, NULL, s, NULL);
		       MenuAddItem(m, mi);
		    }
		  else if (!strcmp(ss, "DIR"))
		    {
		       char                tmp[4096];

		       word(s, 2, tmp);
		       Esnprintf(s, sizeof(s), "%s/%s:%s", dir, tmp,
				 MenuGetName(m));
		       Esnprintf(ss, sizeof(ss), "%s/%s", dir, tmp);
		       mm = MenuCreateFromDirectory(s, m, NULL, ss);
		       mi = MenuItemCreate(tmp, NULL, NULL, mm);
		       MenuAddItem(m, mi);
		    }
	       }
	     fclose(f);
	     return 1;
	  }
     }

   Esnprintf(s, sizeof(s), "Scanning %s", dir);

   if (!Mode.wm.restart)
      p = ProgressbarCreate(s, 600, 16);
   if (p)
      ProgressbarShow(p);

   f = fopen(cs, "w");

   list = E_ls(dir, &num);
   for (i = 0; i < num; i++)
     {
	if (p)
	   ProgressbarSet(p, (i * 100) / num);
	Esnprintf(ss, sizeof(ss), "%s/%s", dir, list[i]);
	/* skip "dot" files and dirs - senisble */
	if ((*(list[i]) == '.') || (stat(ss, &st) < 0))
	   continue;

	ext = FileExtension(ss);
	if (S_ISDIR(st.st_mode))
	  {
	     Esnprintf(s, sizeof(s), "%s/%s:%s", dir, list[i], MenuGetName(m));
	     mm = MenuCreateFromDirectory(s, m, NULL, ss);
	     mi = MenuItemCreate(list[i], NULL, NULL, mm);
	     MenuAddItem(m, mi);
	     if (f)
		fprintf(f, "DIR %s\n", list[i]);
	  }
/* that's it - people are stupid and have executable images and just */
/* don't get it - so I'm disablign this to save people from their own */
/* stupidity */
/*           else if (st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))
 * {
 * mi = MenuItemCreate(list[i], NULL, ss, NULL);
 * MenuAddItem(m, mi);
 * if (f)
 * fprintf(f, "EXE %s\n", list[i]);
 * }
 */
	else if ((!strcmp(ext, "jpg")) || (!strcmp(ext, "JPG"))
		 || (!strcmp(ext, "jpeg")) || (!strcmp(ext, "Jpeg"))
		 || (!strcmp(ext, "JPEG")) || (!strcmp(ext, "Jpg"))
		 || (!strcmp(ext, "gif")) || (!strcmp(ext, "Gif"))
		 || (!strcmp(ext, "GIF")) || (!strcmp(ext, "png"))
		 || (!strcmp(ext, "Png")) || (!strcmp(ext, "PNG"))
		 || (!strcmp(ext, "tif")) || (!strcmp(ext, "Tif"))
		 || (!strcmp(ext, "TIFF")) || (!strcmp(ext, "tiff"))
		 || (!strcmp(ext, "Tiff")) || (!strcmp(ext, "TIFF"))
		 || (!strcmp(ext, "xpm")) || (!strcmp(ext, "Xpm"))
		 || (!strcmp(ext, "XPM")) || (!strcmp(ext, "ppm"))
		 || (!strcmp(ext, "PPM")) || (!strcmp(ext, "pgm"))
		 || (!strcmp(ext, "PGM")) || (!strcmp(ext, "pnm"))
		 || (!strcmp(ext, "PNM")) || (!strcmp(ext, "bmp"))
		 || (!strcmp(ext, "Bmp")) || (!strcmp(ext, "BMP")))
	  {
	     char                s3[512];
	     int                 aa, bb, cc;

	     aa = (int)st.st_ino;
	     bb = filedev_map((int)st.st_dev);
	     cc = 0;
	     if (st.st_mtime > st.st_ctime)
		cc = st.st_mtime;
	     else
		cc = st.st_ctime;
	     Esnprintf(s3, sizeof(s3),
		       ".%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
		       chmap[(aa >> 0) & 0x3f], chmap[(aa >> 6) & 0x3f],
		       chmap[(aa >> 12) & 0x3f], chmap[(aa >> 18) & 0x3f],
		       chmap[(aa >> 24) & 0x3f], chmap[(aa >> 28) & 0x3f],
		       chmap[(bb >> 0) & 0x3f], chmap[(bb >> 6) & 0x3f],
		       chmap[(bb >> 12) & 0x3f], chmap[(bb >> 18) & 0x3f],
		       chmap[(bb >> 24) & 0x3f], chmap[(bb >> 28) & 0x3f],
		       chmap[(cc >> 0) & 0x3f], chmap[(cc >> 6) & 0x3f],
		       chmap[(cc >> 12) & 0x3f], chmap[(cc >> 18) & 0x3f],
		       chmap[(cc >> 24) & 0x3f], chmap[(cc >> 28) & 0x3f]);

	     mi = MenuItemCreateFromBackground(s3, ss);
	     if (mi)
	       {
		  MenuAddItem(m, mi);

		  if (f)
		     fprintf(f, "BG %s %s\n", list[i], s3);
	       }
	  }
     }
   if (f)
      fclose(f);
   if (p)
      ProgressbarDestroy(p);
   if (list)
      StrlistFree(list, num);

   return 1;
}

static Menu        *
MenuCreateFromDirectory(const char *name, Menu * parent, MenuStyle * ms,
			const char *dir)
{
   static int          calls = 0;
   Menu               *m;

   if (calls > 32)
      return NULL;
   calls++;

   m = MenuCreate(name, NULL, parent, ms);
   MenuSetData(m, Estrdup(dir));
   MenuSetLoader(m, MenuLoadFromDirectory);

   /* Hmmm... Make sure background dirs are scanned during startup */
   if (Mode.wm.startup)
      MenuLoadFromDirectory(m);

   calls--;

   return m;
}

static void
FillFlatFileMenu(Menu * m, const char *name, const char *file)
{
   FILE               *f;
   char                first = 1;
   char                s[4096];
   int                 count;
   unsigned int        len;

   f = fopen(file, "r");
   if (!f)
     {
	fprintf(stderr, "Unable to open menu file %s -- %s\n", file,
		strerror(errno));
	return;
     }

   MenuSetIconSize(m, 0);	/* Scale to default */

   count = 0;
   while (fgets(s, 4096, f))
     {
	if (s[0] == '#')
	   continue;
	len = strlen(s);
	while (len && (s[len - 1] == '\n' || s[len - 1] == '\r'))
	   len--;
	if (len == 0)
	   continue;
	s[len] = '\0';

	if (first)
	  {
	     char               *wd;

	     wd = field(s, 0);
	     if (wd)
	       {
		  MenuSetTitle(m, wd);
		  Efree(wd);
	       }
	     first = 0;
	  }
	else
	  {
	     char               *txt = NULL, *icon = NULL, *act = NULL;
	     char               *params = NULL, *tmp = NULL, wd[4096];

	     MenuItem           *mi;
	     ImageClass         *icc = NULL;
	     Menu               *mm;

	     txt = field(s, 0);
	     icon = field(s, 1);
	     act = field(s, 2);
	     params = field(s, 3);
	     tmp = NULL;
	     if (icon && exists(icon))
	       {
		  Esnprintf(wd, sizeof(wd), "__FM.%s", icon);
		  icc = ImageclassFind(wd, 0);
		  if (!icc)
		     icc = ImageclassCreateSimple(wd, icon);
	       }
	     if ((act) && (!strcmp(act, "exec")) && (params))
	       {
		  word(params, 1, wd);
		  tmp = pathtoexec(wd);
		  if (tmp)
		    {
		       Efree(tmp);
		       Esnprintf(s, sizeof(s), "exec %s", params);
		       mi = MenuItemCreate(txt, icc, s, NULL);
		       MenuAddItem(m, mi);
		    }
	       }
	     else if ((act) && (!strcmp(act, "menu")) && (params))
	       {
		  Esnprintf(wd, sizeof(wd), "__FM.%s.%i", name, count++);
		  mm = MenuCreateFromFlatFile(wd, m, NULL, params);
		  if (mm)
		    {
		       mi = MenuItemCreate(txt, icc, NULL, mm);
		       MenuAddItem(m, mi);
		    }
	       }
	     else if (act)
	       {
		  mi = MenuItemCreate(txt, icc, act, NULL);
		  MenuAddItem(m, mi);
	       }
	     if (txt)
		Efree(txt);
	     if (icon)
		Efree(icon);
	     if (act)
		Efree(act);
	     if (params)
		Efree(params);
	  }
     }
   fclose(f);
}

static int
MenuLoadFromFlatFile(Menu * m)
{
   const char         *ff;
   time_t              lastmod;

   ff = MenuGetData(m);
   lastmod = moddate(ff);
   if (lastmod <= MenuGetTimestamp(m))
      return 0;
   MenuSetTimestamp(m, lastmod);

   MenuEmpty(m, 0);
   FillFlatFileMenu(m, MenuGetName(m), ff);

   return 1;
}

static Menu        *
MenuCreateFromFlatFile(const char *name, Menu * parent, MenuStyle * ms,
		       const char *file)
{
   Menu               *m = NULL;
   char               *ff;
   static int          calls = 0;

   if (calls > 32)
      return NULL;
   calls++;

   ff = FindFile(file, NULL, 0);
   if (!ff)
      goto done;

   m = MenuCreate(name, NULL, parent, ms);
   MenuSetData(m, ff);
   MenuSetLoader(m, MenuLoadFromFlatFile);

 done:
   calls--;

   return m;
}

static Menu        *
MenuCreateFromGnome(const char *name, Menu * parent, MenuStyle * ms,
		    const char *dir)
{
   Menu               *m, *mm;
   int                 i, num;
   char              **list, s[4096], ss[4096];
   MenuItem           *mi;
   FILE               *f;
   char                name_buf[20];

   if (Mode.locale.lang)
      Esnprintf(name_buf, sizeof(name_buf), "Name[%s]=", Mode.locale.lang);
   else
      name_buf[0] = '\0';

   m = MenuCreate(name, NULL, parent, ms);

   list = E_ls(dir, &num);
   for (i = 0; i < num; i++)
     {
	if ((strcmp(list[i], ".")) && (strcmp(list[i], "..")))
	  {
	     Esnprintf(ss, sizeof(ss), "%s/%s", dir, list[i]);
	     if (isdir(ss))
	       {
		  Esnprintf(s, sizeof(s), "%s/%s:%s", dir, list[i], name);
		  mm = MenuCreateFromGnome(s, m, ms, ss);

		  name = list[i];
		  if (name_buf[0])
		    {
		       Esnprintf(s, sizeof(s), "%s/.directory", ss);
		       if ((f = fopen(s, "r")) != NULL)
			 {
			    while (fgets(s, sizeof(s), f))
			      {
				 if (!strncmp(s, name_buf, strlen(name_buf)))
				   {
				      if (s[strlen(s) - 1] == '\n')
					 s[strlen(s) - 1] = 0;
				      name = &(s[strlen(name_buf)]);
				      break;
				   }
			      }
			    fclose(f);
			 }
		    }
		  mi = MenuItemCreate(name, NULL, NULL, mm);
		  MenuAddItem(m, mi);
	       }
	     else
	       {
		  f = fopen(ss, "r");
		  if (f)
		    {
		       char               *iname, *exec, *texec, *en_name;
		       char               *tmp;

		       iname = exec = texec = en_name = NULL;

		       while (fgets(s, sizeof(s), f))
			 {
			    if (s[strlen(s) - 1] == '\n')
			       s[strlen(s) - 1] = 0;
			    if (!strncmp(s, "Name=", strlen("Name=")))
			       en_name = Estrdup(&(s[strlen("Name=")]));
			    else if (name_buf[0]
				     && !strncmp(s, name_buf, strlen(name_buf)))
			       iname = Estrdup(&(s[strlen(name_buf)]));
			    else if (!strncmp
				     (s, "TryExec=", strlen("TryExec=")))
			       texec = Estrdup(&(s[strlen("TryExec=")]));
			    else if (!strncmp(s, "Exec=", strlen("Exec=")))
			       exec = Estrdup(&(s[strlen("Exec=")]));
			 }
		       if (iname)
			 {
			    if (en_name)
			       Efree(en_name);
			 }
		       else
			 {
			    if (en_name)
			       iname = en_name;
			 }
		       fclose(f);
		       if ((iname) && (exec))
			 {
			    tmp = NULL;
			    if (texec)
			       tmp = pathtoexec(texec);
			    if ((tmp) || (!texec))
			      {
				 if (tmp)
				    Efree(tmp);

				 Esnprintf(s, sizeof(s), "exec %s", exec);
				 mi = MenuItemCreate(iname, NULL, s, NULL);
				 MenuAddItem(m, mi);
			      }
			 }
		       if (iname)
			  Efree(iname);
		       if (exec)
			  Efree(exec);
		       if (texec)
			  Efree(texec);
		    }
	       }
	  }
     }
   if (list)
      StrlistFree(list, num);

   return m;
}

static int
MenuLoadFromThemes(Menu * m)
{
   char              **lst;
   int                 i, num;
   char                ss[4096], *s;
   MenuItem           *mi;

   if (MenuGetTimestamp(m))
      return 0;
   MenuSetTimestamp(m, 1);

   lst = ThemesList(&num);
   for (i = 0; i < num; i++)
     {
	s = fullfileof(lst[i]);
	Esnprintf(ss, sizeof(ss), "theme use %s", s);
	mi = MenuItemCreate(s, NULL, ss, NULL);
	Efree(s);
	MenuAddItem(m, mi);
     }
   if (lst)
      StrlistFree(lst, i);

   return 1;
}

static Menu        *
MenuCreateFromThemes(const char *name, MenuStyle * ms)
{
   Menu               *m;

   m = MenuCreate(name, NULL, NULL, ms);
   MenuSetTitle(m, _("Themes"));
   MenuSetInternal(m);
   MenuSetLoader(m, MenuLoadFromThemes);

   return m;
}

static int
BorderNameCompare(void *b1, void *b2)
{
   if (b1 && b2)
      return strcmp(BorderGetName(b1), BorderGetName(b2));

   return 0;
}

static Menu        *
MenuCreateFromBorders(const char *name, MenuStyle * ms)
{
   char                s[128];
   Menu               *m;
   Border            **lst;
   int                 i, num;
   MenuItem           *mi;

   m = MenuCreate(name, NULL, NULL, ms);

   lst = BordersGetList(&num);
   if (lst)
      Quicksort((void **)lst, 0, num - 1, BorderNameCompare);
   for (i = 0; i < num; i++)
     {
	/* if its not internal (ie doesnt start with _ ) */
	if (lst[i]->name[0] != '_')
	  {
	     Esnprintf(s, sizeof(s), "wop %% bo %s", lst[i]->name);
	     mi = MenuItemCreate(lst[i]->name, NULL, s, NULL);
	     MenuAddItem(m, mi);
	  }
     }
   if (lst)
      Efree(lst);

   return m;
}

static int
MenuCheckShowEwinDesk(EWin * ewin, void *prm)
{
   if (!EwinGetName(ewin) || ewin->props.skip_winlist)
      return 0;
   return prm == NULL || EwinGetDesk(ewin) == prm;
}

static void
MenuLoadFromEwins(Menu * m, int (*f) (EWin * ewin, void *prm), void *prm)
{
   EWin               *const *lst;
   int                 i, num;
   char                s[256];
   MenuItem           *mi;

   lst = EwinListGetAll(&num);
   for (i = 0; i < num; i++)
     {
	if (!f(lst[i], prm))
	   continue;

	Esnprintf(s, sizeof(s), "wop %#lx focus", _EwinGetClientXwin(lst[i]));
	mi = MenuItemCreate(EwinGetName(lst[i]), NULL, s, NULL);
	MenuAddItem(m, mi);
     }
}

static int
MenuLoadFromAllEwins(Menu * m)
{
   MenuEmpty(m, 0);
   MenuLoadFromEwins(m, MenuCheckShowEwinDesk, NULL);
   return 1;
}

static Menu        *
MenuCreateFromAllEWins(const char *name, MenuStyle * ms)
{
   Menu               *m;

   m = MenuCreate(name, NULL, NULL, ms);
   MenuSetTitle(m, _("Window List"));
   MenuSetInternal(m);
   MenuSetDynamic(m);
   MenuSetLoader(m, MenuLoadFromAllEwins);

   return m;
}

static int
MenuLoadFromDesktops(Menu * m)
{
   Menu               *mm;
   unsigned int        i;
   char                s[256];
   MenuItem           *mi;

   MenuEmpty(m, 0);

   for (i = 0; i < DesksGetNumber(); i++)
     {
	mm = MenuCreate("__SUBMENUDESK_E", NULL, m, NULL);
	Esnprintf(s, sizeof(s), "desk goto %i", i);
	mi = MenuItemCreate(_("Go to this Desktop"), NULL, s, NULL);
	MenuAddItem(mm, mi);
	MenuLoadFromEwins(mm, MenuCheckShowEwinDesk, DeskGet(i));

	Esnprintf(s, sizeof(s), _("Desktop %i"), i);
	mi = MenuItemCreate(s, NULL, NULL, mm);
	MenuAddItem(m, mi);
     }

   return 1;
}

static Menu        *
MenuCreateFromDesktops(const char *name, MenuStyle * ms)
{
   Menu               *m;

   m = MenuCreate(name, NULL, NULL, ms);
   MenuSetTitle(m, _("Desks"));
   MenuSetInternal(m);
   MenuSetDynamic(m);
   MenuSetLoader(m, MenuLoadFromDesktops);

   return m;
}

static int
MenuLoadFromGroups(Menu * m)
{
   Menu               *mm;
   Group             **lst;
   int                 i, j, num;
   char                s[256];
   MenuItem           *mi;

   MenuEmpty(m, 0);

   lst = GroupsGetList(&num);
   if (!lst)
      return 1;

   for (i = 0; i < num; i++)
     {
	mm = MenuCreate("__SUBMENUGROUP_E", NULL, m, NULL);

	Esnprintf(s, sizeof(s), "gop %li showhide",
		  _EwinGetClientXwin(lst[i]->members[0]));
	mi = MenuItemCreate(_("Show/Hide this group"), NULL, s, NULL);

	Esnprintf(s, sizeof(s), "wop %#lx ic",
		  _EwinGetClientXwin(lst[i]->members[0]));
	MenuAddItem(mm, mi);
	mi = MenuItemCreate(_("Iconify this group"), NULL, s, NULL);
	MenuAddItem(mm, mi);

	for (j = 0; j < lst[i]->num_members; j++)
	  {
	     Esnprintf(s, sizeof(s), "wop %#lx focus",
		       _EwinGetClientXwin(lst[i]->members[j]));
	     mi = MenuItemCreate(EwinGetName(lst[i]->members[j]), NULL,
				 s, NULL);
	     MenuAddItem(mm, mi);
	  }
	Esnprintf(s, sizeof(s), _("Group %i"), i);
	mi = MenuItemCreate(s, NULL, NULL, mm);
	MenuAddItem(m, mi);
     }
   Efree(lst);

   return 1;
}

static Menu        *
MenuCreateFromGroups(const char *name, MenuStyle * ms)
{
   Menu               *m;

   m = MenuCreate(name, NULL, NULL, ms);
   MenuSetTitle(m, _("Groups"));
   MenuSetInternal(m);
   MenuSetDynamic(m);
   MenuSetLoader(m, MenuLoadFromGroups);

   return m;
}

#if 0				/* Not finished */
Menu               *
MenuCreateMoveToDesktop(char *name, Menu * parent, MenuStyle * ms)
{
   Menu               *m;
   int                 i;
   char                s1[256], s2[256];

   MenuItem           *mi;

   m = MenuCreate(name, NULL, parent, ms);

   for (i = 0; i < Mode.numdesktops; i++)
     {
	Esnprintf(s1, sizeof(s1), _("Desktop %i"), i);
	Esnprintf(s2, sizeof(s2), "%i", i);
	mi = MenuItemCreate(s1, NULL, s2, NULL);
	MenuAddItem(m, mi);
     }

   return m;
}
#endif

Menu               *
MenusCreateInternal(const char *type, const char *name, const char *style,
		    const char *prm)
{
   Menu               *m;
   MenuStyle          *ms;

   m = NULL;
   ms = NULL;
   if (style)
      ms = MenuStyleFind(style);

   if (!strcmp(type, "file"))
     {
	m = MenuCreateFromFlatFile(name, NULL, ms, prm);
     }
   else if (!strcmp(type, "dirscan"))
     {
	SoundPlay("SOUND_SCANNING");
	m = MenuCreateFromDirectory(name, NULL, ms, prm);
     }
   else if (!strcmp(type, "gnome"))
     {
	m = MenuCreateFromGnome(name, NULL, ms, prm);
     }
   else if (!strcmp(type, "borders"))
     {
	m = MenuCreateFromBorders(name, ms);
     }
   else if (!strcmp(type, "themes"))
     {
	m = MenuCreateFromThemes(name, ms);
     }
   else if (!strcmp(type, "windowlist"))
     {
	m = MenuCreateFromAllEWins(name, ms);
     }
   else if (!strcmp(type, "deskmenu"))
     {
	m = MenuCreateFromDesktops(name, ms);
     }
   else if (!strcmp(type, "groupmenu"))
     {
	m = MenuCreateFromGroups(name, ms);
     }

   return m;
}
