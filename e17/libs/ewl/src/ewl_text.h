
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
	Ewl_Alignment align;
};

Ewl_Widget *ewl_text_new();

void ewl_text_set_text(Ewl_Widget * etext, const char *text);
char *ewl_text_get_text(Ewl_Widget * text);
void ewl_text_set_font(Ewl_Widget * text, char *font);
char *ewl_text_get_font(Ewl_Widget * text);
void ewl_text_set_font_size(Ewl_Widget * text, int size);
int ewl_text_get_font_size(Ewl_Widget * text);
void ewl_text_set_color(Ewl_Widget * widget, int r, int g, int b, int a);
void ewl_text_get_color(Ewl_Widget * widget, int *r, int *g, int *b,
			int *a);
void ewl_text_get_text_geometry(Ewl_Widget * widget, double *x, double *y,
				double *w, double *h);
void ewl_text_get_letter_geometry(Ewl_Widget * widget, int index, int *x,
				  int *y, int *w, int *h);
void ewl_text_get_letter_geometry_at(Ewl_Widget * widget, int x, int y,
				     int *tx, int *ty, int *tw, int *th);

#endif
