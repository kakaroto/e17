#ifndef E_IM_ENGINE_H
#define E_IM_ENGINE_H	1

#include <Imlib2.h>

typedef struct  _elogin_bit             Elogin_Bit;
typedef struct	_elogin_image_object	Elogin_ImageObject;

struct _elogin_bit
{
	char		*name;
	char		*class;
	int			type;
	char		*rel1, *rel2;
	int			x1, y1, x2, y2;
	Imlib_Image	*image;
	Elogin_Bit	*next;
	int			x, y, w, h;
};

struct _elogin_image_object
{
	int			x, y, w, h;
	Elogin_Bit	*bits;
	int			clicked;
};


#define BT_DECORATION        0
#define BT_NO_DRAW           1
#define BT_TEXT              2
#define BT_ICON              3
#define BT_DECOR_ONLY        4

Elogin_ImageObject	*	Elogin_BitLoad(char *name);

#endif
