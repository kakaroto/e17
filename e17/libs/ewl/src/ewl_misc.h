#ifndef __EWL_MISC_H__
#define __EWL_MISC_H__

struct _ewl_options {
	int             debug_level;
	char           *xdisplay;
};

typedef struct _ewl_options Ewl_Options;

void            ewl_init(int argc, char **argv);
void            ewl_main(void);
void            ewl_main_quit(void);
void            ewl_configure_request(Ewl_Widget * w);
void            ewl_configure_cancel_request(Ewl_Widget *w);
void            ewl_realize_request(Ewl_Widget *w);

#endif				/* __EWL_MISC_H__ */
