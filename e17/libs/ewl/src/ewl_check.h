#ifndef _EWL_CHECK_H_
#define _EWL_CHECK_H_

typedef struct _ewl_check Ewl_Check;

#define EWL_CHECK(c) ((Ewl_Check *)c)

struct _ewl_check {
	Ewl_Widget      w;
	int             checked;
};

Ewl_Widget     *ewl_check_new();
void            ewl_check_init(Ewl_Check * c);
int             ewl_check_is_checked(Ewl_Check * c);
void            ewl_check_set_checked(Ewl_Check * c, int checked);

#endif
