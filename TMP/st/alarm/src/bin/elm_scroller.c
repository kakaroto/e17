#include <Elementary.h>
#include "elm_priv.h"

static void _elm_scroller_del(Elm_Scroller *sc);
    
Elm_Scroller_Class _elm_scroller_class =
{
   &_elm_widget_class,
     ELM_OBJ_SCROLLER
};

static void
_elm_scroller_del(Elm_Scroller *sc)
{
   evas_object_del(sc->scroller_pan);
   ((Elm_Obj_Class *)(((Elm_Scroller_Class *)(sc->clas))->parent))->del(ELM_OBJ(sc));
}

static void
_elm_on_child_add(void *data, Elm_Scroller *sc, Elm_Cb_Type type, Elm_Obj *obj)
{
   if (!(obj->hastype(obj, ELM_OBJ_WIDGET))) return;
   printf("set child! %p\n", ((Elm_Widget *)(obj))->base);
   elm_smart_scroller_child_set(sc->base, ((Elm_Widget *)(obj))->base);
}

static void
_elm_on_child_del(void *data, Elm_Scroller *sc, Elm_Cb_Type type, Elm_Obj *obj)
{
   if (!(obj->hastype(obj, ELM_OBJ_WIDGET))) return;
   elm_smart_scroller_child_set(sc->base, NULL);
}

EAPI Elm_Scroller *
elm_scroller_new(Elm_Win *win)
{
   Elm_Scroller *sc;
   
   sc = ELM_NEW(Elm_Scroller);
   _elm_widget_init(sc);
   
   sc->clas = &_elm_scroller_class;
   sc->type = ELM_OBJ_SCROLLER;
   
   sc->del = _elm_scroller_del;

   sc->base = elm_smart_scroller_add(win->evas);
//   elm_smart_scroller_policy_set(sc->base, ELM_SMART_SCROLLER_POLICY_ON, ELM_SMART_SCROLLER_POLICY_ON);
   
   sc->cb_add(sc, ELM_CB_CHILD_ADD, _elm_on_child_add, NULL);
   sc->cb_add(sc, ELM_CB_CHILD_DEL, _elm_on_child_del, NULL);
   win->child_add(win, sc);
   ((Elm_Widget *)(sc->parent))->size_req(sc->parent, sc, 100, 100);
   return sc;
}
