#include "E.h"

Group              *
CreateGroup()
{
   Group              *g;

   g = Emalloc(sizeof(Group));
   if (!g)
      EDBUG_RETURN(NULL);

   g->index = (int)(GetTime() * 100);
   g->iconify = 1;
   g->kill = 0;
   g->move = 1;
   g->raise = 0;
   g->set_border = 1;
   g->stick = 1;
   g->shade = 1;
   g->mirror = 0;
   g->num_members = 0;
   g->members = NULL;

   EDBUG_RETURN(g);
}

void
FreeGroup(Group * g)
{
   if (g)
     {
	if (g->members)
	   Efree(g->members);
	Efree(g);
	if (g == current_group)
	   current_group = NULL;
     }
}

void
BreakWindowGroup(EWin * ewin)
{

   int                 i;
   Group              *g;

   if (ewin)
     {
	if (ewin->group)
	  {
	     g = ewin->group;
	     if (ewin->group->members)
	       {
		  for (i = 0; i < g->num_members; i++)
		    {
		       g->members[i]->group = NULL;
		       RememberImportantInfoForEwin(g->members[i]);
		    }
	       }
	     RemoveItem((char *)g, 0, LIST_FINDBY_POINTER, LIST_TYPE_GROUP);
	     FreeGroup(g);
	  }
     }
}

void
BuildWindowGroup(EWin ** ewins, int num)
{

   int                 i;
   Group              *g;

   g = CreateGroup();
   AddItem(g, NULL, g->index, LIST_TYPE_GROUP);
   current_group = g;

   g->members = Emalloc(sizeof(EWin *) * num);
   g->num_members = num;

   for (i = 0; i < num; i++)
     {
	RemoveEwinFromGroup(ewins[i]);
	g->members[i] = ewins[i];
	ewins[i]->group = g;
	RememberImportantInfoForEwin(ewins[i]);
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

void
AddEwinToGroup(EWin * ewin, Group * g)
{
   if (ewin && g)
     {
	if (!EwinInGroup(ewin, g))
	  {
	     /*RemoveEwinFromGroup(ewin); */
	     ewin->group = g;
	     g->num_members++;
	     g->members = Erealloc(g->members, sizeof(EWin *) * g->num_members);
	     g->members[g->num_members - 1] = ewin;
	     RememberImportantInfoForEwin(ewin);
	  }
     }
}

void
RemoveEwinFromGroup(EWin * ewin)
{
   int                 i, j;

   if (ewin)
     {
	if (ewin->group)
	  {
	     for (i = 0; i < ewin->group->num_members; i++)
	       {
		  if (ewin->group->members[i] == ewin)
		    {
		       for (j = i; j < ewin->group->num_members - 1; j++)
			  ewin->group->members[j] = ewin->group->members[j + 1];
		       ewin->group->num_members--;
		       if (ewin->group->num_members > 0)
			  ewin->group->members = Erealloc(ewin->group->members, sizeof(EWin *) * ewin->group->num_members);
		       else
			 {
			    RemoveItem((char *)ewin->group, 0, LIST_FINDBY_POINTER, LIST_TYPE_GROUP);
			    FreeGroup(ewin->group);
			 }
		       ewin->group = NULL;
		       RememberImportantInfoForEwin(ewin);
		       return;
		    }
	       }
	  }
	ewin->group = NULL;
     }
}

static void         ChooseGroupEscape(int val, void *data);
static void
ChooseGroupEscape(int val, void *data)
{
   DialogClose((Dialog *) data);
   val = 0;
}

static int          tmp_group_index;
static EWin        *tmp_ewin;
static void         ChooseGroupForEwin(int val, void *data);
static void
ChooseGroupForEwin(int val, void *data)
{
   Group             **groups = NULL;
   int                 num_groups;

   if (val < 2)
     {
	groups = (Group **) ListItemType(&num_groups, LIST_TYPE_GROUP);
	if (groups)
	  {
	     if (num_groups >= tmp_group_index)		/* that's paranoid */
		AddEwinToGroup(tmp_ewin, groups[tmp_group_index]);
	     Efree(groups);
	  }
     }
   SaveGroups();
   data = NULL;
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

	Esnprintf(s, sizeof(s), "%s/...e_session-XXXXXX.groups.%i",
		  UserEDir(), root.scr);
	f = fopen(s, "w");
	if (f)
	  {
	     for (i = 0; i < num_groups; i++)
	       {
		  if (groups[i]->members)
		    {
		       fprintf(f, "NEW: %i\n", groups[i]->index);
		       fprintf(f, "ICONIFY: %i\n", groups[i]->iconify);
		       fprintf(f, "KILL: %i\n", groups[i]->kill);
		       fprintf(f, "MOVE: %i\n", groups[i]->move);
		       fprintf(f, "RAISE: %i\n", groups[i]->raise);
		       fprintf(f, "SET_BORDER: %i\n", groups[i]->set_border);
		       fprintf(f, "STICK: %i\n", groups[i]->stick);
		       fprintf(f, "SHADE: %i\n", groups[i]->shade);
		       fprintf(f, "MIRROR: %i\n", groups[i]->mirror);
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

   Esnprintf(s, sizeof(s), "%s/...e_session-XXXXXX.groups.%i",
	     UserEDir(), root.scr);
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
		     g->iconify = (char)atoi(ss);
	       }
	     else if (!strcmp(ss, "KILL:"))
	       {
		  word(s, 2, ss);
		  if (g)
		     g->kill = (char)atoi(ss);
	       }
	     else if (!strcmp(ss, "MOVE:"))
	       {
		  word(s, 2, ss);
		  if (g)
		     g->move = (char)atoi(ss);
	       }
	     else if (!strcmp(ss, "RAISE:"))
	       {
		  word(s, 2, ss);
		  if (g)
		     g->raise = (char)atoi(ss);
	       }
	     else if (!strcmp(ss, "SET_BORDER:"))
	       {
		  word(s, 2, ss);
		  if (g)
		     g->set_border = (char)atoi(ss);
	       }
	     else if (!strcmp(ss, "STICK:"))
	       {
		  word(s, 2, ss);
		  if (g)
		     g->stick = (char)atoi(ss);
	       }
	     else if (!strcmp(ss, "SHADE:"))
	       {
		  word(s, 2, ss);
		  if (g)
		     g->shade = (char)atoi(ss);
	       }
	     else if (!strcmp(ss, "MIRROR:"))
	       {
		  word(s, 2, ss);
		  if (g)
		     g->mirror = (char)atoi(ss);
	       }
	  }
	fclose(f);
     }
}

void
ChooseGroupForEwinDialog(EWin * ewin)
{

   Dialog             *d;
   DItem              *table, *di, *radio;
   Group             **groups = NULL;
   int                 num_groups, i, j;
   char              **group_member_strings;

   if (!ewin)
      return;

   tmp_ewin = ewin;

   groups = (Group **) ListItemType(&num_groups, LIST_TYPE_GROUP);
   if (!groups)
     {
	DIALOG_OK("Window Group Error", "\n  Currently, no groups exist. You have to start a group first.  \n");
	/* FIXME... so we could start a group here by default...? */
	EDBUG_RETURN_;
     }

   group_member_strings = Emalloc(sizeof(char *) * num_groups);

   for (i = 0; i < num_groups; i++)
     {
	group_member_strings[i] = Emalloc(sizeof(char) * 1024);

	group_member_strings[i][0] = 0;
	for (j = 0; j < groups[i]->num_members; j++)
	  {
	     strcat(group_member_strings[i], groups[i]->members[j]->client.title);
	     strcat(group_member_strings[i], "\n");
	  }
     }

   if ((d = FindItem("GROUP_SELECTION", 0, LIST_FINDBY_NAME, LIST_TYPE_DIALOG)))
     {
	AUDIO_PLAY("GROUP_SETTINGS_ACTIVE");
	ShowDialog(d);
	return;
     }
   AUDIO_PLAY("SOUND_SETTINGS_GROUP");

   d = CreateDialog("GROUP_SELECTION");
   DialogSetTitle(d, "Window Group Selection");

   table = DialogInitItem(d);
   DialogItemTableSetOptions(table, 2, 0, 0, 0);

   di = DialogAddItem(table, DITEM_IMAGE);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemImageSetFile(di, "pix/place.png");

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemTextSetText(di,
			 "Enlightenment Window Group\n"
			 "Selection Dialog\n");

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemTextSetText(di, "  Pick the group the window will belong to:  \n");

   radio = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemRadioButtonSetText(di, group_member_strings[0]);
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 0);

   for (i = 1; i < num_groups; i++)
     {
	di = DialogAddItem(table, DITEM_RADIOBUTTON);
	DialogItemSetColSpan(di, 2);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
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

   DialogAddButton(d, "OK", ChooseGroupForEwin, 1);
   DialogAddButton(d, "Apply", ChooseGroupForEwin, 0);
   DialogAddButton(d, "Close", ChooseGroupForEwin, 1);
   DialogSetExitFunction(d, ChooseGroupForEwin, 2, d);
   DialogBindKey(d, "Escape", ChooseGroupEscape, 0, d);
   DialogBindKey(d, "Return", ChooseGroupForEwin, 0, d);

   for (i = 0; i < num_groups; i++)
      Efree(group_member_strings[i]);
   Efree(groups);
   Efree(group_member_strings);

   ShowDialog(d);
}
