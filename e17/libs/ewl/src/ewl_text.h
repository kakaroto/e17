
#ifndef __EWL_TEXT_H
#define __EWL_TEXT_H

typedef struct _ewl_text Ewl_Text;
#define EWL_TEXT(text) ((Ewl_Text *) text)

struct _ewl_text {
	Ewl_Widget widget;
	Etox tox;
	Etox_Color color;
	Etox_Style style;

	char *text;
	char *font;
	int font_size;
	int padding;
};

Ewl_Widget *ewl_text_new();
void ewl_text_init(Ewl_Text * widget);

void ewl_text_set_text(Ewl_Text * etext, char *text);
char *ewl_text_get_text(Ewl_Text * text);
void ewl_text_set_font(Ewl_Text * text, char *font);
char *ewl_text_get_font(Ewl_Text * text);
void ewl_text_set_font_size(Ewl_Text * text, int size);
int ewl_text_get_font_size(Ewl_Text * text);
void ewl_text_set_color(Ewl_Text * widget, int r, int g, int b, int a);
void ewl_text_get_color(Ewl_Text * widget, int *r, int *g, int *b,
			int *a);
void ewl_text_get_text_geometry(Ewl_Text * widget, double *x, double *y,
				double *w, double *h);
void ewl_text_set_style(Ewl_Text * w, char * s);
void ewl_text_get_letter_geometry(Ewl_Text * t, int index, double *x,
				  double *y, double *w, double *h);
void ewl_text_get_letter_geometry_at(Ewl_Text * t, double x, double y,
				     double *tx, double *ty, double *tw, double *th);
void ewl_text_set_alignment(Ewl_Text * t, Ewl_Alignment a);

#endif
