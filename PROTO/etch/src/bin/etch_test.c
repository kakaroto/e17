#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <signal.h>

#include "Etch.h"

/* Helper utility to test properties, animations, etc */

int _timer_event = 0;

void _animation_cb(const Etch_Data *curr, const Etch_Data *prev, void *data)
{
	printf("curr %d old %d\n", curr->data.u32, prev->data.u32);
}

/* Timer function */
void timer_signal_cb(int s)
{
	_timer_event = 1;
}

void timer_setup(void)
{
	struct sigaction sact;
	struct itimerval value;
			
	/* create the timer callback */
	sact.sa_flags = 0;
	sact.sa_handler = timer_signal_cb;
	
	value.it_interval.tv_sec = 0;
	value.it_interval.tv_usec = 33333; /* every 33333 (1/30fps) usecs */
	value.it_value.tv_sec = 0;
	value.it_value.tv_usec = 500000; /* wait 500 usecs, before triggering the first event */
	sigaction(SIGALRM, &sact, NULL);
	setitimer(ITIMER_REAL, &value, NULL);	
}

void animation_setup(Etch *e)
{
	Etch_Animation *ea;
	Etch_Animation_Keyframe *ek;
	
	ea = etch_animation_add(e, ETCH_UINT32, _animation_cb, NULL);
	/* first keyframe */
	ek = etch_animation_keyframe_add(ea);
	etch_animation_keyframe_type_set(ek, ETCH_ANIMATION_COSIN);
	etch_animation_keyframe_value_set(ek, 10);
	etch_animation_keyframe_time_set(ek, 3, 3015);
	/* second keyframe */
	ek = etch_animation_keyframe_add(ea);
	etch_animation_keyframe_type_set(ek, ETCH_ANIMATION_LINEAR);
	etch_animation_keyframe_value_set(ek, 40);
	etch_animation_keyframe_time_set(ek, 25, 1237);
	/* third keyframe */
	ek = etch_animation_keyframe_add(ea);
	etch_animation_keyframe_type_set(ek, ETCH_ANIMATION_LINEAR);
	etch_animation_keyframe_value_set(ek, 30);
	etch_animation_keyframe_time_set(ek, 15, 2530);
	/* fourth keyframe */
	ek = etch_animation_keyframe_add(ea);
	etch_animation_keyframe_type_set(ek, ETCH_ANIMATION_DISCRETE);
	etch_animation_keyframe_value_set(ek, 25);
	etch_animation_keyframe_time_set(ek, 1, 0);
	/* fifth keyframe */
	ek = etch_animation_keyframe_add(ea);
	etch_animation_keyframe_type_set(ek, ETCH_ANIMATION_DISCRETE);
	etch_animation_keyframe_value_set(ek, 15);
	etch_animation_keyframe_time_set(ek, 2, 0);
	
	ek = etch_animation_keyframe_add(ea);
	etch_animation_keyframe_type_set(ek, ETCH_ANIMATION_DISCRETE);
	etch_animation_keyframe_value_set(ek, 25);
	etch_animation_keyframe_time_set(ek, 3, 0);
	etch_animation_repeat_set(ea, 2);
}

int main(void)
{
	Etch *e;
	
	e = etch_new();
	etch_timer_fps_set(e, 30);
	animation_setup(e);
	timer_setup();
	/* to exit the main loop we should check that the etch animation has finished */
	while (!(etch_timer_has_end(e)))
	{
		if (_timer_event)
		{
			/* send a tick to etch :) and wait for events */
			etch_timer_tick(e);
			_timer_event = 0;
		}
	}
	etch_delete(e);

	return 0;
}
