#ifndef _EWL_LABEL_H_
#define _EWL_LABEL_H_ 1

#include "ewlcore.h"

typedef struct _EwlLabel EwlLabel;

struct _EwlLabel {
	EwlWidget              widget;
	Evas_Object            text_obj;

	char                  *text;
	char                  *font;
	int                    font_size;

	double                 angle;
	double                 h_align,
	                       v_align;
};

EwlWidget *ewl_label_new(char *text);
void       ewl_label_init(EwlWidget *widget);

void       ewl_label_set_text(EwlWidget *widget, char *text);
char      *ewl_label_get_text(EwlWidget *widget);
int        ewl_label_get_length(EwlWidget *widget);

void       ewl_label_set_font(EwlWidget *widget, char *font);
char      *ewl_label_get_font(EwlWidget *widget);

void       ewl_label_set_font_size(EwlWidget *widget, int size);
int        ewl_label_get_font_size(EwlWidget *widget);

void       ewl_label_set_angle(EwlWidget *widget, double angle);
double     ewl_label_get_angle(EwlWidget *widget);


/* EwlLabel eent handlers */
EwlBool    ewl_label_handle_realize(EwlWidget *widget,
                                    EwlEvent  *ev,
                                    EwlData   *data);
EwlBool    ewl_label_handle_showhide(EwlWidget *widget,
                                     EwlEvent  *ev,
                                     EwlData   *data);
EwlBool    ewl_label_handle_resize(EwlWidget *widget,
                                   EwlEvent  *ev,
                                   EwlData   *data);

#endif /* _EWL_LABEL_H_ */
