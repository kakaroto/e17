#include "db.h"
#include "x.h"
#include "mem.h"
#include "image.h"
#include "im_engine.h"

static void         Elogin_WidgetAppend(Ewidget * bbit, Ewidget * bit);
static Ewidget     *Elogin_WidgetNew(void);

Ewidget            *
Elogin_WidgetLoad(char *name)
{
   Ewidget            *bit;
   int                 num_bit = 0, n;

   num_bit = 1;
   for (n = 0; n < num_bit; n++)
     {
	bit = Elogin_WidgetNew();
	if (main_view->widgets)
	   Elogin_WidgetAppend(main_view->widgets, bit);
	else
	   main_view->widgets = bit;
	bit->name = e_db_str_get(name, "name");
	bit->class = e_db_str_get(name, "class");
	bit->type = e_db_int_get(name, "type");
	bit->rel1 = e_db_str_get(name, "rel1");
	bit->rel2 = e_db_str_get(name, "rel2");
	bit->x1 = e_db_int_get(name, "x1");
	bit->y1 = e_db_int_get(name, "y1");
	bit->x2 = e_db_int_get(name, "x2");
	bit->y2 = e_db_int_get(name, "y2");
	bit->im = Elogin_LoadImage(e_db_str_get(name, "image"));
	bit->x = e_db_int_get(name, "x");
	bit->y = e_db_int_get(name, "y");
	bit->w = e_db_int_get(name, "w");
	bit->h = e_db_int_get(name, "h");

	printf("%s's w = %d and h = %d\n", bit->name, bit->w, bit->h);
     }

   return bit;
}

static void
Elogin_WidgetAppend(Ewidget * bbit, Ewidget * bit)
{
   Ewidget            *b;

   for (b = bbit; b; b = b->next)
     {
	if (!b->next)
	  {
	     b->next = bit;
	     return;
	  }
     }
}

static Ewidget     *
Elogin_WidgetNew(void)
{
   Ewidget            *b;

   b = NEW(Ewidget, 1);
   b->name = NULL;
   b->class = NULL;
   b->type = BT_DECORATION;
   b->rel1 = NULL;
   b->rel2 = NULL;
   b->im = NULL;
   b->x1 = 0;
   b->y1 = 0;
   b->x2 = -1;
   b->y2 = -1;
   b->next = NULL;
   b->x = 0;
   b->y = 0;
   b->w = -1;
   b->h = -1;

   return b;
}
