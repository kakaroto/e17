
/*\
|*|
|*| Play FX code... just temporary
|*|
\*/

#ifndef __EWL_FX_H__
#define __EWL_FX_H__

enum _ewl_fx_type
{
	EWL_FX_TYPE_FADE_IN,
	EWL_FX_TYPE_FADE_OUT,
	EWL_FX_TYPE_GLOW,
	EWL_FX_TYPE_MAX
};

typedef enum _ewl_fx_type Ewl_FX_Type;

struct _ewl_fx_timer
{
	Ewl_Widget *widget;	/* What widget is it we want to do an effect on ? */
	Ewl_FX_Type type;	/* What type ? */
	int repeat;		/* How many times should we repeat ? */
	double timeout;		/* The initial timeout */
	int start_val;
	int increase;
	char *name;
	void (*func) (Ewl_Widget * widget, void *func_data);
	void *func_data;
};

typedef struct _ewl_fx_timer Ewl_FX_Timer;

#define EWL_FX_TIMER(timer) ((Ewl_FX_Timer *) timer)

int ewl_fx_init(void);
void ewl_fx_add(Ewl_Widget * widget,
		Ewl_FX_Type type,
		void (*func) (Ewl_Widget * widget,
			      void *func_data), void *func_data);

#endif /* __EWL_FX_H__ */
