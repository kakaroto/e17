#ifndef E_IM_ENGINE_H
#define E_IM_ENGINE_H	1

#include <Imlib2.h>
#include "image.h"

typedef struct  _e_widget	Ewidget;
typedef struct	_elogin_image	Elogin_Image;	
struct _e_widget
{
	char			*name;
	char			*class;
	int				type;
	char			*rel1, *rel2;
	int				x1, y1, x2, y2;
	int				x, y, w, h;
	Elogin_Image	*im;
	Ewidget			*next;
};


#define BT_DECORATION        0
#define BT_NO_DRAW           1
#define BT_TEXT              2
#define BT_ICON              3
#define BT_DECOR_ONLY        4

Ewidget	*	Elogin_WidgetLoad	(char *name);
#endif
