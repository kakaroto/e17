#include "Elation.h"

/* external module symbols. the rest is private */
void *init(Elation_Module *em);

/********************/
typedef struct _Elation_Module_Private Elation_Module_Private;

struct _Elation_Module_Private
{
   Evas_Object *key;
   Evas_Object *video;
   Evas_List *items;
};

typedef struct _Item Item;

struct _Item
{
   Evas_Object *label;
   Evas_Object *icon;
};

static void shutdown(Elation_Module *em);
static void resize(Elation_Module *em);
static void show(Elation_Module *em);
static void hide(Elation_Module *em);
static void focus(Elation_Module *em);
static void unfocus(Elation_Module *em);
static void action(Elation_Module *em, int action);

static void key_down(void *data, Evas *e, Evas_Object *obj, void *event_info);

/*** init method - required ***/

void *
init(Elation_Module *em)
{
   Elation_Module_Private *pr;
   Evas_Coord w, h;
   
   pr = calloc(1, sizeof(Elation_Module_Private));
   if (!pr) return NULL;
   
   /* set up module methods */
   em->shutdown = shutdown;
   em->resize = resize;
   em->show = show;
   em->hide = hide;
   em->focus = focus;
   em->unfocus = unfocus;
   em->action = action;
   
   evas_viewport_get(em->info->evas, NULL, NULL, &w, &h);
   
     {
	Evas_Object *o;
	
	o = evas_object_rectangle_add(em->info->evas);
	pr->key = o;
	
	evas_object_event_callback_add(o, EVAS_CALLBACK_KEY_DOWN, key_down, em);
     }
     
   return pr;
}

/*** methods advertised ***/

static void
shutdown(Elation_Module *em)
{
   Elation_Module_Private *pr;
   
   pr = em->data;
   evas_object_del(pr->video);
   free(pr);
}

static void
resize(Elation_Module *em)
{
   Elation_Module_Private *pr;
   Evas_Coord w, h;
   
   pr = em->data;
   evas_viewport_get(em->info->evas, NULL, NULL, &w, &h);
   evas_object_move(pr->video, 0, 0);
   evas_object_resize(pr->video, w, h);
}

static void
show(Elation_Module *em)
{
   Elation_Module_Private *pr;
   
   pr = em->data;
   evas_object_show(pr->video);
}

static void
hide(Elation_Module *em)
{
   Elation_Module_Private *pr;
   
   pr = em->data;
   evas_object_hide(pr->video);
}

static void
focus(Elation_Module *em)
{
   Elation_Module_Private *pr;
   
   pr = em->data;
   evas_object_focus_set(pr->video, 1);
}

static void
unfocus(Elation_Module *em)
{
   Elation_Module_Private *pr;
   
   pr = em->data;
   evas_object_focus_set(pr->video, 0);
}

static void
action(Elation_Module *em, int action)
{
   Elation_Module_Private *pr;
   
   pr = em->data;
   switch (action)
     {
      case ELATION_ACT_EXIT:
	em->hide(em);
	break; 
      case ELATION_ACT_SELECT:
	break;
      case ELATION_ACT_PREV:
	break;
      case ELATION_ACT_NEXT:
	break;
      case ELATION_ACT_NONE:
      default:
	break;
     }
}

/*** private stuff ***/

static void
key_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Key_Down *ev;
   Elation_Module *em;
   Elation_Module_Private *pr;
   int action = ELATION_ACT_NONE;
   
   ev = (Evas_Event_Key_Down *)event_info;
   em = data;
   pr = em->data;
   
   /* translator */
   /* FIXME: create proper translator sys later to amke remote config easy */
   
   if (!strcmp(ev->keyname, "Escape"))      action = ELATION_ACT_EXIT;
   else if (!strcmp(ev->keyname, "Up"))     action = ELATION_ACT_PREV;
   else if (!strcmp(ev->keyname, "Down"))   action = ELATION_ACT_NEXT;
   else if (!strcmp(ev->keyname, "Left"))   action = ELATION_ACT_EXIT;
   else if (!strcmp(ev->keyname, "Right"))  action = ELATION_ACT_SELECT;
   else if (!strcmp(ev->keyname, "Return")) action = ELATION_ACT_SELECT;
   em->action(em, action);
}

static void
add_item(Elation_Module *em, char *text, char *icon, void (*func) (void *data, Elation_Module *em), void *data)
{
}
