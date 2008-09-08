#include <Elementary.h>
#include "elm_priv.h"

static void _elm_label_text_set(Elm_Label *lb, const char *text);

Elm_Label_Class _elm_label_class =
{
   &_elm_widget_class,
     ELM_OBJ_LABEL,
     _elm_label_text_set
};

static void
_elm_label_text_set(Elm_Label *lb, const char *text)
{
   Evas_Coord mw, mh;
   
   edje_object_part_text_set(lb->base, "elm.text", text);
   edje_object_size_min_calc(lb->base, &mw, &mh);
   if ((lb->tw != mw) || (lb->th != mh))
     {
	lb->tw = mw;
	lb->th = mh;
	((Elm_Widget *)(lb->parent))->size_req(lb->parent, lb, lb->tw, lb->th);
	lb->geom_set(lb, lb->x, lb->y, lb->tw, lb->th);
     }
}

static void
_elm_label_size_alloc(Elm_Label *lb, int w, int h)
{
   lb->req.w = lb->tw;
   lb->req.h = lb->th;
}

EAPI Elm_Label *
elm_label_new(Elm_Win *win)
{
   Elm_Label *lb;
   
   lb = ELM_NEW(Elm_Label);
   
   _elm_widget_init(lb);
   lb->clas = &_elm_label_class;
   lb->type = ELM_OBJ_LABEL;

   lb->size_alloc = _elm_label_size_alloc;
   
   lb->text_set = _elm_label_text_set;

   lb->base = edje_object_add(win->evas);
   _elm_theme_set(lb->base, "label", "label");
   win->child_add(win, lb);
   return lb;
}
