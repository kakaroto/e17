
#ifndef __EWL_ENTRY_H__
#define __EWL_ENTRY_H__

typedef struct _ewl_entry Ewl_Entry;

#define EWL_ENTRY(entry) ((Ewl_Entry *) entry)

struct _ewl_entry {
	Ewl_Container   container;

	Ewl_Widget     *text;
	Ewl_Widget     *cursor;
	Ewl_Widget     *selection;
};

Ewl_Widget     *ewl_entry_new(void);
void            ewl_entry_set_text(Ewl_Entry * e, char *t);
char           *ewl_entry_get_text(Ewl_Entry * e);

#endif				/* __EWL_ENTRY_H__ */
