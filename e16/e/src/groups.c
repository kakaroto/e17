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

#define DISABLE_PAGER_ICONBOX_GROUPING 0

Group              *
CreateGroup()
{
   Group              *g;
   double              t;

   g = Emalloc(sizeof(Group));
   if (!g)
      EDBUG_RETURN(NULL);

   t = GetTime();
   g->index = (int)((GetTime() - (floor(t / 1000) * 1000)) * 10000);
   /* g->index = (int)(GetTime() * 100); */

   g->cfg.iconify = conf.group_config.iconify;
   g->cfg.kill = conf.group_config.kill;
   g->cfg.move = conf.group_config.move;
   g->cfg.raise = conf.group_config.raise;
   g->cfg.set_border = conf.group_config.set_border;
   g->cfg.stick = conf.group_config.stick;
   g->cfg.shade = conf.group_config.shade;
   g->cfg.mirror = conf.group_config.mirror;
   g->num_members = 0;
   g->members = NULL;

   EDBUG_RETURN(g);
}

void
FreeGroup(Group * g)
{
   if (g)
     {
	if (g == current_group)
	   current_group = NULL;
	if (g->members)
	   Efree(g->members);
	Efree(g);
     }
}

void
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
			  RememberImportantInfoForEwin(ewin2);
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

   current_group = g = CreateGroup();
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
	RememberImportantInfoForEwin(ewin);
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
   int                 i, j, k, i2, x, y;

   if (ewin && g)
     {
	if (ewin->groups)
	  {
	     for (k = 0; k < ewin->num_groups; k++)
	       {
		  /* if the window is actually part of the given group */
		  if (ewin->groups[k] == g)
		    {
		       for (i = 0; i < g->num_members; i++)
			 {
			    if (g->members[i] == ewin)
			      {
				 /* remove it from the group */
				 for (j = i; j < g->num_members - 1; j++)
				    g->members[j] = g->members[j + 1];
				 g->num_members--;
				 if (g->num_members > 0)
				    g->members =
				       Erealloc(g->members,
						sizeof(EWin *) *
						g->num_members);
				 else
				   {
				      RemoveItem((char *)g, 0,
						 LIST_FINDBY_POINTER,
						 LIST_TYPE_GROUP);
				      FreeGroup(g);
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
				       Erealloc(ewin->groups,
						sizeof(Group *) *
						ewin->num_groups);
				 SaveGroups();

				 x = ewin->x;
				 y = ewin->y;
				 if ((ewin->x + ewin->border->border.left + 1) >
				     root.w)
				    x = root.w - ewin->border->border.left - 1;
				 else if ((ewin->x + ewin->w -
					   ewin->border->border.right - 1) < 0)
				    x = 0 - ewin->w +
				       ewin->border->border.right + 1;
				 if ((ewin->y + ewin->border->border.top + 1) >
				     root.h)
				    y = root.h - ewin->border->border.top - 1;
				 else if ((ewin->y + ewin->h -
					   ewin->border->border.bottom - 1) < 0)
				    y = 0 - ewin->h +
				       ewin->border->border.bottom + 1;

				 MoveEwin(ewin, x, y);

				 RememberImportantInfoForEwin(ewin);
				 return;
			      }
			 }
		    }
	       }
	  }
     }
}

char              **
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
		    groups[i]->members[j]->client.title);
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
   Border             *b = NULL;
   Border             *previous_border;

   if (g)
     {
	gwins = g->members;
	num = g->num_members;
     }
   else
     {
	gwins = ListWinGroupMembersForEwin(ewin, ACTION_NONE, 0, &num);
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

		  b = (Border *) FindItem(gwins[i]->border->group_border_name,
					  0, LIST_FINDBY_NAME,
					  LIST_TYPE_BORDER);
		  if (b)
		    {
		       gwins[i]->previous_border = gwins[i]->border;
		       b->ref_count++;
		    }
	       }
	     else if ((previous_border) && (gwins[i]->previous_border))
	       {
		  b = gwins[i]->previous_border;
		  gwins[i]->border->ref_count--;
		  gwins[i]->previous_border = NULL;
	       }
	     break;
	  case SET_ON:
	     if (!gwins[i]->previous_border)
	       {
		  if (!gwins[i]->border->group_border_name)
		     continue;

		  b = (Border *) FindItem(gwins[i]->border->group_border_name,
					  0, LIST_FINDBY_NAME,
					  LIST_TYPE_BORDER);
		  if (b)
		    {
		       gwins[i]->previous_border = gwins[i]->border;
		       b->ref_count++;
		    }
	       }
	     break;
	  case SET_OFF:
	     if (gwins[i]->previous_border)
	       {
		  b = gwins[i]->previous_border;
		  gwins[i]->border->ref_count--;
		  gwins[i]->previous_border = NULL;
	       }
	     break;
	  default:
	     break;
	  }

	if (b)
	  {
	     EwinSetBorder(gwins[i], b, 1);
	     RememberImportantInfoForEwin(gwins[i]);
	  }
     }
   if (!g)
      Efree(gwins);
   SaveGroups();
}

void
SaveGroups(void)
{
   Group             **groups = NULL;
   int                 i, num_groups;

   groups = (Group **) ListItemType(&num_groups, LIST_TYPE_GROUP);
   if (groups)
     {
	FILE               *f;
	char                s[1024];

	Esnprintf(s, sizeof(s), "%s/...e_session-XXXXXX.groups.%i", EDirUser(),
		  root.scr);
	f = fopen(s, "w");
	if (f)
	  {
	     for (i = 0; i < num_groups; i++)
	       {
		  if (groups[i]->members)
		    {
		       /* Only if the group should be remembered, write info */
		       if (groups[i]->members[0]->snap)
			 {
			    if (groups[i]->members[0]->snap->num_groups)
			      {
				 fprintf(f, "NEW: %i\n", groups[i]->index);
				 fprintf(f, "ICONIFY: %i\n",
					 groups[i]->cfg.iconify);
				 fprintf(f, "KILL: %i\n", groups[i]->cfg.kill);
				 fprintf(f, "MOVE: %i\n", groups[i]->cfg.move);
				 fprintf(f, "RAISE: %i\n",
					 groups[i]->cfg.raise);
				 fprintf(f, "SET_BORDER: %i\n",
					 groups[i]->cfg.set_border);
				 fprintf(f, "STICK: %i\n",
					 groups[i]->cfg.stick);
				 fprintf(f, "SHADE: %i\n",
					 groups[i]->cfg.shade);
				 fprintf(f, "MIRROR: %i\n",
					 groups[i]->cfg.mirror);
			      }
			 }
		    }
	       }
	     fclose(f);
	  }
	Efree(groups);
     }
}

void
LoadGroups(void)
{
   FILE               *f;
   char                s[1024];

   Esnprintf(s, sizeof(s), "%s/...e_session-XXXXXX.groups.%i", EDirUser(),
	     root.scr);
   f = fopen(s, "r");
   if (f)
     {
	Group              *g = NULL;

	while (fgets(s, sizeof(s), f))
	  {
	     char                ss[1024];

	     if (strlen(s) > 0)
		s[strlen(s) - 1] = 0;
	     word(s, 1, ss);
	     if (!strcmp(ss, "NEW:"))
	       {
		  g = CreateGroup();
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
}

static int          tmp_group_index;
static int          tmp_index;
static EWin        *tmp_ewin;
static Group      **tmp_groups;
static int          tmp_action;

static void         ChooseGroupEscape(int val, void *data);
static void
ChooseGroupEscape(int val, void *data)
{
   if (tmp_groups)
     {
	ShowHideWinGroups(tmp_ewin, tmp_groups[tmp_index], SET_OFF);
	Efree(tmp_groups);
     }
   DialogClose((Dialog *) data);
   val = 0;
}

static void         ChooseGroup(int val, void *data);
static void
ChooseGroup(int val, void *data)
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
	       case ACTION_ADD_TO_GROUP:
		  AddEwinToGroup(tmp_ewin, tmp_groups[tmp_group_index]);
		  break;
	       case ACTION_REMOVE_FROM_GROUP:
		  RemoveEwinFromGroup(tmp_ewin, tmp_groups[tmp_group_index]);
		  break;
	       case ACTION_BREAK_GROUP:
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
   data = NULL;
}

void
GroupCallback(int val, void *data)
{
   ShowHideWinGroups(tmp_ewin, tmp_groups[tmp_index], SET_OFF);
   ShowHideWinGroups(tmp_ewin, tmp_groups[val], SET_ON);
   tmp_index = val;
   data = NULL;
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
       && (action == ACTION_BREAK_GROUP || action == ACTION_REMOVE_FROM_GROUP))
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

   if (conf.dialogs.headers)
     {
	di = DialogAddItem(table, DITEM_IMAGE);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemImageSetFile(di, "pix/group.png");

	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemTextSetText(di,
			      _("Enlightenment Window Group\n"
				"Selection Dialog\n"));

	di = DialogAddItem(table, DITEM_SEPARATOR);
	DialogItemSetColSpan(di, 2);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemSeparatorSetOrientation(di, 0);
     }

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemTextSetText(di, message);

   radio = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemSetCallback(di, &GroupCallback, 0, (void *)d);
   DialogItemRadioButtonSetText(di, group_member_strings[0]);
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 0);

   for (i = 1; i < num_groups; i++)
     {
	di = DialogAddItem(table, DITEM_RADIOBUTTON);
	DialogItemSetColSpan(di, 2);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemSetCallback(di, &GroupCallback, i, (void *)d);
	DialogItemRadioButtonSetText(di, group_member_strings[i]);
	DialogItemRadioButtonSetFirst(di, radio);
	DialogItemRadioButtonGroupSetVal(di, i);
     }
   DialogItemRadioButtonGroupSetValPtr(radio, &tmp_group_index);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   DialogAddButton(d, _("OK"), ChooseGroup, 1);
   DialogAddButton(d, _("Close"), ChooseGroup, 1);
   DialogSetExitFunction(d, ChooseGroup, 2, d);
   DialogBindKey(d, "Escape", ChooseGroupEscape, 0, d);
   DialogBindKey(d, "Return", ChooseGroup, 0, d);

   for (i = 0; i < num_groups; i++)
      Efree(group_member_strings[i]);
   Efree(group_member_strings);

   ShowDialog(d);
}
