
#ifndef __EWL_CURSOR_H__
#define __EWL_CURSOR_H__

typedef struct _ewl_cursor Ewl_Cursor;

#define EWL_CURSOR(cursor) ((Ewl_Cursor *) cursor)

struct _ewl_cursor {
	Ewl_Widget      widget;

	struct {
		unsigned int start;
		unsigned int end;
	} position;
};

Ewl_Widget     *ewl_cursor_new(void);
void            ewl_cursor_init(Ewl_Cursor * c);
void            ewl_cursor_set_position(Ewl_Cursor * w, unsigned int start,
					unsigned int end);
unsigned int    ewl_cursor_get_start_position(Ewl_Cursor * w);
unsigned int    ewl_cursor_get_end_position(Ewl_Cursor * w);


#endif				/* __EWL_CURSOR_H__ */
