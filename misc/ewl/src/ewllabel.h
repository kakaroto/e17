#ifndef _EWL_LABEL_H_
#define _EWL_LABEL_H_ 1

#include "ewlcore.h"

#define EWLLABEL_DEFAULT_FONT "helvetica.ttf"

typedef struct _EwlLabel EwlLabel;

enum _EwlLabelFlagEnum	{
	EWL_LABEL_GROW      = 1<<0,
	EWL_LABEL_SHRINK    = 1<<1
};

struct _EwlLabel {
	EwlWidget             *widget;
	char                  *text;
	/*char                  *font;*/
	unsigned int           allocated;
	EwlFlag                flags;

	Imlib_Font             font;
	Imlib_Text_Direction   dir;
	/* double                 angle; */
};

EwlWidget *ewl_label_new(char *text);
void       ewl_label_init(EwlLabel *l, char *text, char *font,
                          Imlib_Text_Direction dir);

void       ewl_label_set_text(EwlWidget *l, char *text);
char      *ewl_label_get_text(EwlWidget *l);
int        ewl_label_get_length(EwlWidget *l);

void       ewl_label_set_font(EwlWidget *l, Imlib_Font font);
Imlib_Font ewl_label_get_font(EwlWidget *l);
void       ewl_label_set_direction(EwlWidget *l, Imlib_Text_Direction dir);
Imlib_Text_Direction  ewl_label_get_direction(EwlWidget *l);

void       ewl_label_set_flag(EwlWidget *l, EwlFlag f, EwlBool v);
EwlBool    ewl_label_get_flag(EwlWidget *l, EwlFlag f);
void       ewl_label_set_flags(EwlWidget *l, EwlFlag mask);

#endif /* _EWL_LABEL_H_ */
