#include "ewllabel.h"

EwlWidget *ewl_label_new(char *text)
{
	EwlLabel *l = malloc(sizeof(EwlLabel));
	FUNC_BGN("ewl_label_new");
	if (!l)	{
		ewl_debug("ewl_label_new", EWL_NULL_WIDGET_ERROR, "l");
	} else {
		ewl_label_init(l,text, NULL, IMLIB_TEXT_TO_LEFT);
	}
	FUNC_END("ewl_label_new");
	return (EwlWidget*) l;
}

void       ewl_label_init(EwlLabel *l, char *text, char *font,
                          Imlib_Text_Direction dir)
{
	EwlWidget *widget = (EwlWidget*) l;
	FUNC_BGN("ewl_label_init");

	/* init parent class */
	ewl_widget_init(widget);
	ewl_widget_set_type(widget,EWL_LABEL);

	/* sanitize data */
	l->text = 0;
	l->allocated = 0;
	l->font = 0;
	l->dir = dir;

	/* LOAD DB SHIT HERE */
	ewl_widget_get_theme(widget,"/EwlLabel");
	if (!font)	font = ewl_theme_get_string("/EwlLabel/font");
	if (font)	l->font = imlib_load_font(font);

	/* misc (eg user) data */
	ewl_label_set_text(widget,text);
	FUNC_END("ewl_label_init");
	return;
}


void       ewl_label_set_text(EwlWidget *l, char *text)
{
	EwlWidget *widget = (EwlWidget*) l;
	EwlLabel  *label = (EwlLabel*) l;
	int        w=0, h=0;
	FUNC_BGN("ewl_label_set_text");
	if (!l)	{
		ewl_debug("ewl_label_set_text", EWL_NULL_WIDGET_ERROR, "l");
	} else if (!text)	{
		ewl_debug("ewl_label_set_text", EWL_NULL_ERROR, "text");
	} else {
		if (label->text)	{
			free(label->text);
			label->text = 0;
			label->allocated = 0;
		}
		label->text = e_string_dup(text);
		label->allocated = strlen(label->text);
		if (!label->font)	{
			ewl_debug("ewl_label_set_text", EWL_NULL_ERROR, "label->font");
		} else {
			ewl_imlib_get_text_size(label->font, label->text,
			                        &w, &h, label->dir);
			ewl_layout_set(widget->layout, 0,0,&w,&h, 0,0,&w,&h,
			                               0,0,0,0, 0,0,0,0);
			ewl_widget_set_flag(widget, NEEDS_REFRESH, TRUE);
			ewl_widget_set_flag(widget, NEEDS_RESIZE,  TRUE);
			ewl_widget_set_flag(widget, CAN_RESIZE,    TRUE);
		}
	}
	FUNC_END("ewl_label_set_text");
	return;
}

char      *ewl_label_get_text(EwlWidget *l)
{
	EwlLabel *label = (EwlLabel*) l;
	char     *str = NULL;
	FUNC_BGN("ewl_label_get_text");
	if (!l)	{
		ewl_debug("ewl_label_get_text", EWL_NULL_WIDGET_ERROR, "l");
	} else if (!label->text)	{
		ewl_debug("ewl_label_get_text", EWL_NULL_ERROR, "label->text");
	} else {
		str = e_string_dup(label->text);
		if (!str)	{
			ewl_debug("ewl_label_get_text", EWL_NULL_ERROR, "str");
		}
	}
	FUNC_END("ewl_label_get_text");
	return str;
}

int        ewl_label_get_length(EwlWidget *l)
{
	EwlLabel *label = (EwlLabel*) l;
	int len = -1;;
	FUNC_BGN("ewl_label_get_length");
	if (!l)	{
		ewl_debug("ewl_label_get_length", EWL_NULL_WIDGET_ERROR, "l");
	} else if (!label->text)	{
		ewl_debug("ewl_label_get_length", EWL_NULL_ERROR, "label->text");
	} else {
		len = strlen(label->text);
	}
	FUNC_END("ewl_label_get_length");
	return len;
}


void       ewl_label_set_font(EwlWidget *l, Imlib_Font font)
{
	EwlLabel *label = (EwlLabel*) l;
	FUNC_BGN("ewl_label_set_font");
	if (!l)	{
		ewl_debug("ewl_label_set_font", EWL_NULL_WIDGET_ERROR, "l");
	} else if (!font)	{
		ewl_debug("ewl_label_set_font", EWL_NULL_ERROR, "font");
	} else {
		label->font = font;
	}
	FUNC_BGN("ewl_label_set_font");
	return;
}

Imlib_Font ewl_label_get_font(EwlWidget *l)
{
	EwlLabel *label = (EwlLabel*) l;
	Imlib_Font font = NULL;
	FUNC_BGN("ewl_label_get_font");
	if (!l)	{
		ewl_debug("ewl_label_get_font", EWL_NULL_WIDGET_ERROR, "l");
	} else if (!label->font)	{
		ewl_debug("ewl_label_get_font", EWL_NULL_ERROR, "font");
	} else {
		font = label->font;
	}
	FUNC_BGN("ewl_label_get_font");
	return font;
}

void       ewl_label_set_direction(EwlWidget *l, Imlib_Text_Direction dir)
{
	EwlLabel *label = (EwlLabel*) l;
	FUNC_BGN("ewl_label_set_direction");
	if (!l)	{
		ewl_debug("ewl_label_set_direction", EWL_NULL_WIDGET_ERROR, "l");
	} else {
		label->dir = dir;
	}
	FUNC_BGN("ewl_label_set_direction");
	return;
}

Imlib_Text_Direction  ewl_label_get_direction(EwlWidget *l)
{
	EwlLabel *label = (EwlLabel*) l;
	Imlib_Text_Direction dir = 0;
	FUNC_BGN("ewl_label_get_direction");
	if (!l)	{
		ewl_debug("ewl_label_get_direction", EWL_NULL_WIDGET_ERROR, "l");
	} else {
		dir = label->dir;
	}
	FUNC_BGN("ewl_label_get_direction");
	return dir;
}


void       ewl_label_set_flag(EwlWidget *l, EwlFlag f, EwlBool v)
{
	EwlLabel *label = (EwlLabel*)l;
	FUNC_BGN("ewl_label_get_flag");
	if (!l)	{
		ewl_debug("ewl_label_get_flag", EWL_NULL_WIDGET_ERROR, "l");
	} else {
		if (v)	{
			label->flags |= f;
		} else if (label->flags|f)	{
			label->flags ^= f;
		}
	}
	FUNC_END("ewl_label_get_flag");
	return;
}

EwlBool    ewl_label_get_flag(EwlWidget *l, EwlFlag f)
{
	EwlBool r = FALSE;
	FUNC_BGN("ewl_label_get_flag");
	if (!l)	{
		ewl_debug("ewl_label_get_flag", EWL_NULL_WIDGET_ERROR, "l");
	} else {
		r = ((EwlLabel*)l)->flags & f;
	}
	FUNC_END("ewl_label_get_flag");
	return r;
}

void       ewl_label_set_flags(EwlWidget *l, EwlFlag mask)
{
	EwlLabel *label = (EwlLabel*)l;
	FUNC_BGN("ewl_label_get_flag");
	if (!label)	{
		ewl_debug("ewl_label_get_flag", EWL_NULL_WIDGET_ERROR, "label");
	} else {
		label->flags = mask;
	}
	FUNC_END("ewl_label_get_flag");
	return;
}


