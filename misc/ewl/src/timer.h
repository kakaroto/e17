#ifndef _TIMER_H_
#define _TIMER_H_

#include "list.h"
#include "widget.h"

#define EWL_TIMER(a) ((EwlTimer*)a)

typedef struct _EwlTimer EwlTimer;

struct _EwlTimer	{
	double       time;
	void        (*cb)(EwlTimer *timer, void *data);
	void         *data;
};

/* TIMER LIST FUNCTIONS */
EwlList  *ewl_timer_get_list();
void      ewl_handle_timers();

/* TIMER NEW/FREE FUNCTIONS */
EwlTimer *ewl_timer_new(double       time, 
                        void        (*cb)(EwlTimer *timer,
                                          void     *data),
                        void         *data);
void      ewl_timer_free(EwlTimer *timer);

/* TIMER ADD/REMOVE FUNCTIONS */
void      ewl_timer_add(double       time,
                        void        (*cb)(EwlTimer *timer,
                                          void     *data),
                        void         *data);
void      ewl_timer_remove(EwlTimer *timer);

#endif /* _TIMER_H_ */
