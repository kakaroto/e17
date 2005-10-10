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
#include "emodule.h"
#include "ewins.h"
#include "snaps.h"
#include <math.h>

#define DISABLE_PAGER_ICONBOX_GROUPING 0

static Group       *
GroupCreate(void)
{
   Group              *g;
   double              t;

   g = Emalloc(sizeof(Group));
   if (!g)
      return NULL;

   t = GetTime();
   g->index = (int)((GetTime() - (floor(t / 1000) * 1000)) * 10000);
   /* g->index = (int)(GetTime() * 100); */

   g->cfg.iconify = Conf.groups.dflt.iconify;
   g->cfg.kill = Conf.groups.dflt.kill;
   g->cfg.move = Conf.groups.dflt.move;
   g->cfg.raise = Conf.groups.dflt.raise;
   g->cfg.set_border = Conf.groups.dflt.set_border;
   g->cfg.stick = Conf.groups.dflt.stick;
   g->cfg.shade = Conf.groups.dflt.shade;
   g->cfg.mirror = Conf.groups.dflt.mirror;
   g->num_members = 0;
   g->members = NULL;

   return g;
}

static void
GroupDestroy(Group * g)
{
   if (!g)
      return;

   if (g == Mode.groups.current)
      Mode.groups.current = NULL;
   if (g->members)
      Efree(g->members);
   Efree(g);
}

static void
CopyGroupConfig(GroupConfig * src, GroupConfig * dest)
{
   if (!(src && dest))
      return;

   memcpy(dest, src, sizeof(GroupConfig));
}

void
BreakWindowGroup(EWin * ewin, Group * g)
{
   int                 i, j, num;
   EWin               *ewin2;

   if (ewin)
     {
	if (ewin->groups)
	  {
	     for (j = 0; j < ewin->num_groups; j++)
		if (ewin->groups[j] == g)
		  {
		     num = g->num_members;
		     for (i = 0; i < num; i++)
		       {
			  ewin2 = g->members[0];
			  RemoveEwinFromGroup(g->members[0], g);
			  SnapshotEwinUpdate(ewin2, SNAP_USE_GROUPS);
		       }
		     return;
		  }
	  }
     }
}

void
BuildWindowGroup(EWin ** ewins, int num)
{
   int                 i;
   Group              *g;

   Mode.groups.current = g = GroupCreate();
   AddItem(g, NULL, g->index, LIST_TYPE_GROUP);

   for (i = 0; i < num; i++)
     {
#if DISABLE_PAGER_ICONBOX_GROUPING
	/* disable iconboxes and pagers to go into groups */
	if ((ewins[i]->ibox) || (ewins[i]->pager))
	  {
	     DialogOK(_("Cannot comply"),
		      _("Iconboxes and Pagers are disallowed from being\n"
			"members of a group. You cannot add these windows\n"
			"to a group.\n"));
	     return;
	  }
#endif
	AddEwinToGroup(ewins[i], g);
     }
}

void
AddEwinToGroup(EWin * ewin, Group * g)
{
   int                 i;

   if (ewin && g)
     {
#if DISABLE_PAGER_ICONBOX_GROUPING
	/* disable iconboxes and pagers to go into groups */
	if ((ewin->ibox) || (ewin->pager))
	  {
	     DialogOK(_("Cannot comply"),
		      _("Iconboxes and Pagers are disallowed from being\n"
			"members of a group. You cannot add these windows\n"
			"to a group.\n"));
	     return;
	  }
#endif
	for (i = 0; i < ewin->num_groups; i++)
	   if (ewin->groups[i] == g)
	      return;
	ewin->num_groups++;
	ewin->groups =
	   Erealloc(ewin->groups, sizeof(Group *) * ewin->num_groups);
	ewin->groups[ewin->num_groups - 1] = g;
	g->num_members++;
	g->members = Erealloc(g->members, sizeof(EWin *) * g->num_members);
	g->members[g->num_members - 1] = ewin;
	SnapshotEwinUpdate(ewin, SNAP_USE_GROUPS);
     }
}

int
EwinInGroup(EWin * ewin, Group * g)
{
   int                 i;

   if (ewin && g)
     {
	for (i = 0; i < g->num_members; i++)
	  {
	     if (g->members[i] == ewin)
		return 1;
	  }
     }
   return 0;
}

Group              *
EwinsInGroup(EWin * ewin1, EWin * ewin2)
{
   int                 i;

   if (ewin1 && ewin2)
     {
	for (i = 0; i < ewin1->num_groups; i++)
	  {
	     if (EwinInGroup(ewin2, ewin1->groups[i]))
		return ewin1->groups[i];
	  }
     }
   return NULL;
}

void
RemoveEwinFromGroup(EWin * ewin, Group * g)
{
   int                 i, j, k, i2;

   if (!ewin || !g)
      return;

   for (k = 0; k < ewin->num_groups; k++)
     {
	/* is the window actually part of the given group */
	if (ewin->groups[k] != g)
	   continue;

	for (i = 0; i < g->num_members; i++)
	  {
	     if (g->members[i] != ewin)
		continue;

	     /* remove it from the group */
	     for (j = i; j < g->num_members - 1; j++)
		g->members[j] = g->members[j + 1];
	     g->num_members--;
	     if (g->num_members > 0)
		g->members =
		   Erealloc(g->members, sizeof(EWin *) * g->num_members);
	     else
	       {
		  RemoveItemByPtr(g, LIST_TYPE_GROUP);
		  GroupDestroy(g);
	       }

	     /* and remove the group from the groups that the window is in */
	     for (i2 = k; i2 < ewin->num_groups - 1; i2++)
		ewin->groups[i2] = ewin->groups[i2 + 1];
	     ewin->num_groups--;
	     if (ewin->num_groups <= 0)
	       {
		  Efree(ewin->groups);
		  ewin->groups = NULL;
		  ewin->num_groups = 0;
	       }
	     else
		ewin->groups =
		   Erealloc(ewin->groups, sizeof(Group *) * ewin->num_groups);

	     SaveGroups();
	     return;
	  }
     }
}

void
GroupsEwinRemove(EWin * ewin)
{
   int                 num, i;

   num = ewin->num_groups;
   for (i = 0; i < num; i++)
      RemoveEwinFromGroup(ewin, ewin->groups[0]);
}

static char       **
GetWinGroupMemberNames(Group ** groups, int num)
{
   int                 i, j;
   char              **group_member_strings = Emalloc(sizeof(char *) * num);

   for (i = 0; i < num; i++)
     {
	group_member_strings[i] = Emalloc(sizeof(char) * 1024);

	group_member_strings[i][0] = 0;
	for (j = 0; j < groups[i]->num_members; j++)
	  {
	     strcat(group_member_strings[i],
		    EwinGetName(groups[i]->members[j]));
	     strcat(group_member_strings[i], "\n");
	  }
     }

   return group_member_strings;
}

void
ShowHideWinGroups(EWin * ewin, Group * g, char onoff)
{
   EWin              **gwins;
   int                 i, num;
   const Border       *b = NULL;
   const Border       *previous_border;

   if (g)
     {
	gwins = g->members;
	num = g->num_members;
     }
   else
     {
	gwins = ListWinGroupMembersForEwin(ewin, GROUP_ACTION_ANY, 0, &num);
     }

   previous_border = ewin->previous_border;

   for (i = 0; i < num; i++)
     {
	b = NULL;
	switch (onoff)
	  {
	  case SET_TOGGLE:
	     if ((!previous_border) && (!gwins[i]->previous_border))
	       {
		  if (!gwins[i]->border->group_border_name)
		     continue;

		  b = FindItem(gwins[i]->border->group_border_name,
			       0, LIST_FINDBY_NAME, LIST_TYPE_BORDER);
		  if (b)
		     gwins[i]->previous_border = gwins[i]->border;
	       }
	     else if ((previous_border) && (gwins[i]->previous_border))
	       {
		  b = gwins[i]->previous_border;
		  gwins[i]->previous_border = NULL;
	       }
	     break;
	  case SET_ON:
	     if (!gwins[i]->previous_border)
	       {
		  if (!gwins[i]->border->group_border_name)
		     continue;

		  b = FindItem(gwins[i]->border->group_border_name,
			       0, LIST_FINDBY_NAME, LIST_TYPE_BORDER);
		  if (b)
		     gwins[i]->previous_border = gwins[i]->border;
	       }
	     break;
	  case SET_OFF:
	     if (gwins[i]->previous_border)
	       {
		  b = gwins[i]->previous_border;
		  gwins[i]->previous_border = NULL;
	       }
	     break;
	  default:
	     break;
	  }

	if (b)
	  {
	     EwinSetBorder(gwins[i], b, 1);
	     SnapshotEwinUpdate(gwins[i], SNAP_USE_GROUPS);
	  }
     }
   if (!g)
      Efree(gwins);
   SaveGroups();
}

void
SaveGroups(void)
{
   Group             **groups;
   int                 i, num_groups;
   FILE               *f;
   char                s[1024];

   groups = (Group **) ListItemType(&num_groups, LIST_TYPE_GROUP);
   if (!groups)
      return;

   Esnprintf(s, sizeof(s), "%s.groups", EGetSavePrefix());
   f = fopen(s, "w");
   if (!f)
      goto done;

   for (i = 0; i < num_groups; i++)
     {
	if (!groups[i]->members)
	   continue;

	/* Only if the group should be remembered, write info */
	if (!groups[i]->members[0]->snap)
	   continue;

#if 0
	if (!groups[i]->members[0]->snap->num_groups)
	   continue;
#endif

	fprintf(f, "NEW: %i\n", groups[i]->index);
	fprintf(f, "ICONIFY: %i\n", groups[i]->cfg.iconify);
	fprintf(f, "KILL: %i\n", groups[i]->cfg.kill);
	fprintf(f, "MOVE: %i\n", groups[i]->cfg.move);
	fprintf(f, "RAISE: %i\n", groups[i]->cfg.raise);
	fprintf(f, "SET_BORDER: %i\n", groups[i]->cfg.set_border);
	fprintf(f, "STICK: %i\n", groups[i]->cfg.stick);
	fprintf(f, "SHADE: %i\n", groups[i]->cfg.shade);
	fprintf(f, "MIRROR: %i\n", groups[i]->cfg.mirror);
     }
   fclose(f);

 done:
   if (groups)
      Efree(groups);
}

static void
GroupsLoad(void)
{
   FILE               *f;
   char                s[1024];
   Group              *g = NULL;

   Esnprintf(s, sizeof(s), "%s.groups", EGetSavePrefix());
   f = fopen(s, "r");
   if (!f)
      return;

   while (fgets(s, sizeof(s), f))
     {
	char                ss[1024];

	if (strlen(s) > 0)
	   s[strlen(s) - 1] = 0;
	word(s, 1, ss);

	if (!strcmp(ss, "NEW:"))
	  {
	     g = GroupCreate();
	     if (g)
	       {
		  word(s, 2, ss);
		  g->index = atoi(ss);
		  AddItem(g, NULL, g->index, LIST_TYPE_GROUP);
	       }
	  }
	else if (!strcmp(ss, "ICONIFY:"))
	  {
	     word(s, 2, ss);
	     if (g)
		g->cfg.iconify = (char)atoi(ss);
	  }
	else if (!strcmp(ss, "KILL:"))
	  {
	     word(s, 2, ss);
	     if (g)
		g->cfg.kill = (char)atoi(ss);
	  }
	else if (!strcmp(ss, "MOVE:"))
	  {
	     word(s, 2, ss);
	     if (g)
		g->cfg.move = (char)atoi(ss);
	  }
	else if (!strcmp(ss, "RAISE:"))
	  {
	     word(s, 2, ss);
	     if (g)
		g->cfg.raise = (char)atoi(ss);
	  }
	else if (!strcmp(ss, "SET_BORDER:"))
	  {
	     word(s, 2, ss);
	     if (g)
		g->cfg.set_border = (char)atoi(ss);
	  }
	else if (!strcmp(ss, "STICK:"))
	  {
	     word(s, 2, ss);
	     if (g)
		g->cfg.stick = (char)atoi(ss);
	  }
	else if (!strcmp(ss, "SHADE:"))
	  {
	     word(s, 2, ss);
	     if (g)
		g->cfg.shade = (char)atoi(ss);
	  }
	else if (!strcmp(ss, "MIRROR:"))
	  {
	     word(s, 2, ss);
	     if (g)
		g->cfg.mirror = (char)atoi(ss);
	  }
     }
   fclose(f);
}

#define GROUP_OP_ADD	1
#define GROUP_OP_DEL	2
#define GROUP_OP_BREAK	3

static int          tmp_group_index;
static int          tmp_index;
static EWin        *tmp_ewin;
static Group      **tmp_groups;
static int          tmp_action;

static void
ChooseGroupEscape(Dialog * d, int val __UNUSED__, void *data __UNUSED__)
{
   if (tmp_groups)
     {
	ShowHideWinGroups(tmp_ewin, tmp_groups[tmp_index], SET_OFF);
	Efree(tmp_groups);
     }
   DialogClose(d);
}

static void
ChooseGroup(Dialog * d __UNUSED__, int val, void *data __UNUSED__)
{
   if (((val == 0) || (val == 2)) && tmp_groups)
     {
	ShowHideWinGroups(tmp_ewin, tmp_groups[tmp_index], SET_OFF);
     }
   if (val == 0)
     {
	if (tmp_groups)
	  {
	     switch (tmp_action)
	       {
	       case GROUP_OP_ADD:
		  AddEwinToGroup(tmp_ewin, tmp_groups[tmp_group_index]);
		  break;
	       case GROUP_OP_DEL:
		  RemoveEwinFromGroup(tmp_ewin, tmp_groups[tmp_group_index]);
		  break;
	       case GROUP_OP_BREAK:
		  BreakWindowGroup(tmp_ewin, tmp_groups[tmp_group_index]);
		  break;
	       default:
		  break;
	       }
	  }
     }
   if (((val == 0) || (val == 2)) && tmp_groups)
     {
	Efree(tmp_groups);
	tmp_groups = NULL;
     }
}

static void
GroupCallback(Dialog * d __UNUSED__, int val, void *data __UNUSED__)
{
   ShowHideWinGroups(tmp_ewin, tmp_groups[tmp_index], SET_OFF);
   ShowHideWinGroups(tmp_ewin, tmp_groups[val], SET_ON);
   tmp_index = val;
}

void
ChooseGroupDialog(EWin * ewin, char *message, char group_select, int action)
{

   Dialog             *d;
   DItem              *table, *di, *radio;
   int                 i, num_groups;
   char              **group_member_strings;

   if (!ewin)
      return;

   tmp_ewin = ewin;
   tmp_group_index = tmp_index = 0;
   tmp_action = action;
   tmp_groups = ListWinGroups(ewin, group_select, &num_groups);

   if ((num_groups == 0)
       && (action == GROUP_OP_BREAK || action == GROUP_OP_DEL))
     {
	DialogOK(_("Window Group Error"),
		 _
		 ("\n  This window currently does not belong to any groups.  \n"
		  "  You can only destroy groups or remove windows from groups  \n"
		  "  through a window that actually belongs to at least one group.\n\n"));
	return;
     }
   if ((num_groups == 0) && (group_select == GROUP_SELECT_ALL_EXCEPT_EWIN))
     {
	DialogOK(_("Window Group Error"),
		 _("\n  Currently, no groups exist or this window  \n"
		   "  already belongs to all existing groups.  \n"
		   "  You have to start other groups first.  \n\n"));
	return;
     }
   if (!tmp_groups)
     {
	DialogOK(_("Window Group Error"),
		 _
		 ("\n  Currently, no groups exist. You have to start a group first.\n\n"));
	return;
     }

   group_member_strings = GetWinGroupMemberNames(tmp_groups, num_groups);
   ShowHideWinGroups(ewin, tmp_groups[0], SET_ON);

   if ((d = FindItem("GROUP_SELECTION", 0, LIST_FINDBY_NAME, LIST_TYPE_DIALOG)))
     {
	SoundPlay("GROUP_SETTINGS_ACTIVE");
	ShowDialog(d);
     }
   SoundPlay("SOUND_SETTINGS_GROUP");

   d = DialogCreate("GROUP_SELECTION");
   DialogSetTitle(d, _("Window Group Selection"));

   table = DialogInitItem(d);
   DialogItemTableSetOptions(table, 2, 0, 0, 0);

   if (Conf.dialogs.headers)
     {
	di = DialogAddItem(table, DITEM_IMAGE);
	DialogItemImageSetFile(di, "pix/group.png");

	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetText(di,
			  _("Enlightenment Window Group\n"
			    "Selection Dialog\n"));

	di = DialogAddItem(table, DITEM_SEPARATOR);
	DialogItemSetColSpan(di, 2);
     }

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 2);
   DialogItemSetAlign(di, 0, 512);
   DialogItemSetText(di, message);

   radio = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetCallback(di, &GroupCallback, 0, (void *)d);
   DialogItemSetText(di, group_member_strings[0]);
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 0);

   for (i = 1; i < num_groups; i++)
     {
	di = DialogAddItem(table, DITEM_RADIOBUTTON);
	DialogItemSetColSpan(di, 2);
	DialogItemSetCallback(di, GroupCallback, i, NULL);
	DialogItemSetText(di, group_member_strings[i]);
	DialogItemRadioButtonSetFirst(di, radio);
	DialogItemRadioButtonGroupSetVal(di, i);
     }
   DialogItemRadioButtonGroupSetValPtr(radio, &tmp_group_index);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   DialogAddButton(d, _("OK"), ChooseGroup, 1, DIALOG_BUTTON_OK);
   DialogAddButton(d, _("Close"), ChooseGroup, 1, DIALOG_BUTTON_CLOSE);
   DialogSetExitFunction(d, ChooseGroup, 2);
   DialogBindKey(d, "Escape", ChooseGroupEscape, 0);
   DialogBindKey(d, "Return", ChooseGroup, 0);

   for (i = 0; i < num_groups; i++)
      Efree(group_member_strings[i]);
   Efree(group_member_strings);

   ShowDialog(d);
}

static GroupConfig *tmp_cfgs = NULL;
static int          tmp_current_group;
static int          tmp_index;
static GroupConfig  tmp_cfg;
static EWin        *tmp_ewin;
static Group      **tmp_groups;

static DItem       *di_border;
static DItem       *di_iconify;
static DItem       *di_kill;
static DItem       *di_move;
static DItem       *di_raise;
static DItem       *di_stick;
static DItem       *di_shade;
static DItem       *di_mirror;

static void
CB_ConfigureGroupEscape(Dialog * d, int val __UNUSED__, void *data __UNUSED__)
{
   if (tmp_cfgs)
     {
	ShowHideWinGroups(tmp_ewin, tmp_groups[tmp_current_group], SET_OFF);
	Efree(tmp_cfgs);
	tmp_cfgs = NULL;
     }
   DialogClose(d);
}

static void
CB_ConfigureGroup(Dialog * d __UNUSED__, int val, void *data __UNUSED__)
{
   int                 i;

   if (val < 2)
     {
	for (i = 0; i < tmp_ewin->num_groups; i++)
	   CopyGroupConfig(&(tmp_cfgs[i]), &(tmp_ewin->groups[i]->cfg));
     }
   if (((val == 0) || (val == 2)) && tmp_cfgs)
     {
	ShowHideWinGroups(tmp_ewin, tmp_groups[tmp_current_group], SET_OFF);
	Efree(tmp_cfgs);
	tmp_cfgs = NULL;
     }
   autosave();
}

static void
GroupSelectCallback(Dialog * d, int val, void *data __UNUSED__)
{
   DialogItemCheckButtonSetState(di_border, tmp_cfgs[val].set_border);
   DialogItemCheckButtonSetState(di_kill, tmp_cfgs[val].kill);
   DialogItemCheckButtonSetState(di_move, tmp_cfgs[val].move);
   DialogItemCheckButtonSetState(di_raise, tmp_cfgs[val].raise);
   DialogItemCheckButtonSetState(di_iconify, tmp_cfgs[val].iconify);
   DialogItemCheckButtonSetState(di_stick, tmp_cfgs[val].stick);
   DialogItemCheckButtonSetState(di_shade, tmp_cfgs[val].shade);
   DialogItemCheckButtonSetState(di_mirror, tmp_cfgs[val].mirror);
   DialogRedraw(d);
   ShowHideWinGroups(tmp_ewin, tmp_groups[tmp_current_group], SET_OFF);
   ShowHideWinGroups(tmp_ewin, tmp_groups[val], SET_ON);
   tmp_current_group = val;
}

static void
GroupFeatureChangeCallback(Dialog * d __UNUSED__, int val, void *data)
{
   switch (val)
     {
     case GROUP_FEATURE_BORDER:
	tmp_cfgs[tmp_current_group].set_border = *((char *)data);
	break;
     case GROUP_FEATURE_KILL:
	tmp_cfgs[tmp_current_group].kill = *((char *)data);
	break;
     case GROUP_FEATURE_MOVE:
	tmp_cfgs[tmp_current_group].move = *((char *)data);
	break;
     case GROUP_FEATURE_RAISE:
	tmp_cfgs[tmp_current_group].raise = *((char *)data);
	break;
     case GROUP_FEATURE_ICONIFY:
	tmp_cfgs[tmp_current_group].iconify = *((char *)data);
	break;
     case GROUP_FEATURE_STICK:
	tmp_cfgs[tmp_current_group].stick = *((char *)data);
	break;
     case GROUP_FEATURE_SHADE:
	tmp_cfgs[tmp_current_group].shade = *((char *)data);
	break;
     case GROUP_FEATURE_MIRROR:
	tmp_cfgs[tmp_current_group].mirror = *((char *)data);
	break;
     default:
	break;
     }
}

static void
SettingsGroups(EWin * ewin)
{
   Dialog             *d;
   DItem              *table, *radio, *di;
   int                 i;
   char              **group_member_strings;

   if (!ewin)
      return;
   if (ewin->num_groups == 0)
     {
	DialogOK(_("Window Group Error"),
		 _
		 ("\n  This window currently does not belong to any groups.  \n\n"));
	return;
     }
   if ((d = FindItem("CONFIGURE_GROUP", 0, LIST_FINDBY_NAME, LIST_TYPE_DIALOG)))
     {
	SoundPlay("GROUP_SETTINGS_ACTIVE");
	ShowDialog(d);
	return;
     }
   SoundPlay("SOUND_SETTINGS_GROUP");

   tmp_ewin = ewin;
   tmp_groups = ewin->groups;
   tmp_cfgs = (GroupConfig *) Emalloc(ewin->num_groups * sizeof(GroupConfig));
   tmp_current_group = 0;
   group_member_strings =
      GetWinGroupMemberNames(ewin->groups, ewin->num_groups);
   ShowHideWinGroups(ewin, ewin->groups[0], SET_ON);

   for (i = 0; i < ewin->num_groups; i++)
      CopyGroupConfig(&(ewin->groups[i]->cfg), &(tmp_cfgs[i]));

   d = DialogCreate("CONFIGURE_GROUP");
   DialogSetTitle(d, _("Window Group Settings"));

   table = DialogInitItem(d);
   DialogItemTableSetOptions(table, 2, 0, 0, 0);

   if (Conf.dialogs.headers)
     {
	di = DialogAddItem(table, DITEM_IMAGE);
	DialogItemImageSetFile(di, "pix/group.png");

	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetText(di,
			  _("Enlightenment Window Group\n"
			    "Settings Dialog\n"));

	di = DialogAddItem(table, DITEM_SEPARATOR);
	DialogItemSetColSpan(di, 2);
     }

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 2);
   DialogItemSetAlign(di, 0, 512);
   DialogItemSetText(di, _("   Pick the group to configure:   "));

   radio = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetCallback(di, &GroupSelectCallback, 0, (void *)d);
   DialogItemSetText(di, group_member_strings[0]);
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 0);

   for (i = 1; i < ewin->num_groups; i++)
     {
	di = DialogAddItem(table, DITEM_RADIOBUTTON);
	DialogItemSetColSpan(di, 2);
	DialogItemSetCallback(di, &GroupSelectCallback, i, (void *)d);
	DialogItemSetText(di, group_member_strings[i]);
	DialogItemRadioButtonSetFirst(di, radio);
	DialogItemRadioButtonGroupSetVal(di, i);
     }
   DialogItemRadioButtonGroupSetValPtr(radio, &tmp_index);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 2);
   DialogItemSetAlign(di, 0, 512);
   DialogItemSetText(di,
		     _("  The following actions are  \n"
		       "  applied to all group members:  "));

   di_border = di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetCallback(di, &GroupFeatureChangeCallback, GROUP_FEATURE_BORDER,
			 &(tmp_cfg.set_border));
   DialogItemSetText(di, _("Changing Border Style"));
   DialogItemCheckButtonSetState(di, tmp_cfgs[0].set_border);
   DialogItemCheckButtonSetPtr(di, &(tmp_cfg.set_border));

   di_iconify = di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetCallback(di, &GroupFeatureChangeCallback,
			 GROUP_FEATURE_ICONIFY, &(tmp_cfg.iconify));
   DialogItemSetText(di, _("Iconifying"));
   DialogItemCheckButtonSetState(di, tmp_cfgs[0].iconify);
   DialogItemCheckButtonSetPtr(di, &(tmp_cfg.iconify));

   di_kill = di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetCallback(di, &GroupFeatureChangeCallback, GROUP_FEATURE_KILL,
			 &(tmp_cfg.kill));
   DialogItemSetText(di, _("Killing"));
   DialogItemCheckButtonSetState(di, tmp_cfgs[0].kill);
   DialogItemCheckButtonSetPtr(di, &(tmp_cfg.kill));

   di_move = di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetCallback(di, &GroupFeatureChangeCallback, GROUP_FEATURE_MOVE,
			 &(tmp_cfg.move));
   DialogItemSetText(di, _("Moving"));
   DialogItemCheckButtonSetState(di, tmp_cfgs[0].move);
   DialogItemCheckButtonSetPtr(di, &(tmp_cfg.move));

   di_raise = di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetCallback(di, &GroupFeatureChangeCallback, GROUP_FEATURE_RAISE,
			 &(tmp_cfg.raise));
   DialogItemSetText(di, _("Raising/Lowering"));
   DialogItemCheckButtonSetState(di, tmp_cfgs[0].raise);
   DialogItemCheckButtonSetPtr(di, &(tmp_cfg.raise));

   di_stick = di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetCallback(di, &GroupFeatureChangeCallback, GROUP_FEATURE_STICK,
			 &(tmp_cfg.stick));
   DialogItemSetText(di, _("Sticking"));
   DialogItemCheckButtonSetState(di, tmp_cfgs[0].stick);
   DialogItemCheckButtonSetPtr(di, &(tmp_cfg.stick));

   di_shade = di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetCallback(di, &GroupFeatureChangeCallback, GROUP_FEATURE_SHADE,
			 &(tmp_cfg.shade));
   DialogItemSetText(di, _("Shading"));
   DialogItemCheckButtonSetState(di, tmp_cfgs[0].shade);
   DialogItemCheckButtonSetPtr(di, &(tmp_cfg.shade));

   di_mirror = di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetCallback(di, &GroupFeatureChangeCallback, GROUP_FEATURE_MIRROR,
			 &(tmp_cfg.mirror));
   DialogItemSetText(di, _("Mirror Shade/Iconify/Stick"));
   DialogItemCheckButtonSetState(di, tmp_cfgs[0].mirror);
   DialogItemCheckButtonSetPtr(di, &(tmp_cfg.mirror));

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   DialogAddButton(d, _("OK"), CB_ConfigureGroup, 1, DIALOG_BUTTON_OK);
   DialogAddButton(d, _("Apply"), CB_ConfigureGroup, 0, DIALOG_BUTTON_APPLY);
   DialogAddButton(d, _("Close"), CB_ConfigureGroup, 1, DIALOG_BUTTON_CLOSE);
   DialogSetExitFunction(d, CB_ConfigureGroup, 2);
   DialogBindKey(d, "Escape", CB_ConfigureGroupEscape, 0);
   DialogBindKey(d, "Return", CB_ConfigureGroup, 0);

   for (i = 0; i < ewin->num_groups; i++)
      Efree(group_member_strings[i]);
   Efree(group_member_strings);

   ShowDialog(d);
}

static GroupConfig  tmp_group_cfg;
static char         tmp_group_swap;
static void
CB_ConfigureDefaultGroupSettings(Dialog * d __UNUSED__, int val,
				 void *data __UNUSED__)
{
   if (val < 2)
     {
	CopyGroupConfig(&tmp_group_cfg, &(Conf.groups.dflt));
	Conf.groups.swapmove = tmp_group_swap;
     }
   autosave();
}

static void
SettingsDefaultGroupControl(void)
{
   Dialog             *d;
   DItem              *table, *di;

   if ((d = FindItem("CONFIGURE_DEFAULT_GROUP_CONTROL", 0, LIST_FINDBY_NAME,
		     LIST_TYPE_DIALOG)))
     {
	SoundPlay("SOUND_SETTINGS_ACTIVE");
	ShowDialog(d);
	return;
     }
   SoundPlay("SOUND_SETTINGS_GROUP");

   CopyGroupConfig(&(Conf.groups.dflt), &tmp_group_cfg);
   tmp_group_swap = Conf.groups.swapmove;

   d = DialogCreate("CONFIGURE_DEFAULT_GROUP_CONTROL");
   DialogSetTitle(d, _("Default Group Control Settings"));

   table = DialogInitItem(d);
   DialogItemTableSetOptions(table, 2, 0, 0, 0);

   if (Conf.dialogs.headers)
     {
	di = DialogAddItem(table, DITEM_IMAGE);
	DialogItemImageSetFile(di, "pix/group.png");

	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetText(di,
			  _("Enlightenment Default\n"
			    "Group Control Settings Dialog\n"));

	di = DialogAddItem(table, DITEM_SEPARATOR);
	DialogItemSetColSpan(di, 2);
     }

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 2);
   DialogItemSetAlign(di, 0, 512);
   DialogItemSetText(di, _(" Per-group settings: "));

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Changing Border Style"));
   DialogItemCheckButtonSetPtr(di, &(tmp_group_cfg.set_border));

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Iconifying"));
   DialogItemCheckButtonSetPtr(di, &(tmp_group_cfg.iconify));

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Killing"));
   DialogItemCheckButtonSetPtr(di, &(tmp_group_cfg.kill));

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Moving"));
   DialogItemCheckButtonSetPtr(di, &(tmp_group_cfg.move));

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Raising/Lowering"));
   DialogItemCheckButtonSetPtr(di, &(tmp_group_cfg.raise));

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Sticking"));
   DialogItemCheckButtonSetPtr(di, &(tmp_group_cfg.stick));

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Shading"));
   DialogItemCheckButtonSetPtr(di, &(tmp_group_cfg.shade));

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Mirror Shade/Iconify/Stick"));
   DialogItemCheckButtonSetPtr(di, &(tmp_group_cfg.mirror));

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 2);
   DialogItemSetAlign(di, 0, 512);
   DialogItemSetText(di, _(" Global settings: "));

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Swap Window Locations"));
   DialogItemCheckButtonSetPtr(di, &(tmp_group_swap));

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   DialogAddButton(d, _("OK"), CB_ConfigureDefaultGroupSettings, 1,
		   DIALOG_BUTTON_OK);
   DialogAddButton(d, _("Apply"), CB_ConfigureDefaultGroupSettings, 0,
		   DIALOG_BUTTON_APPLY);
   DialogAddButton(d, _("Close"), CB_ConfigureDefaultGroupSettings, 1,
		   DIALOG_BUTTON_CLOSE);
   DialogSetExitFunction(d, CB_ConfigureDefaultGroupSettings, 2);
   DialogBindKey(d, "Escape", DialogCallbackClose, 0);
   DialogBindKey(d, "Return", CB_ConfigureDefaultGroupSettings, 0);
   ShowDialog(d);
}

/*
 * Groups module
 */

static void
GroupsConfigure(const char *params)
{
   char                s[128];
   const char         *p;
   int                 l;
   EWin               *ewin;

   p = params;
   l = 0;
   s[0] = '\0';
   sscanf(p, "%100s %n", s, &l);
   p += l;

   if (!strcmp(s, "group"))
     {
	ewin = GetFocusEwin();
	if (ewin)
	   SettingsGroups(ewin);
     }
   else if (!s[0] || !strcmp(s, "group_defaults"))
     {
	SettingsDefaultGroupControl();
     }
   else if (!strcmp(s, "group_membership"))
     {
	ewin = GetFocusEwin();
	if (ewin)
	   ChooseGroupDialog(ewin,
			     _
			     ("  Pick the group the window will belong to:  \n"),
			     GROUP_SELECT_ALL_EXCEPT_EWIN, GROUP_OP_ADD);
     }
}

static void
GroupsSighan(int sig, void *prm __UNUSED__)
{
   switch (sig)
     {
     case ESIGNAL_INIT:
	GroupsLoad();
	break;
     }
}

#if 0				/* FIXME - Obsolete? */
static int
doShowHideGroup(EWin * ewin, const char *params __UNUSED__)
{
   ShowHideWinGroups(ewin, NULL, SET_TOGGLE);
   return 0;
}

static int
doStartGroup(EWin * ewin, const char *params __UNUSED__)
{
   BuildWindowGroup(&ewin, 1);
   SaveGroups();
   return 0;
}

static int
doAddToGroup(EWin * ewin, const char *params __UNUSED__)
{
   if (!Mode.groups.current)
     {
	ChooseGroupDialog(ewin,
			  _("\n  There's no current group at the moment.  \n"
			    "  The current group is the last one you created,  \n"
			    "  and it exists until you create a new one or break  \n"
			    "  the latest one.  \n\n"
			    "  Pick another group that the window will belong to here:  \n\n"),
			  GROUP_SELECT_ALL_EXCEPT_EWIN, GROUP_OP_ADD);
	return 0;
     }
   else
      AddEwinToGroup(ewin, Mode.groups.current);
   SaveGroups();
   return 0;
}

static int
doRemoveFromGroup(EWin * ewin, const char *params __UNUSED__)
{
   ChooseGroupDialog(ewin,
		     _("   Select the group to remove the window from.  "),
		     GROUP_SELECT_EWIN_ONLY, GROUP_OP_DEL);

   SaveGroups();
   return 0;
}

static int
doBreakGroup(EWin * ewin, const char *params __UNUSED__)
{
   ChooseGroupDialog(ewin, _("  Select the group to break  "),
		     GROUP_SELECT_EWIN_ONLY, GROUP_OP_BREAK);
   SaveGroups();
   return 0;
}
#endif

static void
IPC_GroupInfo(const char *params, Client * c __UNUSED__)
{
   char                buf[FILEPATH_LEN_MAX];
   Group             **groups = NULL;
   int                 num_groups, i, j;

   buf[0] = 0;

   if (params)
     {
	Group              *group;
	char                groupid[FILEPATH_LEN_MAX];
	int                 gix;

	groupid[0] = 0;
	word(params, 1, groupid);
	sscanf(groupid, "%d", &gix);

	group = FindItem(NULL, gix, LIST_FINDBY_ID, LIST_TYPE_GROUP);

	if (!group)
	  {
	     IpcPrintf("Error: no such group: %d", gix);
	     return;
	  }

	groups = (Group **) Emalloc(sizeof(Group **));
	if (!groups)
	   return;

	groups[0] = group;
	num_groups = 1;
     }
   else
     {
	groups = (Group **) ListItemType(&num_groups, LIST_TYPE_GROUP);

	IpcPrintf("Number of groups: %d\n", num_groups);
     }

   for (i = 0; i < num_groups; i++)
     {
	for (j = 0; j < groups[i]->num_members; j++)
	   IpcPrintf("%d: %s\n", groups[i]->index,
		     groups[i]->members[j]->icccm.wm_name);

	IpcPrintf("        index: %d\n" "  num_members: %d\n"
		  "      iconify: %d\n" "         kill: %d\n"
		  "         move: %d\n" "        raise: %d\n"
		  "   set_border: %d\n" "        stick: %d\n"
		  "        shade: %d\n" "       mirror: %d\n",
		  groups[i]->index, groups[i]->num_members,
		  groups[i]->cfg.iconify, groups[i]->cfg.kill,
		  groups[i]->cfg.move, groups[i]->cfg.raise,
		  groups[i]->cfg.set_border, groups[i]->cfg.stick,
		  groups[i]->cfg.shade, groups[i]->cfg.mirror);
     }

   if (groups)
      Efree(groups);
}

static void
IPC_GroupOps(const char *params, Client * c __UNUSED__)
{
   Group              *group = Mode.groups.current;
   char                groupid[FILEPATH_LEN_MAX];
   int                 gix;
   char                windowid[FILEPATH_LEN_MAX];
   char                operation[FILEPATH_LEN_MAX];
   char                param1[FILEPATH_LEN_MAX];
   unsigned int        win;
   EWin               *ewin;

   if (!params)
     {
	IpcPrintf("Error: no window specified");
	return;
     }

   windowid[0] = 0;
   operation[0] = 0;
   param1[0] = 0;
   word(params, 1, windowid);
   sscanf(windowid, "%x", &win);
   word(params, 2, operation);

   if (!operation[0])
     {
	IpcPrintf("Error: no operation specified");
	return;
     }

   if (!strcmp(windowid, "*"))
      ewin = GetContextEwin();
   else
      ewin = EwinFindByChildren(win);
   if (!ewin)
     {
	IpcPrintf("Error: no such window: %8x", win);
	return;
     }

   if (!strcmp(operation, "start"))
     {
	BuildWindowGroup(&ewin, 1);
	IpcPrintf("start %8x", win);
     }
   else if (!strcmp(operation, "add"))
     {
	groupid[0] = 0;
	word(params, 3, groupid);

	if (groupid[0])
	  {
	     sscanf(groupid, "%d", &gix);
	     group = FindItem(NULL, gix, LIST_FINDBY_ID, LIST_TYPE_GROUP);
	  }
	AddEwinToGroup(ewin, group);
	IpcPrintf("add %8x", win);
     }
   else if (!strcmp(operation, "del"))
     {
	groupid[0] = 0;
	word(params, 3, groupid);

	if (groupid[0])
	  {
	     sscanf(groupid, "%d", &gix);
	     group = FindItem(NULL, gix, LIST_FINDBY_ID, LIST_TYPE_GROUP);
	  }
	RemoveEwinFromGroup(ewin, group);
	IpcPrintf("del %8x", win);
     }
   else if (!strcmp(operation, "break"))
     {
	groupid[0] = 0;
	word(params, 3, groupid);

	if (groupid[0])
	  {
	     sscanf(groupid, "%d", &gix);
	     group = FindItem(NULL, gix, LIST_FINDBY_ID, LIST_TYPE_GROUP);
	  }
	BreakWindowGroup(ewin, group);
	IpcPrintf("break %8x", win);
     }
   else if (!strcmp(operation, "showhide"))
     {
	ShowHideWinGroups(ewin, NULL, SET_TOGGLE);
	IpcPrintf("showhide %8x", win);
     }
   else
     {
	IpcPrintf("Error: no such operation: %s", operation);
	return;
     }
   SaveGroups();
}

static void
IPC_Group(const char *params, Client * c __UNUSED__)
{
   char                groupid[FILEPATH_LEN_MAX];
   char                operation[FILEPATH_LEN_MAX];
   char                param1[FILEPATH_LEN_MAX];
   int                 gix;
   Group              *group;
   int                 onoff = -1;

   if (!params)
     {
	IpcPrintf("Error: no group specified");
	return;
     }

   groupid[0] = 0;
   operation[0] = 0;
   param1[0] = 0;
   word(params, 1, groupid);
   sscanf(groupid, "%d", &gix);
   word(params, 2, operation);

   if (!operation[0])
     {
	IpcPrintf("Error: no operation specified");
	return;
     }

   group = FindItem(NULL, gix, LIST_FINDBY_ID, LIST_TYPE_GROUP);

   if (!group)
     {
	IpcPrintf("Error: no such group: %d", gix);
	return;
     }

   word(params, 3, param1);
   if (param1[0])
     {
	IpcPrintf("Error: no mode specified");
	return;
     }

   if (!strcmp(param1, "on"))
      onoff = 1;
   else if (!strcmp(param1, "off"))
      onoff = 0;

   if (onoff == -1 && strcmp(param1, "?"))
     {
	IpcPrintf("Error: unknown mode specified");
     }
   else if (!strcmp(operation, "num_members"))
     {
	IpcPrintf("num_members: %d", group->num_members);
	onoff = -1;
     }
   else if (!strcmp(operation, "iconify"))
     {
	if (onoff >= 0)
	   group->cfg.iconify = onoff;
	else
	   onoff = group->cfg.iconify;
     }
   else if (!strcmp(operation, "kill"))
     {
	if (onoff >= 0)
	   group->cfg.kill = onoff;
	else
	   onoff = group->cfg.kill;
     }
   else if (!strcmp(operation, "move"))
     {
	if (onoff >= 0)
	   group->cfg.move = onoff;
	else
	   onoff = group->cfg.move;
     }
   else if (!strcmp(operation, "raise"))
     {
	if (onoff >= 0)
	   group->cfg.raise = onoff;
	else
	   onoff = group->cfg.raise;
     }
   else if (!strcmp(operation, "set_border"))
     {
	if (onoff >= 0)
	   group->cfg.set_border = onoff;
	else
	   onoff = group->cfg.set_border;
     }
   else if (!strcmp(operation, "stick"))
     {
	if (onoff >= 0)
	   group->cfg.stick = onoff;
	else
	   onoff = group->cfg.stick;
     }
   else if (!strcmp(operation, "shade"))
     {
	if (onoff >= 0)
	   group->cfg.shade = onoff;
	else
	   onoff = group->cfg.shade;
     }
   else if (!strcmp(operation, "mirror"))
     {
	if (onoff >= 0)
	   group->cfg.mirror = onoff;
	else
	   onoff = group->cfg.mirror;
     }
   else
     {
	IpcPrintf("Error: no such operation: %s", operation);
	onoff = -1;
     }

   if (onoff == 1)
      IpcPrintf("%s: on", operation);
   else if (onoff == 0)
      IpcPrintf("%s: off", operation);
}

static void
GroupsIpc(const char *params, Client * c __UNUSED__)
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
	/* Show groups */
     }
   else if (!strncmp(cmd, "cfg", 2))
     {
	GroupsConfigure(prm);
     }
}

IpcItem             GroupsIpcArray[] = {
   {
    GroupsIpc,
    "groups", "grp",
    "Configure window groups",
    "  groups cfg           Configure groups\n"}
   ,
   {
    IPC_GroupInfo,
    "group_info", "gl",
    "Retrieve some info on groups",
    "use \"group_info [group_index]\"\n"}
   ,
   {
    IPC_GroupOps,
    "group_op", "gop",
    "Group operations",
    "use \"group_op <windowid> <property> [<value>]\" to perform "
    "group operations on a window.\n" "Available group_op commands are:\n"
    "  group_op <windowid> start\n"
    "  group_op <windowid> add [<group_index>]\n"
    "  group_op <windowid> del [<group_index>]\n"
    "  group_op <windowid> break [<group_index>]\n"
    "  group_op <windowid> showhide\n"}
   ,
   {
    IPC_Group,
    "group", "gc",
    "Group commands",
    "use \"group <groupid> <property> <value>\" to set group properties.\n"
    "Available group commands are:\n"
    "  group <groupid> num_members <on/off/?>\n"
    "  group <groupid> iconify <on/off/?>\n"
    "  group <groupid> kill <on/off/?>\n" "  group <groupid> move <on/off/?>\n"
    "  group <groupid> raise <on/off/?>\n"
    "  group <groupid> set_border <on/off/?>\n"
    "  group <groupid> stick <on/off/?>\n"
    "  group <groupid> shade <on/off/?>\n"
    "  group <groupid> mirror <on/off/?>\n"}
   ,
};
#define N_IPC_FUNCS (sizeof(GroupsIpcArray)/sizeof(IpcItem))

/*
 * Configuration items
 */
static const CfgItem GroupsCfgItems[] = {
   CFG_ITEM_BOOL(Conf.groups, dflt.iconify, 1),
   CFG_ITEM_BOOL(Conf.groups, dflt.kill, 0),
   CFG_ITEM_BOOL(Conf.groups, dflt.mirror, 1),
   CFG_ITEM_BOOL(Conf.groups, dflt.move, 1),
   CFG_ITEM_BOOL(Conf.groups, dflt.raise, 0),
   CFG_ITEM_BOOL(Conf.groups, dflt.set_border, 1),
   CFG_ITEM_BOOL(Conf.groups, dflt.stick, 1),
   CFG_ITEM_BOOL(Conf.groups, dflt.shade, 1),
   CFG_ITEM_BOOL(Conf.groups, swapmove, 1),
};
#define N_CFG_ITEMS (sizeof(GroupsCfgItems)/sizeof(CfgItem))

EModule             ModGroups = {
   "groups", "grp",
   GroupsSighan,
   {N_IPC_FUNCS, GroupsIpcArray},
   {N_CFG_ITEMS, GroupsCfgItems}
};
