
#include <Ewl.h>


static void ewl_fx_handle_fade_in(int val, void *data);
static void ewl_fx_handle_fade_out(int val, void *data);
static void ewl_fx_handle_glow(int val, void *data);


int
ewl_fx_init()
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DRETURN_INT(1, DLEVEL_TESTING);
}

void
ewl_fx_add(Ewl_Widget * widget, Ewl_FX_Type type,
	   void (*func) (Ewl_Widget * widget, void *func_data),
	   void *func_data)
{
	Ewl_FX_Timer *timer = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);

	CHECK_PARAM_POINTER("widget", widget);

	timer = NEW(Ewl_FX_Timer, 1);
	if (!timer)
		DRETURN(DLEVEL_STABLE);

	memset(timer, 0, sizeof(Ewl_FX_Timer));

	timer->widget = widget;
	timer->type = type;
	timer->repeat = 1;
	timer->timeout = ewl_config.fx.max_fps / 1000.0;
	timer->name = malloc(64);
	timer->func = func;
	timer->func_data = func_data;

	snprintf(timer->name, 64, "%p%i", timer->widget, timer->type);

	switch (timer->type)
	  {
	  case EWL_FX_TYPE_FADE_IN:
		  timer->start_val =
			  (int) ewl_config.fx.timeout %
			  (int) ewl_config.fx.max_fps;
		  timer->increase = timer->start_val;
		  break;
	  case EWL_FX_TYPE_FADE_OUT:
		  timer->start_val = 255;
		  timer->increase =
			  (int) ewl_config.fx.timeout %
			  (int) ewl_config.fx.max_fps;
		  break;
	  case EWL_FX_TYPE_GLOW:
		  timer->start_val = 5;
		  break;
	  default:
		  break;
	  }


	if (timer->type == EWL_FX_TYPE_FADE_IN)
		e_add_event_timer(timer->name,
				  timer->timeout,
				  ewl_fx_handle_fade_in, timer->start_val,
				  timer);
	else if (timer->type == EWL_FX_TYPE_FADE_OUT)
		e_add_event_timer(timer->name,
				  timer->timeout,
				  ewl_fx_handle_fade_out, timer->start_val,
				  timer);
	else if (timer->type == EWL_FX_TYPE_GLOW)
		e_add_event_timer(timer->name,
				  timer->timeout,
				  ewl_fx_handle_glow, timer->start_val,
				  timer);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_fx_handle_fade_in(int val, void *data)
{
	Ewl_FX_Timer *timer = NULL;
	Ewl_Widget *widget = NULL;

	CHECK_PARAM_POINTER("data", data);

	timer = data;
	widget = timer->widget;

	evas_set_color(widget->evas, widget->fx_clip_box, 255, 255, 255, val);

	e_del_event_timer(timer->name);

	if (val < 255)
	  {
		  e_add_event_timer(timer->name,
				    timer->timeout,
				    ewl_fx_handle_fade_in,
				    val + timer->increase, timer);
	  }
	else
	  {
		  timer->repeat--;
		  if (timer->repeat > 0)
		    {
			    e_add_event_timer(timer->name,
					      timer->timeout,
					      ewl_fx_handle_fade_in,
					      timer->start_val, timer);
		    }
		  else
		    {
			    if (timer->func)
				    timer->func(timer->widget,
						timer->func_data);
		    }
	  }
}

static void
ewl_fx_handle_fade_out(int val, void *data)
{
	Ewl_FX_Timer *timer = NULL;
	Ewl_Widget *widget = NULL;

	CHECK_PARAM_POINTER("data", data);

	timer = data;
	widget = timer->widget;

	evas_set_color(widget->evas, widget->fx_clip_box, 255, 255, 255, val);

	e_del_event_timer(timer->name);

	if (val > 0)
	  {
		  e_add_event_timer(timer->name,
				    timer->timeout,
				    ewl_fx_handle_fade_out,
				    val - timer->increase, timer);
	  }
	else
	  {
		  timer->repeat--;
		  if (timer->repeat > 0)
		    {
			    e_add_event_timer(timer->name,
					      timer->timeout,
					      ewl_fx_handle_fade_out,
					      timer->start_val, timer);
		    }
		  else
		    {
			    if (timer->func)
				    timer->func(timer->widget,
						timer->func_data);
		    }
	  }
}

static void
ewl_fx_handle_glow(int val, void *data)
{
	Ewl_FX_Timer *timer = NULL;
	Ewl_Widget *widget = NULL;

	CHECK_PARAM_POINTER("data", data);

	timer = data;
	widget = timer->widget;

	evas_set_color(widget->evas, widget->fx_clip_box, 255, 255, 255, val);

	e_del_event_timer(timer->name);

	if (timer->start_val == 5 && val < 255)
	  {
		  e_add_event_timer(timer->name,
				    timer->timeout, ewl_fx_handle_glow,
				    val + 5, timer);
	  }
	else if (timer->start_val == 255 && val > 5)
	  {
		  e_add_event_timer(timer->name,
				    timer->timeout, ewl_fx_handle_glow,
				    val - 5, timer);

	  }

	if (timer->start_val == 255 && val == 5)
	  {
		  timer->start_val = 5;
		  e_add_event_timer(timer->name,
				    timer->timeout,
				    ewl_fx_handle_glow, timer->start_val,
				    timer);
	  }

	if (timer->start_val == 5 && val == 255)
	  {
		  timer->start_val = 255;
		  e_add_event_timer(timer->name,
				    timer->timeout,
				    ewl_fx_handle_glow, timer->start_val,
				    timer);
	  }
}
