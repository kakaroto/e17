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
#endif

/* find a snapshot state that applies to this ewin */
Snapshot           *
FindSnapshot(EWin * ewin)
{
   Snapshot           *sn;
   char                buf[4096];

   if (ewin->snap)
      return ewin->snap;
   if ((ewin->client.name) && (ewin->client.class))
      Esnprintf(buf, sizeof(buf), "%s.%s", ewin->client.name,
		ewin->client.class);
   else if (ewin->client.title)
      Esnprintf(buf, sizeof(buf), "TITLE.%s", ewin->client.title);
   else
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
Snapshot           *
GetSnapshot(EWin * ewin)
{
   Snapshot           *sn;

   sn = FindSnapshot(ewin);
   if (!sn)
     {
	char                buf[4096];

	if ((ewin->client.name) && (ewin->client.class))
	   Esnprintf(buf, sizeof(buf), "%s.%s", ewin->client.name,
		     ewin->client.class);
	else if (ewin->client.title)
	   Esnprintf(buf, sizeof(buf), "TITLE.%s", ewin->client.title);
	sn = NewSnapshot(buf);
	ListChangeItemID(LIST_TYPE_SNAPSHOT, sn, 1);
	if ((ewin->client.name) && (ewin->client.class))
	  {
	     sn->win_title = NULL;
	     sn->win_name = duplicate(ewin->client.name);
	     sn->win_class = duplicate(ewin->client.class);
	  }
	else
	  {
	     sn->win_title = duplicate(ewin->client.title);
	     sn->win_name = NULL;
	     sn->win_class = NULL;
	  }
	sn->used = 1;
	ewin->snap = sn;
     }
   return sn;
}

/* create a new snapshot */
Snapshot           *
NewSnapshot(char *name)
{
   Snapshot           *sn;

   sn = Emalloc(sizeof(Snapshot));
   sn->name = duplicate(name);
   sn->win_title = NULL;
   sn->win_name = NULL;
   sn->win_class = NULL;
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
   sn->iclass_name = NULL;
   sn->use_shade = 0;
   sn->shade = 0;
   sn->use_cmd = 0;
   sn->cmd = NULL;
   sn->groups = NULL;
   sn->num_groups = 0;
   sn->used = 0;
   sn->use_skiplists = 0;
   sn->skiptask = 0;
   sn->skipfocus = 0;
   sn->skipwinlist = 0;
   sn->use_neverfocus = 0;
   sn->neverfocus = 0;
   AddItemEnd(sn, sn->name, 0, LIST_TYPE_SNAPSHOT);
   return sn;
}

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

static void         CB_ApplySnapEscape(int val, void *data);
static void
CB_ApplySnapEscape(int val, void *data)
{
   DialogClose((Dialog *) data);
   val = 0;
}

static Window       tmp_snap_client;
static char         tmp_snap_border;
static char         tmp_snap_desktop;
static char         tmp_snap_size;
static char         tmp_snap_location;
static char         tmp_snap_layer;
static char         tmp_snap_sticky;
static char         tmp_snap_icon;
static char         tmp_snap_shade;
static char         tmp_snap_cmd;
static char         tmp_snap_group;
static char         tmp_snap_skiplists;
static char         tmp_snap_neverfocus;

static void         CB_ApplySnap(int val, void *data);
static void
CB_ApplySnap(int val, void *data)
{
   SaveSnapInfo();
   if (val < 2)
     {
	EWin               *ewin;

	ewin = FindItem("", tmp_snap_client, LIST_FINDBY_ID, LIST_TYPE_EWIN);
	if (ewin)
	  {
	     Snapshot           *sn;

	     UnsnapshotEwin(ewin);
	     sn = GetSnapshot(ewin);
	     if (sn)
	       {
		  ClearSnapshot(sn);
	       }
	     if (tmp_snap_border)
		SnapshotEwinBorder(ewin);
	     if (tmp_snap_desktop)
		SnapshotEwinDesktop(ewin);
	     if (tmp_snap_size)
		SnapshotEwinSize(ewin);
	     if (tmp_snap_location)
		SnapshotEwinLocation(ewin);
	     if (tmp_snap_layer)
		SnapshotEwinLayer(ewin);
	     if (tmp_snap_sticky)
		SnapshotEwinSticky(ewin);
	     if (tmp_snap_icon)
		SnapshotEwinIcon(ewin);
	     if (tmp_snap_shade)
		SnapshotEwinShade(ewin);
	     if (tmp_snap_cmd)
		SnapshotEwinCmd(ewin);
	     if (tmp_snap_group)
		SnapshotEwinGroups(ewin, tmp_snap_group);
	     if (tmp_snap_skiplists)
		SnapshotEwinSkipLists(ewin);
	     if (tmp_snap_neverfocus)
		SnapshotEwinNeverFocus(ewin);
	     SaveSnapInfo();
	  }
     }
   data = NULL;
}

void
SnapshotEwinDialog(EWin * ewin)
{
   Dialog             *d;
   DItem              *table, *di;
   Snapshot           *sn;
   char                s[1024];

   if ((d = FindItem("SNAPSHOT_WINDOW", 0, LIST_FINDBY_NAME, LIST_TYPE_DIALOG)))
     {
	ShowDialog(d);
	return;
     }
   d = CreateDialog("SNAPSHOT_WINDOW");
   DialogSetTitle(d, _("Remembered Application Attributes"));

   table = DialogInitItem(d);
   DialogItemTableSetOptions(table, 4, 0, 0, 0);

   if (mode.dialog_headers)
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

   sn = ewin->snap;
   tmp_snap_client = ewin->client.win;

   tmp_snap_border = 0;
   tmp_snap_desktop = 0;
   tmp_snap_size = 0;
   tmp_snap_location = 0;
   tmp_snap_layer = 0;
   tmp_snap_sticky = 0;
   tmp_snap_icon = 0;
   tmp_snap_shade = 0;
   tmp_snap_cmd = 0;
   tmp_snap_group = 0;
   tmp_snap_skiplists = 0;
   tmp_snap_neverfocus = 0;
   if (sn)
     {
	if (sn->border_name)
	   tmp_snap_border = 1;
	if (sn->use_desktop)
	   tmp_snap_desktop = 1;
	if (sn->use_wh)
	   tmp_snap_size = 1;
	if (sn->use_xy)
	   tmp_snap_location = 1;
	if (sn->use_layer)
	   tmp_snap_layer = 1;
	if (sn->use_sticky)
	   tmp_snap_sticky = 1;
	if (sn->iclass_name)
	   tmp_snap_icon = 1;
	if (sn->use_shade)
	   tmp_snap_shade = 1;
	if (sn->use_cmd)
	   tmp_snap_cmd = 1;
	if (sn->groups)
	   tmp_snap_group = 1;
	if (sn->use_skiplists)
	   tmp_snap_skiplists = 1;
	if (sn->use_neverfocus)
	   tmp_snap_neverfocus = 1;
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
   DialogItemTextSetText(di, ewin->client.title);

   if (ewin->client.name)
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
	DialogItemTextSetText(di, ewin->client.name);
     }

   if (ewin->client.class)
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
	DialogItemTextSetText(di, ewin->client.class);
     }

   if (ewin->client.command)
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
	if (strlen(ewin->client.command) > 80)
	  {
	     int                 i = 0, slice, last;

	     s[0] = 0;
	     slice = 64;
	     while ((i <= (int)strlen(ewin->client.command))
		    && (i < (int)(sizeof(s) / 4)))
	       {
		  last = i;
		  i += 64;
		  slice = 64;
		  /* and make sure that we don't cut in the middle of a word. */
		  while ((ewin->client.command[i++] != ' ')
			 && (i < (int)(sizeof(s) / 4)))
		     slice++;
		  strncat(s, ewin->client.command + last, slice);
		  if (i < (int)(sizeof(s) / 4))
		     strcat(s, "\n");
		  else
		     strcat(s, "...\n");
	       }
	     DialogItemTextSetText(di, s);
	  }
	else
	   DialogItemTextSetText(di, ewin->client.command);
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
   DialogItemCheckButtonSetState(di, tmp_snap_location);
   DialogItemCheckButtonSetPtr(di, &tmp_snap_location);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemCheckButtonSetText(di, _("Border style"));
   DialogItemCheckButtonSetState(di, tmp_snap_border);
   DialogItemCheckButtonSetPtr(di, &tmp_snap_border);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemCheckButtonSetText(di, _("Size"));
   DialogItemCheckButtonSetState(di, tmp_snap_size);
   DialogItemCheckButtonSetPtr(di, &tmp_snap_size);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemCheckButtonSetText(di, _("Desktop"));
   DialogItemCheckButtonSetState(di, tmp_snap_desktop);
   DialogItemCheckButtonSetPtr(di, &tmp_snap_desktop);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemCheckButtonSetText(di, _("Shaded state"));
   DialogItemCheckButtonSetState(di, tmp_snap_shade);
   DialogItemCheckButtonSetPtr(di, &tmp_snap_shade);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemCheckButtonSetText(di, _("Sticky state"));
   DialogItemCheckButtonSetState(di, tmp_snap_sticky);
   DialogItemCheckButtonSetPtr(di, &tmp_snap_sticky);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemCheckButtonSetText(di, _("Stacking layer"));
   DialogItemCheckButtonSetState(di, tmp_snap_layer);
   DialogItemCheckButtonSetPtr(di, &tmp_snap_layer);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemCheckButtonSetText(di, _("Window List Skip"));
   DialogItemCheckButtonSetState(di, tmp_snap_skiplists);
   DialogItemCheckButtonSetPtr(di, &tmp_snap_skiplists);

#if 0
   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemCheckButtonSetText(di, _("Never Focus"));
   DialogItemCheckButtonSetState(di, tmp_snap_neverfocus);
   DialogItemCheckButtonSetPtr(di, &tmp_snap_neverfocus);
#endif
   if (ewin->client.command)
     {
	char                ok = 1;

	if (ewin->client.machine)
	  {
	     if (strcmp(ewin->client.machine, e_machine_name))
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
	     DialogItemCheckButtonSetState(di, tmp_snap_cmd);
	     DialogItemCheckButtonSetPtr(di, &tmp_snap_cmd);
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
	DialogItemCheckButtonSetState(di, tmp_snap_group);
	DialogItemCheckButtonSetPtr(di, &tmp_snap_group);
     }

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 4);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   DialogAddButton(d, _("OK"), CB_ApplySnap, 1);
   DialogAddButton(d, _("Apply"), CB_ApplySnap, 0);
   DialogAddButton(d, _("Close"), CB_ApplySnap, 1);
   DialogSetExitFunction(d, CB_ApplySnap, 2, d);
   DialogBindKey(d, "Escape", CB_ApplySnapEscape, 0, d);
   DialogBindKey(d, "Return", CB_ApplySnap, 0, d);

   ShowDialog(d);
}

/* record info about this Ewin's attributes */
void
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
      sn->border_name = duplicate(ewin->previous_border->name);
   else if (ewin->border)
      sn->border_name = duplicate(ewin->border->name);
}

void
SnapshotEwinDesktop(EWin * ewin)
{
   Snapshot           *sn;

   sn = GetSnapshot(ewin);
   if (!sn)
      return;
   sn->use_desktop = 1;
   sn->desktop = ewin->desktop;
}

void
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

void
SnapshotEwinLocation(EWin * ewin)
{
   Snapshot           *sn;

   sn = GetSnapshot(ewin);
   if (!sn)
      return;
   sn->use_xy = 1;
   if (((ewin->pager) || (ewin->ibox)) && (ewin->border))
     {
	sn->x = ewin->x + ewin->border->border.left;
	sn->y = ewin->y + ewin->border->border.top;
     }
   else
     {
	sn->x = ewin->x;
	sn->y = ewin->y;
     }
   sn->area_x = ewin->area_x;
   sn->area_y = ewin->area_y;
}

void
SnapshotEwinLayer(EWin * ewin)
{
   Snapshot           *sn;

   sn = GetSnapshot(ewin);
   if (!sn)
      return;
   sn->use_layer = 1;
   sn->layer = ewin->layer;
}

void
SnapshotEwinSticky(EWin * ewin)
{
   Snapshot           *sn;

   sn = GetSnapshot(ewin);
   if (!sn)
      return;
   sn->use_sticky = 1;
   sn->sticky = ewin->sticky;
}

void
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

void
SnapshotEwinNeverFocus(EWin * ewin)
{
   Snapshot           *sn;

   sn = GetSnapshot(ewin);
   if (!sn)
      return;
   sn->use_neverfocus = 1;
   sn->neverfocus = ewin->neverfocus;
}

void
SnapshotEwinIcon(EWin * ewin)
{
   Snapshot           *sn;

   sn = GetSnapshot(ewin);
   if (!sn)
      return;
   if (sn->iclass_name)
      Efree(sn->iclass_name);
   sn->iclass_name = NULL;
   /*  sn->iclass_name = duplicate(ewin->border->name); */
}

void
SnapshotEwinShade(EWin * ewin)
{
   Snapshot           *sn;

   sn = GetSnapshot(ewin);
   if (!sn)
      return;
   sn->use_shade = 1;
   sn->shade = ewin->shaded;
}

void
SnapshotEwinCmd(EWin * ewin)
{
   Snapshot           *sn;
   char                ok = 1;

   sn = GetSnapshot(ewin);
   if (!sn)
      return;

   if (ewin->client.machine)
     {
	if (strcmp(ewin->client.machine, e_machine_name))
	   ok = 0;
     }
   if (ok)
     {
	sn->use_cmd = 1;
	if (sn->cmd)
	   Efree(sn->cmd);
	sn->cmd = duplicate(ewin->client.command);
     }
}

void
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

   gwins = ListWinGroupMembersForEwin(ewin, ACTION_NONE, mode.nogroup, &num);
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

/* record ALL the ewins state info */
void
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
}

/* unsnapshot any saved info about this ewin */
void
UnsnapshotEwin(EWin * ewin)
{
   Snapshot           *sn;
   char                buf[4096];

   if ((ewin->client.name) && (ewin->client.class))
      Esnprintf(buf, sizeof(buf), "%s.%s", ewin->client.name,
		ewin->client.class);
   else if (ewin->client.title)
      Esnprintf(buf, sizeof(buf), "TITLE.%s", ewin->client.title);
   else
      return;
   if (ewin->snap)
     {
	ListChangeItemID(LIST_TYPE_SNAPSHOT, ewin->snap, 0);
	ewin->snap->used = 0;
	sn = RemoveItem((char *)ewin->snap, 0, LIST_FINDBY_POINTER,
			LIST_TYPE_SNAPSHOT);
	ewin->snap = NULL;
     }
   else
      sn = RemoveItem(buf, 0, LIST_FINDBY_NAME, LIST_TYPE_SNAPSHOT);
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

/* ... combine writes, only save after a timeout */

void
SaveSnapInfo(void)
{
   DoIn("SAVESNAP_TIMEOUT", 5.0, Real_SaveSnapInfo, 0, NULL);
}

/* save out all snapped info to disk */
void
Real_SaveSnapInfo(int dumval, void *dumdat)
{
   Snapshot          **lst, *sn;
   int                 i, j, num;
   char                buf[4096], s[4096];
   FILE               *f;

   if (dumdat)
      dumval = 0;

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
	     if (sn->use_desktop)
		fprintf(f, "DESKTOP: %i\n", sn->desktop);
	     if (sn->use_xy)
		fprintf(f, "RES: %i %i\n", root.w, root.h);
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
	     if (sn->cmd)
		fprintf(f, "CMD: %s\n", sn->cmd);
	     if (sn->groups)
	       {
		  for (j = 0; j < sn->num_groups; j++)
		     fprintf(f, "GROUP: %i\n", sn->groups[j]);
	       }
	  }
	Efree(lst);
     }

   fclose(f);

   Esnprintf(buf, sizeof(buf), "%s.snapshots.%i", GetGenericSMFile(), root.scr);
   mv(s, buf);
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

   Esnprintf(buf, sizeof(buf), "%s.snapshots.%i", GetSMFile(), root.scr);
   if (!exists(buf))
      Esnprintf(buf, sizeof(buf), "%s.snapshots.%i", GetGenericSMFile(),
		root.scr);
#ifndef __EMX__
   f = fopen(buf, "r");
#else
   f = fopen(buf, "rt");
#endif
   if (!f)
      return;
   res_w = root.w;
   res_h = root.h;
   while (fgets(buf, sizeof(buf), f))
     {
	/* nuke \n */
	buf[strlen(buf) - 1] = 0;
	word(buf, 1, s);
	if (!strcmp(s, "NEW:"))
	  {
	     res_w = root.w;
	     res_h = root.h;
	     sn = NewSnapshot(atword(buf, 2));
	  }
	else if (sn)
	  {
	     if (!strcmp(s, "TITLE:"))
		sn->win_title = duplicate(atword(buf, 2));
	     else if (!strcmp(s, "NAME:"))
		sn->win_name = duplicate(atword(buf, 2));
	     else if (!strcmp(s, "CLASS:"))
		sn->win_class = duplicate(atword(buf, 2));
	     else if (!strcmp(s, "CMD:"))
	       {
		  sn->use_cmd = 1;
		  sn->cmd = duplicate(atword(buf, 2));
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
		  if (res_w != root.w)
		    {
		       if (sn->use_wh)
			 {
			    if ((res_w - sn->w) <= 0)
			       sn->x = 0;
			    else
			       sn->x =
				  (sn->x * (root.w - sn->w)) / (res_w - sn->w);
			 }
		       else
			 {
			    if (sn->x >= root.w)
			       sn->x = root.w - 32;
			 }
		    }
		  if (res_h != root.h)
		    {
		       if (sn->use_wh)
			 {
			    if ((res_h - sn->h) <= 0)
			       sn->y = 0;
			    else
			       sn->y =
				  (sn->y * (root.h - sn->h)) / (res_h - sn->h);
			 }
		       else
			 {
			    if (sn->y >= root.h)
			       sn->y = root.h - 32;
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
		sn->border_name = duplicate(atword(buf, 2));
	     else if (!strcmp(s, "ICON:"))
		sn->iclass_name = duplicate(atword(buf, 2));
	     else if (!strcmp(s, "GROUP:"))
	       {
		  word(buf, 2, s);
		  sn->num_groups++;
		  sn->groups =
		     Erealloc(sn->groups, sizeof(int) * sn->num_groups);

		  sn->groups[sn->num_groups - 1] = atoi(s);
	       }
	  }
     }
   fclose(f);
}

/* make a client window conform to snapshot info */
void
MatchEwinToSnapInfo(EWin * ewin)
{
   Snapshot           *sn;
   Border             *b;
   int                 i;

   sn = FindSnapshot(ewin);
   if (!sn)
      return;

   ewin->snap = sn;
   sn->used = 1;
   ListChangeItemID(LIST_TYPE_SNAPSHOT, ewin->snap, 1);
   if (sn->use_desktop)
      ewin->desktop = sn->desktop;
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
	if ((!((sn->use_sticky) && (sn->sticky))) && (!ewin->sticky))
	  {
	     if (sn->use_desktop)
	       {
		  ewin->client.x +=
		     ((sn->area_x - desks.desk[ewin->desktop].current_area_x) *
		      root.w);
		  ewin->client.y +=
		     ((sn->area_y - desks.desk[ewin->desktop].current_area_y) *
		      root.h);
	       }
	     else
	       {
		  ewin->client.x +=
		     ((sn->area_x - desks.desk[desks.current].current_area_x) *
		      root.w);
		  ewin->client.y +=
		     ((sn->area_y - desks.desk[desks.current].current_area_y) *
		      root.h);
	       }
	  }
	ewin->x = ewin->client.x;
	ewin->y = ewin->client.y;
	EMoveResizeWindow(disp, ewin->client.win, ewin->client.x,
			  ewin->client.y, ewin->client.w, ewin->client.h);
     }
   if (sn->use_layer)
      ewin->layer = sn->layer;
   if (sn->use_sticky)
      ewin->sticky = sn->sticky;
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
     {
	b = (Border *) FindItem(sn->border_name, 0, LIST_FINDBY_NAME,
				LIST_TYPE_BORDER);
	if (b)
	  {
	     ewin->border_new = 1;
	     ewin->border = b;
	     SetFrameProperty(ewin);
	  }
     }
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
}

void
MatchToSnapInfoPager(Pager * p)
{
   XClassHint          hint;
   char                buf[1024];
   Snapshot           *sn = NULL;
   Window              win = PagerGetWin(p);

   if ((!XGetClassHint(disp, win, &hint)))
      return;

   if ((hint.res_name) && (hint.res_class))
     {
	Esnprintf(buf, sizeof(buf), "%s.%s", hint.res_name, hint.res_class);
	sn = FindItem(buf, 0, LIST_FINDBY_BOTH, LIST_TYPE_SNAPSHOT);
     }
   if (hint.res_name)
      XFree(hint.res_name);
   if (hint.res_class)
      XFree(hint.res_class);
   if (!sn)
      return;
   if (sn->use_xy)
      EMoveWindow(disp, win, sn->x, sn->y);
   if (sn->use_wh)
      EResizeWindow(disp, win, sn->w, sn->h);
}

void
MatchToSnapInfoIconbox(Iconbox * ib)
{
   XClassHint          hint;
   char                buf[1024];
   Snapshot           *sn = NULL;
   Window              win = IconboxGetWin(ib);

   if ((!XGetClassHint(disp, win, &hint)))
      return;

   if ((hint.res_name) && (hint.res_class))
     {
	Esnprintf(buf, sizeof(buf), "%s.%s", hint.res_name, hint.res_class);
	sn = FindItem(buf, 0, LIST_FINDBY_BOTH, LIST_TYPE_SNAPSHOT);
     }
   if (hint.res_name)
      XFree(hint.res_name);
   if (hint.res_class)
      XFree(hint.res_class);
   if (!sn)
      return;
   if (sn->use_xy)
      EMoveWindow(disp, win, sn->x, sn->y);
   if (sn->use_wh)
      EResizeWindow(disp, win, sn->w, sn->h);
}

void
MatchEwinToSnapInfoAfter(EWin * ewin)
{
   Snapshot           *sn;
   Border             *b;
   int                 i;

   sn = FindSnapshot(ewin);
   if (!sn)
      return;

   ewin->snap = sn;
   sn->used = 1;
   ListChangeItemID(LIST_TYPE_SNAPSHOT, ewin->snap, 1);
   if (sn->use_desktop)
      MoveEwinToDesktop(ewin, sn->desktop);
   if (sn->use_wh)
      ResizeEwin(ewin, sn->w, sn->h);
   if (sn->use_xy)
     {
	if ((ewin->pager) || (ewin->ibox))
	   MoveEwin(ewin, sn->x - ewin->border->border.left,
		    sn->y - ewin->border->border.top);
	else
	  {
	     if ((!((sn->use_sticky) && (sn->sticky))) && (!ewin->sticky))
	       {
		  if (sn->use_desktop)
		     MoveEwin(ewin,
			      sn->x +
			      ((sn->area_x -
				desks.desk[ewin->desktop].current_area_x) *
			       root.w),
			      sn->y +
			      ((sn->area_y
				-
				desks.desk[ewin->desktop].current_area_y) *
			       root.h));
		  else
		     MoveEwin(ewin,
			      sn->x +
			      ((sn->area_x
				-
				desks.desk[desks.current].current_area_x) *
			       root.w),
			      sn->y +
			      ((sn->area_y
				-
				desks.desk[desks.current].current_area_y) *
			       root.h));
	       }
	     else
		MoveEwin(ewin, sn->x, sn->y);
	  }
     }
   if (sn->use_layer)
     {
	ewin->layer = sn->layer;
	RaiseEwin(ewin);
     }
   if (sn->use_sticky)
     {
	if (sn->sticky)
	   MakeWindowSticky(ewin);
	else
	   MakeWindowUnSticky(ewin);
     }
   if (sn->use_skiplists)
     {
	ewin->skipfocus = sn->skipfocus;
	ewin->skiptask = sn->skiptask;
	ewin->skipwinlist = sn->skipwinlist;
     }
   if (sn->use_neverfocus)
      ewin->neverfocus = sn->neverfocus;
   if (sn->use_shade)
     {
	if (sn->shade)
	   InstantShadeEwin(ewin);
	else
	   InstantUnShadeEwin(ewin);
     }
   if (sn->iclass_name)
     {
	/* FIXME: fill this in */
     }
   if (sn->border_name)
     {
	b = (Border *) FindItem(sn->border_name, 0, LIST_FINDBY_NAME,
				LIST_TYPE_BORDER);
	if (b)
	  {
	     ewin->border_new = 1;
	     SetEwinToBorder(ewin, b);
	     ICCCM_MatchSize(ewin);
	     MoveResizeEwin(ewin, ewin->x, ewin->y, ewin->client.w,
			    ewin->client.h);
	  }
     }
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
}

void
RememberImportantInfoForEwin(EWin * ewin)
{
   if ((ewin->pager) || (ewin->ibox))
     {
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
	SaveSnapInfo();
     }
}

void
RememberImportantInfoForEwins(EWin * ewin)
{
   int                 i, num;
   EWin              **gwins;

   gwins = ListWinGroupMembersForEwin(ewin, ACTION_MOVE, mode.nogroup, &num);
   if (gwins)
     {
	for (i = 0; i < num; i++)
	   RememberImportantInfoForEwin(gwins[i]);
	Efree(gwins);
     }
}
