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

EWin               *
FindEwinByBase(Window win)
{
   EWin              **ewins;
   EWin               *ewin;
   int                 i, num;

   EDBUG(6, "FindEwinByBase");
   ewins = (EWin **) ListItemType(&num, LIST_TYPE_EWIN);
   for (i = 0; i < num; i++)
     {
	if (win == ewins[i]->win)
	  {
	     ewin = ewins[i];
	     Efree(ewins);
	     EDBUG_RETURN(ewin);
	  }
     }
   if (ewins)
      Efree(ewins);
   EDBUG_RETURN(NULL);
}

EWin               *
FindEwinByChildren(Window win)
{
   EWin               *ewin;
   EWin              **ewins;
   int                 i, j, num;

   EDBUG(6, "FindEwinByChildren");

   ewins = (EWin **) ListItemType(&num, LIST_TYPE_EWIN);
   for (i = 0; i < num; i++)
     {
	if ((win == ewins[i]->client.win) || (win == ewins[i]->win_container))
	  {
	     ewin = ewins[i];
	     Efree(ewins);
	     EDBUG_RETURN(ewin);
	  }
	else
	  {
	     for (j = 0; j < ewins[i]->border->num_winparts; j++)
		if (win == ewins[i]->bits[j].win)
		  {
		     ewin = ewins[i];
		     Efree(ewins);
		     EDBUG_RETURN(ewin);
		  }
	  }
     }
   if (ewins)
      Efree(ewins);
   EDBUG_RETURN(NULL);
}

EWin               *
FindEwinByPartial(const char *match, int type)
{
   EWin               *ewin = NULL;
   EWin              **ewins;
   int                 i, num, len;
   char                ewinid[FILEPATH_LEN_MAX];

   EDBUG(6, "FindEwinByPartial");

   len = strlen(match);
   if (len <= 0)
      goto exit;

   ewins = (EWin **) ListItemType(&num, LIST_TYPE_EWIN);
   if (ewins == NULL)
      goto exit;

   for (i = 0; i < num; i++)
     {
	if (type == '+')
	  {
	     /* Match start of window ID */
	     sprintf(ewinid, "%x", (unsigned)ewins[i]->client.win);
	     if (strncmp(ewinid, match, len))
		continue;
	  }
	else if (type == '=')
	  {
	     /* Match name (substring) */
	     if (!strstr(ewins[i]->icccm.wm_name, match))
		continue;
	  }
	else
	   goto exit;

	ewin = ewins[i];
	break;
     }
   Efree(ewins);

 exit:
   EDBUG_RETURN(ewin);
}

EWin               *
FindEwinByDecoration(Window win)
{
   EWin               *ewin;
   EWin              **ewins;
   int                 i, j, num;

   EDBUG(6, "FindEwinByDecoration");

   ewins = (EWin **) ListItemType(&num, LIST_TYPE_EWIN);
   for (i = 0; i < num; i++)
     {
	for (j = 0; j < ewins[i]->border->num_winparts; j++)
	  {
	     if (win == ewins[i]->bits[j].win)
	       {
		  ewin = ewins[i];
		  Efree(ewins);
		  EDBUG_RETURN(ewin);
	       }
	  }
     }
   if (ewins)
      Efree(ewins);
   EDBUG_RETURN(NULL);
}

ActionClass        *
FindActionClass(Window win)
{
   Button             *b;
   EWin               *ewin;
   int                 i;

   EDBUG(6, "FindActionClass");

   b = FindButton(win);
   if (b)
      EDBUG_RETURN(ButtonGetAClass(b));

   ewin = FindEwinByDecoration(win);
   if (ewin)
     {
	for (i = 0; i < ewin->border->num_winparts; i++)
	   if (win == ewin->bits[i].win)
	      EDBUG_RETURN(ewin->border->part[i].aclass);
     }

   for (i = 0; i < conf.desks.num; i++)
     {
	ActionClass        *ac;

	if (win == desks.desk[i].win)
	  {
	     ac = FindItem("DESKBINDINGS", 0, LIST_FINDBY_NAME,
			   LIST_TYPE_ACLASS);
	     EDBUG_RETURN(ac);
	  }
     }

   EDBUG_RETURN(NULL);
}

EWin               *
FindEwinByMenu(Menu * m)
{
   EWin               *ewin = NULL;
   EWin              **ewins;
   int                 i, num;

   EDBUG(6, "FindEwinByMenu");

   ewins = (EWin **) ListItemType(&num, LIST_TYPE_EWIN);
   for (i = 0; i < num; i++)
     {
	if (ewins[i]->menu != m)
	   continue;
	ewin = ewins[i];
	break;
     }
   if (ewins)
      Efree(ewins);

   EDBUG_RETURN(ewin);
}

Group             **
ListWinGroups(EWin * ewin, char group_select, int *num)
{
   Group             **groups = NULL;
   Group             **groups2 = NULL;
   int                 i, j, killed = 0;

   switch (group_select)
     {
     case GROUP_SELECT_EWIN_ONLY:
	groups = (Group **) Emalloc(sizeof(Group *) * ewin->num_groups);
	groups =
	   (Group **) memcpy(groups, ewin->groups,
			     sizeof(Group *) * ewin->num_groups);
	*num = ewin->num_groups;
	break;
     case GROUP_SELECT_ALL_EXCEPT_EWIN:
	groups2 = (Group **) ListItemType(num, LIST_TYPE_GROUP);
	if (groups2)
	  {
	     for (i = 0; i < (*num); i++)
	       {
		  for (j = 0; j < ewin->num_groups; j++)
		    {
		       if (ewin->groups[j] == groups2[i])
			 {
			    groups2[i] = NULL;
			    killed++;
			 }
		    }
	       }
	     groups = (Group **) Emalloc(sizeof(Group *) * (*num - killed));
	     j = 0;
	     for (i = 0; i < (*num); i++)
		if (groups2[i])
		   groups[j++] = groups2[i];
	     (*num) -= killed;
	     Efree(groups2);
	  }
	break;
     case GROUP_SELECT_ALL:
     default:
	groups = (Group **) ListItemType(num, LIST_TYPE_GROUP);
	break;
     }

   return groups;
}

EWin              **
ListWinGroupMembersForEwin(EWin * ewin, int action, char nogroup, int *num)
{

   EWin              **gwins = NULL;
   int                 i, j, k, daddy_says_no_no;
   char                inlist;

   if (ewin)
     {
	if (nogroup)
	  {
	     gwins = Emalloc(sizeof(EWin *));
	     gwins[0] = ewin;
	     *num = 1;
	     return gwins;
	  }

	(*num) = 0;

	for (i = 0; i < ewin->num_groups; i++)
	  {
	     daddy_says_no_no = 0;

	     switch (action)
	       {
	       case ACTION_SET_WINDOW_BORDER:
		  if (!ewin->groups[i]->cfg.set_border)
		     daddy_says_no_no = 1;
		  break;
	       case ACTION_ICONIFY:
		  if (!ewin->groups[i]->cfg.iconify)
		     daddy_says_no_no = 1;
		  break;
	       case ACTION_MOVE:
		  if (!ewin->groups[i]->cfg.move)
		     daddy_says_no_no = 1;
		  break;
	       case ACTION_RAISE:
	       case ACTION_LOWER:
	       case ACTION_RAISE_LOWER:
		  if (!ewin->groups[i]->cfg.raise)
		     daddy_says_no_no = 1;
		  break;
	       case ACTION_STICK:
		  if (!ewin->groups[i]->cfg.stick)
		     daddy_says_no_no = 1;
		  break;
	       case ACTION_SHADE:
		  if (!ewin->groups[i]->cfg.shade)
		     daddy_says_no_no = 1;
		  break;
	       case ACTION_KILL:
		  if (!ewin->groups[i]->cfg.kill)
		     daddy_says_no_no = 1;
		  break;
	       default:
		  break;
	       }

	     if (!daddy_says_no_no)
	       {
		  gwins =
		     Erealloc(gwins,
			      sizeof(EWin *) * (*num +
						ewin->groups[i]->num_members));
		  /* Check if a window is not already in the group */
		  for (k = 0; k < ewin->groups[i]->num_members; k++)
		    {
		       /* To get consistent behaviour, limit groups to a single desktop for now: */
		       if (ewin->groups[i]->members[k]->desktop ==
			   ewin->desktop)
			 {
			    inlist = 0;
			    for (j = 0; j < (*num); j++)
			      {
				 if (gwins[j] == ewin->groups[i]->members[k])
				    inlist = 1;
			      }
			    /* If we do not have this one yet, add it to the result */
			    if (!inlist)
			       gwins[(*num)++] = ewin->groups[i]->members[k];
			 }
		    }
		  /* and shrink the result to the correct size. */
		  gwins = Erealloc(gwins, sizeof(EWin *) * (*num));
	       }
	  }

	if ((*num) == 0)
	  {
	     gwins = Emalloc(sizeof(EWin *));
	     gwins[0] = ewin;
	     *num = 1;
	  }
	EDBUG_RETURN(gwins);
     }
   else
     {
	*num = 0;
	EDBUG_RETURN(NULL);
     }
}

EWin              **
ListTransientsFor(Window win, int *num)
{
   EWin              **ewins, **lst = NULL;
   int                 i, j, n;

   EDBUG(6, "ListTransientsFor");

   ewins = (EWin **) ListItemType(&n, LIST_TYPE_EWIN);
   j = 0;
   for (i = 0; i < n; i++)
     {
	if (win == ewins[i]->client.transient_for)
	  {
	     j++;
	     lst = Erealloc(lst, sizeof(EWin *) * j);
	     lst[j - 1] = ewins[i];
	  }
     }
   if (ewins)
      Efree(ewins);
   *num = j;
   EDBUG_RETURN(lst);
}

EWin              **
ListGroupMembers(Window win, int *num)
{
   EWin              **ewins, **lst = NULL;
   int                 i, j, n;

   EDBUG(6, "ListGroupMembers");

   ewins = (EWin **) ListItemType(&n, LIST_TYPE_EWIN);
   j = 0;
   for (i = 0; i < n; i++)
     {
	if (win == ewins[i]->client.group)
	  {
	     j++;
	     lst = Erealloc(lst, sizeof(EWin *) * j);
	     lst[j - 1] = ewins[i];
	  }
     }
   if (ewins)
      Efree(ewins);
   *num = j;
   EDBUG_RETURN(lst);
}

EWin               *
FindEwinByDialog(Dialog * d)
{
   EWin               *ewin;
   EWin              **ewins;
   int                 i, num;

   EDBUG(6, "FindEwinByDialog");
   ewins = (EWin **) ListItemType(&num, LIST_TYPE_EWIN);
   for (i = 0; i < num; i++)
     {
	if (ewins[i]->dialog == d)
	  {
	     ewin = ewins[i];
	     Efree(ewins);
	     EDBUG_RETURN(ewin);
	  }
     }
   if (ewins)
      Efree(ewins);
   EDBUG_RETURN(NULL);
}

char
FindADialog(void)
{
   EWin              **ewins;
   int                 i, num;

   EDBUG(6, "FindADialog");
   ewins = (EWin **) ListItemType(&num, LIST_TYPE_EWIN);
   for (i = 0; i < num; i++)
     {
	if (ewins[i]->dialog)
	  {
	     Efree(ewins);
	     EDBUG_RETURN(1);
	  }
     }
   if (ewins)
      Efree(ewins);
   EDBUG_RETURN(0);
}
