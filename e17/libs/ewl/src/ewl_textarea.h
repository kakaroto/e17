
#ifndef __EWL_TEXTAREA_H__
#define __EWL_TEXTAREA_H__

typedef struct _ewl_textarea Ewl_TextArea;

#define EWL_TEXTAREA(textarea) ((Ewl_TextArea *) textarea)

struct _ewl_textarea
{
	Ewl_ScrollPane scrollpane;

	Ewl_Widget *text;
	Ewl_Widget *cursor;
};

Ewl_Widget *ewl_textarea_new(void);
void ewl_textarea_init(Ewl_TextArea * ta);

void ewl_textarea_set_text(Ewl_TextArea * ta, char *text);
char *ewl_textarea_get_text(Ewl_TextArea * ta);

#endif /* __EWL_TEXTAREA_H__ */
