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

Button             *
FindButton(Window win)
{
   Button             *b;
   Button            **buttons;
   int                 i, num;

   EDBUG(6, "FindButton");

   buttons = (Button **) ListItemType(&num, LIST_TYPE_BUTTON);
   for (i = 0; i < num; i++)
     {
	if ((win == buttons[i]->win) || (win == buttons[i]->inside_win) ||
	    (win == buttons[i]->event_win))
	  {
	     b = buttons[i];
	     Efree(buttons);
	     EDBUG_RETURN(b);
	  }
     }
   if (buttons)
      Efree(buttons);
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
      EDBUG_RETURN(b->aclass);
   ewin = FindEwinByDecoration(win);
   if (ewin)
     {
	for (i = 0; i < ewin->border->num_winparts; i++)
	   if (win == ewin->bits[i].win)
	      EDBUG_RETURN(ewin->border->part[i].aclass);
     }
   EDBUG_RETURN(NULL);
}

Menu               *
FindMenuItem(Window win, MenuItem ** mi)
{
   Menu               *menu;
   Menu              **menus;
   int                 i, j, num;

   EDBUG(6, "FindMenuItem");

   menus = (Menu **) ListItemType(&num, LIST_TYPE_MENU);
   for (i = 0; i < num; i++)
     {
	for (j = 0; j < menus[i]->num; j++)
	  {
	     if ((win == menus[i]->items[j]->win) ||
		 (win == menus[i]->items[j]->icon_win))
	       {
		  *mi = menus[i]->items[j];
		  menu = menus[i];
		  Efree(menus);
		  EDBUG_RETURN(menu);
	       }
	  }
     }
   if (menus)
      Efree(menus);
   EDBUG_RETURN(NULL);
}

Menu               *
FindMenu(Window win)
{
   Menu               *menu;
   Menu              **menus;
   int                 i, num;

   EDBUG(6, "FindMenu");
   menus = (Menu **) ListItemType(&num, LIST_TYPE_MENU);
   for (i = 0; i < num; i++)
     {
	if (menus[i]->win == win)
	  {
	     menu = menus[i];
	     Efree(menus);
	     EDBUG_RETURN(menu);
	  }
     }
   if (menus)
      Efree(menus);
   EDBUG_RETURN(NULL);
}

EWin               *
FindEwinByMenu(Menu * m)
{
   EWin               *ewin;
   EWin              **ewins;
   int                 i, num;

   EDBUG(6, "FindEwinByMenu");
   ewins = (EWin **) ListItemType(&num, LIST_TYPE_EWIN);
   for (i = 0; i < num; i++)
     {
	if (ewins[i]->menu == m)
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

EWin              **
ListWinGroupMembersForEwin(EWin * ewin, int action, int *num)
{

   EWin              **gwins = NULL;
   int                 daddy_says_no_no = 0;

   if (ewin)
     {
	if (ewin->group)
	  {
	     switch (action)
	       {
	       case ACTION_SET_WINDOW_BORDER:
		  if (!ewin->group->set_border)
		     daddy_says_no_no = 1;
		  break;
	       case ACTION_ICONIFY:
		  if (!ewin->group->iconify)
		     daddy_says_no_no = 1;
		  break;
	       case ACTION_MOVE:
		  if (!ewin->group->move)
		     daddy_says_no_no = 1;
		  break;
	       case ACTION_RAISE:
	       case ACTION_LOWER:
		  if (!ewin->group->raise)
		     daddy_says_no_no = 1;
		  break;
	       case ACTION_STICK:
		  if (!ewin->group->stick)
		     daddy_says_no_no = 1;
		  break;
	       case ACTION_SHADE:
		  if (!ewin->group->shade)
		     daddy_says_no_no = 1;
		  break;
	       case ACTION_KILL:
		  if (!ewin->group->kill)
		     daddy_says_no_no = 1;
		  break;
	       default:
		  break;
	       }
	  }
	else
	  {
	     daddy_says_no_no = 1;
	  }

	if (daddy_says_no_no)
	  {
	     gwins = Emalloc(sizeof(EWin *));
	     gwins[0] = ewin;
	     *num = 1;
	  }
	else
	  {
	     gwins = Emalloc(sizeof(EWin *) * ewin->group->num_members);
	     memcpy(gwins, ewin->group->members, sizeof(EWin *) * ewin->group->num_members);
	     *num = ewin->group->num_members;
	  }
	EDBUG_RETURN(gwins);
     }
   else
      EDBUG_RETURN(NULL);
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

   EDBUG(6, "FindEwinByMenu");
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

Dialog             *
FindDialogButton(Window win, int *bnum)
{
   Dialog             *d;
   Dialog            **ds;
   int                 i, j, num;

   EDBUG(6, "FindDialogButton");

   ds = (Dialog **) ListItemType(&num, LIST_TYPE_DIALOG);
   for (i = 0; i < num; i++)
     {
	for (j = 0; j < ds[i]->num_buttons; j++)
	  {
	     if (win == ds[i]->button[j]->win)
	       {
		  *bnum = j;
		  d = ds[i];
		  Efree(ds);
		  EDBUG_RETURN(d);
	       }
	  }
     }
   if (ds)
      Efree(ds);
   *bnum = 0;
   EDBUG_RETURN(NULL);
}

DItem              *
FindDialogItem(Window win, Dialog ** dret)
{
   DItem              *di;
   Dialog            **ds;
   int                 i, num;

   EDBUG(6, "FindDialogButton");

   ds = (Dialog **) ListItemType(&num, LIST_TYPE_DIALOG);
   for (i = 0; i < num; i++)
     {
	if (ds[i]->item)
	  {
	     if ((di = DialogItemFindWindow(ds[i]->item, win)))
	       {
		  *dret = ds[i];
		  Efree(ds);
		  EDBUG_RETURN(di);
	       }
	  }
     }
   *dret = NULL;
   if (ds)
      Efree(ds);
   EDBUG_RETURN(NULL);
}

Dialog             *
FindDialog(Window win)
{
   Dialog             *d;
   Dialog            **ds;
   int                 i, num;

   EDBUG(6, "FindDialog");
   ds = (Dialog **) ListItemType(&num, LIST_TYPE_DIALOG);
   for (i = 0; i < num; i++)
     {
	if (ds[i]->win == win)
	  {
	     d = ds[i];
	     Efree(ds);
	     EDBUG_RETURN(d);
	  }
     }
   if (ds)
      Efree(ds);
   EDBUG_RETURN(NULL);
}

EWin               *
FindEwinSpawningMenu(Menu * m)
{
   EWin               *ewin;
   EWin              **ewins;
   int                 i, num;

   EDBUG(6, "FindEwinSpawningMenu");
   ewins = (EWin **) ListItemType(&num, LIST_TYPE_EWIN);
   for (i = 0; i < num; i++)
     {
	if (ewins[i]->shownmenu == m->win)
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

Pager              *
FindPager(Window win)
{
   Pager              *p;
   Pager             **ps;
   int                 i, num;

   EDBUG(6, "FindDialog");
   ps = (Pager **) ListItemType(&num, LIST_TYPE_PAGER);
   for (i = 0; i < num; i++)
     {
	if ((ps[i]->win == win) || (ps[i]->hi_win == win))
	  {
	     p = ps[i];
	     Efree(ps);
	     EDBUG_RETURN(p);
	  }
     }
   if (ps)
      Efree(ps);
   EDBUG_RETURN(NULL);
}
