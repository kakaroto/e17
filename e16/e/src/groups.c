#include "E.h"

Group              *
CreateGroup()
{
   Group              *g;

   g = Emalloc(sizeof(Group));
   if (!g)
      EDBUG_RETURN(NULL);

   /* randomizing this makes snapshotting groups a good deal easier */
   g->index = rand();
   g->iconify = 1;
   g->kill = 0;
   g->move = 1;
   g->raise = 0;
   g->set_border = 1;
   g->stick = 1;
   g->shade = 1;
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
		     g->members[i]->group = NULL;
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
     }
}

void
AddEwinToGroup(EWin * ewin, Group * g)
{
   if (ewin && g)
     {
	RemoveEwinFromGroup(ewin);
	ewin->group = g;
	g->num_members++;
	g->members = Erealloc(g->members, sizeof(EWin *) * g->num_members);
	g->members[g->num_members - 1] = ewin;
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
   data = NULL;
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
	DIALOG_OK("Window Group Error", 
		  "\n"
		  "  Currently, no groups exist.  \n"
		  "  You have to start a group first.  \n");
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
	AUDIO_PLAY("SOUND_GROUP_SETTINGS_ACTIVE");
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
