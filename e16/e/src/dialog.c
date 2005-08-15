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
#include "ewins.h"
#include "xwin.h"

#define DEBUG_DIALOGS 0

typedef struct
{
   char                horizontal;

   char                numeric;
   char                numeric_side;

   int                 upper;
   int                 lower;
   int                 unit;
   int                 jump;
   int                 val;
   int                *val_ptr;

   int                 min_length;

   int                 base_orig_w, base_orig_h;
   int                 knob_orig_w, knob_orig_h;
   int                 border_orig_w, border_orig_h;

   int                 base_x, base_y, base_w, base_h;
   int                 knob_x, knob_y, knob_w, knob_h;
   int                 border_x, border_y, border_w, border_h;
   int                 numeric_x, numeric_y, numeric_w, numeric_h;

   ImageClass         *ic_base;
   ImageClass         *ic_knob;
   ImageClass         *ic_border;

   char                in_drag;
   int                 wanted_val;

   Window              base_win;
   Window              knob_win;
   Window              border_win;
} DItemSlider;

typedef struct
{
   Window              area_win;
   int                 w, h;
   DialogItemCallbackFunc *event_func;
} DItemArea;

typedef struct
{
   Window              check_win;
   int                 check_orig_w, check_orig_h;
   char                onoff;
   char               *onoff_ptr;
} DItemCheckButton;

typedef struct
{
   char               *image;
} DItemImage;

typedef struct
{
   char                horizontal;
} DItemSeparator;

typedef struct
{
   int                 num_columns;
   char                border;
   char                homogenous_h;
   char                homogenous_v;
   int                 num_items;
   DItem             **items;
} DItemTable;

typedef struct
{
   Window              radio_win;
   int                 radio_orig_w, radio_orig_h;
   char                onoff;
   int                 val;
   int                *val_ptr;
   DItem              *next;
   DItem              *first;
   DialogItemCallbackFunc *event_func;
} DItemRadioButton;

struct _ditem
{
   int                 type;
   DialogCallbackFunc *func;
   int                 val;
   void               *data;
   ImageClass         *iclass;
   TextClass          *tclass;
   Imlib_Border        padding;
   char                fill_h;
   char                fill_v;
   int                 align_h;
   int                 align_v;
   int                 row_span;
   int                 col_span;

   int                 x, y, w, h;
   char                hilited;
   char                clicked;
   Window              win;
   char               *text;
   union
   {
      DItemCheckButton    check_button;
      DItemTable          table;
      DItemImage          image;
      DItemSeparator      separator;
      DItemRadioButton    radio_button;
      DItemSlider         slider;
      DItemArea           area;
   }
   item;

   char                update;
};

typedef struct
{
   Dialog             *parent;
   char               *text;
   DialogCallbackFunc *func;
   Window              win;
   int                 x, y, w, h;
   char                hilited;
   char                clicked;
   char                close;
   TextClass          *tclass;
   ImageClass         *iclass;
   int                 image;
} DButton;

typedef struct
{
   KeyCode             key;
   int                 val;
   DialogCallbackFunc *func;
} DKeyBind;

struct _dialog
{
   EWin               *ewin;
   char               *name;
   char               *title;
   char               *text;
   int                 num_buttons;
   Window              win;
   DButton           **button;
   TextClass          *tclass;
   ImageClass         *iclass;
   int                 w, h;
   DItem              *item;
   DialogCallbackFunc *exit_func;
   int                 exit_val;
   int                 num_bindings;
   DKeyBind           *keybindings;
   void               *data;

   char                update;
   int                 xu1, yu1, xu2, yu2;
};

static Dialog      *FindDialog(Window win);

static void         DialogHandleEvents(XEvent * ev, void *prm);
static void         DButtonHandleEvents(XEvent * ev, void *prm);

static void         MoveTableBy(Dialog * d, DItem * di, int dx, int dy);
static void         DialogItemsRealize(Dialog * d);

static void         DialogUpdate(Dialog * d);
static char         dialog_update_pending = 0;

void
DialogBindKey(Dialog * d, const char *key, DialogCallbackFunc * func, int val)
{
   d->num_bindings++;
   if (!d->keybindings)
      d->keybindings = Emalloc(sizeof(DKeyBind) * d->num_bindings);
   else
      d->keybindings =
	 Erealloc(d->keybindings, sizeof(DKeyBind) * d->num_bindings);
   d->keybindings[d->num_bindings - 1].val = val;
   d->keybindings[d->num_bindings - 1].func = func;
   d->keybindings[d->num_bindings - 1].key =
      XKeysymToKeycode(disp, XStringToKeysym(key));
}

Dialog             *
DialogCreate(const char *name)
{
   Dialog             *d;

   d = Ecalloc(1, sizeof(Dialog));
   d->name = Estrdup(name);
   d->win = ECreateWindow(VRoot.win, -20, -20, 2, 2, 0);
   EventCallbackRegister(d->win, 0, DialogHandleEvents, d);

   d->tclass = TextclassFind("DIALOG", 1);
   if (d->tclass)
      d->tclass->ref_count++;

   d->iclass = ImageclassFind("DIALOG", 1);
   if (d->iclass)
      d->iclass->ref_count++;

   d->xu1 = d->yu1 = 99999;
   d->xu2 = d->yu2 = 0;

   return d;
}

static void
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

static void
DialogDestroy(Dialog * d)
{
   int                 i;

   RemoveItem(NULL, d->win, LIST_FINDBY_ID, LIST_TYPE_DIALOG);

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

   EDestroyWindow(d->win);

   Efree(d);
}

void
DialogSetText(Dialog * d, const char *text)
{
   int                 w, h;

   if (d->text)
      Efree(d->text);
   d->text = Estrdup(text);

   if ((!d->tclass) || (!d->iclass))
      return;

   TextSize(d->tclass, 0, 0, STATE_NORMAL, text, &w, &h, 17);
   d->w = w + d->iclass->padding.left + d->iclass->padding.right;
   d->h = h + d->iclass->padding.top + d->iclass->padding.bottom;
}

void
DialogSetTitle(Dialog * d, const char *title)
{
   if (d->title)
      Efree(d->title);
   d->title = Estrdup(title);
}

void
DialogSetExitFunction(Dialog * d, DialogCallbackFunc * func, int val)
{
   d->exit_func = func;
   d->exit_val = val;
}

void
DialogSetData(Dialog * d, void *data)
{
   d->data = data;
}

void               *
DialogGetData(Dialog * d)
{
   return d->data;
}

void
DialogAddButton(Dialog * d, const char *text, DialogCallbackFunc * func,
		char doclose, int image)
{
   DButton            *db;
   int                 w, h;

   db = Emalloc(sizeof(DButton));

   d->num_buttons++;
   d->button = Erealloc(d->button, d->num_buttons * (sizeof(DButton *)));
   d->button[d->num_buttons - 1] = db;

   db->parent = d;
   db->text = Estrdup(text);
   db->func = func;
   db->image = image;
   db->win = ECreateWindow(d->win, -20, -20, 2, 2, 0);
   EventCallbackRegister(db->win, 0, DButtonHandleEvents, db);
   EMapWindow(db->win);
   db->x = -1;
   db->y = -1;
   db->w = -1;
   db->h = -1;
   db->hilited = 0;
   db->clicked = 0;
   db->close = doclose;

   db->tclass = TextclassFind("DIALOG_BUTTON", 1);
   if (db->tclass)
      db->tclass->ref_count++;

   db->iclass = ImageclassFind("DIALOG_BUTTON", 1);
   if (db->iclass)
      db->iclass->ref_count++;

   TextSize(db->tclass, 0, 0, STATE_NORMAL, text, &w, &h, 17);
   db->h = h + db->iclass->padding.top + db->iclass->padding.bottom;
   db->w = w + db->iclass->padding.left + db->iclass->padding.right;
   if (Conf.dialogs.button_image && db->image)
      db->w += h + 2;

   ESelectInput(db->win,
		EnterWindowMask | LeaveWindowMask | ButtonPressMask |
		ButtonReleaseMask | ExposureMask);
}

static void
DialogDrawButton(Dialog * d __UNUSED__, DButton * db)
{
   int                 state;
   Imlib_Image        *im;

   state = STATE_NORMAL;
   if ((db->hilited) && (db->clicked))
     {
	state = STATE_CLICKED;
     }
   else if ((db->hilited) && (!db->clicked))
     {
	state = STATE_HILITED;
     }
   else if (!(db->hilited) && (db->clicked))
     {
	state = STATE_CLICKED;
     }

   ImageclassApply(db->iclass, db->win, db->w, db->h, 0, 0, state, 0,
		   ST_WIDGET);

   im = NULL;
   if (Conf.dialogs.button_image)
     {
	switch (db->image)
	  {
	  case DIALOG_BUTTON_OK:
	     im = ELoadImage("pix/ok.png");
	     break;
	  case DIALOG_BUTTON_CANCEL:
	     im = ELoadImage("pix/cancel.png");
	     break;
	  case DIALOG_BUTTON_APPLY:
	     im = ELoadImage("pix/apply.png");
	     break;
	  case DIALOG_BUTTON_CLOSE:
	     im = ELoadImage("pix/close.png");
	     break;
	  default:
	     break;
	  }
     }

   if (im)
     {
	ImageClass         *ic = db->iclass;
	int                 h;

	h = db->h - (ic->padding.top + ic->padding.bottom);

	TextDraw(db->tclass, db->win, 0, 0, state, db->text,
		 h + 2 + ic->padding.left, ic->padding.top,
		 db->w - (h + 2 + ic->padding.left + ic->padding.right),
		 h, h, db->tclass->justification);

	imlib_context_set_image(im);
	imlib_context_set_drawable(db->win);
	imlib_context_set_blend(1);
	imlib_render_image_on_drawable_at_size(ic->padding.left,
					       ic->padding.top, h, h);
	imlib_context_set_blend(0);
	imlib_free_image();
     }
   else
     {
	TextclassApply(db->iclass, db->win, db->w, db->h, 0, 0, state, 1,
		       db->tclass, db->text);
     }
}

static void
DialogDraw(Dialog * d)
{
   if ((!d->tclass) || (!d->iclass))
      return;

   if (d->text)
     {
	TextclassApply(d->iclass, d->win, d->w, d->h, 0, 0, STATE_NORMAL, 1,
		       d->tclass, d->text);
     }
   else if (d->item)
     {
	DialogDrawItems(d, d->item, 0, 0, 99999, 99999);
     }
}

static void
DialogDrawArea(Dialog * d, int x, int y, int w, int h)
{
   if ((!d->tclass) || (!d->iclass))
      return;

   if (d->text)
     {
	TextclassApply(d->iclass, d->win, d->w, d->h, 0, 0, STATE_NORMAL, 1,
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

   ImageclassApply(d->iclass, d->win, d->w, d->h, 0, 0, STATE_NORMAL, 0,
		   ST_DIALOG);

   for (i = 0; i < d->num_buttons; i++)
      DialogDrawButton(d, d->button[i]);

   DialogDraw(d);
}

static void
DialogEwinMoveResize(EWin * ewin, int resize __UNUSED__)
{
   Dialog             *d = ewin->data;

   if (!d || Mode.mode != MODE_NONE)
      return;

   if (TransparencyUpdateNeeded() || ImageclassIsTransparent(d->iclass))
      DialogRedraw(d);
}

static void
DialogEwinClose(EWin * ewin)
{
   DialogDestroy(ewin->data);
   ewin->data = NULL;
}

static void
DialogEwinInit(EWin * ewin, void *ptr)
{
   Dialog             *d = ptr;

   ewin->data = ptr;
   d->ewin = ewin;

   ewin->MoveResize = DialogEwinMoveResize;
   ewin->Close = DialogEwinClose;

   ewin->client.width.min = ewin->client.width.max = ewin->client.w = d->w;
   ewin->client.height.min = ewin->client.height.max = ewin->client.h = d->h;
   ewin->client.no_resize_h = ewin->client.no_resize_v = 1;

   EoSetLayer(ewin, 10);
}

void
ShowDialog(Dialog * d)
{
   int                 i, w, h, mw, mh;
   EWin               *ewin;

   if (d->title)
     {
	HintsSetWindowName(d->win, d->title);
	HintsSetWindowClass(d->win, d->name, "Enlightenment_Dialog");
     }

   ewin = FindEwinByDialog(d);
   if (ewin)
     {
	if (EoGetDesk(ewin) != DesksGetCurrent())
	   EwinMoveToDesktop(ewin, DesksGetCurrent());
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
	EMoveResizeWindow(d->button[i]->win, d->button[i]->x,
			  d->button[i]->y, d->button[i]->w, d->button[i]->h);
     }
   d->w = w;
   d->h = h;
   EResizeWindow(d->win, w, h);

   ewin = AddInternalToFamily(d->win, "DIALOG", EWIN_TYPE_DIALOG, d,
			      DialogEwinInit);
   if (!ewin)
      return;

   ewin->client.event_mask |= KeyPressMask | ExposureMask;
   ESelectInput(d->win, ewin->client.event_mask);

   EwinMoveToDesktop(ewin, EoGetDesk(ewin));

   if (ewin->state.placed)
     {
	EwinMoveResize(ewin, EoGetX(ewin), EoGetY(ewin), w, h);
     }
   else
     {
	EwinResize(ewin, w, h);
	if (FindADialog() > 1)
	   ArrangeEwin(ewin);
	else
	   ArrangeEwinCentered(ewin, 0);
     }
   if (!FindDialog(d->win))
      AddItem(d, d->name, d->win, LIST_TYPE_DIALOG);

   ShowEwin(ewin);
   DialogRedraw(d);
   DialogUpdate(d);
}

void
DialogClose(Dialog * d)
{
   if (!d)
      return;

   if (d->exit_func)
      d->exit_func(d, d->exit_val, NULL);

   HideEwin(d->ewin);
}

DItem              *
DialogInitItem(Dialog * d)
{
   DItem              *di;

   if (d->item)
      return NULL;

   di = Ecalloc(1, sizeof(DItem));
   d->item = di;

   di->type = DITEM_TABLE;
   di->align_h = 512;
   di->align_v = 512;
   di->row_span = 1;
   di->col_span = 1;
   di->item.table.num_columns = 1;
   di->item.table.border = 0;
   di->item.table.homogenous_h = 0;
   di->item.table.homogenous_v = 0;
   di->item.table.num_items = 0;
   di->item.table.items = NULL;

   return di;
}

DItem              *
DialogAddItem(DItem * dii, int type)
{
   DItem              *di;

   di = Ecalloc(1, sizeof(DItem));

   di->type = type;
   di->fill_h = 1;
   di->fill_v = 1;
   di->align_h = 512;
   di->align_v = 512;
   di->row_span = 1;
   di->col_span = 1;
   di->text = NULL;

   switch (di->type)
     {
     default:
	break;
     case DITEM_AREA:
	di->item.area.area_win = 0;
	di->item.area.w = 32;
	di->item.area.h = 32;
	di->item.area.event_func = NULL;
	break;
     case DITEM_CHECKBUTTON:
	di->item.check_button.check_win = 0;
	di->item.check_button.onoff = 0;
	di->item.check_button.onoff_ptr = NULL;
	di->item.check_button.check_orig_w = 10;
	di->item.check_button.check_orig_h = 10;
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
DialogItemSetCallback(DItem * di, DialogCallbackFunc * func, int val,
		      void *data)
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
DialogItemCallCallback(Dialog * d, DItem * di)
{
   if (di->func)
      di->func(d, di->val, di->data);
}

static void
DialogRealizeItem(Dialog * d, DItem * di)
{
   const char         *def = NULL;
   int                 iw = 0, ih = 0;
   int                 register_win_callback;

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
	   di->tclass = TextclassFind(def, 1);
	if (!di->iclass)
	   di->iclass = ImageclassFind(def, 1);
     }
   if (di->tclass)
      di->tclass->ref_count++;
   if (di->iclass)
      di->iclass->ref_count++;

   if (di->type == DITEM_TABLE)
     {
	int                 i;

	for (i = 0; i < di->item.table.num_items; i++)
	   DialogRealizeItem(d, di->item.table.items[i]);
     }

   register_win_callback = 1;
   switch (di->type)
     {
     case DITEM_SLIDER:
	if (di->item.slider.numeric)
	  {
	     di->win = ECreateWindow(d->win, -20, -20, 2, 2, 0);
	     EMapWindow(di->win);
	     ESelectInput(di->win,
			  EnterWindowMask | LeaveWindowMask | ButtonPressMask
			  | ButtonReleaseMask);
	  }
	di->item.slider.base_win = ECreateWindow(d->win, -20, -20, 2, 2, 0);
	EMapWindow(di->item.slider.base_win);
	di->item.slider.knob_win = ECreateWindow(d->win, -20, -20, 2, 2, 0);
	EMapWindow(di->item.slider.knob_win);
	ESelectInput(di->item.slider.base_win,
		     EnterWindowMask | LeaveWindowMask | ButtonPressMask |
		     ButtonReleaseMask);
	EventCallbackRegister(di->item.slider.base_win, 0, DialogHandleEvents,
			      d);
	ESelectInput(di->item.slider.knob_win,
		     ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
	EventCallbackRegister(di->item.slider.knob_win, 0, DialogHandleEvents,
			      d);
	if (!di->item.slider.ic_base)
	  {
	     if (di->item.slider.horizontal)
	       {
		  di->item.slider.ic_base =
		     ImageclassFind("DIALOG_WIDGET_SLIDER_BASE_HORIZONTAL", 1);
	       }
	     else
	       {
		  di->item.slider.ic_base =
		     ImageclassFind("DIALOG_WIDGET_SLIDER_BASE_VERTICAL", 1);
	       }
	  }

	if (di->item.slider.ic_base->norm.normal->im_file)
	  {
	     Imlib_Image        *im;

	     im = ELoadImage(di->item.slider.ic_base->norm.normal->im_file);
	     if (im)
	       {
		  imlib_context_set_image(im);
		  di->item.slider.base_orig_w = imlib_image_get_width();
		  di->item.slider.base_orig_h = imlib_image_get_height();
		  imlib_free_image();
	       }
	  }
	if (di->item.slider.ic_base)
	   di->item.slider.ic_base->ref_count++;

	if (!di->item.slider.ic_knob)
	  {
	     if (di->item.slider.horizontal)
	       {
		  di->item.slider.ic_knob =
		     ImageclassFind("DIALOG_WIDGET_SLIDER_KNOB_HORIZONTAL", 1);
	       }
	     else
	       {
		  di->item.slider.ic_knob =
		     ImageclassFind("DIALOG_WIDGET_SLIDER_KNOB_VERTICAL", 1);
	       }
	  }
	if (di->item.slider.ic_knob)
	   di->item.slider.ic_knob->ref_count++;

	if (di->item.slider.ic_knob->norm.normal->im_file)
	  {
	     Imlib_Image        *im;

	     im = ELoadImage(di->item.slider.ic_knob->norm.normal->im_file);
	     if (im)
	       {
		  imlib_context_set_image(im);
		  di->item.slider.knob_orig_w = imlib_image_get_width();
		  di->item.slider.knob_orig_h = imlib_image_get_height();
		  imlib_free_image();
	       }
	  }
	if (!di->item.slider.ic_border)
	  {
	     if (di->item.slider.horizontal)
	       {
		  di->item.slider.ic_border =
		     ImageclassFind("DIALOG_WIDGET_SLIDER_BORDER_HORIZONTAL",
				    0);
	       }
	     else
	       {
		  di->item.slider.ic_border =
		     ImageclassFind("DIALOG_WIDGET_SLIDER_BORDER_VERTICAL", 0);
	       }
	  }
	if (di->item.slider.ic_border)
	  {
	     if (di->item.slider.ic_border->norm.normal->im_file)
	       {
		  Imlib_Image        *im;

		  im = ELoadImage(di->item.slider.ic_border->norm.
				  normal->im_file);
		  if (im)
		    {
		       imlib_context_set_image(im);
		       di->item.slider.border_orig_w = imlib_image_get_width();
		       di->item.slider.border_orig_h = imlib_image_get_height();
		       imlib_free_image();
		       di->item.slider.border_win =
			  ECreateWindow(d->win, -20, -20, 2, 2, 0);
		       EMapWindow(di->item.slider.border_win);
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
	TextSize(di->tclass, 0, 0, STATE_NORMAL, di->text, &iw, &ih, 17);
	iw += di->iclass->padding.left + di->iclass->padding.right;
	ih += di->iclass->padding.top + di->iclass->padding.bottom;
	di->win = ECreateWindow(d->win, -20, -20, 2, 2, 0);
	EMapWindow(di->win);
	ESelectInput(di->win,
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
	EMapWindow(di->win);
	ESelectInput(di->win, ExposureMask);
	di->item.area.area_win = ECreateWindow(di->win, -20, -20, 2, 2, 0);
	EMapWindow(di->item.area.area_win);
	ESelectInput(di->item.area.area_win,
		     EnterWindowMask | LeaveWindowMask | ButtonPressMask |
		     ButtonReleaseMask | ExposureMask | PointerMotionMask);
	EventCallbackRegister(di->item.area.area_win, 0, DialogHandleEvents, d);
	di->w = iw;
	di->h = ih;
	break;
     case DITEM_CHECKBUTTON:
	if (di->iclass->norm.normal->im_file)
	  {
	     Imlib_Image        *im;

	     im = ELoadImage(di->iclass->norm.normal->im_file);
	     if (im)
	       {
		  imlib_context_set_image(im);
		  di->item.check_button.check_orig_w = imlib_image_get_width();
		  di->item.check_button.check_orig_h = imlib_image_get_height();
		  imlib_free_image();
	       }
	  }
	TextSize(di->tclass, 0, 0, STATE_NORMAL, di->text, &iw, &ih, 17);
	if (ih < di->item.check_button.check_orig_h)
	   ih = di->item.check_button.check_orig_h;
	iw += di->item.check_button.check_orig_w + di->iclass->padding.left;
	di->item.check_button.check_win =
	   ECreateWindow(d->win, -20, -20, 2, 2, 0);
	di->win = ECreateEventWindow(d->win, -20, -20, 2, 2);
	EMapWindow(di->item.check_button.check_win);
	EMapWindow(di->win);
	ESelectInput(di->win,
		     EnterWindowMask | LeaveWindowMask | ButtonPressMask |
		     ButtonReleaseMask | ExposureMask);
	di->w = iw;
	di->h = ih;
	break;
     case DITEM_TEXT:
	TextSize(di->tclass, 0, 0, STATE_NORMAL, di->text, &iw, &ih, 17);
	di->w = iw;
	di->h = ih;
	break;
     case DITEM_IMAGE:
	{
	   Imlib_Image        *im;

	   im = ELoadImage(di->item.image.image);
	   if (im)
	     {
		Pixmap              pmap = 0, mask = 0;

		imlib_context_set_image(im);
		iw = imlib_image_get_width();
		ih = imlib_image_get_height();
		di->win = ECreateWindow(d->win, 0, 0, iw, ih, 0);
		EMapWindow(di->win);
		register_win_callback = 0;
		imlib_render_pixmaps_for_whole_image(&pmap, &mask);
		ESetWindowBackgroundPixmap(di->win, pmap);
		EShapeCombineMask(di->win, ShapeBounding, 0, 0, mask, ShapeSet);
		imlib_free_pixmap_and_mask(pmap);
		imlib_free_image();
	     }
	}
	di->w = iw;
	di->h = ih;
	break;
     case DITEM_SEPARATOR:
	iw = di->iclass->padding.left + di->iclass->padding.right;
	ih = di->iclass->padding.top + di->iclass->padding.bottom;
	di->win = ECreateWindow(d->win, -20, -20, 2, 2, 0);
	EMapWindow(di->win);
	register_win_callback = 0;
	di->w = iw;
	di->h = ih;
	break;
     case DITEM_RADIOBUTTON:
	if (di->iclass->norm.normal->im_file)
	  {
	     Imlib_Image        *im;

	     im = ELoadImage(di->iclass->norm.normal->im_file);
	     if (im)
	       {
		  imlib_context_set_image(im);
		  di->item.radio_button.radio_orig_w = imlib_image_get_width();
		  di->item.radio_button.radio_orig_h = imlib_image_get_height();
		  imlib_free_image();
	       }
	  }
	TextSize(di->tclass, 0, 0, STATE_NORMAL, di->text, &iw, &ih, 17);
	if (ih < di->item.radio_button.radio_orig_h)
	   ih = di->item.radio_button.radio_orig_h;
	iw += di->item.radio_button.radio_orig_w + di->iclass->padding.left;
	di->item.radio_button.radio_win =
	   ECreateWindow(d->win, -20, -20, 2, 2, 0);
	di->win = ECreateEventWindow(d->win, -20, -20, 2, 2);
	EMapWindow(di->item.radio_button.radio_win);
	EMapWindow(di->win);
	ESelectInput(di->win,
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
			     EMoveResizeWindow(dii->win, dii->x, dii->y,
					       dii->w, dii->h);
			  if (dii->type == DITEM_CHECKBUTTON)
			     EMoveResizeWindow(dii->item.check_button.
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
			     EMoveResizeWindow(dii->item.radio_button.
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
			       EMoveResizeWindow(dii->item.area.area_win,
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
				  EMoveResizeWindow(dii->item.slider.base_win,
						    dii->x +
						    dii->item.slider.base_x,
						    dii->y +
						    dii->item.slider.base_y,
						    dii->item.slider.base_w,
						    dii->item.slider.base_h);
			       if (dii->item.slider.border_win)
				  EMoveResizeWindow(dii->item.slider.
						    border_win,
						    dii->x +
						    dii->item.slider.border_x,
						    dii->y +
						    dii->item.slider.border_y,
						    dii->item.slider.border_w,
						    dii->item.slider.border_h);
			       if (dii->win)
				  EMoveResizeWindow(dii->win,
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

   if (di->win && register_win_callback)
      EventCallbackRegister(di->win, 0, DialogHandleEvents, d);
}

static void
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
	   EMoveWindow(dii->win, dii->x, dii->y);
	if (dii->type == DITEM_CHECKBUTTON)
	   EMoveWindow(dii->item.check_button.check_win, dii->x,
		       dii->y +
		       ((dii->h - dii->item.check_button.check_orig_h) / 2));
	if (dii->type == DITEM_RADIOBUTTON)
	   EMoveWindow(dii->item.radio_button.radio_win, dii->x,
		       dii->y +
		       ((dii->h - dii->item.radio_button.radio_orig_h) / 2));
	if (dii->type == DITEM_SLIDER)
	  {
	     if (dii->item.slider.base_win)
		EMoveResizeWindow(dii->item.slider.base_win,
				  dii->x + dii->item.slider.base_x,
				  dii->y + dii->item.slider.base_y,
				  dii->item.slider.base_w,
				  dii->item.slider.base_h);
	     if (dii->item.slider.knob_win)
		EMoveResizeWindow(dii->item.slider.knob_win,
				  dii->x + dii->item.slider.knob_x,
				  dii->y + dii->item.slider.knob_y,
				  dii->item.slider.knob_w,
				  dii->item.slider.knob_h);
	     if (dii->item.slider.border_win)
		EMoveResizeWindow(dii->item.slider.border_win,
				  dii->x + dii->item.slider.border_x,
				  dii->y + dii->item.slider.border_y,
				  dii->item.slider.border_w,
				  dii->item.slider.border_h);
	     if (dii->win)
		EMoveResizeWindow(dii->win,
				  dii->x + dii->item.slider.numeric_x,
				  dii->y + dii->item.slider.numeric_y,
				  dii->item.slider.numeric_w,
				  dii->item.slider.numeric_h);
	  }
     }
}

void
DialogDrawItems(Dialog * d, DItem * di, int x, int y, int w, int h)
{
   d->update = 1;
   di->update = 1;

   if (d->xu1 > x)
      d->xu1 = x;
   if (d->yu1 > y)
      d->yu1 = y;
   x += w;
   y += h;
   if (d->xu2 < x)
      d->xu2 = x;
   if (d->yu2 < y)
      d->yu2 = y;

   dialog_update_pending = 1;

#if DEBUG_DIALOGS
   Eprintf("DialogDrawItems t=%d u=%d - %d,%d -> %d,%d\n", di->type, di->update,
	   d->xu1, d->yu1, d->xu2, d->yu2);
#endif
}

static void
DialogDrawItem(Dialog * d, DItem * di)
{
   int                 state;

   if (!di->update && di->type != DITEM_TABLE)
      return;

   if (di->x > d->xu2 || di->y > d->yu2 ||
       di->x + di->w <= d->xu1 || di->y + di->h <= d->yu1)
      goto done;

#if DEBUG_DIALOGS
   Eprintf("DialogDrawItem t=%d u=%d - %d,%d -> %d,%d\n", di->type, di->update,
	   d->xu1, d->yu1, d->xu2, d->yu2);
#endif

   switch (di->type)
     {
     case DITEM_TABLE:
	{
	   int                 i;
	   DItem              *dii;

	   for (i = 0; i < di->item.table.num_items; i++)
	     {
		dii = di->item.table.items[i];
		if (di->update)
		   dii->update = 1;
		DialogDrawItem(d, dii);
	     }
	}
	break;

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
	   EMoveResizeWindow(di->item.slider.knob_win,
			     di->x + di->item.slider.knob_x,
			     di->y + di->item.slider.knob_y,
			     di->item.slider.knob_w, di->item.slider.knob_h);
	if (di->item.slider.base_win)
	   ImageclassApply(di->item.slider.ic_base,
			   di->item.slider.base_win,
			   di->item.slider.base_w, di->item.slider.base_h,
			   0, 0, STATE_NORMAL, 0, ST_WIDGET);
	if (di->item.slider.border_win)
	   ImageclassApply(di->item.slider.ic_border,
			   di->item.slider.border_win,
			   di->item.slider.border_w,
			   di->item.slider.border_h, 0, 0, STATE_NORMAL, 0,
			   ST_WIDGET);
	state = STATE_NORMAL;
	if ((di->hilited) && (di->clicked))
	   state = STATE_CLICKED;
	else if ((di->hilited) && (!di->clicked))
	   state = STATE_HILITED;
	else if (!(di->hilited) && (di->clicked))
	   state = STATE_CLICKED;
	if (di->item.slider.knob_win)
	   ImageclassApply(di->item.slider.ic_knob,
			   di->item.slider.knob_win,
			   di->item.slider.knob_w, di->item.slider.knob_h,
			   0, 0, state, 0, ST_WIDGET);
	break;
     case DITEM_BUTTON:
	state = STATE_NORMAL;
	if ((di->hilited) && (di->clicked))
	   state = STATE_CLICKED;
	else if ((di->hilited) && (!di->clicked))
	   state = STATE_HILITED;
	else if (!(di->hilited) && (di->clicked))
	   state = STATE_CLICKED;
	ImageclassApply(di->iclass, di->win, di->w, di->h, 0, 0, state, 0,
			ST_WIDGET);
	TextclassApply(di->iclass, di->win, di->w, di->h, 0, 0, state, 1,
		       di->tclass, di->text);
	break;
     case DITEM_AREA:
	ImageclassApply(di->iclass, di->win, di->w, di->h, 0, 0,
			STATE_NORMAL, 0, ST_DIALOG);
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
	   ImageclassApply(di->iclass, di->item.check_button.check_win,
			   di->item.check_button.check_orig_w,
			   di->item.check_button.check_orig_h, 1, 0, state,
			   0, ST_WIDGET);
	else
	   ImageclassApply(di->iclass, di->item.check_button.check_win,
			   di->item.check_button.check_orig_w,
			   di->item.check_button.check_orig_h, 0, 0, state,
			   0, ST_WIDGET);
	EClearArea(d->win, di->x, di->y, di->w, di->h, False);
	TextDraw(di->tclass, d->win, 0, 0, STATE_NORMAL, di->text,
		 di->x + di->item.check_button.check_orig_w +
		 di->iclass->padding.left, di->y,
		 di->w - di->item.check_button.check_orig_w -
		 di->iclass->padding.left, 99999, 17,
		 di->tclass->justification);
	break;
     case DITEM_TEXT:
	EClearArea(d->win, di->x, di->y, di->w, di->h, False);
	TextDraw(di->tclass, d->win, 0, 0, STATE_NORMAL, di->text,
		 di->x, di->y, di->w, 99999, 17, di->tclass->justification);
	break;
     case DITEM_IMAGE:
	break;
     case DITEM_SEPARATOR:
	if (di->item.separator.horizontal)
	   ImageclassApply(di->iclass, di->win, di->w, di->h, 0, 0,
			   STATE_NORMAL, 0, ST_WIDGET);
	else
	   ImageclassApply(di->iclass, di->win, di->w, di->h, 0, 0,
			   STATE_CLICKED, 0, ST_WIDGET);
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
	   ImageclassApply(di->iclass, di->item.radio_button.radio_win,
			   di->item.radio_button.radio_orig_w,
			   di->item.radio_button.radio_orig_h, 1, 0, state,
			   0, ST_WIDGET);
	else
	   ImageclassApply(di->iclass, di->item.radio_button.radio_win,
			   di->item.radio_button.radio_orig_w,
			   di->item.radio_button.radio_orig_w, 0, 0, state,
			   0, ST_WIDGET);
	EClearArea(d->win, di->x, di->y, di->w, di->h, False);
	TextDraw(di->tclass, d->win, 0, 0, STATE_NORMAL, di->text,
		 di->x + di->item.radio_button.radio_orig_w +
		 di->iclass->padding.left, di->y,
		 di->w - di->item.radio_button.radio_orig_w -
		 di->iclass->padding.left, 99999, 17,
		 di->tclass->justification);
	break;
     default:
	break;
     }

 done:
   di->update = 0;
}

static void
DialogUpdate(Dialog * d)
{
   if (d->item)
      DialogDrawItem(d, d->item);
   d->update = 0;
   d->xu1 = d->yu1 = 99999;
   d->xu2 = d->yu2 = 0;
}

void
DialogsCheckUpdate(void)
{
   Dialog             *d, **ds;
   int                 i, num;

   if (!dialog_update_pending)
      return;
   dialog_update_pending = 0;

   ds = (Dialog **) ListItemType(&num, LIST_TYPE_DIALOG);
   for (i = 0; i < num; i++)
     {
	d = ds[i];
	if (!d->update)
	   continue;
	DialogUpdate(d);
     }
   if (ds)
      Efree(ds);
}

static void
DialogItemsRealize(Dialog * d)
{
   if (!d->item)
      return;
   DialogRealizeItem(d, d->item);
   DialogDrawItems(d, d->item, 0, 0, 99999, 99999);
   d->w = d->item->w + d->iclass->padding.left + d->iclass->padding.right;
   d->h = d->item->h + d->iclass->padding.top + d->iclass->padding.bottom;
}

void
DialogItemSetText(DItem * di, const char *text)
{
   if (di->text)
      Efree(di->text);
   di->text = Estrdup(text);
}

void
DialogItemRadioButtonSetEventFunc(DItem * di, DialogItemCallbackFunc * func)
{
   di->item.radio_button.event_func = func;
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
   DialogItemCheckButtonSetState(di, *onoff_ptr);
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
DialogItemImageSetFile(DItem * di, const char *image)
{
   if (di->item.image.image)
      Efree(di->item.image.image);
   di->item.image.image = Estrdup(image);
   di->fill_h = 0;
   di->fill_v = 0;
}

void
DialogItemSliderSetVal(DItem * di, int val)
{
   if (val < di->item.slider.lower)
      val = di->item.slider.lower;
   else if (val > di->item.slider.upper)
      val = di->item.slider.upper;
   di->item.slider.val = val;
   if (di->item.slider.val_ptr)
      *di->item.slider.val_ptr = val;
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

int
DialogItemSliderGetVal(DItem * di)
{
   return di->item.slider.val;
}

void
DialogItemSliderGetBounds(DItem * di, int *lower, int *upper)
{
   if (lower)
      *lower = di->item.slider.lower;
   if (upper)
      *upper = di->item.slider.upper;
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
DialogItemAreaSetEventFunc(DItem * di, DialogItemCallbackFunc * func)
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

   if (di->text)
      Efree(di->text);

   switch (di->type)
     {
     default:
	break;
     case DITEM_IMAGE:
	if (di->item.image.image)
	   Efree(di->item.image.image);
	break;
     case DITEM_SLIDER:
	if (di->item.slider.ic_base)
	   di->item.slider.ic_base->ref_count--;
	if (di->item.slider.ic_knob)
	   di->item.slider.ic_knob->ref_count--;
	if (di->item.slider.ic_border)
	   di->item.slider.ic_border->ref_count--;
	break;
     case DITEM_TABLE:
	if (di->item.table.items)
	   Efree(di->item.table.items);
	break;
     }

   if (di->iclass)
      di->iclass->ref_count--;
   if (di->tclass)
      di->tclass->ref_count--;

   Efree(di);
}

static DItem       *
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

DItem              *
DialogItem(Dialog * d)
{
   return d->item;
}

/* Convenience callback to close dialog */
void
DialogCallbackClose(Dialog * d, int val __UNUSED__, void *data __UNUSED__)
{
   DialogClose(d);
}

/*
 * Predefined dialogs
 */

void
DialogOK(const char *title, const char *fmt, ...)
{
   char                text[10240];
   va_list             ap;

   va_start(ap, fmt);
   Evsnprintf(text, sizeof(text), fmt, ap);
   va_end(ap);

   DialogOKstr(title, text);
}

void
DialogOKstr(const char *title, const char *txt)
{
   Dialog             *d;

   d = DialogCreate("DIALOG");
   DialogSetTitle(d, title);
   DialogSetText(d, txt);

   DialogAddButton(d, _("OK"), NULL, 1, DIALOG_BUTTON_OK);
   ShowDialog(d);
}

void
DialogAlert(const char *fmt, ...)
{
   char                text[10240];
   va_list             ap;

   va_start(ap, fmt);
   Evsnprintf(text, 10240, fmt, ap);
   va_end(ap);
   Alert(text);
}

void
DialogAlertOK(const char *fmt, ...)
{
   char                text[10240];
   va_list             ap;

   va_start(ap, fmt);
   Evsnprintf(text, 10240, fmt, ap);
   va_end(ap);
   AlertX(_("Attention !!!"), _("OK"), NULL, NULL, text);
}

/*
 * Dialog event handlers
 */

static void
DialogEventKeyPress(Dialog * d, XEvent * ev)
{
   int                 i;

   for (i = 0; i < d->num_bindings; i++)
     {
	if (ev->xkey.keycode != d->keybindings[i].key)
	   continue;
	d->keybindings[i].func(d, d->keybindings[i].val, NULL);
	break;
     }
}

static DItem       *
DialogFindDItem(Dialog * d, Window win)
{
   return (d->item) ? DialogItemFindWindow(d->item, win) : NULL;
}

static void
DialogEventMotion(Dialog * d, XEvent * ev)
{
   Window              win = ev->xmotion.window;
   DItem              *di;
   int                 dx, dy;

   di = DialogFindDItem(d, win);
   if (!di)
      return;

   switch (di->type)
     {
     case DITEM_AREA:
	if (di->item.area.event_func)
	   di->item.area.event_func(0, ev);
	break;

     case DITEM_SLIDER:
	if (!di->item.slider.in_drag)
	   break;
	if (ev->xmotion.window == di->item.slider.knob_win)
	  {
	     dx = Mode.events.x - Mode.events.px;
	     dy = Mode.events.y - Mode.events.py;
	     if (di->item.slider.horizontal)
	       {
		  di->item.slider.wanted_val += dx;
		  di->item.slider.val =
		     di->item.slider.lower +
		     (((di->item.slider.wanted_val *
			(di->item.slider.upper -
			 di->item.slider.lower)) /
		       (di->item.slider.base_w -
			di->item.slider.knob_w)) /
		      di->item.slider.unit) * di->item.slider.unit;
	       }
	     else
	       {
		  di->item.slider.wanted_val += dy;
		  di->item.slider.val =
		     di->item.slider.lower +
		     ((((di->item.
			 slider.base_h - di->item.slider.knob_h -
			 di->item.slider.wanted_val) *
			(di->item.slider.upper -
			 di->item.slider.lower)) /
		       (di->item.slider.base_h -
			di->item.slider.knob_h)) /
		      di->item.slider.unit) * di->item.slider.unit;
	       }
	     if (di->item.slider.val < di->item.slider.lower)
		di->item.slider.val = di->item.slider.lower;
	     if (di->item.slider.val > di->item.slider.upper)
		di->item.slider.val = di->item.slider.upper;
	     if (di->item.slider.val_ptr)
		*di->item.slider.val_ptr = di->item.slider.val;
	     if (di->func)
		(di->func) (d, di->val, di->data);
	  }

	DialogDrawItems(d, di, di->x, di->y, di->w, di->h);
	break;
     }
}

static void
DialogEventExpose(Dialog * d, XEvent * ev)
{
   Window              win = ev->xexpose.window;
   DItem              *di;

   DialogDrawArea(d, ev->xexpose.x, ev->xexpose.y,
		  ev->xexpose.width, ev->xexpose.height);

   di = DialogFindDItem(d, win);
   if (!di)
      return;

   switch (di->type)
     {
     case DITEM_AREA:
	if (di->func)
	   di->func(d, di->val, di->data);
	break;
     }
}

static void
DialogEventMouseDown(Dialog * d, XEvent * ev)
{
   Window              win = ev->xbutton.window;
   DItem              *di;
   int                 x, y, wheel_jump;

   di = DialogFindDItem(d, win);
   if (!di)
      return;

   switch (di->type)
     {
     case DITEM_AREA:
	if (di->item.area.event_func)
	   di->item.area.event_func(0, ev);
	break;

     case DITEM_SLIDER:
#if 0				/* Do any themes have this? */
	if (win == di->item.slider.border_win)
	   break;
#endif

	if (win == di->item.slider.knob_win)
	  {
	     if (ev->xbutton.button >= 1 && ev->xbutton.button <= 3)
	       {
		  di->item.slider.in_drag = 1;
		  if (di->item.slider.horizontal)
		     di->item.slider.wanted_val = di->item.slider.knob_x;
		  else
		     di->item.slider.wanted_val = di->item.slider.knob_y;
		  break;
	       }
	  }

	/* Coords -> item.slider.base_win */
	ETranslateCoordinates(win, di->item.slider.base_win,
			      ev->xbutton.x, ev->xbutton.y, &x, &y, NULL);

	switch (ev->xbutton.button)
	  {
	  case 1:
	  case 3:
	     if (di->item.slider.horizontal)
	       {
		  if (ev->xbutton.x >
		      (di->item.slider.knob_x + (di->item.slider.knob_w / 2)))
		     di->item.slider.val += di->item.slider.jump;
		  else
		     di->item.slider.val -= di->item.slider.jump;
	       }
	     else
	       {
		  if (ev->xbutton.y >
		      (di->item.slider.knob_y + (di->item.slider.knob_h / 2)))
		     di->item.slider.val -= di->item.slider.jump;
		  else
		     di->item.slider.val += di->item.slider.jump;
	       }
	     break;

	  case 2:
	     if (di->item.slider.horizontal)
		di->item.slider.val = x *
		   (di->item.slider.upper - di->item.slider.lower) / di->w;
	     else
		di->item.slider.val = ((di->h - y) *
				       (di->item.slider.upper -
					di->item.slider.lower) / di->h);
	     break;

	  case 4:
	  case 5:
	     wheel_jump = di->item.slider.jump / 2;
	     if (!wheel_jump)
		wheel_jump++;

	     if (ev->xbutton.button == 5)
	       {
		  di->item.slider.val -= wheel_jump;
	       }
	     else if (ev->xbutton.button == 4)
	       {
		  di->item.slider.val += wheel_jump;
	       }
	     break;
	  }
	if (di->item.slider.val < di->item.slider.lower)
	   di->item.slider.val = di->item.slider.lower;
	if (di->item.slider.val > di->item.slider.upper)
	   di->item.slider.val = di->item.slider.upper;
	if (di->item.slider.val_ptr)
	   *di->item.slider.val_ptr = di->item.slider.val;
#if 0				/* Remove? */
	if (di->func)
	   (di->func) (d, di->val, di->data);
#endif
	break;
     }

   di->clicked = 1;

   DialogDrawItems(d, di, di->x, di->y, di->w, di->h);
}

static void
DialogEventMouseUp(Dialog * d, XEvent * ev)
{
   Window              win = ev->xbutton.window;
   DItem              *di, *dii;

   if (win != Mode.events.last_bpress)
      return;

   di = DialogFindDItem(d, win);
   if (!di)
      return;

   di->clicked = 0;

   switch (di->type)
     {
     case DITEM_AREA:
	if (di->item.area.event_func)
	   di->item.area.event_func(0, ev);
	break;

     case DITEM_CHECKBUTTON:
	if (di->item.check_button.onoff)
	   di->item.check_button.onoff = 0;
	else
	   di->item.check_button.onoff = 1;
	if (di->item.check_button.onoff_ptr)
	   *di->item.check_button.onoff_ptr = di->item.check_button.onoff;
	break;

     case DITEM_RADIOBUTTON:
	dii = di->item.radio_button.first;
	while (dii)
	  {
	     if (dii->item.radio_button.onoff)
	       {
		  dii->item.radio_button.onoff = 0;
		  DialogDrawItems(d, dii, dii->x, dii->y, dii->w, dii->h);
	       }
	     dii = dii->item.radio_button.next;
	  }
	di->item.radio_button.onoff = 1;
	if (di->item.radio_button.val_ptr)
	   *di->item.radio_button.val_ptr = di->item.radio_button.val;
	break;

     case DITEM_SLIDER:
	if (win == di->item.slider.knob_win)
	   di->item.slider.in_drag = 0;
	break;
     }

   DialogDrawItems(d, di, di->x, di->y, di->w, di->h);

   if (di->func)
      di->func(d, di->val, di->data);
}

static void
DialogEventMouseIn(Dialog * d, XEvent * ev)
{
   Window              win = ev->xcrossing.window;
   DItem              *di;

   di = DialogFindDItem(d, win);
   if (!di)
      return;

   switch (di->type)
     {
     case DITEM_AREA:
	if (di->item.area.event_func)
	   di->item.area.event_func(0, ev);
	break;

     case DITEM_RADIOBUTTON:
	if (di->item.radio_button.event_func)
	   di->item.radio_button.event_func(di->item.radio_button.val, ev);
	break;
     }

   di->hilited = 1;

   DialogDrawItems(d, di, di->x, di->y, di->w, di->h);
}

static void
DialogEventMouseOut(Dialog * d, XEvent * ev)
{
   Window              win = ev->xcrossing.window;
   DItem              *di;

   di = DialogFindDItem(d, win);
   if (!di)
      return;

   switch (di->type)
     {
     case DITEM_AREA:
	if (di->item.area.event_func)
	   di->item.area.event_func(0, ev);
	break;

     case DITEM_RADIOBUTTON:
	if (di->item.radio_button.event_func)
	   di->item.radio_button.event_func(di->item.radio_button.val, NULL);
	break;
     }

   di->hilited = 0;

   DialogDrawItems(d, di, di->x, di->y, di->w, di->h);
}

static void
DialogHandleEvents(XEvent * ev, void *prm)
{
   Dialog             *d = (Dialog *) prm;

   switch (ev->type)
     {
     case KeyPress:
	DialogEventKeyPress(d, ev);
	break;
     case ButtonPress:
	DialogEventMouseDown(d, ev);
	break;
     case ButtonRelease:
	DialogEventMouseUp(d, ev);
	break;
     case MotionNotify:
	DialogEventMotion(d, ev);
	break;
     case EnterNotify:
	DialogEventMouseIn(d, ev);
	break;
     case LeaveNotify:
	DialogEventMouseOut(d, ev);
	break;
     case Expose:
	DialogEventExpose(d, ev);
	break;
     }
}

#if 0				/* TBD */
static void
DItemHandleEvents(XEvent * ev, void *prm)
{
   DItem              *di = (Dialog *) prm;
   Window              win = ev->xany.window;

   switch (ev->type)
     {
     case ButtonPress:
	break;
     case ButtonRelease:
	break;
     case EnterNotify:
	break;
     case LeaveNotify:
	break;
     case Expose:
	break;
     }
}
#endif

static void
DButtonHandleEvents(XEvent * ev, void *prm)
{
   DButton            *db = (DButton *) prm;
   Dialog             *d;
   int                 doact = 0;

   d = db->parent;

   switch (ev->type)
     {
     case ButtonPress:
	db->clicked = 1;
	break;
     case ButtonRelease:
	if (db->hilited && db->clicked)
	   doact = 1;
	db->clicked = 0;
	break;
     case EnterNotify:
	db->hilited = 1;
	break;
     case LeaveNotify:
	db->hilited = 0;
	break;
     case Expose:
	break;
     default:
	return;
     }

   DialogDrawButton(d, db);

   if (doact)
     {
	if (db->func)
	  {
	     int                 i;

	     for (i = 0; i < d->num_buttons; i++)
		if (d->button[i] == db)
		   break;
	     db->func(d, i, NULL);
	  }

	if (db->close)
	   DialogClose(d);
     }
}

/*
 * Finders
 */

static Dialog      *
FindDialog(Window win)
{
   Dialog            **ds, *d = NULL;
   int                 i, num;

   ds = (Dialog **) ListItemType(&num, LIST_TYPE_DIALOG);
   for (i = 0; i < num; i++)
     {
	if (ds[i]->win != win)
	   continue;
	d = ds[i];
	break;
     }
   if (ds)
      Efree(ds);
   return d;
}

EWin               *
FindEwinByDialog(Dialog * d)
{
   EWin               *const *ewins;
   int                 i, num;

   ewins = EwinListGetAll(&num);
   for (i = 0; i < num; i++)
     {
	if ((Dialog *) (ewins[i]->data) == d)
	   return ewins[i];
     }

   return NULL;
}

int
FindADialog(void)
{
   EWin               *const *ewins;
   int                 i, num, n;

   ewins = EwinListGetAll(&num);
   for (i = n = 0; i < num; i++)
     {
	if (ewins[i]->type == EWIN_TYPE_DIALOG)
	   n++;
     }

   return n;
}

#if 0				/* Unused */
static int
doAlert(EWin * edummy __UNUSED__, const char *params)
{
   char               *pp;
   int                 i;

   pp = Estrdup(params);
   if (!pp)
      return 1;
   if (strlen(pp) <= 0)
      return 1;

   i = 1;
   while (pp[i])
     {
	if ((pp[i - 1] == '\\') && (params[i] == 'n'))
	  {
	     pp[i - 1] = ' ';
	     pp[i] = '\n';
	  }
	i++;
     }
   DialogAlertOK(pp);
   Efree(pp);

   return 0;
}
#endif
