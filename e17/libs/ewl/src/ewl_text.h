
#ifndef __EWL_TEXT_H
#define __EWL_TEXT_H

typedef struct _ewl_text Ewl_Text;

#define EWL_TEXT(text) ((Ewl_Text *) text)

struct _ewl_text
{
	Ewl_Widget widget;
	Etox *tox;
	Etox_Color color;
	Etox_Style style;

	char *text;
	char *font;
	int font_size;
	int padding;
};

Ewl_Widget *ewl_text_new();

void ewl_text_set_text(Ewl_Widget * w, char *text);
char *ewl_text_get_text(Ewl_Widget * w);
void ewl_text_set_font(Ewl_Widget * w, char *font);
char *ewl_text_get_font(Ewl_Widget * w);
void ewl_text_set_font_size(Ewl_Widget * w, int size);
int ewl_text_get_font_size(Ewl_Widget * w);
void ewl_text_set_color(Ewl_Widget * w, int r, int g, int b, int a);
void ewl_text_get_color(Ewl_Widget * w, int *r, int *g, int *b, int *a);
void ewl_text_get_text_geometry(Ewl_Widget * w, double *xx, double *yy,
				double *ww, double *hh);
void ewl_text_set_style(Ewl_Widget * w, char *s);
void ewl_text_get_letter_geometry(Ewl_Widget * w, int index, double *xx,
				  double *yy, double *ww, double *hh);
void ewl_text_get_letter_geometry_at(Ewl_Widget * w, double x, double y,
				     double *tx, double *ty, double *tw,
				     double *th);
void ewl_text_set_alignment(Ewl_Widget * w, Ewl_Alignment a);
int ewl_text_get_index_at(Ewl_Widget * w, double x, double y, int *index);

#endif
