
#ifndef __EWL_TEXT_H
#define __EWL_TEXT_H

struct _ewl_text {
	Ewl_Widget			widget;
	char			  * text;
	char			  * font;
	int					font_size;
	struct {
		int				r, g, b, a;
	} color;
};

typedef struct _ewl_text Ewl_Text;

#define EWL_TEXT(text) ((Ewl_Text *) text)

Ewl_Widget	  * ewl_text_new();

void			ewl_text_set_text(Ewl_Widget * etext, char * text);
char		  * ewl_text_get_text(Ewl_Widget * text);
void			ewl_text_set_font(Ewl_Widget * text, char * font);
char		  * ewl_text_get_font(Ewl_Widget * text);
void			ewl_text_set_font_size(Ewl_Widget * text, int size);
void			ewl_text_get_font_size(Ewl_Widget * text, int * size);
void			ewl_text_set_color(Ewl_Widget * widget,
								   int r, int g, int b, int a);
void			ewl_text_get_color(Ewl_Widget * widget,
								   int * r, int * g, int * b, int * a);
void			ewl_text_get_letter_geometry(Ewl_Widget * widget,
								int index, int * x, int * y, int * w, int * h);
void			ewl_text_get_letter_geometry_at(Ewl_Widget * widget,
							int x, int y, int * tx, int *ty, int * tw, int *th);

#endif
