#include "ewllabel.h"

EwlWidget *ewl_label_new(char *text)
{
	EwlWidget *l = (EwlWidget*) malloc(sizeof(EwlLabel));
	EwlLabel  *label = (EwlLabel*) l;
	FUNC_BGN("ewl_label_new");
	if (!l)	{
		ewl_debug("ewl_label_new", EWL_NULL_WIDGET_ERROR, "l");
	} else {
		label->text = e_string_dup(text);
		ewl_label_init(l);
	}
	FUNC_END("ewl_label_new");
	return l;
}

void       ewl_label_init(EwlWidget *widget)
{
	EwlLabel *label = (EwlLabel*) widget;
	char     *font = NULL;
	FUNC_BGN("ewl_label_init");

	/* init parent class */
	ewl_widget_init(widget);
	ewl_widget_set_type(widget,EWL_LABEL);

	/* sanitize data */
	label->font = ewl_theme_get_string("/EwlLabel/font");
	if (!label->font)
		label->font = ewl_get_default_font();;
	if (!ewl_theme_get_int("/EwlLabel/font_size", &(label->font_size)))
		label->font_size = 12;
	label->angle = 0.0;

	ewl_callback_add(widget, EWL_EVENT_SHOW,
	                 ewl_label_handle_showhide, NULL);
	ewl_callback_add(widget, EWL_EVENT_HIDE,
	                 ewl_label_handle_showhide, NULL);
	ewl_callback_add(widget, EWL_EVENT_REALIZE,
	                 ewl_label_handle_realize, NULL);
	ewl_callback_add(widget, EWL_EVENT_RESIZE,
	                 ewl_label_handle_resize, NULL);


	FUNC_END("ewl_label_init");
	return;
}

void       ewl_label_set_text(EwlWidget *widget, char *text)
{
	EwlLabel *label = (EwlLabel*) widget;
	FUNC_BGN("ewl_label_set_text");
	if (!label) {
		ewl_debug("ewl_label_set_text", EWL_NULL_WIDGET_ERROR, "label");
	} else if (!text) {
		ewl_debug("ewl_label_set_text", EWL_NULL_ERROR, "text");
	} else {
		label->text = e_string_dup(text);
		if (ewl_widget_get_flag(widget,REALIZED))	{
			evas_set_text(ewl_widget_get_evas(widget),
			              label->text_obj, text);
		}
	}
	FUNC_END("ewl_label_set_text");
	return;
}

char      *ewl_label_get_text(EwlWidget *widget)
{
	EwlLabel *label = (EwlLabel*) widget;
	char     *text  = NULL;
	FUNC_BGN("ewl_label_get_text");
	if (!label) {
		ewl_debug("ewl_label_get_text", EWL_NULL_WIDGET_ERROR, "label");
	} else {
		text = e_string_dup(label->text);
	}
	FUNC_END("ewl_label_get_text");
	return text;
}

int        ewl_label_get_length(EwlWidget *widget)
{
	EwlLabel *label = (EwlLabel*) widget;
	int       length = 0;
	FUNC_BGN("ewl_label_get_length");
	if (!label) {
		ewl_debug("ewl_label_get_length", EWL_NULL_WIDGET_ERROR, "label");
	} else {
		length = label->text?strlen(label->text):0;
	}
	FUNC_END("ewl_label_get_length");
	return length;
}


void       ewl_label_set_font(EwlWidget *widget, char *font)
{
	EwlLabel *label = (EwlLabel*) widget;
	FUNC_BGN("ewl_label_set_font");
	if (!label) {
		ewl_debug("ewl_label_set_font", EWL_NULL_WIDGET_ERROR, "label");
	} else if (!font) {
		ewl_debug("ewl_label_set_font", EWL_NULL_ERROR, "font");
	} else {
		label->font = e_string_dup(font);
		if (ewl_widget_get_flag(widget,REALIZED))	{
			evas_set_font(ewl_widget_get_evas(widget),
			              label->text_obj, font, label->font_size);
		}
	}
	FUNC_END("ewl_label_set_font");
	return;
}

char      *ewl_label_get_font(EwlWidget *widget)
{
	EwlLabel *label = (EwlLabel*) widget;
	char     *font  = NULL;
	FUNC_BGN("ewl_label_get_font");
	if (!label) {
		ewl_debug("ewl_label_get_font", EWL_NULL_WIDGET_ERROR, "label");
	} else {
		font = e_string_dup(label->font);
	}
	FUNC_END("ewl_label_get_font");
	return font;
}

void       ewl_label_set_font_size(EwlWidget *widget, int size)
{
	EwlLabel *label = (EwlLabel*) widget;
	FUNC_BGN("ewl_label_set_font_size");
	if (!label) {
		ewl_debug("ewl_label_set_font_size", EWL_NULL_WIDGET_ERROR, "label");
	} else {
		label->font_size = size;
		if (ewl_widget_get_flag(widget, REALIZED))	{
			evas_set_font(ewl_widget_get_evas(widget),
			              label->text_obj, label->font, label->font_size);
		}
	}
	FUNC_END("ewl_label_set_font_size");
	return;
}

int        ewl_label_get_font_size(EwlWidget *widget)
{
	EwlLabel *label = (EwlLabel*) widget;
	int       size = 0;
	FUNC_BGN("ewl_label_get_font_size");
	if (!label) {
		ewl_debug("ewl_label_get_font_size", EWL_NULL_WIDGET_ERROR, "label");
	} else {
		size = label->font_size;
	}
	FUNC_END("ewl_label_get_font_size");
	return size;
}


void       ewl_label_set_angle(EwlWidget *widget, double angle)
{
	EwlLabel *label = (EwlLabel*) widget;
	FUNC_BGN("ewl_label_set_angle");
	if (!label) {
		ewl_debug("ewl_label_set_angle", EWL_NULL_WIDGET_ERROR, "label");
	} else {
		label->angle = angle;
		if (ewl_widget_get_flag(widget, REALIZED))	{
			evas_set_angle(ewl_widget_get_evas(widget),
			               label->text_obj, label->angle);
		}
	}
	FUNC_END("ewl_label_set_angle");
	return;
}

double     ewl_label_get_angle(EwlWidget *widget)
{
	EwlLabel *label = (EwlLabel*) widget;
	double    angle;
	FUNC_BGN("ewl_label_get_angle");
	if (!label) {
		ewl_debug("ewl_label_get_angle", EWL_NULL_WIDGET_ERROR, "label");
	} else {
		angle = label->angle;
	}
	FUNC_END("ewl_label_get_angle");
	return angle;
}



/* EwlLabel eent handlers */
EwlBool    ewl_label_handle_realize(EwlWidget *widget,
                                    EwlEvent  *ev,
                                    EwlData   *data)
{
	EwlLabel *label = (EwlLabel*) widget;
	FUNC_BGN("ewl_label_handle_realize");
	ewl_widget_get_theme(widget,"/EwlLabel");
	label->text_obj = evas_add_text(ewl_widget_get_evas(widget),
	                                label->font, label->font_size,
	                                label->text);
	evas_set_layer(ewl_widget_get_evas(widget), label->text_obj,
	               ewl_widget_get_stacking_layer(widget)+5);
	evas_set_color(ewl_widget_get_evas(widget), label->text_obj,
	               255, 0, 0, 128);
	evas_set_pass_events(ewl_widget_get_evas(widget), label->text_obj, 1);
	if (ewl_widget_get_flag(widget,VISIBLE))	{
		evas_show(ewl_widget_get_evas(widget), label->text_obj);
		fprintf(stderr, "EWLLABELREALIZE: showig label->text_obj\nfont=%s, font size=%d, text=%s\n", label->font, label->font_size, label->text);
	}
	FUNC_END("ewl_label_handle_realize");
	return TRUE;
}

EwlBool    ewl_label_handle_showhide(EwlWidget *widget,
                                     EwlEvent  *ev,
                                     EwlData   *data)
{
	EwlLabel *label = (EwlLabel*) widget;
	FUNC_BGN("ewl_label_hanle_showhide");
	switch (ev->type)	{
	case EWL_EVENT_SHOW:
		if (ewl_widget_get_flag(widget,REALIZED))
			evas_show(ewl_widget_get_evas(widget), label->text_obj);
		fprintf(stderr, "EWLLABEL: showig label->text_obj\n");
		break;
	case EWL_EVENT_HIDE:
		if (ewl_widget_get_flag(widget,REALIZED))
			evas_hide(ewl_widget_get_evas(widget), label->text_obj);
		break;
	default:
		break;
	}
	
	FUNC_END("ewl_label_hanle_showhide");
	return TRUE;
}

EwlBool    ewl_label_handle_resize(EwlWidget *widget,
                                   EwlEvent  *ev,
                                   EwlData   *data)
{
	EwlLabel *label = (EwlLabel*) widget;
	int *fp, tx, ty, tw, th;
	EwlRect *rect = NULL;
	double x, y, w, h, gw, gh;
	FUNC_BGN("ewl_label_hanle_resize");
	if  (ewl_widget_get_flag(widget,REALIZED))	{
		/* ripped straight from the ewlcontainer widge resize code */
		/*rect = ewl_widget_get_rect(widget);*/
		fp = ewl_widget_get_full_padding(widget);
		w = widget->layout->rect->w      -
		    widget->padding[EWL_PAD_LEFT] -
		    widget->padding[EWL_PAD_RIGHT];
		h = widget->layout->rect->h      -
		    widget->padding[EWL_PAD_TOP]  -
		    widget->padding[EWL_PAD_BOTTOM];
		evas_get_geometry(ewl_widget_get_evas(widget), label->text_obj,
		                  0, 0, &gw, &gh);
		x = fp[0] + w/2.0 - gw/2;
		y = fp[1] + h/2.0 - gh/2;
		/*w = rect->w - widget->padding[EWL_PAD_LEFT] -
		    widget->padding[EWL_PAD_RIGHT];
		h = rect->h - widget->padding[EWL_PAD_TOP] -
		    widget->padding[EWL_PAD_BOTTOM];
		x = rect->x + (w/2.0 - gw/2);
		x = rect->y + (h/2.0 - gh/2);*/
		
		evas_move(ewl_widget_get_evas(widget), label->text_obj, x, y);
		/*evas_resize(ewl_widget_get_evas(widget), label->text_obj, w, h);*/
		/*evas_text_at(ewl_widget_get_evas(widget), label->text_obj,
		                    &w, &h, 0,0);*/
		/*fprintf(stderr,"ewl_label_handle_resize(): label 0x%08x to "
		        "%3.0f %2.0f, %3.0f, %3.0f\n", 
		        (unsigned int) label, x+w/2, y+h/2, w, h);*/
		ewl_rect_free(rect);
		free(fp);
	}
	FUNC_END("ewl_label_hanle_resize");
	return TRUE;
}

