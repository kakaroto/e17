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

struct _snapshot
{
   char               *name;
   char               *win_title;
   char               *win_name;
   char               *win_class;
   char               *border_name;
   char                used;
   char                use_desktop;
   int                 desktop;
   int                 area_x, area_y;
   char                use_wh;
   int                 w, h;
   char                use_xy;
   int                 x, y;
   char                use_layer;
   int                 layer;
   char                use_sticky;
   char                sticky;
   char               *iclass_name;
   char                use_shade;
   char                shade;
   char                use_cmd;
   char               *cmd;
   int                *groups;
   int                 num_groups;
   char                use_skiplists;
   char                skiptask;
   char                skipfocus;
   char                skipwinlist;
   char                use_neverfocus;
   char                neverfocus;
#if USE_COMPOSITE
   char                use_opacity;
   int                 opacity;
   char                use_shadow;
   char                shadow;
#endif
};

static Snapshot    *NewSnapshot(const char *name);

/*
 * Stupid hack to fix apps that set WM_WINDOW_ROLE to
 * a <name>-<pid>-<something>-<time> like thing.
 * Is this even ICCCM compliant?
 */
static const char  *
SnapGetRole(const char *role, char *buf, int len)
{
   int                 l1, l2;

   l1 = strlen(role);
   if (l1 >= len)
      l1 = len - 1;
   l2 = strcspn(role, "-0123456789");
   if (l1 - l2 > 8)
      l1 = l2;
   memcpy(buf, role, l1);
   buf[l1] = '\0';

   return buf;
}

/* Format the window identifier string */
static int
EwinMakeID(EWin * ewin, char *buf, int len)
{
   char                s[256];

   if ((ewin->icccm.wm_role) && (ewin->icccm.wm_res_name)
       && (ewin->icccm.wm_res_class))
      Esnprintf(buf, len, "%s.%s:%s", ewin->icccm.wm_res_name,
		ewin->icccm.wm_res_class,
		SnapGetRole(ewin->icccm.wm_role, s, sizeof(s)));
   else if ((ewin->icccm.wm_res_name) && (ewin->icccm.wm_res_class))
      Esnprintf(buf, len, "%s.%s", ewin->icccm.wm_res_name,
		ewin->icccm.wm_res_class);
   else if (ewin->icccm.wm_name)
      Esnprintf(buf, len, "TITLE.%s", ewin->icccm.wm_name);
   else
      return -1;

   return 0;
}

/* find a snapshot state that applies to this ewin */
static Snapshot    *
SnapshotFind(EWin * ewin)
{
   Snapshot           *sn;
   char                buf[4096];

   if (ewin->snap)
      return ewin->snap;

   if (EwinMakeID(ewin, buf, sizeof(buf)))
      return NULL;

   sn = FindItem(buf, 0, LIST_FINDBY_BOTH, LIST_TYPE_SNAPSHOT);
   if (sn)
     {
	ListChangeItemID(LIST_TYPE_SNAPSHOT, sn, 1);
	sn->used = 1;
     }

   return sn;
}

/* find a snapshot state that applies to this ewin Or if that doesnt exist */
/* create a new one */
static Snapshot    *
GetSnapshot(EWin * ewin)
{
   Snapshot           *sn;

   sn = SnapshotFind(ewin);
   if (!sn)
     {
	char                buf[4096];

	if (EwinMakeID(ewin, buf, sizeof(buf)))
	   return NULL;

	sn = NewSnapshot(buf);
	ListChangeItemID(LIST_TYPE_SNAPSHOT, sn, 1);
	if ((ewin->icccm.wm_res_name) && (ewin->icccm.wm_res_class))
	  {
	     sn->win_title = NULL;
	     sn->win_name = Estrdup(ewin->icccm.wm_res_name);
	     sn->win_class = Estrdup(ewin->icccm.wm_res_class);
	  }
	else
	  {
	     sn->win_title = Estrdup(ewin->icccm.wm_name);
	     sn->win_name = NULL;
	     sn->win_class = NULL;
	  }
	sn->used = 1;
	ewin->snap = sn;
     }

   return sn;
}

/* create a new snapshot */
static Snapshot    *
NewSnapshot(const char *name)
{
   Snapshot           *sn;

   sn = Ecalloc(1, sizeof(Snapshot));
   sn->name = Estrdup(name);
   AddItemEnd(sn, sn->name, 0, LIST_TYPE_SNAPSHOT);

   return sn;
}

#if 0				/* Not used */
/* clear all information out of a snapshot and set its refernce use to 0 */
void
ClearSnapshot(Snapshot * sn)
{
   if (sn->border_name)
      Efree(sn->border_name);
   sn->border_name = NULL;
   sn->use_desktop = 0;
   sn->desktop = 0;
   sn->area_x = 0;
   sn->area_y = 0;
   sn->use_wh = 0;
   sn->w = 0;
   sn->h = 0;
   sn->use_xy = 0;
   sn->x = 0;
   sn->y = 0;
   sn->use_layer = 0;
   sn->layer = 0;
   sn->use_sticky = 0;
   sn->sticky = 0;
   if (sn->iclass_name)
      Efree(sn->iclass_name);
   sn->iclass_name = NULL;
   sn->use_shade = 0;
   sn->shade = 0;
   sn->use_cmd = 0;
   if (sn->cmd)
      Efree(sn->cmd);
   sn->cmd = NULL;
   if (sn->groups)
      Efree(sn->groups);
   sn->num_groups = 0;
   sn->used = 0;
   sn->use_skiplists = 0;
   sn->skiptask = 0;
   sn->skipfocus = 0;
   sn->skipwinlist = 0;
   sn->use_neverfocus = 0;
   sn->neverfocus = 0;
   ListChangeItemID(LIST_TYPE_SNAPSHOT, sn, 0);
}
#endif

/* record info about this Ewin's attributes */
static void
SnapshotEwinBorder(EWin * ewin)
{
   Snapshot           *sn;

   sn = GetSnapshot(ewin);
   if (!sn)
      return;
   if (sn->border_name)
      Efree(sn->border_name);
   sn->border_name = NULL;
   if (ewin->previous_border)
      sn->border_name = Estrdup(ewin->previous_border->name);
   else if (ewin->normal_border)
      sn->border_name = Estrdup(ewin->normal_border->name);
}

static void
SnapshotEwinDesktop(EWin * ewin)
{
   Snapshot           *sn;

   sn = GetSnapshot(ewin);
   if (!sn)
      return;
   sn->use_desktop = 1;
   sn->desktop = EoGetDesk(ewin);
}

static void
SnapshotEwinSize(EWin * ewin)
{
   Snapshot           *sn;

   sn = GetSnapshot(ewin);
   if (!sn)
      return;
   sn->use_wh = 1;
   sn->w = ewin->client.w;
   sn->h = ewin->client.h;
}

static void
SnapshotEwinLocation(EWin * ewin)
{
   Snapshot           *sn;
   int                 ax, ay;

   sn = GetSnapshot(ewin);
   if (!sn)
      return;
   sn->use_xy = 1;
   sn->x = EoGetX(ewin);
   sn->y = EoGetY(ewin);
   sn->area_x = ewin->area_x;
   sn->area_y = ewin->area_y;
   if (!EoIsSticky(ewin))
     {
	DeskGetArea(EoGetDesk(ewin), &ax, &ay);
	sn->x += ((ax - sn->area_x) * VRoot.w);
	sn->y += ((ay - sn->area_y) * VRoot.h);
     }
}

static void
SnapshotEwinLayer(EWin * ewin)
{
   Snapshot           *sn;

   sn = GetSnapshot(ewin);
   if (!sn)
      return;
   sn->use_layer = 1;
   sn->layer = EoGetLayer(ewin);
}

static void
SnapshotEwinSticky(EWin * ewin)
{
   Snapshot           *sn;

   sn = GetSnapshot(ewin);
   if (!sn)
      return;
   sn->use_sticky = 1;
   sn->sticky = EoIsSticky(ewin);
}

static void
SnapshotEwinSkipLists(EWin * ewin)
{
   Snapshot           *sn;

   sn = GetSnapshot(ewin);
   if (!sn)
      return;
   sn->use_skiplists = 1;
   sn->skiptask = ewin->skiptask;
   sn->skipwinlist = ewin->skipwinlist;
   sn->skipfocus = ewin->skipfocus;
}

static void
SnapshotEwinNeverFocus(EWin * ewin)
{
   Snapshot           *sn;

   sn = GetSnapshot(ewin);
   if (!sn)
      return;
   sn->use_neverfocus = 1;
   sn->neverfocus = ewin->neverfocus;
}
static void
SnapshotEwinIcon(EWin * ewin)
{
   Snapshot           *sn;

   sn = GetSnapshot(ewin);
   if (!sn)
      return;
   if (sn->iclass_name)
      Efree(sn->iclass_name);
   sn->iclass_name = NULL;
   /*  sn->iclass_name = Estrdup(ewin->border->name); */
}

static void
SnapshotEwinShade(EWin * ewin)
{
   Snapshot           *sn;

   sn = GetSnapshot(ewin);
   if (!sn)
      return;
   sn->use_shade = 1;
   sn->shade = ewin->shaded;
}

static void
SnapshotEwinCmd(EWin * ewin)
{
   Snapshot           *sn;
   char                ok = 1;

   sn = GetSnapshot(ewin);
   if (!sn)
      return;

   if (ewin->icccm.wm_machine)
     {
	if (strcmp(ewin->icccm.wm_machine, Mode.wm.machine_name))
	   ok = 0;
     }
   if (ok)
     {
	sn->use_cmd = 1;
	if (sn->cmd)
	   Efree(sn->cmd);
	sn->cmd = Estrdup(ewin->icccm.wm_command);
     }
}

static void
SnapshotEwinGroups(EWin * ewin, char onoff)
{
   Snapshot           *sn;
   EWin              **gwins = NULL;
   Group             **groups;
   int                 i, j, num, num_groups;

   if (!ewin)
      return;
   if (!ewin->groups)
     {
	sn = GetSnapshot(ewin);
	if (sn)
	  {
	     if (sn->groups)
		Efree(sn->groups);
	     sn->num_groups = 0;
	  }
	return;
     }

   gwins =
      ListWinGroupMembersForEwin(ewin, GROUP_ACTION_ANY, Mode.nogroup, &num);
   for (i = 0; i < num; i++)
     {
	if (onoff)
	  {
	     groups =
		ListWinGroups(gwins[i], GROUP_SELECT_EWIN_ONLY, &num_groups);
	     if (groups)
	       {
		  sn = gwins[i]->snap;
		  if (!sn)
		     sn = GetSnapshot(gwins[i]);
		  if (sn)
		    {
		       if (sn->groups)
			  Efree(sn->groups);

		       sn->groups = Emalloc(sizeof(int) * num_groups);

		       sn->num_groups = num_groups;

		       for (j = 0; j < num_groups; j++)
			  sn->groups[j] = groups[j]->index;
		    }
		  Efree(groups);
	       }
	  }
	else
	  {
	     if (ewin->snap)
	       {
		  sn = GetSnapshot(gwins[i]);
		  if (sn)
		    {
		       if (sn->groups)
			  Efree(sn->groups);
		       sn->num_groups = 0;
		    }
	       }
	  }
     }
   Efree(gwins);
}

#if USE_COMPOSITE

static void
SnapshotEwinOpacity(EWin * ewin)
{
   Snapshot           *sn;

   sn = GetSnapshot(ewin);
   if (!sn)
      return;
   sn->use_opacity = 1;
   sn->opacity = ewin->props.opacity >> 24;
}

static void
SnapshotEwinShadow(EWin * ewin)
{
   Snapshot           *sn;

   sn = GetSnapshot(ewin);
   if (!sn)
      return;
   sn->use_shadow = 1;
   sn->shadow = EoGetShadow(ewin);
}

#endif

/* record ALL the ewins state info */
static void
SnapshotEwinAll(EWin * ewin)
{
   SnapshotEwinBorder(ewin);
   SnapshotEwinDesktop(ewin);
   SnapshotEwinSize(ewin);
   SnapshotEwinLocation(ewin);
   SnapshotEwinLayer(ewin);
   SnapshotEwinSticky(ewin);
   SnapshotEwinIcon(ewin);
   SnapshotEwinShade(ewin);
   SnapshotEwinCmd(ewin);
   SnapshotEwinGroups(ewin, ewin->num_groups);
   SnapshotEwinSkipLists(ewin);
   SnapshotEwinNeverFocus(ewin);
#if USE_COMPOSITE
   SnapshotEwinOpacity(ewin);
   SnapshotEwinShadow(ewin);
#endif
}

/* unsnapshot any saved info about this ewin */
static void
SnapshotEwinRemove(EWin * ewin)
{
   Snapshot           *sn;

   if (ewin->snap)
     {
	sn = ewin->snap;
	UnmatchEwinToSnapInfo(ewin);
	sn = RemoveItem((char *)sn, 0, LIST_FINDBY_POINTER, LIST_TYPE_SNAPSHOT);
     }
   else
     {
	char                buf[4096];

	if (EwinMakeID(ewin, buf, sizeof(buf)))
	   return;
	sn = RemoveItem(buf, 0, LIST_FINDBY_BOTH, LIST_TYPE_SNAPSHOT);
     }

   if (sn)
     {
	if (sn->name)
	   Efree(sn->name);
	if (sn->border_name)
	   Efree(sn->border_name);
	if (sn->iclass_name)
	   Efree(sn->iclass_name);
	if (sn->groups)
	   Efree(sn->groups);
	Efree(sn);
     }
}

/*
 * Snapshot dialogs
 */
typedef struct
{
   Window              client;
   char                snap_border;
   char                snap_desktop;
   char                snap_size;
   char                snap_location;
   char                snap_layer;
   char                snap_sticky;
   char                snap_icon;
   char                snap_shade;
   char                snap_cmd;
   char                snap_group;
   char                snap_skiplists;
   char                snap_neverfocus;

#if USE_COMPOSITE
   char                snap_opacity;
   char                snap_shadow;
#endif
} SnapDlgData;

static void
CB_ApplySnap(Dialog * d, int val, void *data __UNUSED__)
{
   EWin               *ewin;
   Snapshot           *sn;
   SnapDlgData        *sd = DialogGetData(d);

   if (val >= 2)
      goto done;

   ewin = FindItem("", sd->client, LIST_FINDBY_ID, LIST_TYPE_EWIN);
   if (!ewin)
      goto done;

   SnapshotEwinRemove(ewin);
   sn = GetSnapshot(ewin);
   if (!sn || !sd)
      goto done;

#if 0				/* ?!? */
   if (sn)
     {
	ClearSnapshot(sn);
     }
#endif
   if (sd->snap_border)
      SnapshotEwinBorder(ewin);
   if (sd->snap_desktop)
      SnapshotEwinDesktop(ewin);
   if (sd->snap_size)
      SnapshotEwinSize(ewin);
   if (sd->snap_location)
      SnapshotEwinLocation(ewin);
   if (sd->snap_layer)
      SnapshotEwinLayer(ewin);
   if (sd->snap_sticky)
      SnapshotEwinSticky(ewin);
   if (sd->snap_icon)
      SnapshotEwinIcon(ewin);
   if (sd->snap_shade)
      SnapshotEwinShade(ewin);
   if (sd->snap_cmd)
      SnapshotEwinCmd(ewin);
   if (sd->snap_group)
      SnapshotEwinGroups(ewin, sd->snap_group);
   if (sd->snap_skiplists)
      SnapshotEwinSkipLists(ewin);
   if (sd->snap_neverfocus)
      SnapshotEwinNeverFocus(ewin);
#if USE_COMPOSITE
   if (sd->snap_opacity)
      SnapshotEwinOpacity(ewin);
   if (sd->snap_shadow)
      SnapshotEwinShadow(ewin);
#endif

 done:
   if (sd)
      Efree(sd);
   DialogSetData(d, NULL);

   SaveSnapInfo();
}

static void
SnapshotEwinDialog(EWin * ewin)
{
   Dialog             *d;
   DItem              *table, *di;
   Snapshot           *sn;
   SnapDlgData        *sd;
   char                s[1024];

   Esnprintf(s, sizeof(s), "SNAPSHOT_WINDOW-%#lx", ewin->client.win);

   if ((d = FindItem(s, 0, LIST_FINDBY_NAME, LIST_TYPE_DIALOG)))
     {
	ShowDialog(d);
	return;
     }
   d = DialogCreate(s);
   DialogSetTitle(d, _("Remembered Application Attributes"));

   table = DialogInitItem(d);
   DialogItemTableSetOptions(table, 4, 0, 0, 0);

   if (Conf.dialogs.headers)
     {
	di = DialogAddItem(table, DITEM_IMAGE);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetColSpan(di, 2);
	DialogItemImageSetFile(di, "pix/snapshots.png");

	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetColSpan(di, 2);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemTextSetText(di,
			      _("Select the attributes of this\n"
				"window you wish to Remember\n"
				"from now on\n"));

	di = DialogAddItem(table, DITEM_SEPARATOR);
	DialogItemSetColSpan(di, 4);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemSeparatorSetOrientation(di, 0);
     }

   sd = Ecalloc(1, sizeof(SnapDlgData));
   DialogSetData(d, sd);
   sd->client = ewin->client.win;

   sn = ewin->snap;
   if (sn)
     {
	if (sn->border_name)
	   sd->snap_border = 1;
	if (sn->use_desktop)
	   sd->snap_desktop = 1;
	if (sn->use_wh)
	   sd->snap_size = 1;
	if (sn->use_xy)
	   sd->snap_location = 1;
	if (sn->use_layer)
	   sd->snap_layer = 1;
	if (sn->use_sticky)
	   sd->snap_sticky = 1;
	if (sn->iclass_name)
	   sd->snap_icon = 1;
	if (sn->use_shade)
	   sd->snap_shade = 1;
	if (sn->use_cmd)
	   sd->snap_cmd = 1;
	if (sn->groups)
	   sd->snap_group = 1;
	if (sn->use_skiplists)
	   sd->snap_skiplists = 1;
	if (sn->use_neverfocus)
	   sd->snap_neverfocus = 1;
#if USE_COMPOSITE
	if (sn->use_opacity)
	   sd->snap_opacity = 1;
	if (sn->use_shadow)
	   sd->snap_shadow = 1;
#endif
     }

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemTextSetText(di, _("Title:"));

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 3);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetAlign(di, 1024, 512);
   DialogItemTextSetText(di, ewin->icccm.wm_name);

   if (ewin->icccm.wm_res_name)
     {
	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemSetAlign(di, 0, 512);
	DialogItemTextSetText(di, _("Name:"));

	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetColSpan(di, 3);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemSetAlign(di, 1024, 512);
	DialogItemTextSetText(di, ewin->icccm.wm_res_name);
     }

   if (ewin->icccm.wm_res_class)
     {
	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemSetAlign(di, 0, 512);
	DialogItemTextSetText(di, _("Class:"));

	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetColSpan(di, 3);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemSetAlign(di, 1024, 512);
	DialogItemTextSetText(di, ewin->icccm.wm_res_class);
     }

   if (ewin->icccm.wm_command)
     {
	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemSetAlign(di, 0, 512);
	DialogItemTextSetText(di, _("Command:"));

	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetColSpan(di, 3);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemSetAlign(di, 1024, 512);

	/* if the command is long, cut in into slices of about 80 characters */
	if (strlen(ewin->icccm.wm_command) > 80)
	  {
	     int                 i = 0, slice, last;

	     s[0] = 0;
	     slice = 64;
	     while ((i <= (int)strlen(ewin->icccm.wm_command))
		    && (i < (int)(sizeof(s) / 4)))
	       {
		  last = i;
		  i += 64;
		  slice = 64;
		  /* and make sure that we don't cut in the middle of a word. */
		  while ((ewin->icccm.wm_command[i++] != ' ')
			 && (i < (int)(sizeof(s) / 4)))
		     slice++;
		  strncat(s, ewin->icccm.wm_command + last, slice);
		  if (i < (int)(sizeof(s) / 4))
		     strcat(s, "\n");
		  else
		     strcat(s, "...\n");
	       }
	     DialogItemTextSetText(di, s);
	  }
	else
	   DialogItemTextSetText(di, ewin->icccm.wm_command);
     }

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 4);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemCheckButtonSetText(di, _("Location"));
   DialogItemCheckButtonSetState(di, sd->snap_location);
   DialogItemCheckButtonSetPtr(di, &sd->snap_location);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemCheckButtonSetText(di, _("Border style"));
   DialogItemCheckButtonSetState(di, sd->snap_border);
   DialogItemCheckButtonSetPtr(di, &sd->snap_border);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemCheckButtonSetText(di, _("Size"));
   DialogItemCheckButtonSetState(di, sd->snap_size);
   DialogItemCheckButtonSetPtr(di, &sd->snap_size);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemCheckButtonSetText(di, _("Desktop"));
   DialogItemCheckButtonSetState(di, sd->snap_desktop);
   DialogItemCheckButtonSetPtr(di, &sd->snap_desktop);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemCheckButtonSetText(di, _("Shaded state"));
   DialogItemCheckButtonSetState(di, sd->snap_shade);
   DialogItemCheckButtonSetPtr(di, &sd->snap_shade);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemCheckButtonSetText(di, _("Sticky state"));
   DialogItemCheckButtonSetState(di, sd->snap_sticky);
   DialogItemCheckButtonSetPtr(di, &sd->snap_sticky);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemCheckButtonSetText(di, _("Stacking layer"));
   DialogItemCheckButtonSetState(di, sd->snap_layer);
   DialogItemCheckButtonSetPtr(di, &sd->snap_layer);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemCheckButtonSetText(di, _("Window List Skip"));
   DialogItemCheckButtonSetState(di, sd->snap_skiplists);
   DialogItemCheckButtonSetPtr(di, &sd->snap_skiplists);

#if USE_COMPOSITE
   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemCheckButtonSetText(di, _("Opacity"));
   DialogItemCheckButtonSetState(di, sd->snap_opacity);
   DialogItemCheckButtonSetPtr(di, &sd->snap_opacity);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemCheckButtonSetText(di, _("Shadowing"));
   DialogItemCheckButtonSetState(di, sd->snap_shadow);
   DialogItemCheckButtonSetPtr(di, &sd->snap_shadow);
#endif

#if 0				/* Disabled (why?) */
   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemCheckButtonSetText(di, _("Never Focus"));
   DialogItemCheckButtonSetState(di, sd->snap_neverfocus);
   DialogItemCheckButtonSetPtr(di, &sd->snap_neverfocus);
#endif
   if (ewin->icccm.wm_command)
     {
	char                ok = 1;

	if (ewin->icccm.wm_machine)
	  {
	     if (strcmp(ewin->icccm.wm_machine, Mode.wm.machine_name))
		ok = 0;
	  }
	if (ok)
	  {
	     di = DialogAddItem(table, DITEM_CHECKBUTTON);
	     DialogItemSetColSpan(di, 4);
	     DialogItemSetPadding(di, 2, 2, 2, 2);
	     DialogItemSetFill(di, 1, 0);
	     DialogItemCheckButtonSetText(di,
					  _("Restart application on login"));
	     DialogItemCheckButtonSetState(di, sd->snap_cmd);
	     DialogItemCheckButtonSetPtr(di, &sd->snap_cmd);
	  }
	else
	  {
	     di = DialogAddItem(table, DITEM_NONE);
	     DialogItemSetColSpan(di, 4);
	  }
     }
   else
     {
	di = DialogAddItem(table, DITEM_NONE);
	DialogItemSetColSpan(di, 4);
     }

   if (ewin->groups)
     {
	di = DialogAddItem(table, DITEM_CHECKBUTTON);
	DialogItemSetColSpan(di, 4);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemCheckButtonSetText(di, _("Remember this window's group(s)"));
	DialogItemCheckButtonSetState(di, sd->snap_group);
	DialogItemCheckButtonSetPtr(di, &sd->snap_group);
     }

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 4);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   DialogAddButton(d, _("OK"), CB_ApplySnap, 1);
   DialogAddButton(d, _("Apply"), CB_ApplySnap, 0);
   DialogAddButton(d, _("Close"), CB_ApplySnap, 1);
   DialogSetExitFunction(d, CB_ApplySnap, 2);
   DialogBindKey(d, "Escape", DialogCallbackClose, 0);
   DialogBindKey(d, "Return", CB_ApplySnap, 0);

   ShowDialog(d);
}

/* list of remembered items for the remember dialog -- it's either
 * _another_ global var, or a wrapper struct to pass data to the 
 * callback funcs besides the dialog itself -- this is much easier */

typedef struct _remwinlist
{
   EWin               *ewin;
   char                remember;
} RememberWinList;

static RememberWinList **rd_ewin_list;

#if 0				/* Unused */
void
RemoveRememberedWindow(EWin * ewin)
{
   RememberWinList    *rd;

   for (rd = rd_ewin_list[0]; rd; rd++)
      if (rd->ewin == ewin)
	{
	   rd->ewin = 0;
	   rd->remember = 0;
	   break;
	}

   return;
}
#endif

static void
CB_ApplyRemember(Dialog * d __UNUSED__, int val, void *data __UNUSED__)
{
   int                 i;

   if (val < 2 && rd_ewin_list)
     {
	for (i = 0; rd_ewin_list[i]; i++)
	  {
	     if (rd_ewin_list[i])
	       {
		  if (rd_ewin_list[i]->ewin && !rd_ewin_list[i]->remember)
		    {
		       SnapshotEwinRemove(rd_ewin_list[i]->ewin);
		       /* would this be a better way to do things? */
		       /* sn = SnapshotFind(rd_ewin_list[i]->ewin); */
		       /* ClearSnapshot(sn); */
		       /* rd_ewin_list[i]->ewin->snap = 0; */
		    }
	       }
	  }
	/* save snapshot info to disk */
	SaveSnapInfo();
     }
   if (((val == 0) || (val == 2)) && rd_ewin_list)
     {
	for (i = 0; rd_ewin_list[i]; i++)
	   Efree(rd_ewin_list[i]);
	Efree(rd_ewin_list);
	rd_ewin_list = 0;
     }
}

static void
CB_ApplyRememberEscape(Dialog * d, int val __UNUSED__, void *data __UNUSED__)
{
   int                 i;

   DialogClose(d);

   if (rd_ewin_list)
     {
	for (i = 0; rd_ewin_list[i]; i++)
	   Efree(rd_ewin_list[i]);
	Efree(rd_ewin_list);
	rd_ewin_list = 0;
     }
}

static void
CB_RememberWindowSettings(Dialog * d __UNUSED__, int val __UNUSED__, void *data)
{
   RememberWinList    *rd;
   EWin               *ewin;

   if (!data)
      return;
   rd = (RememberWinList *) data;
   ewin = (EWin *) rd->ewin;
   if (!ewin)
      return;
   SnapshotEwinDialog(ewin);
}

#if 0
Snapshot          **lst, *sn;
int                 i, num;

lst = (Snapshot **) ListItemType(&num, LIST_TYPE_SNAPSHOT);
#endif

void
SettingsRemember(void)
{
   Dialog             *d;
   DItem              *table, *di;
   EWin               *const *lst, *ewin;
   int                 i, ri, num;

   /* init remember window */
   if ((d = FindItem("REMEMBER_WINDOW", 0, LIST_FINDBY_NAME, LIST_TYPE_DIALOG)))
     {
	SoundPlay("SOUND_SETTINGS_ACTIVE");
	ShowDialog(d);
	return;
     }
   SoundPlay("SOUND_SETTINGS_REMEMBER");

   d = DialogCreate("REMEMBER_WINDOW");
   DialogSetTitle(d, _("Remembered Windows Settings"));

   table = DialogInitItem(d);
   DialogItemTableSetOptions(table, 3, 0, 0, 0);

   if (Conf.dialogs.headers)
     {
	di = DialogAddItem(table, DITEM_IMAGE);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemImageSetFile(di, "pix/snapshots.png");

	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetColSpan(di, 2);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemTextSetText(di,
			      _("Enlightenment Remembered\n"
				"Windows Settings Dialog\n"));

	di = DialogAddItem(table, DITEM_SEPARATOR);
	DialogItemSetColSpan(di, 3);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemSeparatorSetOrientation(di, 0);
     }

   /* there's a much more efficient way of doing this, but this will work
    * for now */
   lst = EwinListGetAll(&num);
   rd_ewin_list = Emalloc(sizeof(RememberWinList *) * (num + 1));
   ri = 0;
   if ((lst) && (num > 0))
     {
	for (i = 0; i < num; i++)
	  {
	     ewin = lst[i];
	     if (!ewin || !SnapshotFind(ewin)
		 || !(ewin->icccm.wm_name || ewin->icccm.wm_res_name
		      || ewin->icccm.wm_res_class))
	       {
		  /* fprintf(stderr,"Skipping window #%d \"%s\".\n",
		   * i, ewin->icccm.wm_name?ewin->icccm.wm_name:"null"); */
	       }
	     else
	       {
		  rd_ewin_list[ri] = Emalloc(sizeof(RememberWinList));
		  rd_ewin_list[ri]->ewin = ewin;
		  rd_ewin_list[ri]->remember = 1;
		  /* fprintf(stderr," Window #%d \"%s\" is remembered (ri==%d)\n",
		   * i, ewin->icccm.wm_name?ewin->icccm.wm_name:"null", ri);
		   * fprintf(stderr,"  title:\t%s\n  name:\t%s\n  class:\t%s\n  command:\t%s\n",
		   * ewin->icccm.wm_name?ewin->icccm.wm_name:"null",
		   * ewin->icccm.wm_res_name?ewin->icccm.wm_res_name:"null",
		   * ewin->icccm.wm_res_class?ewin->icccm.wm_res_class:"null",
		   * ewin->icccm.wm_command?ewin->icccm.wm_command:"null"
		   * ); */

		  di = DialogAddItem(table, DITEM_CHECKBUTTON);
		  DialogItemSetColSpan(di, 2);
		  DialogItemSetPadding(di, 2, 2, 2, 2);
		  DialogItemSetFill(di, 1, 0);
		  DialogItemSetAlign(di, 0, 512);
		  DialogItemCheckButtonSetText(di, ewin->icccm.wm_name);
		  DialogItemCheckButtonSetState(di, rd_ewin_list[ri]->remember);
		  DialogItemCheckButtonSetPtr(di,
					      &(rd_ewin_list[ri]->remember));

		  di = DialogAddItem(table, DITEM_BUTTON);
		  DialogItemSetPadding(di, 2, 2, 2, 2);
		  DialogItemSetFill(di, 1, 0);
		  DialogItemSetAlign(di, 1024, 512);
		  DialogItemButtonSetText(di, _("Remembered Settings..."));
		  DialogItemSetCallback(di, CB_RememberWindowSettings, 0,
					(char *)rd_ewin_list[ri]);

		  ri++;
	       }
	  }
     }
   rd_ewin_list[ri] = 0;

   /* finish remember window */
   if (!ri)
     {
	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetColSpan(di, 3);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemTextSetText(di,
			      _
			      ("There are no active windows with remembered attributes."));
     }

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 3);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   DialogAddButton(d, _("OK"), CB_ApplyRemember, 1);
   DialogAddButton(d, _("Apply"), CB_ApplyRemember, 0);
   DialogAddButton(d, _("Close"), CB_ApplyRemember, 1);
   DialogSetExitFunction(d, CB_ApplyRemember, 2);
   DialogBindKey(d, "Escape", CB_ApplyRememberEscape, 0);
   DialogBindKey(d, "Return", CB_ApplyRemember, 0);

   ShowDialog(d);
}

/* ... combine writes, only save after a timeout */

void
SaveSnapInfo(void)
{
   DoIn("SAVESNAP_TIMEOUT", 5.0, Real_SaveSnapInfo, 0, NULL);
}

/* save out all snapped info to disk */
void
Real_SaveSnapInfo(int dumval __UNUSED__, void *dumdat __UNUSED__)
{
   Snapshot          **lst, *sn;
   int                 i, j, num;
   char                buf[4096], s[4096];
   FILE               *f;

   if (!Mode.wm.save_ok)
      return;

   Etmp(s);
   f = fopen(s, "w");
   if (!f)
      return;

   lst = (Snapshot **) ListItemType(&num, LIST_TYPE_SNAPSHOT);
   if (lst)
     {
	for (i = 0; i < num; i++)
	  {
	     sn = lst[i];
	     fprintf(f, "NEW: %s\n", sn->name);
	     if (sn->win_title)
		fprintf(f, "TITLE: %s\n", sn->win_title);
	     if (sn->win_name)
		fprintf(f, "NAME: %s\n", sn->win_name);
	     if (sn->win_class)
		fprintf(f, "CLASS: %s\n", sn->win_class);
	     if (sn->cmd)
		fprintf(f, "CMD: %s\n", sn->cmd);
	     if (sn->use_desktop)
		fprintf(f, "DESKTOP: %i\n", sn->desktop);
	     if (sn->use_xy)
		fprintf(f, "RES: %i %i\n", VRoot.w, VRoot.h);
	     if (sn->use_wh)
		fprintf(f, "WH: %i %i\n", sn->w, sn->h);
	     if (sn->use_xy)
		fprintf(f, "XY: %i %i %i %i\n", sn->x, sn->y, sn->area_x,
			sn->area_y);
	     if (sn->use_layer)
		fprintf(f, "LAYER: %i\n", sn->layer);
	     if (sn->use_sticky)
		fprintf(f, "STICKY: %i\n", sn->sticky);
	     if (sn->use_skiplists)
	       {
		  fprintf(f, "SKIPTASK: %i\n", sn->skiptask);
		  fprintf(f, "SKIPWINLIST: %i\n", sn->skipwinlist);
		  fprintf(f, "SKIPFOCUS: %i\n", sn->skipfocus);
	       }
	     if (sn->use_neverfocus)
		fprintf(f, "NEVERFOCUS: %i\n", sn->neverfocus);
	     if (sn->use_shade)
		fprintf(f, "SHADE: %i\n", sn->shade);
	     if (sn->border_name)
		fprintf(f, "BORDER: %s\n", sn->border_name);
	     if (sn->iclass_name)
		fprintf(f, "ICON: %s\n", sn->iclass_name);
#if USE_COMPOSITE
	     if (sn->use_opacity)
		fprintf(f, "OPACITY: %i\n", sn->opacity);
	     if (sn->use_shadow)
		fprintf(f, "SHADOW: %i\n", sn->shadow);
#endif
	     if (sn->groups)
	       {
		  for (j = 0; j < sn->num_groups; j++)
		     fprintf(f, "GROUP: %i\n", sn->groups[j]);
	       }
	  }
	Efree(lst);
     }

   fclose(f);

   Esnprintf(buf, sizeof(buf), "%s.snapshots", EGetSavePrefix());

   if (EventDebug(EDBUG_TYPE_SNAPS))
      Eprintf("Real_SaveSnapInfo: %s\n", buf);
   E_mv(s, buf);
   if (!isfile(buf))
      Alert(_("Error saving snaps file\n"));

   SaveGroups();
}

void
SpawnSnappedCmds(void)
{
   Snapshot          **lst, *sn;
   int                 i, num;

   lst = (Snapshot **) ListItemType(&num, LIST_TYPE_SNAPSHOT);

   if (lst)
     {
	for (i = 0; i < num; i++)
	  {
	     sn = lst[i];
	     if ((sn->use_cmd) && (sn->cmd) && !sn->used)
		execApplication(sn->cmd);
	  }
	Efree(lst);
     }
}

/* load all snapped info */
void
LoadSnapInfo(void)
{
   Snapshot           *sn = NULL;
   char                buf[4096], s[4096];
   FILE               *f;
   int                 res_w, res_h;

   Esnprintf(buf, sizeof(buf), "%s.snapshots", EGetSavePrefix());
   f = fopen(buf, "r");
   if (!f)
      return;

   res_w = VRoot.w;
   res_h = VRoot.h;
   while (fgets(buf, sizeof(buf), f))
     {
	/* nuke \n */
	buf[strlen(buf) - 1] = 0;
	word(buf, 1, s);
	if (!strcmp(s, "NEW:"))
	  {
	     res_w = VRoot.w;
	     res_h = VRoot.h;
	     sn = NewSnapshot(atword(buf, 2));
	  }
	else if (sn)
	  {
	     if (!strcmp(s, "TITLE:"))
		sn->win_title = Estrdup(atword(buf, 2));
	     else if (!strcmp(s, "NAME:"))
		sn->win_name = Estrdup(atword(buf, 2));
	     else if (!strcmp(s, "CLASS:"))
		sn->win_class = Estrdup(atword(buf, 2));
	     else if (!strcmp(s, "CMD:"))
	       {
		  sn->use_cmd = 1;
		  sn->cmd = Estrdup(atword(buf, 2));
	       }
	     else if (!strcmp(s, "DESKTOP:"))
	       {
		  sn->use_desktop = 1;
		  word(buf, 2, s);
		  sn->desktop = atoi(s);
	       }
	     else if (!strcmp(s, "RES:"))
	       {
		  word(buf, 2, s);
		  res_w = atoi(s);
		  word(buf, 3, s);
		  res_h = atoi(s);
	       }
	     else if (!strcmp(s, "WH:"))
	       {
		  sn->use_wh = 1;
		  word(buf, 2, s);
		  sn->w = atoi(s);
		  word(buf, 3, s);
		  sn->h = atoi(s);
	       }
	     else if (!strcmp(s, "XY:"))
	       {
		  sn->use_xy = 1;
		  word(buf, 2, s);
		  sn->x = atoi(s);
		  word(buf, 3, s);
		  sn->y = atoi(s);
		  /* we changed reses since we last used this snapshot file */
		  if (res_w != VRoot.w)
		    {
		       if (sn->use_wh)
			 {
			    if ((res_w - sn->w) <= 0)
			       sn->x = 0;
			    else
			       sn->x =
				  (sn->x * (VRoot.w - sn->w)) / (res_w - sn->w);
			 }
		       else
			 {
			    if (sn->x >= VRoot.w)
			       sn->x = VRoot.w - 32;
			 }
		    }
		  if (res_h != VRoot.h)
		    {
		       if (sn->use_wh)
			 {
			    if ((res_h - sn->h) <= 0)
			       sn->y = 0;
			    else
			       sn->y =
				  (sn->y * (VRoot.h - sn->h)) / (res_h - sn->h);
			 }
		       else
			 {
			    if (sn->y >= VRoot.h)
			       sn->y = VRoot.h - 32;
			 }
		    }
		  word(buf, 4, s);
		  sn->area_x = atoi(s);
		  word(buf, 5, s);
		  sn->area_y = atoi(s);
	       }
	     else if (!strcmp(s, "LAYER:"))
	       {
		  sn->use_layer = 1;
		  word(buf, 2, s);
		  sn->layer = atoi(s);
	       }
	     else if (!strcmp(s, "STICKY:"))
	       {
		  sn->use_sticky = 1;
		  word(buf, 2, s);
		  sn->sticky = atoi(s);
	       }
	     else if (!strcmp(s, "SKIPFOCUS:"))
	       {
		  sn->use_skiplists = 1;
		  word(buf, 2, s);
		  sn->skipfocus = atoi(s);
	       }
	     else if (!strcmp(s, "SKIPTASK:"))
	       {
		  sn->use_skiplists = 1;
		  word(buf, 2, s);
		  sn->skiptask = atoi(s);
	       }
	     else if (!strcmp(s, "SKIPWINLIST:"))
	       {
		  sn->use_skiplists = 1;
		  word(buf, 2, s);
		  sn->skipwinlist = atoi(s);
	       }
	     else if (!strcmp(s, "NEVERFOCUS:"))
	       {
		  sn->use_neverfocus = 1;
		  word(buf, 2, s);
		  sn->neverfocus = atoi(s);
	       }
	     else if (!strcmp(s, "SHADE:"))
	       {
		  sn->use_shade = 1;
		  word(buf, 2, s);
		  sn->shade = atoi(s);
	       }
	     else if (!strcmp(s, "BORDER:"))
		sn->border_name = Estrdup(atword(buf, 2));
	     else if (!strcmp(s, "ICON:"))
		sn->iclass_name = Estrdup(atword(buf, 2));
	     else if (!strcmp(s, "GROUP:"))
	       {
		  word(buf, 2, s);
		  sn->num_groups++;
		  sn->groups =
		     Erealloc(sn->groups, sizeof(int) * sn->num_groups);

		  sn->groups[sn->num_groups - 1] = atoi(s);
	       }
#if USE_COMPOSITE
	     else if (!strcmp(s, "OPACITY:"))
	       {
		  sn->use_opacity = 1;
		  word(buf, 2, s);
		  sn->opacity = atoi(s);
	       }
	     else if (!strcmp(s, "SHADOW:"))
	       {
		  sn->use_shadow = 1;
		  word(buf, 2, s);
		  sn->shadow = atoi(s);
	       }
#endif
	  }
     }
   fclose(f);
}

/* make a client window conform to snapshot info */
void
MatchEwinToSnapInfo(EWin * ewin)
{
   Snapshot           *sn;
   int                 i, ax, ay;

   sn = SnapshotFind(ewin);
   if (!sn)
      return;

   ewin->snap = sn;
   sn->used = 1;
   ListChangeItemID(LIST_TYPE_SNAPSHOT, ewin->snap, 1);

   if (sn->use_sticky)
      EoSetSticky(ewin, sn->sticky);

   if (sn->use_desktop)
      EoSetDesk(ewin, sn->desktop);

   if (sn->use_wh)
     {
	ewin->client.w = sn->w;
	ewin->client.h = sn->h;
     }

   if (sn->use_xy)
     {
	ewin->client.already_placed = 1;
	ewin->client.x = sn->x;
	ewin->client.y = sn->y;
	ewin->client.grav = NorthWestGravity;
	ewin->area_x = sn->area_x;
	ewin->area_y = sn->area_y;
	if (!EoIsSticky(ewin))
	  {
	     DeskGetArea(EoGetDesk(ewin), &ax, &ay);
	     ewin->client.x += ((sn->area_x - ax) * VRoot.w);
	     ewin->client.y += ((sn->area_y - ay) * VRoot.h);
	  }
	EMoveResizeWindow(disp, ewin->client.win, ewin->client.x,
			  ewin->client.y, ewin->client.w, ewin->client.h);
     }

   if (sn->use_layer)
      EoSetLayer(ewin, sn->layer);

   if (sn->use_skiplists)
     {
	ewin->skipfocus = sn->skipfocus;
	ewin->skiptask = sn->skiptask;
	ewin->skipwinlist = sn->skipwinlist;
     }

   if (sn->use_neverfocus)
      ewin->neverfocus = sn->neverfocus;

   if (sn->use_shade)
      ewin->shaded = sn->shade;

   if (sn->iclass_name)
     {
	/* FIXME: fill this in */
     }

   if (sn->border_name)
      EwinSetBorderByName(ewin, sn->border_name, 0);

   if (sn->groups)
     {
	for (i = 0; i < sn->num_groups; i++)
	  {
	     Group              *g;

	     g = (Group *) FindItem(NULL, sn->groups[i], LIST_FINDBY_ID,
				    LIST_TYPE_GROUP);
	     if (!g)
	       {
		  BuildWindowGroup(&ewin, 1);
		  ewin->groups[ewin->num_groups - 1]->index = sn->groups[i];
		  ListChangeItemID(LIST_TYPE_GROUP,
				   ewin->groups[ewin->num_groups - 1],
				   sn->groups[i]);
	       }
	     else
		AddEwinToGroup(ewin, g);
	  }
     }

#if USE_COMPOSITE
   if (sn->use_opacity)
      ewin->props.opacity = OpacityExt(sn->opacity);

   if (sn->use_shadow)
      EoSetShadow(ewin, sn->shadow);
#endif

   if (EventDebug(EDBUG_TYPE_SNAPS))
      Eprintf("Snap get snap  %#lx: %4d+%4d %4dx%4d: %s\n",
	      ewin->client.win, ewin->client.x, ewin->client.y,
	      ewin->client.w, ewin->client.h, EwinGetName(ewin));
}

/* make a client window conform to snapshot info */
void
UnmatchEwinToSnapInfo(EWin * ewin)
{
   Snapshot           *sn;

   sn = ewin->snap;
   if (sn == NULL)
      return;

   ewin->snap = NULL;
   sn->used = 0;
   ListChangeItemID(LIST_TYPE_SNAPSHOT, sn, 0);
}

void
RememberImportantInfoForEwin(EWin * ewin)
{
   if (!ewin->props.autosave)
      return;

   if (EventDebug(EDBUG_TYPE_SNAPS))
      Eprintf("RememberImportantInfoForEwin  %#lx %s\n",
	      ewin->client.win, EwinGetName(ewin));

   SnapshotEwinBorder(ewin);
   SnapshotEwinDesktop(ewin);
   SnapshotEwinSize(ewin);
   SnapshotEwinLocation(ewin);
   SnapshotEwinLayer(ewin);
   SnapshotEwinSticky(ewin);
   SnapshotEwinShade(ewin);
   SnapshotEwinGroups(ewin, ewin->num_groups);
   SnapshotEwinSkipLists(ewin);
   SnapshotEwinNeverFocus(ewin);
#if USE_COMPOSITE
   SnapshotEwinOpacity(ewin);
   SnapshotEwinShadow(ewin);
#endif
   SaveSnapInfo();
}

void
SnapshotEwinSet(EWin * ewin, const char *params)
{
   char                param[FILEPATH_LEN_MAX];

   for (; params;)
     {
	param[0] = 0;
	word(params, 1, param);

	if (!strcmp(param, "all"))
	  {
	     SnapshotEwinAll(ewin);
	     break;
	  }
	else if (!strcmp(param, "dialog"))
	  {
	     SnapshotEwinDialog(ewin);
	     break;
	  }
	else if (!strcmp(param, "none"))
	   SnapshotEwinRemove(ewin);
	else if (!strcmp(param, "border"))
	   SnapshotEwinBorder(ewin);
	else if (!strcmp(param, "command"))
	   SnapshotEwinCmd(ewin);
	else if (!strcmp(param, "desktop"))
	   SnapshotEwinDesktop(ewin);
	else if (!strcmp(param, "group"))
	   SnapshotEwinGroups(ewin, 1);
	else if (!strcmp(param, "icon"))
	   SnapshotEwinIcon(ewin);
	else if (!strcmp(param, "layer"))
	   SnapshotEwinLayer(ewin);
	else if (!strcmp(param, "location"))
	   SnapshotEwinLocation(ewin);
	else if (!strcmp(param, "size"))
	   SnapshotEwinSize(ewin);
	else if (!strcmp(param, "shade"))
	   SnapshotEwinShade(ewin);
	else if (!strcmp(param, "sticky"))
	   SnapshotEwinSticky(ewin);
#if USE_COMPOSITE
	else if (!strcmp(param, "opacity"))
	   SnapshotEwinOpacity(ewin);
	else if (!strcmp(param, "shadow"))
	   SnapshotEwinShadow(ewin);
#endif

	params = atword(params, 2);
     }

   SaveSnapInfo();
}

/*
 * IPC functions
 * A bit ugly...
 */
const char          SnapIpcText[] =
   "usage:\n" "  list_remember [full]\n"
   "  Retrieve a list of remembered windows.  with full, the list\n"
   "  includes the window's remembered attributes\n";

void
SnapIpcFunc(const char *params, Client * c __UNUSED__)
{
   Snapshot          **lst, *sn;
   int                 i, num, full;
   char                param[FILEPATH_LEN_MAX];
   const char         *name, nstr[] = "null";

   lst = (Snapshot **) ListItemType(&num, LIST_TYPE_SNAPSHOT);
   if (!lst)
     {
	IpcPrintf("No remembered windows\n");
	return;
     }

   full = 0;
   if (params)
     {
	param[0] = '\0';
	word(params, 1, param);
	if (!strcmp(param, "full") || param[0] == 'a')
	   full = 1;
     }

   for (i = 0; i < num; i++)
     {
	sn = lst[i];
	if (!sn)
	   continue;		/* ??? */

	name = (sn->name) ? sn->name : "???";

	if (!full)
	  {
	     if (sn->used)
		IpcPrintf("%s\n", name);
	     else
		IpcPrintf("%s (unused)\n", name);
	     continue;
	  }

	IpcPrintf("             Name: %s    %s\n"
		  "     Window Title: %s\n"
		  "      Window Name: %s\n"
		  "     Window Class: %s\n" "      Border Name: %s\n"
		  "      use_desktop: %d     desktop: %d    area (x, y): %d, %d\n"
		  "           use_wh: %d      (w, h): %d, %d\n"
		  "           use_xy: %d      (x, y): %d, %d\n"
		  "        use_layer: %d       layer: %d\n"
		  "       use_sticky: %d      sticky: %d\n"
		  "        use_shade: %d       shade: %d\n"
		  "      use_command: %d     command: %s\n"
		  "    use_skiplists: %d    skiptask: %d    skipfocus: %d    skipwinlist: %d\n"
		  "   use_neverfocus: %d  neverfocus: %d\n\n",
		  name, (sn->used) ? "" : "*** Unused ***",
		  sn->win_title ? sn->win_title : nstr,
		  sn->win_name ? sn->win_name : nstr,
		  sn->win_class ? sn->win_class : nstr,
		  sn->border_name ? sn->border_name : nstr,
		  sn->use_desktop, sn->desktop, sn->area_x, sn->area_y,
		  sn->use_wh, sn->w, sn->h,
		  sn->use_xy, sn->x, sn->y,
		  sn->use_layer, sn->layer,
		  sn->use_sticky, sn->sticky,
		  sn->use_shade, sn->shade,
		  sn->use_cmd, sn->cmd ? sn->cmd : nstr,
		  sn->use_skiplists, sn->skiptask, sn->skipfocus,
		  sn->skipwinlist, sn->use_neverfocus, sn->neverfocus);
     }
}
