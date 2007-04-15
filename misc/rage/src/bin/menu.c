/* FIXME: add multiple menu contexts - named (and swallowed into that region)
 * so the video screen can have its own menus
 */
#include "main.h"

typedef struct _Context   Context;
typedef struct _Menu      Menu;
typedef struct _Menu_Item Menu_Item;

struct _Context
{
   char        *name;
   Evas_Object *o_box;
   Evas_Object *o_box2;
   Evas_List   *menus;
};

struct _Menu
{
   const char    *name;
   Evas_List     *items;
   unsigned char  selected : 1;
   Evas_Object   *box;
   void          *data;
   void         (*free_func) (void *data);
};

struct _Menu_Item
{
   Evas_Object   *base;
   Evas_Object   *ic;
   const char    *label;
   const char    *icon;
   const char    *desc;
   const char    *info;
   void         (*func) (void *data);
   void          *data;
   void         (*free_func) (void *data);
   void         (*over_func) (void *data);
   void         (*out_func) (void *data);
   unsigned char  selected : 1;
   unsigned char  enabled : 1;
};

static void         _menu_init(void);
static Menu        *_menu_current_get(void);
static void         _menu_item_select_update(Menu *m, Menu_Item *mi);

static Evas_Object *o_box = NULL;
static Evas_List   *menus   = NULL;

static void
_menu_init(void)
{
   if (o_box) return;
   o_box = e_box_add(evas);
   e_box_orientation_set(o_box, 1);
   e_box_align_set(o_box, 0.5, 0.5);
   layout_swallow("menu", o_box);
}

static Menu *
_menu_current_get(void)
{
   Evas_List *l;
   Menu *m;
   
   for (l = menus; l; l = l->next)
     {
	m = l->data;
	if (m->selected) return m;
     }
   return NULL;
}

static void
_menu_item_select_update(Menu *m, Menu_Item *mi)
{
   if (mi->selected)
     {
	Evas_List *l;
	int sel = -1, i, c;
	double a = 0.5;
   
	edje_object_signal_emit(mi->base, "select", "on");
	if ((mi->ic)/* && (mi->icon) && (mi->icon[0] != '/')*/)
	  edje_object_signal_emit(mi->ic, "select", "on");
	evas_object_raise(mi->base);
	c = evas_list_count(m->items);
	for (i = 0, l = m->items; l; l = l->next, i++)
	  {
	     Menu_Item *mi2;
	     
	     mi2 = l->data;
	     if (mi2->selected) sel = i;
	  }
	if (c > 1) a = (double)sel / ((double)c - 1.0);
	e_box_align_set(o_box, 0.5, 1.0 - a);
	if (mi->over_func) mi->over_func(mi->data);
     }
   else
     {
	edje_object_signal_emit(mi->base, "select", "off"); 
	if ((mi->ic)/* && (mi->icon) && (mi->icon[0] != '/')*/)
	  edje_object_signal_emit(mi->ic, "select", "off");
	if (mi->out_func) mi->out_func(mi->data);
    }
}

static void
_menu_realize(Menu *m)
{
   Evas_List *l;
   Evas_Coord mw, mh, w, h;
   Menu_Item *miu = NULL;
   
   if (m->box) return;
   m->box = e_box_add(evas);
   e_box_orientation_set(m->box, 0);
   e_box_pack_options_set(m->box, 
			  1, 1, /* fill */
			  1, 1, /* expand */
			  0.5, 0.5, /* align */
			  0, 0, /* min */
			  20000, 20000); /* max */
   e_box_pack_end(o_box, m->box);
   for (l = m->items; l; l = l->next)
     {
	Menu_Item *mi;
	
	mi = l->data;
	mi->base = edje_object_add(evas);
	edje_object_file_set(mi->base, theme, "menu_item");
	if (mi->label)
	  edje_object_part_text_set(mi->base, "label", mi->label);
	else
	  edje_object_part_text_set(mi->base, "label", "");
	if (mi->info)
	  edje_object_part_text_set(mi->base, "info", mi->info);
	else
	  edje_object_part_text_set(mi->base, "info", "");
	if (mi->icon)
	  {
	     if (mi->icon[0] != '/')
	       {
		  mi->ic = edje_object_add(evas);
		  edje_object_file_set(mi->ic, theme, mi->icon);
		  edje_object_part_swallow(mi->base, "item", mi->ic);
		  evas_object_show(mi->ic);
	       }
	     else
	       {
		  mi->ic = mini_add(mi->base, mi->icon);
                  edje_object_part_swallow(mi->base, "item", mi->ic);
		  evas_object_show(mi->ic);
	       }
	  }
	e_box_pack_end(m->box, mi->base);
	mw = mh = 0;
	edje_object_size_min_get(mi->base, &mw, &mh);
	if (mw == 0) mw = 800;
	if (mh == 0) mh = 40;
	e_box_pack_options_set(mi->base,
			       1, 1, /* fill */
			       1, 1, /* expand */
			       0.5, 0.5, /* align */
			       mw, mh, /* min */
			       mw, mh); /* max */
	evas_object_show(mi->base);
	if (mi->selected) miu = mi;
     }
   e_box_min_size_get(m->box, &mw, &mh);
   e_box_pack_options_set(m->box,
			  1, 1, /* fill */
			  1, 1, /* expand */
			  0.5, 0.5, /* align */
			  mw, mh, /* min */
			  mw, mh /* max */);
   e_box_min_size_get(o_box, &mw, &mh);
   evas_object_geometry_get(o_box, NULL, NULL, &w, &h);
   evas_object_show(m->box);
   if (miu) _menu_item_select_update(m, miu);
}

static void
_menu_unrealize(Menu *m)
{
   Evas_List *l;

   if (!m->box) return;
   evas_object_del(m->box);
   m->box = NULL;
   for (l = m->items; l; l = l->next)
     {
	Menu_Item *mi;
	
	mi = l->data;
	evas_object_del(mi->base);
	evas_object_del(mi->ic);
	mi->base = NULL;
	mi->ic = NULL;
     }
}

void
menu_show(void)
{
   Evas_List *l;
   Menu *m;
                  
   for (l = menus; l; l = l->next)
     {
	m = l->data;
	if (m->selected)
	  {
	     _menu_realize(m);
	     break;
	  }
     }
}

void
menu_hide(void)
{
   Evas_List *l;
   Menu *m;
                  
   for (l = menus; l; l = l->next)
     {
	m = l->data;
	if (m->selected)
	  {
	     _menu_unrealize(m);
	     break;
	  }
     }
}

void
menu_push(const char *context, const char *name, void (*free_func) (void *data), void *data)
{
   Menu *m;
   
   _menu_init();
   m = _menu_current_get();
   if (m)
     {
	m->selected = 0;
	_menu_unrealize(m);
     }
   m = calloc(1, sizeof(Menu));
   m->selected = 1;
   m->name = evas_stringshare_add(name);
   m->data = data;
   m->free_func = free_func;
   menus = evas_list_prepend(menus, m);
}

void
menu_pop(void)
{
   Evas_List *l;
   Menu *m, *mm;
    
   for (l = menus; l; l = l->next)
     {
	m = l->data;
	if (m->selected)
	  {
	     _menu_unrealize(m);
	     m->selected = 0;
	     while (m->items)
	       {
		  Menu_Item *mi;
		  
		  mi = m->items->data;
		  if (mi->selected)
		    {
		       if (mi->out_func) mi->out_func(mi->data);
		    }
		  m->items = evas_list_remove_list(m->items, m->items);
		  if (mi->label) evas_stringshare_del(mi->label);
		  if (mi->icon) evas_stringshare_del(mi->icon);
		  if (mi->desc) evas_stringshare_del(mi->desc);
		  if (mi->info) evas_stringshare_del(mi->info);
		  if (mi->free_func) mi->free_func(mi->data);
		  free(mi);
	       }
	     if (m->free_func) m->free_func(m->data);
	     if (l->next)
	       {
		  mm = l->next->data;
		  mm->selected = 1;
		  _menu_realize(mm);
	       }
	     
	     evas_stringshare_del(m->name);
	     evas_list_free(m->items);
	     menus = evas_list_remove_list(menus, l);
	     free(m);
	     break;
	  }
     }
}

void
menu_context_pop(void)
{
}

void
menu_pop_until(const char *context, const char *name)
{
}

void
menu_go(void)
{
   Menu *m;
 
   m = _menu_current_get();
   if (!m) return;
   _menu_realize(m);
}

void
menu_item_add(const char *icon, const char *label, 
	      const char *desc, const char *info,
	      void (*func) (void *data), void *data,
	      void (*free_func) (void *data),
	      void (*over_func) (void *data),
	      void (*out_func) (void *data)
	      )
{
   Menu *m;
   Menu_Item *mi;

   m = _menu_current_get();
   if (!m) return;
   mi = calloc(1, sizeof(Menu_Item));
   if (label) mi->label = evas_stringshare_add(label);
   if (icon) mi->icon = evas_stringshare_add(icon);
   if (desc) mi->desc = evas_stringshare_add(desc);
   if (info) mi->info = evas_stringshare_add(info);
   mi->func = func;
   mi->over_func = over_func;
   mi->out_func = out_func;
   mi->data = data;
   m->items = evas_list_append(m->items, mi);
}

void
menu_item_enabled_set(const char *name, const char *label, int enabled)
{
   Evas_List *l;
   Menu *m;
   Menu_Item *mi;
   
   for (l = menus; l; l = l->next)
     {
	m = l->data;
	if (!strcmp(name, m->name))
	  {
	     for (l = m->items; l; l = l->next)
	       {
		  mi = l->data;
		  if (!strcmp(mi->label, label))
		    {
		       mi->enabled = enabled;
		       /* FIXME: update UI */
		       break;
		    }
	       }
	     break;
	  }
     }
}

void
menu_item_select(const char *label)
{
   Evas_List *l;
   Menu *m;
   Menu_Item *mi;
 
   m = _menu_current_get();
   if (!m) return;
   for (l = m->items; l; l = l->next)
     {
	mi = l->data;
	if (strcmp(mi->label, label))
	  {
	     if (mi->selected)
	       {
		  mi->selected = 0;
		  _menu_item_select_update(m, mi);
	       }
	  }
     }
   for (l = m->items; l; l = l->next)
     {
	mi = l->data;
	if (!strcmp(mi->label, label))
	  {
	     if (!mi->selected)
	       {
		  mi->selected = 1;
		  _menu_item_select_update(m, mi);
	       }
	  }
     }
  // FIXME: start timer/animator if not running
}

void
menu_item_select_jump(int jump)
{
   Evas_List *l;
   Menu *m;
   Menu_Item *mi, **mia;
   int i, c, n, s;

   m = _menu_current_get();
   if (!m) return;
   c = evas_list_count(m->items);
   if (c <= 0) return;
   mia = alloca(sizeof(Menu_Item *) * c);
   n = -1;
   for (i = 0, l = m->items; l; l = l->next, i++)
     {
	mi = l->data;
	mia[i] = mi;
	if (mi->selected) n = i;
     }
   if (n == -1) jump = 1;
   s = n + jump;
   
   if (s < 0) s = 0;
   if (s >= c) s = c - 1;
   
   if (s == n) return;
   if (s == n) return;
   mia[n]->selected = 0;
   _menu_item_select_update(m, mia[n]);
   mia[s]->selected = 1;
   _menu_item_select_update(m, mia[s]);
}

void
menu_item_select_go(void)
{
   Evas_List *l;
   Menu *m;
   Menu_Item *mi;

   m = _menu_current_get();
   if (!m) return;
   for (l = m->items; l; l = l->next)
     {
	mi = l->data;
	if (mi->selected)
	  {
	     if (mi->func) mi->func((void *)mi->data);
	     break;
	  }
     }
  // FIXME: start timer/animator if not running
}

void
menu_key(Evas_Event_Key_Down *ev)
{
   if (!strcmp(ev->keyname, "Up")) menu_item_select_jump(-1);
   else if (!strcmp(ev->keyname, "Down")) menu_item_select_jump(1);
   else if (!strcmp(ev->keyname, "Right"))  menu_item_select_go();
   else if (!strcmp(ev->keyname, "Left"))
     {
	if ((menus) && (menus->next)) menu_pop();
     }
   else if (!strcmp(ev->keyname, "Return"))  menu_item_select_go();
}

void
menu_info_show(const char *info)
{
}

const void *
menu_data_get(void)
{
   Menu *m;
   
   m = _menu_current_get();
   if (!m) return NULL;
   return m->data;
}
