#ifndef _LABEL_H
#define _LABEL_H

typedef struct _Label Label;
typedef struct _Label_Private Label_Private;

struct _Label
{
	Widget widget;
	Label_Private *private;
};

Label *label_new(void);
Ekeko_Type   *label_type_get(void);
void label_text_set(Label *label, char *text);
char *label_text_get(Label *label);

#endif
