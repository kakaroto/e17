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
#include "desktops.h"
#include "ewins.h"
#include "snaps.h"
#include "xwin.h"

struct _snapshot
{
   char               *name;
   char               *win_title;
   char               *win_name;
   char               *win_class;
   EWin               *used;
   char                track_changes;
   unsigned int        use_flags;

   char               *border_name;
   int                 desktop;
   int                 area_x, area_y;
   int                 x, y;
   int                 w, h;
   int                 layer;
   char                sticky;
   char                shaded;
   char               *cmd;
   int                *groups;
   int                 num_groups;
   char                skiptask;
   char                skipfocus;
   char                skipwinlist;
   char                neverfocus;
#if USE_COMPOSITE
   int                 opacity;
   char                shadow;
#endif
};

static Snapshot    *
SnapshotCreate(const char *name)
{
   Snapshot           *sn;

   sn = Ecalloc(1, sizeof(Snapshot));
   sn->name = Estrdup(name);
   AddItemEnd(sn, sn->name, 0, LIST_TYPE_SNAPSHOT);

   return sn;
}

static void
SnapshotDestroy(Snapshot * sn)
{
   /* Just making sure */
   sn = RemoveItemByPtr(sn, LIST_TYPE_SNAPSHOT);
   if (!sn)
      return;

   if (sn->used)
      sn->used->snap = NULL;

   if (sn->name)
      Efree(sn->name);
   if (sn->win_title)
      Efree(sn->win_title);
   if (sn->win_name)
      Efree(sn->win_name);
   if (sn->win_class)
      Efree(sn->win_class);
   if (sn->border_name)
      Efree(sn->border_name);
   if (sn->cmd)
      Efree(sn->cmd);
   if (sn->groups)
      Efree(sn->groups);
   Efree(sn);
}

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
SnapEwinMakeID(EWin * ewin, char *buf, int len)
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
SnapshotEwinFind(EWin * ewin)
{
   Snapshot           *sn;
   char                buf[4096];

   if (ewin->snap)
      return ewin->snap;

   if (SnapEwinMakeID(ewin, buf, sizeof(buf)))
      return NULL;

   sn = FindItem(buf, 0, LIST_FINDBY_BOTH, LIST_TYPE_SNAPSHOT);
   if (sn)
     {
	ListChangeItemID(LIST_TYPE_SNAPSHOT, sn, 1);
	sn->used = ewin;
     }

   return sn;
}

/* find a snapshot state that applies to this ewin Or if that doesnt exist */
/* create a new one */
static Snapshot    *
SnapshotEwinGet(EWin * ewin)
{
   Snapshot           *sn;
   char                buf[4096];

   sn = SnapshotEwinFind(ewin);
   if (sn)
      return sn;

   if (SnapEwinMakeID(ewin, buf, sizeof(buf)))
      return NULL;

   sn = SnapshotCreate(buf);
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
   sn->used = ewin;
   ewin->snap = sn;

   return sn;
}

/* record info about this Ewin's attributes */

static void
SnapEwinBorder(Snapshot * sn, EWin * ewin)
{
   if (sn->border_name)
      Efree(sn->border_name);
   sn->border_name = NULL;
   if (ewin->previous_border)
      sn->border_name = Estrdup(ewin->previous_border->name);
   else if (ewin->normal_border)
      sn->border_name = Estrdup(ewin->normal_border->name);
}

static void
SnapEwinDesktop(Snapshot * sn, EWin * ewin)
{
   sn->desktop = EoGetDeskNum(ewin);
}

static void
SnapEwinSize(Snapshot * sn, EWin * ewin)
{
   sn->w = ewin->client.w;
   sn->h = ewin->client.h;
}

static void
SnapEwinLocation(Snapshot * sn, EWin * ewin)
{
   int                 ax, ay;

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
SnapEwinLayer(Snapshot * sn, EWin * ewin)
{
   sn->layer = EoGetLayer(ewin);
}

static void
SnapEwinSticky(Snapshot * sn, EWin * ewin)
{
   sn->sticky = EoIsSticky(ewin);
}

static void
SnapEwinShade(Snapshot * sn, EWin * ewin)
{
   sn->shaded = ewin->state.shaded;
}

static void
SnapEwinSkipLists(Snapshot * sn, EWin * ewin)
{
   sn->skiptask = ewin->props.skip_ext_task;
   sn->skipwinlist = ewin->props.skip_winlist;
   sn->skipfocus = ewin->props.skip_focuslist;
}

static void
SnapEwinNeverFocus(Snapshot * sn, EWin * ewin)
{
   sn->neverfocus = ewin->props.never_focus;
}

static void
SnapEwinCmd(Snapshot * sn, EWin * ewin)
{
   if (ewin->icccm.wm_machine &&
       strcmp(ewin->icccm.wm_machine, Mode.wm.machine_name))
      return;

   if (sn->cmd)
      Efree(sn->cmd);
   sn->cmd = Estrdup(ewin->icccm.wm_command);
}

static void
SnapEwinGroups(Snapshot * sn, EWin * ewin, char onoff)
{
   EWin              **gwins = NULL;
   Group             **groups;
   int                 i, j, num, num_groups;

   if (!ewin)
      return;

   if (!ewin->groups)
     {
	if (sn->groups)
	   Efree(sn->groups);
	sn->num_groups = 0;
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
		     sn = SnapshotEwinGet(gwins[i]);
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
		  sn = SnapshotEwinFind(gwins[i]);
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
SnapEwinOpacity(Snapshot * sn, EWin * ewin)
{
   sn->opacity = ewin->ewmh.opacity >> 24;
}

static void
SnapEwinShadow(Snapshot * sn, EWin * ewin)
{
   sn->shadow = EoGetShadow(ewin);
}

#endif

static void
SnapEwinUpdate(Snapshot * sn, EWin * ewin, unsigned int flags)
{
   /* FIXME - We should check if anything is actually changed */

   if (flags & SNAP_USE_BORDER)
      SnapEwinBorder(sn, ewin);
   if (flags & SNAP_USE_COMMAND)
      SnapEwinCmd(sn, ewin);
   if (flags & SNAP_USE_DESK)
      SnapEwinDesktop(sn, ewin);
   if (flags & SNAP_USE_POS)
      SnapEwinLocation(sn, ewin);
   if (flags & SNAP_USE_SIZE)
      SnapEwinSize(sn, ewin);
   if (flags & SNAP_USE_LAYER)
      SnapEwinLayer(sn, ewin);
   if (flags & SNAP_USE_STICKY)
      SnapEwinSticky(sn, ewin);
   if (flags & SNAP_USE_SHADED)
      SnapEwinShade(sn, ewin);
   if (flags & SNAP_USE_SKIP_LISTS)
      SnapEwinSkipLists(sn, ewin);
   if (flags & SNAP_USE_FOCUS_NEVER)
      SnapEwinNeverFocus(sn, ewin);
#if USE_COMPOSITE
   if (flags & SNAP_USE_OPACITY)
      SnapEwinOpacity(sn, ewin);
   if (flags & SNAP_USE_SHADOW)
      SnapEwinShadow(sn, ewin);
#endif
   if (flags & SNAP_USE_GROUPS)
      SnapEwinGroups(sn, ewin, ewin->num_groups);

   SaveSnapInfo();
}

static void
SnapshotEwinSet(EWin * ewin, unsigned int flags)
{
   Snapshot           *sn;

   /* Quit if nothing to be saved */
   if (!(flags & SNAP_USE_ALL))
      return;

   sn = SnapshotEwinGet(ewin);
   if (!sn)
      return;

   if (flags & SNAP_AUTO)
      sn->track_changes = 1;

   sn->use_flags = flags & SNAP_USE_ALL;
   SnapEwinUpdate(sn, ewin, flags);
}

void
SnapshotEwinUpdate(EWin * ewin, unsigned int flags)
{
   Snapshot           *sn;

   sn = ewin->snap;
   if (!sn || !sn->track_changes)
      return;

#if 0
   Eprintf("SnapshotEwinUpdate %s: %#x\n", EwinGetName(ewin), flags);
#endif

   if (flags & sn->use_flags)
      SnapEwinUpdate(sn, ewin, flags);
}

/* unsnapshot any saved info about this ewin */
static void
SnapshotEwinRemove(EWin * ewin)
{
   if (ewin->snap)
      SnapshotDestroy(ewin->snap);
   ewin->snap = NULL;
}

/*
 * Snapshot dialogs
 */
typedef struct
{
   Window              client;
   char                track_changes;
   char                snap_border;
   char                snap_desktop;
   char                snap_size;
   char                snap_location;
   char                snap_layer;
   char                snap_sticky;
   char                snap_icon;
   char                snap_shaded;
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
   SnapDlgData        *sd = DialogGetData(d);
   unsigned int        use_flags;

   if (val >= 2 || !sd)
      goto done;

   ewin = EwinFindByClient(sd->client);
   if (!ewin)
      goto done;

   SnapshotEwinRemove(ewin);

   use_flags = 0;
   if (sd->track_changes)
      use_flags |= SNAP_AUTO;
   if (sd->snap_border)
      use_flags |= SNAP_USE_BORDER;
   if (sd->snap_cmd)
      use_flags |= SNAP_USE_COMMAND;
   if (sd->snap_desktop)
      use_flags |= SNAP_USE_DESK;
   if (sd->snap_location)
      use_flags |= SNAP_USE_POS;
   if (sd->snap_size)
      use_flags |= SNAP_USE_SIZE;
   if (sd->snap_layer)
      use_flags |= SNAP_USE_LAYER;
   if (sd->snap_sticky)
      use_flags |= SNAP_USE_STICKY;
   if (sd->snap_shaded)
      use_flags |= SNAP_USE_SHADED;
   if (sd->snap_skiplists)
      use_flags |= SNAP_USE_SKIP_LISTS;
   if (sd->snap_neverfocus)
      use_flags |= SNAP_USE_FOCUS_NEVER;
#if USE_COMPOSITE
   if (sd->snap_opacity)
      use_flags |= SNAP_USE_OPACITY;
   if (sd->snap_shadow)
      use_flags |= SNAP_USE_SHADOW;
#endif
   if (sd->snap_group)
      use_flags |= SNAP_USE_GROUPS;

   SnapshotEwinSet(ewin, use_flags);

 done:
   if (sd && val == 2)
     {
	Efree(sd);
	DialogSetData(d, NULL);
     }

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

   Esnprintf(s, sizeof(s), "SNAPSHOT_WINDOW-%#lx", _EwinGetClientXwin(ewin));

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
	DialogItemSetText(di,
			  _("Select the attributes of this\n"
			    "window you wish to Remember\n" "from now on\n"));

	di = DialogAddItem(table, DITEM_SEPARATOR);
	DialogItemSetColSpan(di, 4);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemSeparatorSetOrientation(di, 0);
     }

   sd = Ecalloc(1, sizeof(SnapDlgData));
   DialogSetData(d, sd);
   sd->client = _EwinGetClientXwin(ewin);

   sn = ewin->snap;
   if (sn)
     {
	if (sn->track_changes)
	   sd->track_changes = 1;
	if (sn->use_flags & SNAP_USE_BORDER)
	   sd->snap_border = 1;
	if (sn->use_flags & SNAP_USE_COMMAND)
	   sd->snap_cmd = 1;
	if (sn->use_flags & SNAP_USE_DESK)
	   sd->snap_desktop = 1;
	if (sn->use_flags & SNAP_USE_POS)
	   sd->snap_location = 1;
	if (sn->use_flags & SNAP_USE_SIZE)
	   sd->snap_size = 1;
	if (sn->use_flags & SNAP_USE_LAYER)
	   sd->snap_layer = 1;
	if (sn->use_flags & SNAP_USE_STICKY)
	   sd->snap_sticky = 1;
	if (sn->use_flags & SNAP_USE_SHADED)
	   sd->snap_shaded = 1;
	if (sn->use_flags & SNAP_USE_SKIP_LISTS)
	   sd->snap_skiplists = 1;
	if (sn->use_flags & SNAP_USE_FOCUS_NEVER)
	   sd->snap_neverfocus = 1;
#if USE_COMPOSITE
	if (sn->use_flags & SNAP_USE_OPACITY)
	   sd->snap_opacity = 1;
	if (sn->use_flags & SNAP_USE_SHADOW)
	   sd->snap_shadow = 1;
#endif
	if (sn->use_flags & SNAP_USE_GROUPS)
	   sd->snap_group = 1;
     }

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemSetText(di, _("Title:"));

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 3);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetAlign(di, 1024, 512);
   DialogItemSetText(di, ewin->icccm.wm_name);

   if (ewin->icccm.wm_res_name)
     {
	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemSetAlign(di, 0, 512);
	DialogItemSetText(di, _("Name:"));

	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetColSpan(di, 3);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemSetAlign(di, 1024, 512);
	DialogItemSetText(di, ewin->icccm.wm_res_name);
     }

   if (ewin->icccm.wm_res_class)
     {
	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemSetAlign(di, 0, 512);
	DialogItemSetText(di, _("Class:"));

	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetColSpan(di, 3);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemSetAlign(di, 1024, 512);
	DialogItemSetText(di, ewin->icccm.wm_res_class);
     }

   if (ewin->icccm.wm_command)
     {
	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemSetAlign(di, 0, 512);
	DialogItemSetText(di, _("Command:"));

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
	     DialogItemSetText(di, s);
	  }
	else
	   DialogItemSetText(di, ewin->icccm.wm_command);
     }

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 4);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 4);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetText(di, _("Track Changes"));
   DialogItemCheckButtonSetState(di, sd->track_changes);
   DialogItemCheckButtonSetPtr(di, &sd->track_changes);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetText(di, _("Location"));
   DialogItemCheckButtonSetState(di, sd->snap_location);
   DialogItemCheckButtonSetPtr(di, &sd->snap_location);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetText(di, _("Border style"));
   DialogItemCheckButtonSetState(di, sd->snap_border);
   DialogItemCheckButtonSetPtr(di, &sd->snap_border);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetText(di, _("Size"));
   DialogItemCheckButtonSetState(di, sd->snap_size);
   DialogItemCheckButtonSetPtr(di, &sd->snap_size);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetText(di, _("Desktop"));
   DialogItemCheckButtonSetState(di, sd->snap_desktop);
   DialogItemCheckButtonSetPtr(di, &sd->snap_desktop);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetText(di, _("Shaded state"));
   DialogItemCheckButtonSetState(di, sd->snap_shaded);
   DialogItemCheckButtonSetPtr(di, &sd->snap_shaded);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetText(di, _("Sticky state"));
   DialogItemCheckButtonSetState(di, sd->snap_sticky);
   DialogItemCheckButtonSetPtr(di, &sd->snap_sticky);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetText(di, _("Stacking layer"));
   DialogItemCheckButtonSetState(di, sd->snap_layer);
   DialogItemCheckButtonSetPtr(di, &sd->snap_layer);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetText(di, _("Window List Skip"));
   DialogItemCheckButtonSetState(di, sd->snap_skiplists);
   DialogItemCheckButtonSetPtr(di, &sd->snap_skiplists);

#if USE_COMPOSITE
   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetText(di, _("Opacity"));
   DialogItemCheckButtonSetState(di, sd->snap_opacity);
   DialogItemCheckButtonSetPtr(di, &sd->snap_opacity);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetText(di, _("Shadowing"));
   DialogItemCheckButtonSetState(di, sd->snap_shadow);
   DialogItemCheckButtonSetPtr(di, &sd->snap_shadow);
#endif

#if 0				/* Disabled (why?) */
   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetText(di, _("Never Focus"));
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
	     DialogItemSetText(di, _("Restart application on login"));
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
	DialogItemSetText(di, _("Remember this window's group(s)"));
	DialogItemCheckButtonSetState(di, sd->snap_group);
	DialogItemCheckButtonSetPtr(di, &sd->snap_group);
     }

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 4);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   DialogAddButton(d, _("OK"), CB_ApplySnap, 1, DIALOG_BUTTON_OK);
   DialogAddButton(d, _("Apply"), CB_ApplySnap, 0, DIALOG_BUTTON_APPLY);
   DialogAddButton(d, _("Close"), CB_ApplySnap, 1, DIALOG_BUTTON_CLOSE);
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
   Snapshot           *snap;
   char                remove;
} RememberWinList;

static RememberWinList *rd_ewin_list;

static void
CB_ApplyRemember(Dialog * d __UNUSED__, int val, void *data __UNUSED__)
{
   int                 i;

   if (val < 2 && rd_ewin_list)
     {
	for (i = 0; rd_ewin_list[i].snap; i++)
	  {
	     if (!rd_ewin_list[i].remove)
		continue;

	     SnapshotDestroy(rd_ewin_list[i].snap);
	  }
	/* save snapshot info to disk */
	SaveSnapInfo();
     }

   if (((val == 0) || (val == 2)) && rd_ewin_list)
     {
	Efree(rd_ewin_list);
	rd_ewin_list = NULL;
     }
}

static void
CB_ApplyRememberEscape(Dialog * d, int val __UNUSED__, void *data __UNUSED__)
{
   DialogClose(d);

   if (rd_ewin_list)
     {
	Efree(rd_ewin_list);
	rd_ewin_list = NULL;
     }
}

static void
CB_RememberWindowSettings(Dialog * d __UNUSED__, int val __UNUSED__, void *data)
{
   RememberWinList    *rd;
   Snapshot           *sn;

   if (!data)
      return;
   rd = (RememberWinList *) data;

   /* Make sure its still there */
   sn = FindItem(rd->snap, 0, LIST_FINDBY_POINTER, LIST_TYPE_SNAPSHOT);

   if (!sn || !sn->used)
      return;
   SnapshotEwinDialog(sn->used);
}

void
SettingsRemember(void)
{
   Dialog             *d;
   DItem              *table, *di;
   Snapshot          **lst, *sn;
   int                 i, num;
   char                buf[128];
   const char         *s;

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
	DialogItemSetText(di,
			  _("Enlightenment Remembered\n"
			    "Windows Settings Dialog\n"));

	di = DialogAddItem(table, DITEM_SEPARATOR);
	DialogItemSetColSpan(di, 3);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemSeparatorSetOrientation(di, 0);
     }

   lst = (Snapshot **) ListItemType(&num, LIST_TYPE_SNAPSHOT);
   rd_ewin_list = Emalloc(sizeof(RememberWinList) * (num + 1));

   if (num > 0)
     {
	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetColSpan(di, 3);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 0, 0);
	DialogItemSetAlign(di, 0, 512);
	DialogItemSetText(di, _("Delete"));
     }

   for (i = 0; i < num; i++)
     {
	sn = lst[i];
	rd_ewin_list[i].snap = sn;
	rd_ewin_list[i].remove = 0;

	di = DialogAddItem(table, DITEM_CHECKBUTTON);
	DialogItemSetColSpan(di, 2);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemSetAlign(di, 0, 512);
	if (sn->used)
	   s = EwinGetName(sn->used);
	else if (sn->win_title)
	   s = sn->win_title;
	else
	  {
	     Esnprintf(buf, sizeof(buf), "%s.%s", sn->win_name, sn->win_class);
	     s = buf;
	  }
	DialogItemSetText(di, s);
	DialogItemCheckButtonSetState(di, rd_ewin_list[i].remove);
	DialogItemCheckButtonSetPtr(di, &(rd_ewin_list[i].remove));

	if (sn->used)
	  {
	     di = DialogAddItem(table, DITEM_BUTTON);
	     DialogItemSetPadding(di, 2, 2, 2, 2);
	     DialogItemSetFill(di, 1, 0);
	     DialogItemSetAlign(di, 0, 512);
	     DialogItemSetText(di, _("Remembered Settings..."));
	     DialogItemSetCallback(di, CB_RememberWindowSettings, 0,
				   (char *)(&rd_ewin_list[i]));
	  }
	else
	  {
	     di = DialogAddItem(table, DITEM_TEXT);
	     DialogItemSetPadding(di, 2, 2, 2, 2);
	     DialogItemSetFill(di, 1, 0);
	     DialogItemSetText(di, _("Unused"));
	  }
     }
   if (lst)
      Efree(lst);
   rd_ewin_list[num].snap = NULL;

   /* finish remember window */
   if (!num)
     {
	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetColSpan(di, 3);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemSetText(di,
			  _
			  ("There are no active windows with remembered attributes."));
     }

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 3);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   DialogAddButton(d, _("OK"), CB_ApplyRemember, 1, DIALOG_BUTTON_OK);
   DialogAddButton(d, _("Close"), CB_ApplyRemember, 1, DIALOG_BUTTON_CLOSE);
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
	     if (sn->track_changes)
		fprintf(f, "AUTO: yes\n");
	     if ((sn->use_flags & SNAP_USE_BORDER) && sn->border_name)
		fprintf(f, "BORDER: %s\n", sn->border_name);
	     if ((sn->use_flags & SNAP_USE_COMMAND) && sn->cmd)
		fprintf(f, "CMD: %s\n", sn->cmd);
	     if (sn->use_flags & SNAP_USE_DESK)
		fprintf(f, "DESKTOP: %i\n", sn->desktop);
	     if (sn->use_flags & SNAP_USE_POS)
		fprintf(f, "RES: %i %i\n", VRoot.w, VRoot.h);
	     if (sn->use_flags & SNAP_USE_SIZE)
		fprintf(f, "WH: %i %i\n", sn->w, sn->h);
	     if (sn->use_flags & SNAP_USE_POS)
		fprintf(f, "XY: %i %i %i %i\n", sn->x, sn->y, sn->area_x,
			sn->area_y);
	     if (sn->use_flags & SNAP_USE_LAYER)
		fprintf(f, "LAYER: %i\n", sn->layer);
	     if (sn->use_flags & SNAP_USE_STICKY)
		fprintf(f, "STICKY: %i\n", sn->sticky);
	     if (sn->use_flags & SNAP_USE_SHADED)
		fprintf(f, "SHADE: %i\n", sn->shaded);
	     if (sn->use_flags & SNAP_USE_SKIP_LISTS)
	       {
		  fprintf(f, "SKIPTASK: %i\n", sn->skiptask);
		  fprintf(f, "SKIPWINLIST: %i\n", sn->skipwinlist);
		  fprintf(f, "SKIPFOCUS: %i\n", sn->skipfocus);
	       }
	     if (sn->use_flags & SNAP_USE_FOCUS_NEVER)
		fprintf(f, "NEVERFOCUS: %i\n", sn->neverfocus);
#if USE_COMPOSITE
	     if (sn->use_flags & SNAP_USE_OPACITY)
		fprintf(f, "OPACITY: %i\n", sn->opacity);
	     if (sn->use_flags & SNAP_USE_SHADOW)
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
	     if ((sn->use_flags & SNAP_USE_COMMAND) && (sn->cmd) && !sn->used)
		EspawnCmd(sn->cmd);
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
	     sn = SnapshotCreate(atword(buf, 2));
	  }
	else if (sn)
	  {
	     if (!strcmp(s, "TITLE:"))
		sn->win_title = Estrdup(atword(buf, 2));
	     else if (!strcmp(s, "NAME:"))
		sn->win_name = Estrdup(atword(buf, 2));
	     else if (!strcmp(s, "CLASS:"))
		sn->win_class = Estrdup(atword(buf, 2));
	     else if (!strcmp(s, "AUTO:"))
		sn->track_changes = 1;
	     else if (!strcmp(s, "BORDER:"))
	       {
		  sn->use_flags |= SNAP_USE_BORDER;
		  sn->border_name = Estrdup(atword(buf, 2));
	       }
	     else if (!strcmp(s, "CMD:"))
	       {
		  sn->use_flags |= SNAP_USE_COMMAND;
		  sn->cmd = Estrdup(atword(buf, 2));
	       }
	     else if (!strcmp(s, "DESKTOP:"))
	       {
		  sn->use_flags |= SNAP_USE_DESK;
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
		  sn->use_flags |= SNAP_USE_SIZE;
		  word(buf, 2, s);
		  sn->w = atoi(s);
		  word(buf, 3, s);
		  sn->h = atoi(s);
	       }
	     else if (!strcmp(s, "XY:"))
	       {
		  sn->use_flags |= SNAP_USE_POS;
		  word(buf, 2, s);
		  sn->x = atoi(s);
		  word(buf, 3, s);
		  sn->y = atoi(s);
		  /* we changed reses since we last used this snapshot file */
		  if (res_w != VRoot.w)
		    {
		       if (sn->use_flags & SNAP_USE_SIZE)
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
		       if (sn->use_flags & SNAP_USE_SIZE)
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
		  sn->use_flags |= SNAP_USE_LAYER;
		  word(buf, 2, s);
		  sn->layer = atoi(s);
	       }
	     else if (!strcmp(s, "STICKY:"))
	       {
		  sn->use_flags |= SNAP_USE_STICKY;
		  word(buf, 2, s);
		  sn->sticky = atoi(s);
	       }
	     else if (!strcmp(s, "SHADE:"))
	       {
		  sn->use_flags |= SNAP_USE_SHADED;
		  word(buf, 2, s);
		  sn->shaded = atoi(s);
	       }
	     else if (!strcmp(s, "SKIPFOCUS:"))
	       {
		  sn->use_flags |= SNAP_USE_SKIP_LISTS;
		  word(buf, 2, s);
		  sn->skipfocus = atoi(s);
	       }
	     else if (!strcmp(s, "SKIPTASK:"))
	       {
		  sn->use_flags |= SNAP_USE_SKIP_LISTS;
		  word(buf, 2, s);
		  sn->skiptask = atoi(s);
	       }
	     else if (!strcmp(s, "SKIPWINLIST:"))
	       {
		  sn->use_flags |= SNAP_USE_SKIP_LISTS;
		  word(buf, 2, s);
		  sn->skipwinlist = atoi(s);
	       }
	     else if (!strcmp(s, "NEVERFOCUS:"))
	       {
		  sn->use_flags |= SNAP_USE_FOCUS_NEVER;
		  word(buf, 2, s);
		  sn->neverfocus = atoi(s);
	       }
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
		  sn->use_flags |= SNAP_USE_OPACITY;
		  word(buf, 2, s);
		  sn->opacity = atoi(s);
	       }
	     else if (!strcmp(s, "SHADOW:"))
	       {
		  sn->use_flags |= SNAP_USE_SHADOW;
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
SnapshotEwinMatch(EWin * ewin)
{
   Snapshot           *sn;
   int                 i, ax, ay;

   sn = SnapshotEwinFind(ewin);
   if (!sn)
     {
	if (ewin->props.autosave)
	   SnapshotEwinSet(ewin, SNAP_USE_ALL | SNAP_AUTO);
	return;
     }

   ewin->snap = sn;
   ListChangeItemID(LIST_TYPE_SNAPSHOT, ewin->snap, 1);

   if (ewin->props.autosave)
      sn->track_changes = 1;

   if (sn->use_flags & SNAP_USE_STICKY)
      EoSetSticky(ewin, sn->sticky);

   if (sn->use_flags & SNAP_USE_DESK)
      EoSetDesk(ewin, DeskGet(sn->desktop));

   if (sn->use_flags & SNAP_USE_SIZE)
     {
	ewin->client.w = sn->w;
	ewin->client.h = sn->h;
     }

   if (sn->use_flags & SNAP_USE_POS)
     {
	ewin->state.placed = 1;
	ewin->client.x = sn->x;
	ewin->client.y = sn->y;
	ewin->client.grav = NorthWestGravity;
#if 0				/* No, do later in EwinDetermineArea() */
	ewin->area_x = sn->area_x;
	ewin->area_y = sn->area_y;
#endif
	if (!EoIsSticky(ewin))
	  {
	     DeskGetArea(EoGetDesk(ewin), &ax, &ay);
	     ewin->client.x += ((sn->area_x - ax) * VRoot.w);
	     ewin->client.y += ((sn->area_y - ay) * VRoot.h);
	  }
	EMoveResizeWindow(_EwinGetClientWin(ewin), ewin->client.x,
			  ewin->client.y, ewin->client.w, ewin->client.h);
     }

   if (sn->use_flags & SNAP_USE_LAYER)
      EoSetLayer(ewin, sn->layer);

   if (sn->use_flags & SNAP_USE_SKIP_LISTS)
     {
	ewin->props.skip_focuslist = sn->skipfocus;
	ewin->props.skip_ext_task = sn->skiptask;
	ewin->props.skip_winlist = sn->skipwinlist;
     }

   if (sn->use_flags & SNAP_USE_FOCUS_NEVER)
      ewin->props.never_focus = sn->neverfocus;

   if (sn->use_flags & SNAP_USE_SHADED)
      ewin->state.shaded = sn->shaded;

   if (sn->use_flags & SNAP_USE_BORDER)
      EwinSetBorderByName(ewin, sn->border_name);

   if (sn->groups)
     {
	for (i = 0; i < sn->num_groups; i++)
	  {
	     Group              *g;

	     g = FindItem(NULL, sn->groups[i], LIST_FINDBY_ID, LIST_TYPE_GROUP);
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
   if (sn->use_flags & SNAP_USE_OPACITY)
      ewin->ewmh.opacity = OpacityExt(sn->opacity);

   if (sn->use_flags & SNAP_USE_SHADOW)
      EoSetShadow(ewin, sn->shadow);
#endif

   if (EventDebug(EDBUG_TYPE_SNAPS))
      Eprintf("Snap get snap  %#lx: %4d+%4d %4dx%4d: %s\n",
	      _EwinGetClientXwin(ewin), ewin->client.x, ewin->client.y,
	      ewin->client.w, ewin->client.h, EwinGetName(ewin));
}

/* Detach snapshot from ewin */
void
SnapshotEwinUnmatch(EWin * ewin)
{
   Snapshot           *sn;

   sn = ewin->snap;
   if (sn == NULL)
      return;

   ewin->snap = NULL;
   sn->used = NULL;
   ListChangeItemID(LIST_TYPE_SNAPSHOT, sn, 0);
}

void
SnapshotEwinParse(EWin * ewin, const char *params)
{
   char                param[FILEPATH_LEN_MAX];
   unsigned int        use_flags;

   use_flags = 0;
   for (; params;)
     {
	param[0] = 0;
	word(params, 1, param);

	if (!strcmp(param, "all"))
	  {
	     use_flags = SNAP_USE_ALL;
	  }
	else if (!strcmp(param, "dialog"))
	  {
	     SnapshotEwinDialog(ewin);
	     break;
	  }
	else if (!strcmp(param, "none"))
	   SnapshotEwinRemove(ewin);
	else if (!strcmp(param, "auto"))
	   use_flags |= SNAP_AUTO;
	else if (!strcmp(param, "border"))
	   use_flags |= SNAP_USE_BORDER;
	else if (!strcmp(param, "command"))
	   use_flags |= SNAP_USE_COMMAND;
	else if (!strcmp(param, "desktop"))
	   use_flags |= SNAP_USE_DESK;
	else if (!strcmp(param, "location"))
	   use_flags |= SNAP_USE_POS;
	else if (!strcmp(param, "size"))
	   use_flags |= SNAP_USE_SIZE;
	else if (!strcmp(param, "layer"))
	   use_flags |= SNAP_USE_LAYER;
	else if (!strcmp(param, "shade"))
	   use_flags |= SNAP_USE_SHADED;
	else if (!strcmp(param, "sticky"))
	   use_flags |= SNAP_USE_STICKY;
#if USE_COMPOSITE
	else if (!strcmp(param, "opacity"))
	   use_flags |= SNAP_USE_OPACITY;
	else if (!strcmp(param, "shadow"))
	   use_flags |= SNAP_USE_SHADOW;
#endif
	else if (!strcmp(param, "group"))
	   use_flags |= SNAP_USE_GROUPS;

	params = atword(params, 2);
     }

   if (ewin->snap)
      use_flags |= ewin->snap->use_flags;
   SnapshotEwinSet(ewin, use_flags);

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

#define SS(s) ((s) ? (s) : NoText)
static const char   NoText[] = "-NONE-";

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
		  "     Window Class: %s\n",
		  name, (sn->used) ? "" : "*** Unused ***",
		  sn->win_title ? sn->win_title : nstr,
		  sn->win_name ? sn->win_name : nstr,
		  sn->win_class ? sn->win_class : nstr);

	if (sn->use_flags & SNAP_AUTO)
	   IpcPrintf("      Tracking changes\n");
	if (sn->use_flags & SNAP_USE_BORDER)
	   IpcPrintf("      Border Name: %s\n", SS(sn->border_name));
	if (sn->use_flags & SNAP_USE_DESK)
	   IpcPrintf("          desktop: %d\n", sn->desktop);
	if (sn->use_flags & SNAP_USE_POS)
	   IpcPrintf("           (x, y): %d, %d    area (x, y): %d, %d\n",
		     sn->x, sn->y, sn->area_x, sn->area_y);
	if (sn->use_flags & SNAP_USE_SIZE)
	   IpcPrintf("           (w, h): %d, %d\n", sn->w, sn->h);
	if (sn->use_flags & SNAP_USE_LAYER)
	   IpcPrintf("            layer: %d\n", sn->layer);
	if (sn->use_flags & SNAP_USE_STICKY)
	   IpcPrintf("           sticky: %d\n", sn->sticky);
	if (sn->use_flags & SNAP_USE_SHADED)
	   IpcPrintf("            shade: %d\n", sn->shaded);
	if (sn->use_flags & SNAP_USE_COMMAND)
	   IpcPrintf("          command: %s\n", SS(sn->cmd));
	if (sn->use_flags & SNAP_USE_SKIP_LISTS)
	   IpcPrintf
	      ("         skiptask: %d    skipfocus: %d    skipwinlist: %d\n",
	       sn->skiptask, sn->skipfocus, sn->skipwinlist);
	if (sn->use_flags & SNAP_USE_FOCUS_NEVER)
	   IpcPrintf("       neverfocus: %d\n", sn->neverfocus);
	IpcPrintf("\n");
     }
}
