/*
 * Copyright (C) 2000 Carsten Haitzler, Geoff Harrison and various contributors
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
static ImageClass  *d_ic_default = NULL;
static TextClass   *d_tc_default = NULL;

static void         DialogRealizeTClassDefault(void);
static void         DialogRealizeIClassDefault(void);

static void
DialogRealizeTClassDefault(void)
{
   if (!d_tc_default)
     {
	d_tc_default = CreateTclass();
	d_tc_default->norm.normal = CreateTextState();
	d_tc_default->norm.normal->fontname =
	   duplicate("-*-helvetica-medium-r-*-*-12-*-*-*-*-*-*-*");
	d_tc_default->norm.normal->fg_col.r = 0;
	d_tc_default->norm.normal->fg_col.g = 0;
	d_tc_default->norm.normal->fg_col.b = 0;
     }
}

static void
DialogRealizeIClassDefault(void)
{
   if (!d_ic_default)
     {
	d_ic_default = CreateIclass();
	d_ic_default->name = NULL;

	d_ic_default->norm.normal = CreateImageState();
	d_ic_default->norm.normal->hihi.r = 255;
	d_ic_default->norm.normal->hihi.g = 255;
	d_ic_default->norm.normal->hihi.b = 255;
	d_ic_default->norm.normal->hi.r = 220;
	d_ic_default->norm.normal->hi.g = 220;
	d_ic_default->norm.normal->hi.b = 220;
	d_ic_default->norm.normal->bg.r = 160;
	d_ic_default->norm.normal->bg.g = 160;
	d_ic_default->norm.normal->bg.b = 160;
	d_ic_default->norm.normal->lo.r = 100;
	d_ic_default->norm.normal->lo.g = 100;
	d_ic_default->norm.normal->lo.b = 100;
	d_ic_default->norm.normal->lolo.r = 0;
	d_ic_default->norm.normal->lolo.g = 0;
	d_ic_default->norm.normal->lolo.b = 0;
	d_ic_default->norm.normal->bevelstyle = BEVEL_NEXT;

	d_ic_default->norm.hilited = CreateImageState();
	d_ic_default->norm.hilited->hihi.r = 255;
	d_ic_default->norm.hilited->hihi.g = 255;
	d_ic_default->norm.hilited->hihi.b = 255;
	d_ic_default->norm.hilited->hi.r = 240;
	d_ic_default->norm.hilited->hi.g = 240;
	d_ic_default->norm.hilited->hi.b = 240;
	d_ic_default->norm.hilited->bg.r = 200;
	d_ic_default->norm.hilited->bg.g = 200;
	d_ic_default->norm.hilited->bg.b = 200;
	d_ic_default->norm.hilited->lo.r = 160;
	d_ic_default->norm.hilited->lo.g = 160;
	d_ic_default->norm.hilited->lo.b = 160;
	d_ic_default->norm.hilited->lolo.r = 0;
	d_ic_default->norm.hilited->lolo.g = 0;
	d_ic_default->norm.hilited->lolo.b = 0;
	d_ic_default->norm.hilited->bevelstyle = BEVEL_NEXT;

	d_ic_default->norm.clicked = CreateImageState();
	d_ic_default->norm.clicked->hihi.r = 0;
	d_ic_default->norm.clicked->hihi.g = 0;
	d_ic_default->norm.clicked->hihi.b = 0;
	d_ic_default->norm.clicked->hi.r = 100;
	d_ic_default->norm.clicked->hi.g = 100;
	d_ic_default->norm.clicked->hi.b = 100;
	d_ic_default->norm.clicked->bg.r = 160;
	d_ic_default->norm.clicked->bg.g = 160;
	d_ic_default->norm.clicked->bg.b = 160;
	d_ic_default->norm.clicked->lo.r = 220;
	d_ic_default->norm.clicked->lo.g = 220;
	d_ic_default->norm.clicked->lo.b = 220;
	d_ic_default->norm.clicked->lolo.r = 255;
	d_ic_default->norm.clicked->lolo.g = 255;
	d_ic_default->norm.clicked->lolo.b = 255;
	d_ic_default->norm.clicked->bevelstyle = BEVEL_NEXT;

	d_ic_default->active.normal = CreateImageState();
	d_ic_default->active.normal->hihi.r = 0;
	d_ic_default->active.normal->hihi.g = 0;
	d_ic_default->active.normal->hihi.b = 0;
	d_ic_default->active.normal->hi.r = 100;
	d_ic_default->active.normal->hi.g = 100;
	d_ic_default->active.normal->hi.b = 100;
	d_ic_default->active.normal->bg.r = 160;
	d_ic_default->active.normal->bg.g = 160;
	d_ic_default->active.normal->bg.b = 160;
	d_ic_default->active.normal->lo.r = 220;
	d_ic_default->active.normal->lo.g = 220;
	d_ic_default->active.normal->lo.b = 220;
	d_ic_default->active.normal->lolo.r = 255;
	d_ic_default->active.normal->lolo.g = 255;
	d_ic_default->active.normal->lolo.b = 255;
	d_ic_default->active.normal->bevelstyle = BEVEL_NEXT;

	d_ic_default->active.hilited = CreateImageState();
	d_ic_default->active.hilited->hihi.r = 0;
	d_ic_default->active.hilited->hihi.g = 0;
	d_ic_default->active.hilited->hihi.b = 0;
	d_ic_default->active.hilited->hi.r = 100;
	d_ic_default->active.hilited->hi.g = 100;
	d_ic_default->active.hilited->hi.b = 100;
	d_ic_default->active.hilited->bg.r = 160;
	d_ic_default->active.hilited->bg.g = 160;
	d_ic_default->active.hilited->bg.b = 160;
	d_ic_default->active.hilited->lo.r = 220;
	d_ic_default->active.hilited->lo.g = 220;
	d_ic_default->active.hilited->lo.b = 220;
	d_ic_default->active.hilited->lolo.r = 255;
	d_ic_default->active.hilited->lolo.g = 255;
	d_ic_default->active.hilited->lolo.b = 255;
	d_ic_default->active.hilited->bevelstyle = BEVEL_NEXT;

	d_ic_default->active.clicked = CreateImageState();
	d_ic_default->active.clicked->hihi.r = 0;
	d_ic_default->active.clicked->hihi.g = 0;
	d_ic_default->active.clicked->hihi.b = 0;
	d_ic_default->active.clicked->hi.r = 100;
	d_ic_default->active.clicked->hi.g = 100;
	d_ic_default->active.clicked->hi.b = 100;
	d_ic_default->active.clicked->bg.r = 160;
	d_ic_default->active.clicked->bg.g = 160;
	d_ic_default->active.clicked->bg.b = 160;
	d_ic_default->active.clicked->lo.r = 220;
	d_ic_default->active.clicked->lo.g = 220;
	d_ic_default->active.clicked->lo.b = 220;
	d_ic_default->active.clicked->lolo.r = 255;
	d_ic_default->active.clicked->lolo.g = 255;
	d_ic_default->active.clicked->lolo.b = 255;
	d_ic_default->active.clicked->bevelstyle = BEVEL_NEXT;

	d_ic_default->padding.left = 8;
	d_ic_default->padding.right = 8;
	d_ic_default->padding.top = 8;
	d_ic_default->padding.bottom = 8;
     }
   IclassPopulate(d_ic_default);
}

void
DialogBindKey(Dialog * d, char *key, void (*func) (int val, void *data),
	      int val, void *data)
{
   d->num_bindings++;
   if (!d->keybindings)
      d->keybindings = Emalloc(sizeof(DKeyBind) * d->num_bindings);
   else
      d->keybindings =
	 Erealloc(d->keybindings, sizeof(DKeyBind) * d->num_bindings);
   d->keybindings[d->num_bindings - 1].val = val;
   d->keybindings[d->num_bindings - 1].data = data;
   d->keybindings[d->num_bindings - 1].func = func;
   d->keybindings[d->num_bindings - 1].key =
      XKeysymToKeycode(disp, XStringToKeysym(key));;
}

Dialog             *
CreateDialog(char *name)
{
   Dialog             *d;

   d = Emalloc(sizeof(Dialog));
   d->name = duplicate(name);
   d->title = NULL;
   d->text = NULL;
   d->num_buttons = 0;
   d->win = 0;
   d->button = NULL;
   d->win = ECreateWindow(root.win, -20, -20, 2, 2, 0);

   d->item = NULL;
   d->exit_func = NULL;
   d->exit_val = 0;
   d->exit_data = NULL;

   d->tclass = FindItem("DIALOG", 0, LIST_FINDBY_NAME, LIST_TYPE_TCLASS);
   if (!d->tclass)
     {
	DialogRealizeTClassDefault();
	d->tclass = d_tc_default;
     }
   if (d->tclass)
      d->tclass->ref_count++;

   d->iclass = FindItem("DIALOG", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
   if (!d->iclass)
     {
	DialogRealizeIClassDefault();
	d->iclass = d_ic_default;
     }
   if (d->iclass)
      d->iclass->ref_count++;
   d->num_bindings = 0;
   d->keybindings = NULL;

   return d;
}

void
FreeDButton(DButton * db)
{
   if (db->text)
      Efree(db->text);
   if (db->iclass)
      db->iclass->ref_count--;
   if (db->tclass)
      db->tclass->ref_count--;
   Efree(db);
}

void
FreeDialog(Dialog * d)
{
   int                 i;

   if (d->name)
      Efree(d->name);
   if (d->title)
      Efree(d->title);
   if (d->text)
      Efree(d->text);
   for (i = 0; i < d->num_buttons; i++)
      FreeDButton(d->button[i]);
   if (d->button)
      Efree(d->button);
   if (d->item)
      DialogFreeItem(d->item);
   if (d->iclass)
      d->iclass->ref_count--;
   if (d->tclass)
      d->tclass->ref_count--;
   if (d->keybindings)
      Efree(d->keybindings);
   Efree(d);
}

void
DialogRestart(int val, void *data)
{
   doExit("restart");
   val = 0;
   data = NULL;
}

void
DialogQuit(int val, void *data)
{
   doExit("error");
   val = 0;
   data = NULL;
}

void
DialogAlert(char *fmt, ...)
{
   char                text[10240];
   va_list             ap;

   SC_Kill();
   va_start(ap, fmt);
   Evsnprintf(text, 10240, fmt, ap);
   va_end(ap);
   Alert(text);
}

void
DialogAlertOK(char *fmt, ...)
{
   char                text[10240];
   va_list             ap;

   va_start(ap, fmt);
   Evsnprintf(text, 10240, fmt, ap);
   va_end(ap);
   ASSIGN_ALERT(_("Attention !!!"), _("OK"), " ", " ");
   Alert(text);
   RESET_ALERT;
}

void
DialogSetParamText(Dialog * d, char *fmt, ...)
{
   char                text[10240];
   va_list             ap;

   va_start(ap, fmt);
   Evsnprintf(text, 10240, fmt, ap);
   va_end(ap);
   DialogSetText(d, text);
}

void
DialogSetText(Dialog * d, char *text)
{
   int                 w, h;

   if (d->text)
      Efree(d->text);

   d->text = duplicate(text);
   if ((!d->tclass) || (!d->iclass))
      return;
   TextSize(d->tclass, 0, 0, STATE_NORMAL, text, &w, &h, 17);
   d->w = w + d->iclass->padding.left + d->iclass->padding.right;
   d->h = h + d->iclass->padding.top + d->iclass->padding.bottom;
}

void
DialogSetTitle(Dialog * d, char *title)
{
   if (d->title)
      Efree(d->title);
   d->title = duplicate(title);
}

void
DialogSetExitFunction(Dialog * d, void (*func) (int val, void *data), int val,
		      void *data)
{
   d->exit_func = func;
   d->exit_val = val;
   d->exit_data = data;
}

void
DialogAddButton(Dialog * d, char *text, void (*func) (int val, void *data),
		char close)
{
   DButton            *db;
   int                 w, h;

   db = Emalloc(sizeof(DButton));
   d->num_buttons++;
   d->button = Erealloc(d->button, d->num_buttons * (sizeof(DButton *)));
   d->button[d->num_buttons - 1] = db;
   db->text = duplicate(text);
   db->func = func;
   db->win = ECreateWindow(d->win, -20, -20, 2, 2, 0);
   EMapWindow(disp, db->win);
   db->x = -1;
   db->y = -1;
   db->w = -1;
   db->h = -1;
   db->hilited = 0;
   db->clicked = 0;
   db->close = close;

   db->tclass =
      FindItem("DIALOG_BUTTON", 0, LIST_FINDBY_NAME, LIST_TYPE_TCLASS);
   if (!db->tclass)
     {
	DialogRealizeTClassDefault();
	db->tclass = d_tc_default;
     }
   if (db->tclass)
      db->tclass->ref_count++;

   db->iclass =
      FindItem("DIALOG_BUTTON", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
   if (!db->iclass)
     {
	DialogRealizeTClassDefault();
	db->iclass = d_ic_default;
     }
   if (db->iclass)
      db->iclass->ref_count++;

   TextSize(db->tclass, 0, 0, STATE_NORMAL, text, &w, &h, 17);
   db->w = w + db->iclass->padding.left + db->iclass->padding.right;
   db->h = h + db->iclass->padding.top + db->iclass->padding.bottom;
   XSelectInput(disp, db->win,
		EnterWindowMask | LeaveWindowMask | ButtonPressMask |
		ButtonReleaseMask | ExposureMask);
}

void
DialogDrawButton(Dialog * d, int bnum)
{
   int                 state;

   state = STATE_NORMAL;
   if ((d->button[bnum]->hilited) && (d->button[bnum]->clicked))
     {
	state = STATE_CLICKED;
     }
   else if ((d->button[bnum]->hilited) && (!d->button[bnum]->clicked))
     {
	state = STATE_HILITED;
     }
   else if (!(d->button[bnum]->hilited) && (d->button[bnum]->clicked))
     {
	state = STATE_CLICKED;
     }
   IclassApply(d->button[bnum]->iclass, d->button[bnum]->win,
	       d->button[bnum]->w, d->button[bnum]->h, 0, 0, state, 0);
   TclassApply(d->button[bnum]->iclass, d->button[bnum]->win,
	       d->button[bnum]->w, d->button[bnum]->h, 0, 0, state, 1,
	       d->button[bnum]->tclass, d->button[bnum]->text);
}

void
DialogActivateButton(Window win, int inclick)
{
   Dialog             *d;
   int                 bnum;
   char                doact = 0;

   d = FindDialogButton(win, &bnum);
   if (!d)
      return;
   if ((d->button[bnum]->hilited) && (d->button[bnum]->clicked)
       && (inclick == 3))
      doact = 1;
   if (inclick == 0)
      d->button[bnum]->hilited = 1;
   if (inclick == 1)
      d->button[bnum]->hilited = 0;
   if (inclick == 2)
      d->button[bnum]->clicked = 1;
   if (inclick == 3)
      d->button[bnum]->clicked = 0;
   DialogDrawButton(d, bnum);
   if ((doact) && (d->button[bnum]->func))
      (d->button[bnum]->func) (bnum, d);
   if ((doact) && (d->button[bnum]->close))
      DialogClose(d);
}

void
DialogDraw(Dialog * d)
{
   if ((!d->tclass) || (!d->iclass))
      return;

   if (d->text)
     {
	TclassApply(d->iclass, d->win, d->w, d->h, 0, 0, STATE_NORMAL, 1,
		    d->tclass, d->text);
     }
   else if (d->item)
     {
	DialogDrawItems(d, d->item, 0, 0, 99999, 99999);
     }
}

void
DialogDrawArea(Dialog * d, int x, int y, int w, int h)
{
   if ((!d->tclass) || (!d->iclass))
      return;

   if (d->text)
     {
	TclassApply(d->iclass, d->win, d->w, d->h, 0, 0, STATE_NORMAL, 1,
		    d->tclass, d->text);
     }
   else if (d->item)
     {
	DialogDrawItems(d, d->item, x, y, w, h);
     }
}

void
DialogRedraw(Dialog * d)
{
   int                 i;

   if ((!d->tclass) || (!d->iclass))
      return;
   for (i = 0; i < d->num_buttons; i++)
      DialogDrawButton(d, i);
   DialogDraw(d);
}

void
ShowDialog(Dialog * d)
{
   char                pq;
   int                 i, w, h, mw, mh;
   EWin               *ewin;
   XTextProperty       xtp;
   XClassHint         *xch;
   Snapshot           *sn;

   SC_Kill();
   if (d->title)
     {
	xtp.encoding = XA_STRING;
	xtp.format = 8;
	xtp.value = (unsigned char *)(d->title);
	xtp.nitems = strlen((char *)(xtp.value));
	XSetWMName(disp, d->win, &xtp);
	xch = XAllocClassHint();
	xch->res_name = d->name;
	xch->res_class = "Enlightenment_Dialog";
	XSetClassHint(disp, d->win, xch);
	XFree(xch);
     }
   ewin = FindEwinByDialog(d);
   if (ewin)
     {
	if (ewin->desktop != desks.current)
	   MoveEwinToDesktopAt(ewin, desks.current, ewin->x, ewin->y);
	RaiseEwin(ewin);
	ShowEwin(ewin);
	return;
     }
   if (d->item)
      DialogItemsRealize(d);

   w = d->w;
   h = d->h;
   mw = 0;
   mh = 0;
   for (i = 0; i < d->num_buttons; i++)
     {
	if (d->button[i]->w > mw)
	   mw = d->button[i]->w;
	if (d->button[i]->h > mh)
	   mh = d->button[i]->h;
     }
   h += d->iclass->padding.top + d->iclass->padding.bottom + mh;

   if ((d->iclass->padding.left + d->iclass->padding.right +
	(d->num_buttons *
	 (mw + d->iclass->padding.left + d->iclass->padding.right))) > w)
      w = d->iclass->padding.left + d->iclass->padding.right +
	 (d->num_buttons *
	  (mw + d->iclass->padding.left + d->iclass->padding.right));

   for (i = 0; i < d->num_buttons; i++)
     {
	d->button[i]->x =
	   (((w
	      - (d->iclass->padding.left + d->iclass->padding.right)) -
	     (d->num_buttons *
	      (mw + d->iclass->padding.left +
	       d->iclass->padding.right))) / 2) + d->iclass->padding.left +
	   (i * (mw + d->iclass->padding.left + d->iclass->padding.right)) +
	   d->iclass->padding.left;
	d->button[i]->y =
	   d->h - d->iclass->padding.bottom + d->iclass->padding.top;

	d->button[i]->w = mw;
	d->button[i]->h = mh;
	EMoveResizeWindow(disp, d->button[i]->win, d->button[i]->x,
			  d->button[i]->y, d->button[i]->w, d->button[i]->h);
     }
   d->w = w;
   d->h = d->h - d->iclass->padding.bottom;
   EResizeWindow(disp, d->win, w, h);
   pq = queue_up;
   queue_up = 0;
   IclassApply(d->iclass, d->win, w, h, 0, 0, STATE_NORMAL, 0);

   for (i = 0; i < d->num_buttons; i++)
      IclassApply(d->button[i]->iclass, d->button[i]->win, d->button[i]->w,
		  d->button[i]->h, 0, 0, STATE_NORMAL, 0);
   queue_up = pq;

   ewin = AddInternalToFamily(d->win, 1, NULL, EWIN_TYPE_DIALOG, d);
   XSelectInput(disp, d->win,
		ExposureMask | PointerMotionMask | EnterWindowMask |
		LeaveWindowMask | FocusChangeMask | KeyPressMask);
   if (ewin)
     {
	DesktopRemoveEwin(ewin);
	DesktopAddEwinToTop(ewin);
	sn = FindSnapshot(ewin);
	/* get the size right damnit! */
	if (sn && sn->use_wh)
	   ResizeEwin(ewin, sn->w, sn->h);
	if (sn && sn->use_xy)
	  {
	     MoveEwin(ewin, sn->x, sn->y);
	  }
	else
	  {
	     if (FindADialog())
		ArrangeEwin(ewin);
	     else
		ArrangeEwinCentered(ewin, 0);
	  }
	RestackEwin(ewin);
	ShowEwin(ewin);
	ewin->dialog = d;
     }
   if (!FindDialog(d->win))
      AddItem(d, d->name, d->win, LIST_TYPE_DIALOG);
   XSync(disp, False);
   DialogRedraw(d);
}

void
DialogClose(Dialog * d)
{
   EWin               *ewin;
   XEvent              ev;

   if (!d)
      return;
   ewin = FindEwinByDialog(d);
   EDestroyWindow(disp, d->win);
   if (ewin)
     {
	HideEwin(ewin);
	ev.xunmap.window = d->win;
	HandleUnmap(&ev);
     }
   if (d->exit_func)
      (d->exit_func) (d->exit_val, d->exit_data);
   RemoveItem(NULL, d->win, LIST_FINDBY_ID, LIST_TYPE_DIALOG);
   FreeDialog(d);
}

DItem              *
DialogInitItem(Dialog * d)
{
   if (!d->item)
     {
	DItem              *di;

	di = Emalloc(sizeof(DItem));
	d->item = di;
	di->type = DITEM_TABLE;
	di->func = NULL;
	di->val = 0;
	di->data = NULL;
	di->iclass = NULL;
	di->tclass = NULL;
	di->padding.left = 0;
	di->padding.right = 0;
	di->padding.top = 0;
	di->padding.bottom = 0;
	di->fill_h = 0;
	di->fill_v = 0;
	di->align_h = 512;
	di->align_v = 512;
	di->row_span = 1;
	di->col_span = 1;
	di->x = 0;
	di->y = 0;
	di->w = 0;
	di->h = 0;
	di->hilited = 0;
	di->clicked = 0;
	di->win = 0;
	di->item.table.num_columns = 1;
	di->item.table.border = 0;
	di->item.table.homogenous_h = 0;
	di->item.table.homogenous_v = 0;
	di->item.table.num_items = 0;
	di->item.table.items = NULL;
	return di;
     }
   return NULL;
}

DItem              *
DialogAddItem(DItem * dii, int type)
{
   DItem              *di;

   di = Emalloc(sizeof(DItem));
   di->type = type;
   di->func = NULL;
   di->val = 0;
   di->data = NULL;
   di->iclass = NULL;
   di->tclass = NULL;
   di->padding.left = 0;
   di->padding.right = 0;
   di->padding.top = 0;
   di->padding.bottom = 0;
   di->fill_h = 1;
   di->fill_v = 1;
   di->align_h = 512;
   di->align_v = 512;
   di->row_span = 1;
   di->col_span = 1;

   di->x = 0;
   di->y = 0;
   di->w = 0;
   di->h = 0;
   di->hilited = 0;
   di->clicked = 0;
   di->win = 0;
   switch (di->type)
     {
     case DITEM_NONE:
	break;
     case DITEM_AREA:
	di->item.area.area_win = 0;
	di->item.area.w = 32;
	di->item.area.h = 32;
	di->item.area.event_func = NULL;
	break;
     case DITEM_BUTTON:
	di->item.button.text = NULL;
	break;
     case DITEM_CHECKBUTTON:
	di->item.check_button.text = NULL;
	di->item.check_button.check_win = 0;
	di->item.check_button.onoff = 0;
	di->item.check_button.onoff_ptr = NULL;
	di->item.check_button.check_orig_w = 10;
	di->item.check_button.check_orig_h = 10;
	break;
     case DITEM_TEXT:
	di->item.text.text = NULL;
	break;
     case DITEM_TABLE:
	di->item.table.num_columns = 1;
	di->item.table.border = 0;
	di->item.table.homogenous_h = 0;
	di->item.table.homogenous_v = 0;
	di->item.table.num_items = 0;
	di->item.table.items = NULL;
	break;
     case DITEM_IMAGE:
	di->item.image.image = NULL;
	break;
     case DITEM_SEPARATOR:
	di->item.separator.horizontal = 0;
	break;
     case DITEM_RADIOBUTTON:
	di->item.radio_button.text = NULL;
	di->item.radio_button.radio_win = 0;
	di->item.radio_button.onoff = 0;
	di->item.radio_button.val = 0;
	di->item.radio_button.val_ptr = 0;
	di->item.radio_button.next = NULL;
	di->item.radio_button.first = NULL;
	di->item.radio_button.radio_orig_w = 10;
	di->item.radio_button.radio_orig_h = 10;
	di->item.radio_button.event_func = NULL;
	break;
     case DITEM_SLIDER:
	di->item.slider.horizontal = 1;
	di->item.slider.numeric = 0;
	di->item.slider.numeric_side = 0;
	di->item.slider.upper = 100;
	di->item.slider.lower = 0;
	di->item.slider.unit = 10;
	di->item.slider.jump = 20;
	di->item.slider.val = 0;
	di->item.slider.val_ptr = NULL;
	di->item.slider.min_length = 64;
	di->item.slider.ic_base = NULL;
	di->item.slider.ic_knob = NULL;
	di->item.slider.ic_border = NULL;
	di->item.slider.base_win = 0;
	di->item.slider.knob_win = 0;
	di->item.slider.border_win = 0;
	di->item.slider.base_orig_w = 10;
	di->item.slider.base_orig_h = 10;
	di->item.slider.knob_orig_w = 6;
	di->item.slider.knob_orig_h = 6;
	di->item.slider.border_orig_w = 14;
	di->item.slider.border_orig_h = 14;
	di->item.slider.base_x = 0;
	di->item.slider.base_y = 0;
	di->item.slider.base_w = 0;
	di->item.slider.base_h = 0;
	di->item.slider.knob_x = 0;
	di->item.slider.knob_y = 0;
	di->item.slider.knob_w = 0;
	di->item.slider.knob_h = 0;
	di->item.slider.border_x = 0;
	di->item.slider.border_y = 0;
	di->item.slider.border_w = 0;
	di->item.slider.border_h = 0;
	di->item.slider.numeric_x = 0;
	di->item.slider.numeric_y = 0;
	di->item.slider.numeric_w = 0;
	di->item.slider.numeric_h = 0;
	di->item.slider.in_drag = 0;
	di->item.slider.wanted_val = 0;
	break;
     default:
	break;
     }

   if (dii)
     {
	dii->item.table.num_items++;
	dii->item.table.items =
	   Erealloc(dii->item.table.items,
		    sizeof(DItem *) * dii->item.table.num_items);
	dii->item.table.items[dii->item.table.num_items - 1] = di;
     }
   return di;
}

void
DialogItemSetCallback(DItem * di, void (*func) (int val, void *data), int val,
		      char *data)
{
   di->func = func;
   di->val = val;
   di->data = data;
}

void
DialogItemSetClass(DItem * di, ImageClass * iclass, TextClass * tclass)
{
   if (di->iclass)
      di->iclass->ref_count--;
   di->iclass = iclass;
   if (di->iclass)
      di->iclass->ref_count++;

   if (di->tclass)
      di->tclass->ref_count--;
   di->tclass = tclass;
   if (di->tclass)
      di->tclass->ref_count++;
}

void
DialogItemSetPadding(DItem * di, int left, int right, int top, int bottom)
{
   di->padding.left = left;
   di->padding.right = right;
   di->padding.top = top;
   di->padding.bottom = bottom;
}

void
DialogItemSetFill(DItem * di, char fill_h, char fill_v)
{
   di->fill_h = fill_h;
   di->fill_v = fill_v;
}

void
DialogItemSetAlign(DItem * di, int align_h, int align_v)
{
   di->align_h = align_h;
   di->align_v = align_v;
}

void
DialogItemSetRowSpan(DItem * di, int row_span)
{
   di->row_span = row_span;
}

void
DialogItemSetColSpan(DItem * di, int col_span)
{
   di->col_span = col_span;
}

void
DialogRealizeItem(Dialog * d, DItem * di)
{
   char               *def = NULL;
   int                 iw = 0, ih = 0;

   if (di->type == DITEM_BUTTON)
     {
	def = "DIALOG_WIDGET_BUTTON";
     }
   else if (di->type == DITEM_CHECKBUTTON)
     {
	def = "DIALOG_WIDGET_CHECK_BUTTON";
     }
   else if (di->type == DITEM_TEXT)
     {
	def = "DIALOG_WIDGET_TEXT";
     }
   else if (di->type == DITEM_SEPARATOR)
     {
	def = "DIALOG_WIDGET_SEPARATOR";
     }
   else if (di->type == DITEM_TABLE)
     {
	def = "DIALOG_WIDGET_TABLE";
     }
   else if (di->type == DITEM_RADIOBUTTON)
     {
	def = "DIALOG_WIDGET_RADIO_BUTTON";
     }
   else if (di->type == DITEM_AREA)
     {
	def = "DIALOG_WIDGET_AREA";
     }
   else
     {
	def = "DIALOG_WIDGET_BUTTON";
     }

   if (def)
     {
	if (!di->tclass)
	   di->tclass = FindItem(def, 0, LIST_FINDBY_NAME, LIST_TYPE_TCLASS);
	if (!di->iclass)
	   di->iclass = FindItem(def, 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
     }
   if (!di->tclass)
     {
	DialogRealizeTClassDefault();
	di->tclass = d_tc_default;
     }
   if (di->tclass)
      di->tclass->ref_count++;

   if (!di->iclass)
     {
	DialogRealizeIClassDefault();
	di->iclass = d_ic_default;
     }
   if (di->iclass)
      di->iclass->ref_count++;

   if (di->type == DITEM_TABLE)
     {
	int                 i;

	for (i = 0; i < di->item.table.num_items; i++)
	   DialogRealizeItem(d, di->item.table.items[i]);
     }
   switch (di->type)
     {
     case DITEM_SLIDER:
	if (di->item.slider.numeric)
	  {
	     di->win = ECreateWindow(d->win, -20, -20, 2, 2, 0);
	     EMapWindow(disp, di->win);
	     XSelectInput(disp, di->win,
			  EnterWindowMask | LeaveWindowMask | ButtonPressMask
			  | ButtonReleaseMask);
	  }
	di->item.slider.base_win = ECreateWindow(d->win, -20, -20, 2, 2, 0);
	EMapWindow(disp, di->item.slider.base_win);
	di->item.slider.knob_win = ECreateWindow(d->win, -20, -20, 2, 2, 0);
	EMapWindow(disp, di->item.slider.knob_win);
	XSelectInput(disp, di->item.slider.base_win,
		     EnterWindowMask | LeaveWindowMask | ButtonPressMask |
		     ButtonReleaseMask);
	XSelectInput(disp, di->item.slider.knob_win,
		     ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
	if (!di->item.slider.ic_base)
	  {
	     if (di->item.slider.horizontal)
	       {
		  di->item.slider.ic_base =
		     FindItem("DIALOG_WIDGET_SLIDER_BASE_HORIZONTAL", 0,
			      LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
	       }
	     else
	       {
		  di->item.slider.ic_base =
		     FindItem("DIALOG_WIDGET_SLIDER_BASE_VERTICAL", 0,
			      LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
	       }
	  }
	if (!di->item.slider.ic_base)
	  {
	     DialogRealizeIClassDefault();
	     di->item.slider.ic_base = d_ic_default;
	  }
	if (di->item.slider.ic_base->norm.normal->im_file)
	  {
	     ImlibImage         *im;

	     im = ELoadImage(di->item.slider.ic_base->norm.normal->im_file);
	     if (im)
	       {
		  di->item.slider.base_orig_w = im->rgb_width;
		  di->item.slider.base_orig_h = im->rgb_height;
		  Imlib_destroy_image(id, im);
	       }
	  }
	if (di->item.slider.ic_base)
	   di->item.slider.ic_base->ref_count++;

	if (!di->item.slider.ic_knob)
	  {
	     if (di->item.slider.horizontal)
	       {
		  di->item.slider.ic_knob =
		     FindItem("DIALOG_WIDGET_SLIDER_KNOB_HORIZONTAL", 0,
			      LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
	       }
	     else
	       {
		  di->item.slider.ic_knob =
		     FindItem("DIALOG_WIDGET_SLIDER_KNOB_VERTICAL", 0,
			      LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
	       }
	  }
	if (!di->item.slider.ic_knob)
	  {
	     DialogRealizeIClassDefault();
	     di->item.slider.ic_knob = d_ic_default;
	  }
	if (di->item.slider.ic_knob)
	   di->item.slider.ic_knob->ref_count++;

	if (di->item.slider.ic_knob->norm.normal->im_file)
	  {
	     ImlibImage         *im;

	     im = ELoadImage(di->item.slider.ic_knob->norm.normal->im_file);
	     if (im)
	       {
		  di->item.slider.knob_orig_w = im->rgb_width;
		  di->item.slider.knob_orig_h = im->rgb_height;
		  Imlib_destroy_image(id, im);
	       }
	  }
	if (!di->item.slider.ic_border)
	  {
	     if (di->item.slider.horizontal)
	       {
		  di->item.slider.ic_border =
		     FindItem("DIALOG_WIDGET_SLIDER_BORDER_HORIZONTAL", 0,
			      LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
	       }
	     else
	       {
		  di->item.slider.ic_border =
		     FindItem("DIALOG_WIDGET_SLIDER_BORDER_VERTICAL", 0,
			      LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
	       }
	  }
	if (di->item.slider.ic_border)
	  {
	     if (di->item.slider.ic_border->norm.normal->im_file)
	       {
		  ImlibImage         *im;

		  im = ELoadImage(di->item.slider.ic_border->norm.
				  normal->im_file);
		  if (im)
		    {
		       di->item.slider.border_orig_w = im->rgb_width;
		       di->item.slider.border_orig_h = im->rgb_height;
		       Imlib_destroy_image(id, im);
		       di->item.slider.border_win =
			  ECreateWindow(d->win, -20, -20, 2, 2, 0);
		       EMapWindow(disp, di->item.slider.border_win);
		    }
	       }
	     di->item.slider.ic_border->ref_count++;
	  }
	if (di->item.slider.horizontal)
	  {
	     iw = di->item.slider.min_length +
		di->item.slider.ic_base->padding.left +
		di->item.slider.ic_base->padding.right;
	     ih = di->item.slider.base_orig_h;
	  }
	else
	  {
	     iw = di->item.slider.base_orig_w;
	     ih = di->item.slider.min_length +
		di->item.slider.ic_base->padding.top +
		di->item.slider.ic_base->padding.bottom;
	  }
	di->w = iw;
	di->h = ih;
	break;
     case DITEM_BUTTON:
	TextSize(di->tclass, 0, 0, STATE_NORMAL, di->item.button.text, &iw,
		 &ih, 17);
	iw += di->iclass->padding.left + di->iclass->padding.right;
	ih += di->iclass->padding.top + di->iclass->padding.bottom;
	di->win = ECreateWindow(d->win, -20, -20, 2, 2, 0);
	EMapWindow(disp, di->win);
	XSelectInput(disp, di->win,
		     EnterWindowMask | LeaveWindowMask | ButtonPressMask |
		     ButtonReleaseMask | ExposureMask);
	di->w = iw;
	di->h = ih;
	break;
     case DITEM_AREA:
	iw = di->item.area.w;
	ih = di->item.area.h;
	iw += di->iclass->padding.left + di->iclass->padding.right;
	ih += di->iclass->padding.top + di->iclass->padding.bottom;
	di->win = ECreateWindow(d->win, -20, -20, 2, 2, 0);
	EMapWindow(disp, di->win);
	XSelectInput(disp, di->win, ExposureMask);
	di->item.area.area_win = ECreateWindow(di->win, -20, -20, 2, 2, 0);
	EMapWindow(disp, di->item.area.area_win);
	XSelectInput(disp, di->item.area.area_win,
		     EnterWindowMask | LeaveWindowMask | ButtonPressMask |
		     ButtonReleaseMask | ExposureMask | PointerMotionMask);
	di->w = iw;
	di->h = ih;
	break;
     case DITEM_CHECKBUTTON:
	if (di->iclass->norm.normal->im_file)
	  {
	     ImlibImage         *im;

	     im = ELoadImage(di->iclass->norm.normal->im_file);
	     if (im)
	       {
		  di->item.check_button.check_orig_w = im->rgb_width;
		  di->item.check_button.check_orig_h = im->rgb_height;
		  Imlib_destroy_image(id, im);
	       }
	  }
	TextSize(di->tclass, 0, 0, STATE_NORMAL, di->item.check_button.text,
		 &iw, &ih, 17);
	if (ih < di->item.check_button.check_orig_h)
	   ih = di->item.check_button.check_orig_h;
	iw += di->item.check_button.check_orig_w + di->iclass->padding.left;
	di->item.check_button.check_win =
	   ECreateWindow(d->win, -20, -20, 2, 2, 0);
	di->win = ECreateEventWindow(d->win, -20, -20, 2, 2);
	EMapWindow(disp, di->item.check_button.check_win);
	EMapWindow(disp, di->win);
	XSelectInput(disp, di->win,
		     EnterWindowMask | LeaveWindowMask | ButtonPressMask |
		     ButtonReleaseMask | ExposureMask);
	di->w = iw;
	di->h = ih;
	break;
     case DITEM_TEXT:
	TextSize(di->tclass, 0, 0, STATE_NORMAL, di->item.text.text, &iw, &ih,
		 17);
	di->w = iw;
	di->h = ih;
	break;
     case DITEM_IMAGE:
	{
	   ImlibImage         *im;

	   im = ELoadImage(di->item.image.image);
	   if (im)
	     {
		iw = im->rgb_width;
		ih = im->rgb_height;
		di->win = ECreateWindow(d->win, 0, 0, iw, ih, 0);
		EMapWindow(disp, di->win);
		Imlib_apply_image(id, im, di->win);
		Imlib_destroy_image(id, im);
	     }
	}
	di->w = iw;
	di->h = ih;
	break;
     case DITEM_SEPARATOR:
	iw = di->iclass->padding.left + di->iclass->padding.right;
	ih = di->iclass->padding.top + di->iclass->padding.bottom;
	di->win = ECreateWindow(d->win, -20, -20, 2, 2, 0);
	EMapWindow(disp, di->win);
	di->w = iw;
	di->h = ih;
	break;
     case DITEM_RADIOBUTTON:
	if (di->iclass->norm.normal->im_file)
	  {
	     ImlibImage         *im;

	     im = ELoadImage(di->iclass->norm.normal->im_file);
	     if (im)
	       {
		  di->item.radio_button.radio_orig_w = im->rgb_width;
		  di->item.radio_button.radio_orig_h = im->rgb_height;
		  Imlib_destroy_image(id, im);
	       }
	  }
	TextSize(di->tclass, 0, 0, STATE_NORMAL, di->item.radio_button.text,
		 &iw, &ih, 17);
	if (ih < di->item.radio_button.radio_orig_h)
	   ih = di->item.radio_button.radio_orig_h;
	iw += di->item.radio_button.radio_orig_w + di->iclass->padding.left;
	di->item.radio_button.radio_win =
	   ECreateWindow(d->win, -20, -20, 2, 2, 0);
	di->win = ECreateEventWindow(d->win, -20, -20, 2, 2);
	EMapWindow(disp, di->item.radio_button.radio_win);
	EMapWindow(disp, di->win);
	XSelectInput(disp, di->win,
		     EnterWindowMask | LeaveWindowMask | ButtonPressMask |
		     ButtonReleaseMask | ExposureMask);
	di->w = iw;
	di->h = ih;
	break;
     case DITEM_TABLE:
	{
	   int                 cols, rows;

	   cols = di->item.table.num_columns;
	   rows = 1;
	   if ((cols > 0) && (rows > 0))
	     {
		int                 i, *col_size, *row_size = NULL, r = 0, c =
		   0, x = 0, y = 0;

		col_size = Emalloc(sizeof(int) * cols);
		row_size = Erealloc(row_size, sizeof(int));

		row_size[0] = 0;
		for (i = 0; i < cols; i++)
		   col_size[i] = 0;
		for (i = 0; i < di->item.table.num_items; i++)
		  {
		     DItem              *dii;
		     int                 w, h, j, csum = 0;

		     dii = di->item.table.items[i];
		     w = dii->w + (dii->padding.left + dii->padding.right);
		     h = dii->h + (dii->padding.top + dii->padding.bottom);
		     for (j = 0; j < dii->col_span; j++)
			csum += col_size[c + j];
		     if (w > csum)
			col_size[c + di->col_span - 1] += (w - csum);
		     if (h > row_size[r])
			row_size[r] = h;
		     c += dii->col_span;
		     if (c >= cols)
		       {
			  c = 0;
			  r++;
			  rows++;
			  row_size = Erealloc(row_size, sizeof(int) * rows);

			  row_size[rows - 1] = 0;
		       }
		  }
		if (di->item.table.homogenous_h)
		  {
		     int                 max = 0;

		     for (i = 0; i < cols; i++)
		       {
			  if (col_size[i] > max)
			     max = col_size[i];
		       }
		     for (i = 0; i < cols; i++)
			col_size[i] = max;
		  }
		if (di->item.table.homogenous_v)
		  {
		     int                 max = 0;

		     for (i = 0; i < rows; i++)
		       {
			  if (row_size[i] > max)
			     max = row_size[i];
		       }
		     for (i = 0; i < rows; i++)
			row_size[i] = max;
		  }
		iw = 0;
		ih = 0;
		for (i = 0; i < cols; i++)
		   iw += col_size[i];
		for (i = 0; i < rows; i++)
		   ih += row_size[i];
		di->w = iw;
		di->h = ih;
		r = 0;
		c = 0;
		for (i = 0; i < di->item.table.num_items; i++)
		  {
		     DItem              *dii;
		     int                 j, sw = 0, sh = 0;

		     dii = di->item.table.items[i];

		     for (j = 0; j < dii->col_span; j++)
			sw += col_size[c + j];
		     for (j = 0; j < dii->row_span; j++)
			sh += row_size[r + j];
		     if (dii->fill_h)
			dii->w = sw - (dii->padding.left + dii->padding.right);
		     if (dii->fill_v)
			dii->h = sh - (dii->padding.top + dii->padding.bottom);
		     if (dii->type == DITEM_TABLE)
		       {
			  int                 dx, dy, newx, newy;

			  newx =
			     di->x + x + d->iclass->padding.left +
			     dii->padding.left +
			     (((sw
				- (dii->padding.left + dii->padding.right) -
				dii->w) * dii->align_h) >> 10);
			  newy =
			     di->y + y + d->iclass->padding.top +
			     dii->padding.top +
			     (((sh
				- (dii->padding.top + dii->padding.bottom) -
				dii->h) * dii->align_v) >> 10);
			  dx = newx - dii->x - d->iclass->padding.left;
			  dy = newy - dii->y - d->iclass->padding.top;
			  MoveTableBy(d, dii, dx, dy);
		       }
		     else
		       {
			  dii->x =
			     di->x + x + d->iclass->padding.left +
			     dii->padding.left +
			     (((sw
				- (dii->padding.left + dii->padding.right) -
				dii->w) * dii->align_h) >> 10);
			  dii->y =
			     di->y + y + d->iclass->padding.top +
			     dii->padding.top +
			     (((sh
				- (dii->padding.top + dii->padding.bottom) -
				dii->h) * dii->align_v) >> 10);
			  if (dii->win)
			     EMoveResizeWindow(disp, dii->win, dii->x, dii->y,
					       dii->w, dii->h);
			  if (dii->type == DITEM_CHECKBUTTON)
			     EMoveResizeWindow(disp,
					       dii->item.check_button.
					       check_win, dii->x,
					       dii->y +
					       ((dii->h -
						 dii->item.check_button.
						 check_orig_h) / 2),
					       dii->item.check_button.
					       check_orig_w,
					       dii->item.check_button.
					       check_orig_h);
			  if (dii->type == DITEM_RADIOBUTTON)
			     EMoveResizeWindow(disp,
					       dii->item.radio_button.
					       radio_win, dii->x,
					       dii->y +
					       ((dii->h -
						 dii->item.radio_button.
						 radio_orig_h) / 2),
					       dii->item.radio_button.
					       radio_orig_w,
					       dii->item.radio_button.
					       radio_orig_h);
			  if (dii->type == DITEM_AREA)
			    {
			       dii->item.area.w =
				  dii->w - (dii->iclass->padding.left +
					    dii->iclass->padding.right);
			       dii->item.area.h =
				  dii->h - (dii->iclass->padding.top +
					    dii->iclass->padding.bottom);
			       EMoveResizeWindow(disp,
						 dii->item.area.area_win,
						 dii->iclass->padding.left,
						 dii->iclass->padding.top,
						 dii->item.area.w,
						 dii->item.area.h);
			    }
			  if (dii->type == DITEM_SLIDER)
			    {
			       dii->item.slider.base_x = 0;
			       dii->item.slider.base_y = 0;
			       dii->item.slider.base_w = dii->w;
			       dii->item.slider.base_h = dii->h;
			       dii->item.slider.border_x = 0;
			       dii->item.slider.border_y = 0;
			       dii->item.slider.border_w = dii->w;
			       dii->item.slider.border_h = dii->h;
			       dii->item.slider.knob_w =
				  dii->item.slider.knob_orig_w;
			       dii->item.slider.knob_h =
				  dii->item.slider.knob_orig_h;
			       if (dii->item.slider.base_win)
				  EMoveResizeWindow(disp,
						    dii->item.slider.base_win,
						    dii->x +
						    dii->item.slider.base_x,
						    dii->y +
						    dii->item.slider.base_y,
						    dii->item.slider.base_w,
						    dii->item.slider.base_h);
			       if (dii->item.slider.border_win)
				  EMoveResizeWindow(disp,
						    dii->item.slider.
						    border_win,
						    dii->x +
						    dii->item.slider.border_x,
						    dii->y +
						    dii->item.slider.border_y,
						    dii->item.slider.border_w,
						    dii->item.slider.border_h);
			       if (dii->win)
				  EMoveResizeWindow(disp, dii->win,
						    dii->x +
						    dii->item.slider.
						    numeric_x,
						    dii->y +
						    dii->item.slider.
						    numeric_y,
						    dii->item.slider.
						    numeric_w,
						    dii->item.slider.numeric_h);
			    }
		       }
		     x += sw;
		     c += dii->col_span;
		     if (c >= cols)
		       {
			  x = 0;
			  y += row_size[r];
			  c = 0;
			  r++;
		       }
		  }
		if (col_size)
		   Efree(col_size);
		if (row_size)
		   Efree(row_size);
	     }
	}
	break;
     case DITEM_NONE:
     default:
	di->w = 0;
	di->h = 0;
	break;
     }
}

void
MoveTableBy(Dialog * d, DItem * di, int dx, int dy)
{
   int                 i;

   di->x += dx;
   di->y += dy;
   for (i = 0; i < di->item.table.num_items; i++)
     {
	DItem              *dii;

	dii = di->item.table.items[i];
	dii->x += dx;
	dii->y += dy;
	if (dii->type == DITEM_TABLE)
	   MoveTableBy(d, dii, dx, dy);
	if (dii->win)
	   EMoveWindow(disp, dii->win, dii->x, dii->y);
	if (dii->type == DITEM_CHECKBUTTON)
	   EMoveWindow(disp, dii->item.check_button.check_win, dii->x,
		       dii->y +
		       ((dii->h - dii->item.check_button.check_orig_h) / 2));
	if (dii->type == DITEM_RADIOBUTTON)
	   EMoveWindow(disp, dii->item.radio_button.radio_win, dii->x,
		       dii->y +
		       ((dii->h - dii->item.radio_button.radio_orig_h) / 2));
	if (dii->type == DITEM_SLIDER)
	  {
	     if (dii->item.slider.base_win)
		EMoveResizeWindow(disp, dii->item.slider.base_win,
				  dii->x + dii->item.slider.base_x,
				  dii->y + dii->item.slider.base_y,
				  dii->item.slider.base_w,
				  dii->item.slider.base_h);
	     if (dii->item.slider.knob_win)
		EMoveResizeWindow(disp, dii->item.slider.knob_win,
				  dii->x + dii->item.slider.knob_x,
				  dii->y + dii->item.slider.knob_y,
				  dii->item.slider.knob_w,
				  dii->item.slider.knob_h);
	     if (dii->item.slider.border_win)
		EMoveResizeWindow(disp, dii->item.slider.border_win,
				  dii->x + dii->item.slider.border_x,
				  dii->y + dii->item.slider.border_y,
				  dii->item.slider.border_w,
				  dii->item.slider.border_h);
	     if (dii->win)
		EMoveResizeWindow(disp, dii->win,
				  dii->x + dii->item.slider.numeric_x,
				  dii->y + dii->item.slider.numeric_y,
				  dii->item.slider.numeric_w,
				  dii->item.slider.numeric_h);
	  }
     }
}

#define INTERSECTS(x, y, w, h, xx, yy, ww, hh) \
((x < (xx + ww)) && \
 (y < (yy + hh)) && \
 ((x + w) > xx) && \
 ((y + h) > yy))

void
DialogDrawItems(Dialog * d, DItem * di, int x, int y, int w, int h)
{
   int                 state;

   if (queue_up)
     {
	DrawQueue          *dq;

	dq = Emalloc(sizeof(DrawQueue));
	dq->win = 0;
	dq->iclass = NULL;
	dq->w = w;
	dq->h = h;
	dq->active = 0;
	dq->sticky = 0;
	dq->state = 0;
	dq->expose = 0;
	dq->tclass = NULL;
	dq->text = NULL;
	dq->shape_propagate = 0;
	dq->pager = NULL;
	dq->redraw_pager = NULL;
	dq->d = d;
	dq->di = di;
	dq->x = x;
	dq->y = y;
	AddItem(dq, "DRAW", 0, LIST_TYPE_DRAW);
	EDBUG_RETURN_;
     }
   if (di->type == DITEM_TABLE)
     {
	int                 i;

	if (INTERSECTS(x, y, w, h, di->x, di->y, di->w, di->h))
	  {
	     for (i = 0; i < di->item.table.num_items; i++)
		DialogDrawItems(d, di->item.table.items[i], x, y, w, h);
	  }
	return;
     }
   if (INTERSECTS(x, y, w, h, di->x, di->y, di->w, di->h))
     {
	switch (di->type)
	  {
	  case DITEM_SLIDER:
	     if (di->item.slider.horizontal)
	       {
		  di->item.slider.knob_x =
		     di->item.slider.base_x +
		     (((di->item.slider.base_w -
			di->item.slider.knob_w) * (di->item.slider.val -
						   di->item.slider.lower)) /
		      (di->item.slider.upper - di->item.slider.lower));
		  di->item.slider.knob_y =
		     di->item.slider.base_y +
		     ((di->item.slider.base_h - di->item.slider.knob_h) / 2);
	       }
	     else
	       {
		  di->item.slider.knob_y =
		     (di->item.slider.base_y + di->item.slider.base_h -
		      di->item.slider.knob_h) -
		     (((di->item.slider.base_h -
			di->item.slider.knob_h) * (di->item.slider.val -
						   di->item.slider.lower)) /
		      (di->item.slider.upper - di->item.slider.lower));
		  di->item.slider.knob_x =
		     di->item.slider.base_x +
		     ((di->item.slider.base_w - di->item.slider.knob_w) / 2);
	       }
	     if (di->item.slider.knob_win)
		EMoveResizeWindow(disp, di->item.slider.knob_win,
				  di->x + di->item.slider.knob_x,
				  di->y + di->item.slider.knob_y,
				  di->item.slider.knob_w,
				  di->item.slider.knob_h);
	     if (di->item.slider.base_win)
		IclassApply(di->item.slider.ic_base, di->item.slider.base_win,
			    di->item.slider.base_w, di->item.slider.base_h, 0,
			    0, STATE_NORMAL, 0);
	     if (di->item.slider.border_win)
		IclassApply(di->item.slider.ic_border,
			    di->item.slider.border_win,
			    di->item.slider.border_w,
			    di->item.slider.border_h, 0, 0, STATE_NORMAL, 0);
	     state = STATE_NORMAL;
	     if ((di->hilited) && (di->clicked))
		state = STATE_CLICKED;
	     else if ((di->hilited) && (!di->clicked))
		state = STATE_HILITED;
	     else if (!(di->hilited) && (di->clicked))
		state = STATE_CLICKED;
	     if (di->item.slider.knob_win)
		IclassApply(di->item.slider.ic_knob, di->item.slider.knob_win,
			    di->item.slider.knob_w, di->item.slider.knob_h, 0,
			    0, state, 0);
	     break;
	  case DITEM_BUTTON:
	     state = STATE_NORMAL;
	     if ((di->hilited) && (di->clicked))
		state = STATE_CLICKED;
	     else if ((di->hilited) && (!di->clicked))
		state = STATE_HILITED;
	     else if (!(di->hilited) && (di->clicked))
		state = STATE_CLICKED;
	     IclassApply(di->iclass, di->win, di->w, di->h, 0, 0, state, 0);
	     TclassApply(di->iclass, di->win, di->w, di->h, 0, 0, state, 1,
			 di->tclass, di->item.button.text);
	     break;
	  case DITEM_AREA:
	     IclassApply(di->iclass, di->win, di->w, di->h, 0, 0,
			 STATE_NORMAL, 0);
	     break;
	  case DITEM_CHECKBUTTON:
	     state = STATE_NORMAL;
	     if ((di->hilited) && (di->clicked))
		state = STATE_CLICKED;
	     else if ((di->hilited) && (!di->clicked))
		state = STATE_HILITED;
	     else if (!(di->hilited) && (di->clicked))
		state = STATE_CLICKED;
	     if (di->item.check_button.onoff)
		IclassApply(di->iclass, di->item.check_button.check_win,
			    di->item.check_button.check_orig_w,
			    di->item.check_button.check_orig_h, 1, 0, state, 0);
	     else
		IclassApply(di->iclass, di->item.check_button.check_win,
			    di->item.check_button.check_orig_w,
			    di->item.check_button.check_orig_h, 0, 0, state, 0);
	     XClearArea(disp, d->win, di->x, di->y, di->w, di->h, False);
	     TextDraw(di->tclass, d->win, 0, 0, STATE_NORMAL,
		      di->item.check_button.text,
		      di->x + di->item.check_button.check_orig_w +
		      di->iclass->padding.left, di->y,
		      di->w - di->item.check_button.check_orig_w -
		      di->iclass->padding.left, 99999, 17,
		      di->tclass->justification);
	     break;
	  case DITEM_TEXT:
	     XClearArea(disp, d->win, di->x, di->y, di->w, di->h, False);
	     TextDraw(di->tclass, d->win, 0, 0, STATE_NORMAL,
		      di->item.text.text, di->x, di->y, di->w, 99999, 17,
		      di->tclass->justification);
	     break;
	  case DITEM_IMAGE:
	     break;
	  case DITEM_SEPARATOR:
	     if (di->item.separator.horizontal)
		IclassApply(di->iclass, di->win, di->w, di->h, 0, 0,
			    STATE_NORMAL, 0);
	     else
		IclassApply(di->iclass, di->win, di->w, di->h, 0, 0,
			    STATE_CLICKED, 0);
	     break;
	  case DITEM_RADIOBUTTON:
	     state = STATE_NORMAL;
	     if ((di->hilited) && (di->clicked))
		state = STATE_CLICKED;
	     else if ((di->hilited) && (!di->clicked))
		state = STATE_HILITED;
	     else if (!(di->hilited) && (di->clicked))
		state = STATE_CLICKED;
	     if (di->item.radio_button.onoff)
		IclassApply(di->iclass, di->item.radio_button.radio_win,
			    di->item.radio_button.radio_orig_w,
			    di->item.radio_button.radio_orig_h, 1, 0, state, 0);
	     else
		IclassApply(di->iclass, di->item.radio_button.radio_win,
			    di->item.radio_button.radio_orig_w,
			    di->item.radio_button.radio_orig_w, 0, 0, state, 0);
	     XClearArea(disp, d->win, di->x, di->y, di->w, di->h, False);
	     TextDraw(di->tclass, d->win, 0, 0, STATE_NORMAL,
		      di->item.radio_button.text,
		      di->x + di->item.radio_button.radio_orig_w +
		      di->iclass->padding.left, di->y,
		      di->w - di->item.radio_button.radio_orig_w -
		      di->iclass->padding.left, 99999, 17,
		      di->tclass->justification);
	     break;
	  default:
	     break;
	  }
     }
}

void
DialogItemsRealize(Dialog * d)
{
   char                pq;

   if (!d->item)
      return;
   DialogRealizeItem(d, d->item);
   pq = queue_up;
   queue_up = 0;
   DialogDrawItems(d, d->item, 0, 0, 99999, 99999);
   queue_up = pq;
   d->w = d->item->w + d->iclass->padding.left + d->iclass->padding.right;
   d->h = d->item->h + d->iclass->padding.top + d->iclass->padding.bottom;
}

void
DialogItemButtonSetText(DItem * di, char *text)
{
   if (di->item.button.text)
      Efree(di->item.button.text);
   di->item.button.text = duplicate(text);
}

void
DialogItemCheckButtonSetText(DItem * di, char *text)
{
   if (di->item.check_button.text)
      Efree(di->item.check_button.text);
   di->item.check_button.text = duplicate(text);
}

void
DialogItemTextSetText(DItem * di, char *text)
{
   if (di->item.text.text)
      Efree(di->item.text.text);
   di->item.text.text = duplicate(text);
}

void
DialogItemRadioButtonSetEventFunc(DItem * di,
				  void (*func) (int val, void *data))
{
   di->item.radio_button.event_func = func;
}

void
DialogItemRadioButtonSetText(DItem * di, char *text)
{
   if (di->item.radio_button.text)
      Efree(di->item.radio_button.text);
   di->item.radio_button.text = duplicate(text);
}

void
DialogItemRadioButtonSetFirst(DItem * di, DItem * first)
{
   di->item.radio_button.first = first;
   if (di == first)
      return;
   while (first->item.radio_button.next)
      first = first->item.radio_button.next;
   first->item.radio_button.next = di;
}

void
DialogItemRadioButtonGroupSetValPtr(DItem * di, int *val_ptr)
{
   while (di)
     {
	di->item.radio_button.val_ptr = val_ptr;
	if (*val_ptr == di->item.radio_button.val)
	   di->item.check_button.onoff = 1;
	di = di->item.radio_button.next;
     }
}

void
DialogItemRadioButtonGroupSetVal(DItem * di, int val)
{
   di->item.radio_button.val = val;
}

void
DialogItemCheckButtonSetState(DItem * di, char onoff)
{
   di->item.check_button.onoff = onoff;
}

void
DialogItemCheckButtonSetPtr(DItem * di, char *onoff_ptr)
{
   di->item.check_button.onoff_ptr = onoff_ptr;
}

void
DialogItemTableSetOptions(DItem * di, int num_columns, char border,
			  char homogenous_h, char homogenous_v)
{
   di->item.table.num_columns = num_columns;
   di->item.table.border = border;
   di->item.table.homogenous_h = homogenous_h;
   di->item.table.homogenous_v = homogenous_v;
}

void
DialogItemSeparatorSetOrientation(DItem * di, char horizontal)
{
   di->item.separator.horizontal = horizontal;
}

void
DialogItemImageSetFile(DItem * di, char *image)
{
   if (di->item.image.image)
      Efree(di->item.image.image);
   di->item.image.image = duplicate(image);
   di->fill_h = 0;
   di->fill_v = 0;
}

void
DialogItemSliderSetVal(DItem * di, int val)
{
   di->item.slider.val = val;
   if (val < di->item.slider.lower)
     {
	di->item.slider.val = di->item.slider.lower;
     }
   else if (val > di->item.slider.upper)
     {
	di->item.slider.val = di->item.slider.upper;
     }
}

void
DialogItemSliderSetBounds(DItem * di, int lower, int upper)
{
   if (lower < upper)
     {
	di->item.slider.lower = lower;
	di->item.slider.upper = upper;
     }
   else
     {
	di->item.slider.lower = upper;
	di->item.slider.upper = lower;
     }
   if (di->item.slider.upper <= di->item.slider.lower)
      di->item.slider.upper = di->item.slider.lower + 1;
}

void
DialogItemSliderSetUnits(DItem * di, int units)
{
   di->item.slider.unit = units;
}

void
DialogItemSliderSetJump(DItem * di, int jump)
{
   di->item.slider.jump = jump;
}

void
DialogItemSliderSetMinLength(DItem * di, int min)
{
   di->item.slider.min_length = min;
}

void
DialogItemSliderSetValPtr(DItem * di, int *val_ptr)
{
   di->item.slider.val_ptr = val_ptr;
}

void
DialogItemSliderSetOrientation(DItem * di, char horizontal)
{
   di->item.slider.horizontal = horizontal;
}

void
DialogItemAreaSetSize(DItem * di, int w, int h)
{
   di->item.area.w = w;
   di->item.area.h = h;
}

Window
DialogItemAreaGetWindow(DItem * di)
{
   return di->item.area.area_win;
}

void
DialogItemAreaGetSize(DItem * di, int *w, int *h)
{
   *w = di->item.area.w;
   *h = di->item.area.h;
}

void
DialogItemAreaSetEventFunc(DItem * di, void (*func) (int val, void *data))
{
   di->item.area.event_func = func;
}

void
DialogFreeItem(DItem * di)
{
   if (di->type == DITEM_TABLE)
     {
	int                 i;

	for (i = 0; i < di->item.table.num_items; i++)
	   DialogFreeItem(di->item.table.items[i]);
     }
   switch (di->type)
     {
     case DITEM_BUTTON:
	if (di->item.button.text)
	   Efree(di->item.button.text);
	break;
     case DITEM_CHECKBUTTON:
	if (di->item.check_button.text)
	   Efree(di->item.check_button.text);
	break;
     case DITEM_TEXT:
	if (di->item.text.text)
	   Efree(di->item.text.text);
	break;
     case DITEM_IMAGE:
	if (di->item.image.image)
	   Efree(di->item.image.image);
	break;
     case DITEM_RADIOBUTTON:
	if (di->item.radio_button.text)
	   Efree(di->item.radio_button.text);
	break;
     case DITEM_SLIDER:
	if (di->item.slider.ic_base)
	   di->item.slider.ic_base->ref_count--;
	if (di->item.slider.ic_knob)
	   di->item.slider.ic_knob->ref_count--;
	if (di->item.slider.ic_border)
	   di->item.slider.ic_border->ref_count--;
	break;
     case DITEM_SEPARATOR:
	break;
     case DITEM_TABLE:
	if (di->item.table.items)
	   Efree(di->item.table.items);
	break;
     default:
	break;
     }

   if (di->iclass)
      di->iclass->ref_count--;
   if (di->tclass)
      di->tclass->ref_count--;

   Efree(di);
}

DItem              *
DialogItemFindWindow(DItem * di, Window win)
{
   DItem              *dii = NULL;

   if (di->type == DITEM_TABLE)
     {
	int                 i;

	for (i = 0; i < di->item.table.num_items; i++)
	  {
	     if ((dii = DialogItemFindWindow(di->item.table.items[i], win)))
		return dii;
	  }
     }
   else if ((di->win == win)
	    || ((di->type == DITEM_SLIDER)
		&& ((di->item.slider.base_win == win)
		    || (di->item.slider.knob_win == win)))
	    || ((di->type == DITEM_AREA) && (di->item.area.area_win == win)))
     {
	return di;
     }
   return NULL;
}
