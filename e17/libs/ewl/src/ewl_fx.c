
#include <Ewl.h>


static inline void __ewl_fx_timer_func(int val, void *data);
static inline void __ewl_fx_func_start(Ewl_Widget * w, void *ev_data,
				       void *user_data);
static inline void __ewl_fx_func_stop(Ewl_Widget * w, void *ev_data,
				      void *user_data);

static Ewd_Hash *fx_protos = NULL;
static Ewd_Hash *fx_timers = NULL;
static int fx_group_id = -1;


int
ewl_fx_init(void)
{
	int count;

	DENTER_FUNCTION(DLEVEL_STABLE);

	fx_protos = ewd_hash_new(ewd_str_hash, ewd_str_compare);
	fx_timers = ewd_hash_new(ewd_str_hash, ewd_str_compare);

	fx_group_id = ewd_plugin_group_new("fx");

	count = ewl_config_get_int("/fx/paths/count");

	if (count)
	  {
		  char key[PATH_LEN];
		  char *path;
		  int i;

		  for (i = 0; i < count; i++)
		    {
			    snprintf(key, PATH_LEN, "/fx/paths/%i", i);

			    path = ewl_config_get_str(key);

			    ewd_plugin_path_add(fx_group_id, path);
		    }
	  }

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

int
ewl_fx_deinit(void)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ewd_plugin_group_del(fx_group_id);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

int
ewl_fx_add_proto(char *name,
		 Ewl_FX_Function fx_start,
		 Ewl_FX_Timer_Function fx_cont,
		 Ewl_FX_Function fx_stop, Ewl_FX_Modifies modifies)
{
	Ewl_FX_Proto *fxp;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("name", name, -1);

	fxp = ewd_hash_get(fx_protos, name);

	if (fxp)
		DRETURN_INT(0, DLEVEL_STABLE);

	fxp = NEW(Ewl_FX_Proto, 1);
	ZERO(fxp, Ewl_FX_Proto, 1);

	fxp->fx_start = fx_start;
	fxp->fx_cont = fx_cont;
	fxp->fx_stop = fx_stop;
	fxp->modifies = modifies;
	fxp->name = name;

	ewd_hash_set(fx_protos, name, fxp);

	DRETURN_INT(1, DLEVEL_STABLE);
}

int
ewl_fx_del_proto(char *name)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("name", name, -1);

	DRETURN_INT(1, DLEVEL_STABLE);
}

Ewl_FX_Proto *
ewl_fx_proto_get(char *name)
{
	Ewl_FX_Proto *fxp;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("name", name, NULL);

	fxp = ewd_hash_get(fx_protos, name);

	DRETURN_PTR(fxp, DLEVEL_STABLE);
}

int
ewl_fx_add(Ewl_Widget * w, char *name)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, -1);
	DCHECK_PARAM_PTR_RET("name", name, -1);

	DRETURN_INT(1, DLEVEL_STABLE);
}

int
ewl_fx_del(Ewl_Widget * w, char *name)
{
	Ewl_FX_Proto *fxp;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, -1);
	DCHECK_PARAM_PTR_RET("name", name, -1);

	if (!w->fx || ewd_list_is_empty(w->fx))
		DRETURN_INT(-1, DLEVEL_STABLE);

	ewd_list_goto_first(w->fx);

	while ((fxp = ewd_list_next(w->fx)) != NULL)
	  {
		  if (!fxp->name || strcmp(fxp->name, name))
			  continue;
	  }

	if (!fxp)
		DRETURN_INT(-1, DLEVEL_STABLE);

	fxp->fx_stop(w);

	DRETURN_INT(1, DLEVEL_STABLE);
}

int
ewl_fx_add_all(Ewl_Widget * w, char *name, Ewl_Callback_Type cb_start,
	       Ewl_Callback_Type cb_stop)
{
	Ewl_FX_Proto *fxp;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, -1);
	DCHECK_PARAM_PTR_RET("name", name, -1);

	fxp = ewd_hash_get(fx_protos, name);

	if (!fxp)
		fxp = ewl_fx_plugin_load(name);

	if (!fxp)
		DRETURN_INT(0, DLEVEL_STABLE);

	if (!w->fx)
		w->fx = ewd_list_new();

	ewd_list_append(w->fx, strdup(name));

	if (cb_start != EWL_CALLBACK_NONE)
		ewl_callback_append(w, cb_start, __ewl_fx_func_start, fxp);

	if (cb_stop != EWL_CALLBACK_NONE)
		ewl_callback_append(w, cb_stop, __ewl_fx_func_stop, fxp);

	if (cb_stop != EWL_CALLBACK_DESTROY)
		ewl_callback_append(w, EWL_CALLBACK_DESTROY,
				    __ewl_fx_func_stop, fxp);

	D(DLEVEL_STABLE, "Effect %s added to %p properly", name, w);

	DRETURN_INT(1, DLEVEL_STABLE);
}

void
ewl_fx_timer_add(Ewl_Widget * w, char *name, double interval,
		 double step, int count, void *data)
{
	Ewl_FX_Timer *timer;
	Ewl_FX_Proto *fxp;
	int l;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("name", name);

	fxp = ewd_hash_get(fx_protos, name);

	if (!fxp)
	  {
		  printf("Cannot add timer since fxp %s does not exist.\n",
			 name);
		  DRETURN(DLEVEL_STABLE);
	  }

	timer = NEW(Ewl_FX_Timer, 1);
	ZERO(timer, Ewl_FX_Timer, 1);

	timer->widget = w;
	timer->func = fxp->fx_cont;
	timer->step = step;
	timer->interval = interval;
	timer->count = 0;
	timer->hits = count;
	timer->data = data;

	l = strlen(name);
	l += 20;

	timer->name = NEW(char, l);

	snprintf(timer->name, l, "%s%p", name, w);
	D(DLEVEL_STABLE, "adding timer %s", timer->name);

	ecore_add_event_timer(timer->name, timer->interval,
			      __ewl_fx_timer_func, 0, timer);

	ewd_hash_set(fx_timers, timer->name, timer);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_fx_timer_del(Ewl_Widget * w, char *name)
{
	Ewl_FX_Timer *timer;
	char name2[PATH_LEN];

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("name", name);

	snprintf(name2, PATH_LEN, "%s%p", name, w);

	D(DLEVEL_STABLE, "attempting to remove %s", name2);

	timer = ewd_hash_remove(fx_timers, name2);

	if (!timer)
	  {
		  printf("Cannot delete timer since %s has not been added.\n",
			 name);
		  DRETURN(DLEVEL_STABLE);
	  }

	ecore_del_event_timer(timer->name);

	timer->widget = NULL;
	timer->func = NULL;
	timer->step = 0.0;
	timer->interval = 0.0;
	timer->count = 0;
	timer->data = NULL;
	FREE(timer->name);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

double
ewl_fx_calculate_interval(double fps, double timeout)
{
	double ival = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ival = timeout / (fps * timeout);

	D(DLEVEL_STABLE, "interval = %f", ival);

	DRETURN_FLOAT(ival, DLEVEL_STABLE);
}

double
ewl_fx_calculate_step(double start_val, double end_val,
		      double fps, double timeout)
{
	double step = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);

	step = timeout / (fps * timeout);

	if (end_val > start_val)
		step *= (end_val - start_val);
	else if (start_val > end_val)
		step *= (start_val - end_val);

	D(DLEVEL_STABLE, "step = %f", step);

	DRETURN_FLOAT(step, DLEVEL_STABLE);
}

void
ewl_fx_clip_box_get_color(Ewl_Widget * w, int *r, int *g, int *b, int *a)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (w->evas && w->fx_clip_box)
		evas_get_color(w->evas, w->fx_clip_box, r, g, b, a);
	else
	  {
		  if (r)
			  *r = -1;
		  if (g)
			  *g = -1;
		  if (b)
			  *b = -1;
		  if (a)
			  *a = -1;
	  }

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_fx_clip_box_set_color(Ewl_Widget * w, int r, int g, int b, int a)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (w->evas && w->fx_clip_box)
		evas_set_color(w->evas, w->fx_clip_box, r, g, b, a);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

Ewl_FX_Proto *
ewl_fx_plugin_load(char *name)
{
	Ewd_Plugin *plugin;
	Ewl_FX_Load_Function load;
	Ewl_FX_Proto *fxp = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("name", name, NULL);

	plugin = ewd_plugin_load(fx_group_id, name);

	if (!plugin)
	  {
		  DWARNING("Could not load fx plugin %s.\n", name);
		  DRETURN_INT(0, DLEVEL_STABLE);
	  }

	load = ewd_plugin_call(plugin, "load");

	if (load)
		fxp = load();

	if (!fxp)
	  {
		  DWARNING("Symbol load() failed to be called in plugin %s\n"
			   "Please contact the author.\n", name);
		  DRETURN_INT(0, DLEVEL_STABLE);
	  }

	fxp->plugin = plugin;
	ewd_hash_set(fx_protos, name, fxp);

	DRETURN_PTR(fxp, DLEVEL_STABLE);
}

Ewd_List *
ewl_fx_get_available(void)
{
	Ewd_List *avail;

	avail = ewd_plugin_get_available(fx_group_id);

	DRETURN_PTR(avail, DLEVEL_STABLE);
}
static inline void
__ewl_fx_timer_func(int val, void *data)
{
	Ewl_FX_Timer *timer;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("data", data);

	timer = data;

	if (!timer->func)
		DRETURN(DLEVEL_STABLE);

	if (!timer->hits || timer->count <= timer->hits)
		ecore_add_event_timer(timer->name, timer->interval,
				      __ewl_fx_timer_func, ++timer->count,
				      timer);
	timer->func(timer);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static inline void
__ewl_fx_func_start(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_FX_Proto *fxp;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("user_data", user_data);

	fxp = user_data;
	fxp->fx_start(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static inline void
__ewl_fx_func_stop(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_FX_Proto *fxp;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("user_data", user_data);

	fxp = user_data;
	fxp->fx_stop(w);

	ewl_fx_timer_del(w, fxp->name);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
