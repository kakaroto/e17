
#ifndef __EWL_ENTRY_H__
#define __EWL_ENTRY_H__

typedef struct _ewl_entry Ewl_Entry;

#define EWL_ENTRY(entry) ((Ewl_Entry *) entry)

struct _ewl_entry {
	Ewl_Container   container;

	Ewl_Widget     *text;
	Ewl_Widget     *cursor;
};

Ewl_Widget     *ewl_entry_new(char *text);
void            ewl_entry_init(Ewl_Entry * e, char *text);
void            ewl_entry_set_text(Ewl_Entry * e, char *t);
char           *ewl_entry_get_text(Ewl_Entry * e);

#endif				/* __EWL_ENTRY_H__ */
