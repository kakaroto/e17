#ifndef _EW_H
#define _EW_H

struct __Entrance_Widget {
	Etk_Widget *owner;
	Etk_Widget *box;
};

typedef struct __Entrance_Widget _Entrance_Widget;
typedef struct __Entrance_Widget *Entrance_Widget;


#define EW_FREE(ew) if(1) \
{ \
	if(ew) \
	{ \
		if(ew->owner) \
		{ \
			free(ew->owner); \
		} \
		if(ew->box) \
		{ \
			free(ew->box); \
		} \
		free(ew); \
	} \
} \
else \

#define EW_FALSE 0
#define EW_TRUE 1

int ew_init(int *, char ***);
int ew_shutdown(void);
void ew_main(void);
void ew_main_quit(void);

Entrance_Widget ew_new(void);

#endif
