#include "timer.h"

EwlList  *ewl_get_timer_list()
{
	static EwlList *timer_list = NULL;
	if (!timer_list)	{
		timer_list = ewl_list_new();
	}
	return timer_list;
}

void      ewl_handle_timers()
{
	EwlList     *timer_list = ewl_get_timer_list();
	EwlIterator *i, *next;
	EwlTimer    *timer;
	struct timeval timev;
	double       curr_time;

	gettimeofday(&timev, NULL);
	curr_time = ((double)timev.tv_sec) + ((double)timev.tv_usec / 1000000);
	for (i=ewl_iterator_start(timer_list); i; i=next)	{
		next = ewl_iterator_next(i);
		timer = EWL_TIMER(i->data);
		if (timer->time<=curr_time)	{
			timer->cb(timer, timer->data);
			ewl_timer_remove(timer);
			ewl_timer_free(timer);
		}
	}
	return;
}


EwlTimer *ewl_timer_new(double        time, 
                        void        (*cb)(EwlTimer *timer,
                                          void     *data),
                        void         *data)
{
	EwlTimer *timer = malloc(sizeof(EwlTimer));
	timer->time = time;
	timer->cb = cb;
	timer->data = data;
	return timer;
}

void      ewl_timer_free(EwlTimer *timer)
{
	free(timer);
	return;
}


void      ewl_timer_add(double       time,
                        void        (*cb)(EwlTimer *timer,
                                          void     *data),
                        void         *data)
{
	EwlList  *timer_list = ewl_get_timer_list();
	EwlTimer *timer = ewl_timer_new(time, cb, data);
	ewl_list_insert(timer_list, ewl_list_node_new(timer));
	return;
}

void      ewl_timer_remove(EwlTimer *timer)
{
	EwlList  *timer_list = ewl_get_timer_list();
	EwlListNode *node = ewl_list_find_by_value(timer_list, timer);
	ewl_list_remove(timer_list, node);
	return;
}
