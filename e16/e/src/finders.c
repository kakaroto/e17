/*
 * Copyright (C) 2000-2005 Carsten Haitzler, Geoff Harrison and various contributors
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
#include "ewins.h"

EWin               *
EwinFindByPtr(const EWin * ewin)
{
   EWin               *const *ewins;
   int                 i, num;

   ewins = EwinListGetAll(&num);
   for (i = 0; i < num; i++)
     {
	if (ewin == ewins[i])
	   return ewins[i];
     }
   return NULL;
}

EWin               *
EwinFindByFrame(Window win)
{
   EWin               *const *ewins;
   int                 i, num;

   ewins = EwinListGetAll(&num);
   for (i = 0; i < num; i++)
     {
	if (win == EoGetWin(ewins[i]))
	   return ewins[i];
     }
   return NULL;
}

EWin               *
EwinFindByClient(Window win)
{
   EWin               *const *ewins;
   int                 i, num;

   ewins = EwinListGetAll(&num);
   for (i = 0; i < num; i++)
     {
	if (win == ewins[i]->client.win)
	   return ewins[i];
     }
   return NULL;
}

EWin               *
EwinFindByChildren(Window win)
{
   EWin               *const *ewins;
   int                 i, j, num;

   ewins = EwinListGetAll(&num);
   for (i = 0; i < num; i++)
     {
	if ((win == ewins[i]->client.win) || (win == ewins[i]->win_container))
	  {
	     return ewins[i];
	  }
	else
	  {
	     for (j = 0; j < ewins[i]->border->num_winparts; j++)
		if (win == ewins[i]->bits[j].win)
		  {
		     return ewins[i];
		  }
	  }
     }
   return NULL;
}

EWin               *
EwinFindByString(const char *match, int type)
{
   EWin               *ewin = NULL;
   EWin               *const *ewins;
   int                 i, num, len;
   char                ewinid[FILEPATH_LEN_MAX];
   const char         *name;

   len = strlen(match);
   if (len <= 0)
      goto done;

   ewins = EwinListGetAll(&num);
   if (ewins == NULL)
      goto done;

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
	     name = ewins[i]->icccm.wm_name;
	     if (!name)
		continue;
	     if (!strstr(name, match))
		continue;
	  }
	else
	   goto done;

	ewin = ewins[i];
	break;
     }

 done:
   return ewin;
}

static EWin        *
FindEwinByDecoration(Window win)
{
   EWin               *const *ewins;
   int                 i, j, num;

   ewins = EwinListGetAll(&num);
   for (i = 0; i < num; i++)
     {
	for (j = 0; j < ewins[i]->border->num_winparts; j++)
	  {
	     if (win == ewins[i]->bits[j].win)
		return ewins[i];
	  }
     }

   return NULL;
}

ActionClass        *
FindActionClass(Window win)
{
   Button             *b;
   EWin               *ewin;
   int                 i;

   b = FindButton(win);
   if (b)
      return ButtonGetAClass(b);

   ewin = FindEwinByDecoration(win);
   if (ewin)
     {
	for (i = 0; i < ewin->border->num_winparts; i++)
	   if (win == ewin->bits[i].win)
	      return ewin->border->part[i].aclass;
     }

   for (i = 0; i < Conf.desks.num; i++)
     {
	ActionClass        *ac;

	if (win == DeskGetWin(i))
	  {
	     ac = FindItem("DESKBINDINGS", 0, LIST_FINDBY_NAME,
			   LIST_TYPE_ACLASS);
	     return ac;
	  }
     }

   return NULL;
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
	       case GROUP_ACTION_SET_WINDOW_BORDER:
		  if (!ewin->groups[i]->cfg.set_border)
		     daddy_says_no_no = 1;
		  break;
	       case GROUP_ACTION_ICONIFY:
		  if (!ewin->groups[i]->cfg.iconify)
		     daddy_says_no_no = 1;
		  break;
	       case GROUP_ACTION_MOVE:
		  if (!ewin->groups[i]->cfg.move)
		     daddy_says_no_no = 1;
		  break;
	       case GROUP_ACTION_RAISE:
	       case GROUP_ACTION_LOWER:
	       case GROUP_ACTION_RAISE_LOWER:
		  if (!ewin->groups[i]->cfg.raise)
		     daddy_says_no_no = 1;
		  break;
	       case GROUP_ACTION_STICK:
		  if (!ewin->groups[i]->cfg.stick)
		     daddy_says_no_no = 1;
		  break;
	       case GROUP_ACTION_SHADE:
		  if (!ewin->groups[i]->cfg.shade)
		     daddy_says_no_no = 1;
		  break;
	       case GROUP_ACTION_KILL:
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
		       if (EoGetDesk(ewin->groups[i]->members[k]) ==
			   EoGetDesk(ewin))
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
	return gwins;
     }
   else
     {
	*num = 0;
	return NULL;
     }
}

EWin              **
EwinListTransients(EWin * ewin, int *num, int group)
{
   EWin               *const *ewins, **lst, *ew;
   int                 i, j, n;

   j = 0;
   lst = NULL;

   if (EwinGetTransientCount(ewin) <= 0)
      goto done;

   ewins = EwinListGetAll(&n);

   /* Find regular transients */
   for (i = 0; i < n; i++)
     {
	ew = ewins[i];

	/* Skip self-reference */
	if (ew == ewin)
	   continue;

	if (EwinGetTransientFor(ew) == ewin->client.win)
	  {
	     lst = Erealloc(lst, (j + 1) * sizeof(EWin *));
	     lst[j++] = ew;
	  }
     }

   if (!group)
      goto done;

   /* Group transients (if ewin is not a transient) */
   if (EwinIsTransient(ewin))
      goto done;

   for (i = 0; i < n; i++)
     {
	ew = ewins[i];

	/* Skip self-reference */
	if (ew == ewin)
	   continue;

	if (EwinGetTransientFor(ew) == VRoot.win &&
	    EwinGetWindowGroup(ew) == EwinGetWindowGroup(ewin))
	  {
	     lst = Erealloc(lst, (j + 1) * sizeof(EWin *));
	     lst[j++] = ew;
	  }
     }

 done:
   *num = j;
   return lst;
}

EWin              **
EwinListTransientFor(EWin * ewin, int *num)
{
   EWin               *const *ewins, **lst, *ew;
   int                 i, j, n;

   j = 0;
   lst = NULL;

   if (!EwinIsTransient(ewin))
      goto done;

   ewins = EwinListGetAll(&n);
   for (i = 0; i < n; i++)
     {
	ew = ewins[i];

	/* Skip self-reference */
	if (ew == ewin)
	   continue;

	/* Regular parent or if root trans, top level group members */
	if ((EwinGetTransientFor(ewin) == ew->client.win) ||
	    (!EwinIsTransient(ew) &&
	     EwinGetTransientFor(ewin) == VRoot.win &&
	     EwinGetWindowGroup(ew) == EwinGetWindowGroup(ewin)))
	  {
	     lst = Erealloc(lst, (j + 1) * sizeof(EWin *));
	     lst[j++] = ew;
	  }
     }

 done:
   *num = j;
   return lst;
}
