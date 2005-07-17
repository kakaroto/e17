/*
 * Copyright (C) 2000-2005 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2005 Kim Woelders
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
#include "conf.h"
#include "ewins.h"
#include "xwin.h"
#include <errno.h>
#include <sys/stat.h>

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

Menu               *
MenuCreateFromDirectory(const char *name, Menu * parent, MenuStyle * ms,
			const char *dir)
{
   Progressbar        *p = NULL;
   Menu               *m, *mm;
   int                 i, num;
   char              **list, s[4096], ss[4096], cs[4096];
   const char         *ext;
   MenuItem           *mi;
   struct stat         st;
   const char         *chmap =
      "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-_";
   FILE               *f;

   m = MenuCreate(name, NULL, parent, ms);

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
		       Esnprintf(s, sizeof(s), "%s/%s:%s", dir, tmp, name);
		       Esnprintf(ss, sizeof(ss), "%s/%s", dir, tmp);
		       mm = MenuCreateFromDirectory(s, m, ms, ss);
		       mi = MenuItemCreate(tmp, NULL, NULL, mm);
		       MenuAddItem(m, mi);
		    }
	       }
	     fclose(f);
	     return m;
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
	     Esnprintf(s, sizeof(s), "%s/%s:%s", dir, list[i], name);
	     mm = MenuCreateFromDirectory(s, m, ms, ss);
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

   return m;
}

static void
FillFlatFileMenu(Menu * m, const char *name, const char *file)
{
   FILE               *f;
   char                first = 1;
   char                s[4096];

   f = fopen(file, "r");
   if (!f)
     {
	fprintf(stderr, "Unable to open menu file %s -- %s\n", file,
		strerror(errno));
	return;
     }

   while (fgets(s, 4096, f))
     {
	s[strlen(s) - 1] = 0;
	if ((s[0]) && s[0] != '#')
	  {
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
		  static int          count = 0;

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
		       Esnprintf(wd, sizeof(wd), "__FM.%s.%i", name, count);
		       count++;
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
     }
   fclose(f);
}

static void
FileMenuUpdate(int val __UNUSED__, void *data)
{
   Menu               *m;
   time_t              lastmod;
   char                s[4096];
   const char         *ff;

   m = (Menu *) data;
   if (!m)
      return;

   if (!FindItem(m, 0, LIST_FINDBY_POINTER, LIST_TYPE_MENU))
      return;

   /* if the menu is up dont update */
   if (MenusActive())
      goto done;

   ff = MenuGetData(m);
   if (!exists(ff))
     {
	MenuHide(m);
	MenuEmpty(m);
	return;
     }

   lastmod = moddate(ff);
   if (lastmod > MenuGetTimestamp(m))
     {
	MenuSetTimestamp(m, lastmod);
	MenuEmpty(m);
	FillFlatFileMenu(m, MenuGetName(m), ff);
	MenuRepack(m);
     }

 done:
   Esnprintf(s, sizeof(s), "__.%s", MenuGetName(m));
   DoIn(s, 5.0, FileMenuUpdate, 0, m);
}

Menu               *
MenuCreateFromFlatFile(const char *name, Menu * parent, MenuStyle * ms,
		       const char *file)
{
   Menu               *m = NULL;
   char                s[4096], *ff;
   static int          calls = 0;

   if (calls > 32)
      return NULL;
   calls++;

   ff = FindFile(file, NULL, 0);
   if (!ff)
      goto done;

   if (canread(ff))
     {
	m = MenuCreate(name, NULL, parent, ms);
	MenuSetTimestamp(m, moddate(ff));
	FillFlatFileMenu(m, MenuGetName(m), ff);
	MenuSetData(m, ff);
	Esnprintf(s, sizeof(s), "__.%s", MenuGetName(m));
	DoIn(s, 5.0, FileMenuUpdate, 0, m);
	goto done;
     }
   Efree(ff);

 done:
   calls--;

   return m;
}

Menu               *
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
		       char               *iname = NULL, *exec = NULL, *texec =
			  NULL, *tmp;
		       char               *en_name = NULL;

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

Menu               *
MenuCreateFromThemes(const char *name, MenuStyle * ms)
{
   Menu               *m;
   char              **lst;
   int                 i, num;
   char                ss[4096], *s;

   MenuItem           *mi;

   m = MenuCreate(name, NULL, NULL, ms);

   lst = ThemesList(&num);
   for (i = 0; i < num; i++)
     {
	s = fullfileof(lst[i]);
	Esnprintf(ss, sizeof(ss), "themes use %s", s);
	mi = MenuItemCreate(s, NULL, ss, NULL);
	Efree(s);
	MenuAddItem(m, mi);
     }
   if (lst)
      StrlistFree(lst, i);

   return m;
}

static int
BorderNameCompare(void *b1, void *b2)
{
   if (b1 && b2)
      return strcmp(((Border *) b1)->name, ((Border *) b2)->name);

   return 0;
}

Menu               *
MenuCreateFromBorders(const char *name, MenuStyle * ms)
{
   char                s[128];
   Menu               *m;
   Border            **lst;
   int                 i, num;
   MenuItem           *mi;

   m = MenuCreate(name, NULL, NULL, ms);

   lst = (Border **) ListItemType(&num, LIST_TYPE_BORDER);
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

Menu               *
MenuCreateFromAllEWins(const char *name, MenuStyle * ms)
{
   Menu               *m;
   EWin               *const *lst;
   int                 i, num;
   char                s[256];

   MenuItem           *mi;

   m = MenuCreate(name, NULL, NULL, ms);

   lst = EwinListGetAll(&num);
   for (i = 0; i < num; i++)
     {
	if (lst[i]->props.skip_winlist || !EwinGetName(lst[i]))
	   continue;

	Esnprintf(s, sizeof(s), "wop %#lx focus", lst[i]->client.win);
	mi = MenuItemCreate(EwinGetName(lst[i]), NULL, s, NULL);
	MenuAddItem(m, mi);
     }

   return m;
}

#if 0				/* Not used */
static Menu        *
MenuCreateFromDesktopEWins(char *name, Menu * parent, MenuStyle * ms, int desk)
{
   Menu               *m;
   EWin               *const *lst;
   int                 i, num;
   char                s[256];

   MenuItem           *mi;

   m = MenuCreate(name, NULL, parent, ms);

   lst = EwinListGetAll(&num);
   for (i = 0; i < num; i++)
     {
	if (lst[i]->props.skip_winlist || !EwinGetName(lst[i]) ||
	    EoGetDesk(lst[i]) != j)
	   continue;

	Esnprintf(s, sizeof(s), "%lu", lst[i]->client.win);
	mi = MenuItemCreate(lst[i]->client.title, NULL, s, NULL);
	MenuAddItem(m, mi);
     }

   return m;
}
#endif

Menu               *
MenuCreateFromDesktops(const char *name, MenuStyle * ms)
{
   Menu               *m, *mm;
   EWin               *const *lst;
   int                 j, i, num;
   char                s[256];
   MenuItem           *mi;

   m = MenuCreate(name, NULL, NULL, ms);

   lst = EwinListGetAll(&num);
   for (j = 0; j < Conf.desks.num; j++)
     {
	mm = MenuCreate("__SUBMENUDESK_E", NULL, m, ms);

	Esnprintf(s, sizeof(s), "desk goto %i", j);
	mi = MenuItemCreate(_("Go to this Desktop"), NULL, s, NULL);
	MenuAddItem(mm, mi);
	for (i = 0; i < num; i++)
	  {
	     if (lst[i]->props.skip_winlist || !EwinGetName(lst[i]) ||
		 EoGetDesk(lst[i]) != j)
		continue;

	     Esnprintf(s, sizeof(s), "wop %#lx focus", lst[i]->client.win);
	     mi = MenuItemCreate(EwinGetName(lst[i]), NULL, s, NULL);
	     MenuAddItem(mm, mi);
	  }
	Esnprintf(s, sizeof(s), _("Desktop %i"), j);
	mi = MenuItemCreate(s, NULL, NULL, mm);
	MenuAddItem(m, mi);
     }

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

static Menu        *
MenuCreateFromGroups(const char *name, MenuStyle * ms)
{
   Menu               *m, *mm;
   Group             **lst;
   int                 i, j, num;
   char                s[256];

   MenuItem           *mi;

   m = MenuCreate(name, NULL, NULL, ms);

   lst = (Group **) ListItemType(&num, LIST_TYPE_GROUP);
   if (lst)
     {
	for (i = 0; i < num; i++)
	  {
	     mm = MenuCreate("__SUBMENUGROUP_E", NULL, m, ms);

	     Esnprintf(s, sizeof(s), "gop %li showhide",
		       lst[i]->members[0]->client.win);
	     mi = MenuItemCreate(_("Show/Hide this group"), NULL, s, NULL);

	     Esnprintf(s, sizeof(s), "wop %#lx ic",
		       lst[i]->members[0]->client.win);
	     MenuAddItem(mm, mi);
	     mi = MenuItemCreate(_("Iconify this group"), NULL, s, NULL);
	     MenuAddItem(mm, mi);

	     for (j = 0; j < lst[i]->num_members; j++)
	       {
		  Esnprintf(s, sizeof(s), "wop %#lx focus",
			    lst[i]->members[j]->client.win);
		  mi = MenuItemCreate(EwinGetName(lst[i]->members[j]), NULL,
				      s, NULL);
		  MenuAddItem(mm, mi);
	       }
	     Esnprintf(s, sizeof(s), _("Group %i"), i);
	     mi = MenuItemCreate(s, NULL, NULL, mm);
	     MenuAddItem(m, mi);
	  }
	Efree(lst);
     }

   return m;
}

#if 0				/* Not used */
static Menu        *
RefreshTaskMenu(int desk)
{
   char                was = 0;
   int                 lx = 0, ly = 0;
   EWin               *ewin;

   if (task_menu[desk])
     {
	ewin = FindEwinByMenu(task_menu[desk]);
	if ((task_menu[desk]->win) && (ewin))
	  {
	     lx = ewin->x;
	     ly = ewin->y;
	     was = 1;
	  }
	MenuDestroy(task_menu[desk]);
     }
   task_menu[desk] = NULL;
   if (!task_menu_style)
     {
	return NULL;
     }
   task_menu[desk] = MenuCreateFromDesktopEWins("MENU", task_menu_style, desk);
   if ((was) && (task_menu[desk]))
     {
	task_menu[desk]->internal = 1;
	MenuShow(task_menu[desk], 1);
	ewin = FindEwinByMenu(task_menu[desk]);
	if (ewin)
	  {
	     EwinMove(ewin, lx, ly);
	     ShowEwin(ewin);
	  }
	Mode.cur_menu[0] = task_menu[desk];
	Mode.cur_menu_depth = 1;
	MenuShowMasker(task_menu[desk]);
     }
   return task_menu[desk];
}

void
ShowTaskMenu(void)
{
}
#endif

/* FIXME - Menus should have update function? */
void                ShowAllTaskMenu(void);
void                ShowDeskMenu(void);
void                ShowGroupMenu(void);

void
ShowAllTaskMenu(void)
{
   static MenuStyle   *ms = NULL;
   static Menu        *m = NULL;

   ShowInternalMenu(&m, &ms, "TASK_MENU", MenuCreateFromAllEWins);
}

void
ShowDeskMenu(void)
{
   static MenuStyle   *ms = NULL;
   static Menu        *m = NULL;

   ShowInternalMenu(&m, &ms, "DESK_MENU", MenuCreateFromDesktops);
}

void
ShowGroupMenu(void)
{
   static MenuStyle   *ms = NULL;
   static Menu        *m = NULL;

   ShowInternalMenu(&m, &ms, "GROUP_MENU", MenuCreateFromGroups);
}
