#include "db.h"
#include "x.h"
#include "mem.h"
#include "image.h"
#include "im_engine.h"


static void Elogin_BitAppend		(Elogin_Bit *bbit, Elogin_Bit *bit);
static Elogin_Bit *Elogin_BitNew	(void);

Elogin_ImageObject	*
Elogin_BitLoad (char *name)
{
	Elogin_ImageObject	*ob;
	Elogin_Bit			*bit;
	int 				num_bit = 0, n;
	
	ob = NEW(Elogin_ImageObject, 1);
	ob->bits = NULL;
	ob->x = 0;
	ob->y = 0;
	ob->w = 0;
	ob->h = 0;
	ob->clicked = 0;
	
	num_bit = 1;
	for (n = 0; n < num_bit; n++)
	{
		bit = Elogin_BitNew();
		if(ob->bits)
			Elogin_BitAppend(ob->bits, bit);
		else
			ob->bits = bit;
		bit->name	= e_db_str_get(name, "name");
		bit->class	= e_db_str_get(name, "class");
		bit->type	= e_db_int_get(name, "type");
		bit->rel1	= e_db_str_get(name, "rel1");
		bit->rel2   = e_db_str_get(name, "rel2");
		bit->x1   	= e_db_int_get(name, "x1");
		bit->y1   	= e_db_int_get(name, "y1");
		bit->x2   	= e_db_int_get(name, "x2");
		bit->y2   	= e_db_int_get(name, "y2");
		bit->image	= Elogin_LoadImage(e_db_str_get(name, "image"));
		bit->x   	= e_db_int_get(name, "x");
		bit->y   	= e_db_int_get(name, "y");
		bit->w   	= e_db_int_get(name, "w");
		bit->h   	= e_db_int_get(name, "h");
	}
	
	return ob;
};

static void
Elogin_BitAppend (Elogin_Bit *bbit, Elogin_Bit *bit)
{
	Elogin_Bit	*b;
	for (b = bbit; b; b = b->next)
	{
		if (!b->next)
		{
			b->next = bit;
			return;
		}
	}
}

static Elogin_Bit		*
Elogin_BitNew(void)
{
	Elogin_Bit	*b;

	b = NEW(Elogin_Bit, 1);
	b->name = NULL;
	b->class = NULL;
	b->type = BT_DECORATION;
	b->rel1 = NULL;
	b->rel2 = NULL;
	b->image = NULL;
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
