#include <Ewl.h>
#include <stdlib.h>

typedef struct {
	Ewl_Widget     *win;
	Ewl_Widget     *wvbox;
	Ewl_Widget     *whbox;
	Ewl_Widget     *closebtn;
	Ewl_Widget     *creditsbtn;
	Ewl_Widget     *title;
	Ewl_Widget     *label;
} Welcome;

typedef struct {
	Ewl_Widget     *win;
	Ewl_Widget     *credits;
} Credits;

extern Welcome *welcome;
extern Credits *credits;

void            open_welcome(void);
void            close_welcome(void);
void            close_welcome_cb(void *data);

void            open_credits_cb(void *data);
void            close_credits_cb(void *data);
void            open_credits();
void            close_credits();

/*  Callbacks  */
